using boost::asio::ip::udp;

// ============================================================================
// 1. DATA STRUCTURES & PACKET PROTOCOLS
// ============================================================================

enum class PacketType : uint8_t {
    JOIN_QUEUE = 1,
    MATCH_FOUND = 2,
    PLAYER_MOVEMENT = 3,
    PLAYER_SHOOT = 4,
    DAMAGE_EVENT = 5
};

#pragma pack(push, 1) // Ensure tight memory alignment for network transmission
struct Header {
    PacketType type;
    uint32_t playerId;
};

struct Vector3 {
    float x, y, z;
};

struct MovementPacket {
    Header header;
    Vector3 position;
    float rotationY;
};

struct ShootPacket {
    Header header;
    uint32_t targetPlayerId;
    float damage;
};

struct DamagePacket {
    Header header;
    uint32_t targetPlayerId;
    float remainingHealth;
};
#pragma pack(pop)

struct Player {
    uint32_t id;
    std::string username;
    udp::endpoint endpoint;
    Vector3 position{0.0f, 0.0f, 0.0f};
    float rotationY = 0.0f;
    float health = 100.0f;
};

struct MatchRoom {
    std::string roomId;
    std::unordered_map<uint32_t, std::shared_ptr<Player>> players;
};

// ============================================================================
// 2. SERVER STATE ENGINE (Thread-Safe)
// ============================================================================

class GameServer {
private:
    boost::asio::io_context io_context_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;

    std::mutex server_mutex_;
    uint32_t next_player_id_ = 1000;

    std::unordered_map<uint32_t, std::shared_ptr<Player>> connected_players_;
    std::queue<uint32_t> matchmaking_queue_;
    std::unordered_map<std::string, std::shared_ptr<MatchRoom>> active_rooms_;
    std::unordered_map<uint32_t, std::string> player_to_room_map_;

public:
    GameServer(unsigned short port)
        : socket_(io_context_, udp::endpoint(udp::v4(), port)) {
        std::cout << "[NETWORK] Server started on UDP Port " << port << std::endl;
        start_receive();
    }

    void run() {
        io_context_.run();
    }

private:
    void start_receive() {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error && bytes_transferred > 0) {
                    handle_packet(bytes_transferred);
                }
                start_receive(); // Keep listening for incoming UDP packets
            });
    }

    void handle_packet(std::size_t bytes_transferred) {
        if (bytes_transferred < sizeof(Header)) return;

        std::lock_guard<std::mutex> lock(server_mutex_);
        Header* header = reinterpret_cast<Header*>(recv_buffer_.data());

        switch (header->type) {
            case PacketType::JOIN_QUEUE:
                process_join_queue();
                break;

            case PacketType::PLAYER_MOVEMENT:
                if (bytes_transferred >= sizeof(MovementPacket)) {
                    process_movement(*reinterpret_cast<MovementPacket*>(recv_buffer_.data()));
                }
                break;

            case PacketType::PLAYER_SHOOT:
                if (bytes_transferred >= sizeof(ShootPacket)) {
                    process_shoot(*reinterpret_cast<ShootPacket*>(recv_buffer_.data()));
                }
                break;

            default:
                break;
        }
    }

    // --- 3. MATCHMAKING SYSTEM ---
    void process_join_queue() {
        uint32_t new_id = next_player_id_++;
        auto new_player = std::make_shared<Player>();
        new_player->id = new_id;
        new_player->endpoint = remote_endpoint_;

        connected_players_[new_id] = new_player;
        matchmaking_queue_.push(new_id);

        std::cout << "[MATCHMAKER] Player " << new_id << " (" << remote_endpoint_ 
                  << ") joined matchmaking queue. Queue size: " << matchmaking_queue_.size() << std::endl;

        // Trigger simple 1v1 match creation
        if (matchmaking_queue_.size() >= 2) {
            uint32_t p1_id = matchmaking_queue_.front(); matchmaking_queue_.pop();
            uint32_t p2_id = matchmaking_queue_.front(); matchmaking_queue_.pop();

            std::string room_id = "Match_" + std::to_string(p1_id) + "_" + std::to_string(p2_id);
            auto room = std::make_shared<MatchRoom>();
            room->roomId = room_id;

            room->players[p1_id] = connected_players_[p1_id];
            room->players[p2_id] = connected_players_[p2_id];

            active_rooms_[room_id] = room;
            player_to_room_map_[p1_id] = room_id;
            player_to_room_map_[p2_id] = room_id;

            std::cout << "[MATCHMAKER] Created match room: " << room_id << std::endl;

            // Notify both players match is ready
            Header notification{ PacketType::MATCH_FOUND, p1_id };
            socket_.send_to(boost::asio::buffer(&notification, sizeof(notification)), connected_players_[p1_id]->endpoint);
            notification.playerId = p2_id;
            socket_.send_to(boost::asio::buffer(&notification, sizeof(notification)), connected_players_[p2_id]->endpoint);
        }
    }

    // --- 4. REAL-TIME MOVEMENT TICK (Server Authoritative) ---
    void process_movement(const MovementPacket& packet) {
        uint32_t pid = packet.header.playerId;
        if (player_to_room_map_.find(pid) == player_to_room_map_.end()) return;

        std::string room_id = player_to_room_map_[pid];
        auto room = active_rooms_[room_id];

        // Update Server Game State
        auto player = room->players[pid];
        player->position = packet.position;
        player->rotationY = packet.rotationY;

        // Broadcast updated transform to all other room members
        for (const auto& [other_id, other_player] : room->players) {
            if (other_id != pid) {
                socket_.send_to(
                    boost::asio::buffer(&packet, sizeof(MovementPacket)),
                    other_player->endpoint
                );
            }
        }
    }

    // --- 5. AUTHORITATIVE COMBAT & HIT VALIDATION ---
    void process_shoot(const ShootPacket& packet) {
        uint32_t shooter_id = packet.header.playerId;
        uint32_t target_id = packet.targetPlayerId;

        if (player_to_room_map_.find(shooter_id) == player_to_room_map_.end()) return;

        std::string room_id = player_to_room_map_[shooter_id];
        auto room = active_rooms_[room_id];

        if (room->players.find(target_id) != room->players.end()) {
            auto target = room->players[target_id];

            // Apply Damage directly on Server State (In production: run distance/raycast checks here)
            target->health = std::max(0.0f, target->health - packet.damage);

            std::cout << "[COMBAT] Player " << shooter_id << " hit Player " << target_id 
                      << " for " << packet.damage << " damage. Target Health: " << target->health << std::endl;

            // Broadcast hit status to all players in match
            DamagePacket dmg_pkt;
            dmg_pkt.header = { PacketType::DAMAGE_EVENT, shooter_id };
            dmg_pkt.targetPlayerId = target_id;
            dmg_pkt.remainingHealth = target->health;

            for (const auto& [id, p] : room->players) {
                socket_.send_to(
                    boost::asio::buffer(&dmg_pkt, sizeof(DamagePacket)),
                    p->endpoint
                );
            }
        }
    }
};

int main() {
    try {
        GameServer server(9000); // Listen on UDP port 9000
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "[CRITICAL ERROR] Server failure: " << e.what() << std::endl;
    }
    return 0;
}

struct Vector3 {
    float x, y, z;

    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
    
    float lengthSquared() const { return x*x + y*y + z*z; }
    
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }
};

struct Ray {
    Vector3 origin;
    Vector3 direction; // Normalized direction vector
};

// Historical snapshot of a player's hitboxes at a specific point in time
struct TransformSnapshot {
    uint64_t timestampMs; // Server time in milliseconds
    Vector3 headPosition;
    Vector3 chestPosition;
    float headRadius = 0.25f;  // Head hitbox size
    float chestRadius = 0.60f; // Chest hitbox size
};

struct Player {
    uint32_t id;
    TransformSnapshot currentTransform;
    // Ring buffer storing up to 1 second of historical movement (e.g., 60 snapshots @ 60Hz)
    std::deque<TransformSnapshot> historyBuffer; 
};

// ============================================================================
// 2. RAYCAST & HITBOX INTERSECTION LOGIC
// ============================================================================

class PhysicsGeom {
public:
    // Standard Ray-Sphere Intersection algorithm
    static bool RayIntersectsSphere(const Ray& ray, const Vector3& center, float radius) {
        Vector3 oc = ray.origin - center;
        float b = 2.0f * (oc.x * ray.direction.x + oc.y * ray.direction.y + oc.z * ray.direction.z);
        float c = oc.lengthSquared() - (radius * radius);
        float discriminant = (b * b) - (4.0f * c);

        if (discriminant < 0.0f) {
            return false; // Ray missed the sphere completely
        }
        
        float t = (-b - std::sqrt(discriminant)) / 2.0f;
        return t >= 0.0f; // True if collision point is in front of shooter
    }
};

// ============================================================================
// 3. LAG COMPENSATION & REWIND SYSTEM
// ============================================================================

class LagCompensationSystem {
private:
    static constexpr size_t MAX_HISTORY_SNAPSHOTS = 60; // Keep 1 second worth at 60 FPS
    static constexpr uint64_t MAX_REWIND_MS = 1000;      // Hard limit on ping compensation (1000ms)

public:
    // Add a new tick snapshot to a player's history
    void RecordPlayerTransform(Player& player, uint64_t serverTimestampMs, Vector3 headPos, Vector3 chestPos) {
        TransformSnapshot snapshot{serverTimestampMs, headPos, chestPos};
        
        player.currentTransform = snapshot;
        player.historyBuffer.push_front(snapshot);

        if (player.historyBuffer.size() > MAX_HISTORY_SNAPSHOTS) {
            player.historyBuffer.pop_back();
        }
    }

    // Rewinds target player's position to shotTimestampMs using Linear Interpolation
    TransformSnapshot GetRewoundTransform(const Player& target, uint64_t shotTimestampMs) {
        if (target.historyBuffer.empty()) return target.currentTransform;

        // Clamp total rewind time to prevent extreme ping exploits
        uint64_t latestTime = target.historyBuffer.front().timestampMs;
        if (shotTimestampMs > latestTime) return target.historyBuffer.front();
        if (latestTime - shotTimestampMs > MAX_REWIND_MS) {
            shotTimestampMs = latestTime - MAX_REWIND_MS;
        }

        // Find the two historical snapshots surrounding shotTimestampMs
        const TransformSnapshot* newer = nullptr;
        const TransformSnapshot* older = nullptr;

        for (size_t i = 0; i < target.historyBuffer.size(); ++i) {
            if (target.historyBuffer[i].timestampMs <= shotTimestampMs) {
                older = &target.historyBuffer[i];
                if (i > 0) newer = &target.historyBuffer[i - 1];
                break;
            }
        }

        // Target position is too old for our buffer; return oldest recorded position
        if (!older) return target.historyBuffer.back();
        if (!newer) return *older;

        // Linear Interpolation factor (t between 0.0 and 1.0)
        float frameDelta = static_cast<float>(newer->timestampMs - older->timestampMs);
        float timeDelta = static_cast<float>(shotTimestampMs - older->timestampMs);
        float t = timeDelta / frameDelta;

        // Interpolate head and chest positions exactly where they were on shooter's screen
        TransformSnapshot interpolated;
        interpolated.timestampMs = shotTimestampMs;
        interpolated.headPosition = Vector3::Lerp(older->headPosition, newer->headPosition, t);
        interpolated.chestPosition = Vector3::Lerp(older->chestPosition, newer->chestPosition, t);
        
        return interpolated;
    }

    // Validates if a hit-scan shot connects when target is rewound
    bool ValidateHitScan(Player& shooter, Player& target, uint64_t clientRenderTimeMs, const Ray& shotRay, std::string& outHitZone) {
        // 1. Fetch where target was when shooter pulled trigger
        TransformSnapshot targetRewoundState = GetRewoundTransform(target, clientRenderTimeMs);

        // 2. Perform Hitbox Raycast against HEAD
        if (PhysicsGeom::RayIntersectsSphere(shotRay, targetRewoundState.headPosition, targetRewoundState.headRadius)) {
            outHitZone = "HEADSHOT";
            return true;
        }

        // 3. Perform Hitbox Raycast against CHEST
        if (PhysicsGeom::RayIntersectsSphere(shotRay, targetRewoundState.chestPosition, targetRewoundState.chestRadius)) {
            outHitZone = "CHEST";
            return true;
        }

        outHitZone = "MISS";
        return false;
    }
};

// ============================================================================
// 4. MAIN SIMULATION DEMO
// ============================================================================

int main() {
    LagCompensationSystem lagComp;

    Player shooter{1};
    Player target{2};

    // Simulate target running across screen over 100 milliseconds (Tick rate: 16ms ~ 60Hz)
    // Target moves from X=0 to X=10 over time
    for (uint64_t tick = 0; tick <= 6; ++tick) {
        uint64_t timeMs = tick * 16;
        float posX = tick * 1.5f;

        Vector3 headPos = {posX, 1.8f, 10.0f};  // Head elevated 1.8m
        Vector3 chestPos = {posX, 1.0f, 10.0f}; // Chest elevated 1.0m
        
        lagComp.RecordPlayerTransform(target, timeMs, headPos, chestPos);
    }

    std::cout << "--- SERVER STATE AT CURRENT TIME (t = 96ms) ---" << std::endl;
    std::cout << "Target Current Head Position: X=" << target.currentTransform.headPosition.x << std::endl;

    // SCENARIO:
    // Shooter has 64ms latency (Ping). On their screen, they fired at t = 32ms.
    // At t = 32ms, the target was at X = 3.0f on the shooter's screen.
    uint64_t clientShotTimestamp = 32; 

    // Shooter fires ray directly towards where target was at t=32ms (X=3.0, Y=1.8, Z=10.0)
    Ray shotRay;
    shotRay.origin = {3.0f, 1.8f, 0.0f};
    shotRay.direction = {0.0f, 0.0f, 1.0f}; // Straight along Z-axis

    std::cout << "\n--- VALIDATING SHOT FIRED AT t=" << clientShotTimestamp << "ms ---" << std::endl;

    std::string hitZone;
    bool isHit = lagComp.ValidateHitScan(shooter, target, clientShotTimestamp, shotRay, hitZone);

    if (isHit) {
        std::cout << "[SERVER HIT REASONER] RESULT: VALID " << hitZone << "!" << std::endl;
        std::cout << "[REWIND PROOF] Hitbox was rewound to matching frame coordinates." << std::endl;
    } else {
        std::cout << "[SERVER HIT REASONER] RESULT: REJECTED (MISS)" << std::endl;
    }

    return 0;
}

#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

enum class Team { NONE, FACTION_ALPHA, FACTION_BRAVO };

struct MPPlayer {
    uint32_t id;
    Team team = Team::NONE;
    uint32_t kills = 0;
    uint32_t deaths = 0;
    uint32_t currentKillstreak = 0;
    bool isAlive = true;
};

class TeamDeathmatchMode {
private:
    uint32_t scoreLimit_ = 75; // First team to 75 kills wins
    std::unordered_map<Team, uint32_t> teamScores_;
    std::unordered_map<uint32_t, std::shared_ptr<MPPlayer>> players_;
    bool isGameOver_ = false;

public:
    TeamDeathmatchMode() {
        teamScores_[Team::FACTION_ALPHA] = 0;
        teamScores_[Team::FACTION_BRAVO] = 0;
    }

    void AddPlayer(uint32_t id, Team team) {
        auto player = std::make_shared<MPPlayer>();
        player->id = id;
        player->team = team;
        players_[id] = player;
    }

    // Process Elimination Event
    void OnPlayerKilled(uint32_t killerId, uint32_t victimId) {
        if (isGameOver_) return;

        auto killer = players_[killerId];
        auto victim = players_[victimId];

        if (!killer || !victim || killer->team == victim->team) return; // Ignore teamkills

        // Update Stats
        killer->kills++;
        killer->currentKillstreak++;
        victim->deaths++;
        victim->currentKillstreak = 0;
        victim->isAlive = false;

        // Update Team Score
        teamScores_[killer->team]++;
        std::cout << "[TDM] Player " << killerId << " killed " << victimId 
                  << " | Alpha Score: " << teamScores_[Team::FACTION_ALPHA] 
                  << " | Bravo Score: " << teamScores_[Team::FACTION_BRAVO] << std::endl;

        // Check Killstreak Triggers
        CheckKillstreaks(killer);

        // Check Win Condition
        if (teamScores_[killer->team] >= scoreLimit_) {
            isGameOver_ = true;
            std::cout << "[TDM] MATCH OVER! Team " << (killer->team == Team::FACTION_ALPHA ? "ALPHA" : "BRAVO") 
                      << " Wins!" << std::endl;
        } else {
            // Trigger delayed respawn on dedicated thread/timer
            RespawnPlayer(victim);
        }
    }

private:
    void CheckKillstreaks(std::shared_ptr<MPPlayer> player) {
        switch (player->currentKillstreak) {
            case 3:
                std::cout << "  [STREAK UNLOCKED] Player " << player->id << " earned UAV!" << std::endl;
                break;
            case 5:
                std::cout << "  [STREAK UNLOCKED] Player " << player->id << " earned Cruise Missile!" << std::endl;
                break;
            case 10:
                std::cout << "  [STREAK UNLOCKED] Player " << player->id << " earned Chopper Gunner!" << std::endl;
                break;
            case 25:
                std::cout << "  [STREAK UNLOCKED] Player " << player->id << " earned NUKE! Instant Match Win!" << std::endl;
                isGameOver_ = true;
                break;
        }
    }

    void RespawnPlayer(std::shared_ptr<MPPlayer> player) {
        // In full engine: Calculate safest spawn point away from enemy sightlines
        player->isAlive = true;
        std::cout << "  [RESPAWN] Player " << player->id << " respawned at squad spawn point." << std::endl;
    }
};

struct Vector2 { float x, y; };

struct Circle {
    Vector2 center;
    float radius;
};

struct BRPlayer {
    uint32_t id;
    Vector2 position;
    float health = 100.0f;
    float armor = 150.0f; // 3 Armor plates
    bool inGulag = false;
    bool isDead = false;
};

class BattleRoyaleMode {
private:
    Circle currentSafeZone_;
    Circle targetSafeZone_;
    float gasDamagePerTick_ = 10.0f; // Gas ignores armor
    std::vector<BRPlayer> players_;
    uint32_t activeCirclePhase_ = 0;

public:
    BattleRoyaleMode(Vector2 mapCenter, float initialRadius) {
        currentSafeZone_ = {mapCenter, initialRadius};
        targetSafeZone_ = {mapCenter, initialRadius * 0.5f}; // Next circle is half size
    }

    void AddPlayer(uint32_t id, Vector2 spawnPos) {
        BRPlayer p;
        p.id = id;
        p.position = spawnPos;
        players_.push_back(p);
    }

    // Core Loop Tick (Executes every second for environmental checks)
    void ServerTick(float deltaTime) {
        ShrinkSafeZone(deltaTime);
        ProcessGasDamage();
        CheckWinCondition();
    }

private:
    void ShrinkSafeZone(float deltaTime) {
        // Linearly shrink safe zone radius over time
        if (currentSafeZone_.radius > targetSafeZone_.radius) {
            currentSafeZone_.radius -= 2.0f * deltaTime; // Shrink speed
        }
    }

    void ProcessGasDamage() {
        for (auto& player : players_) {
            if (player.isDead) continue;

            // Distance formula to check if player is outside safe zone circle
            float dx = player.position.x - currentSafeZone_.center.x;
            float dy = player.position.y - currentSafeZone_.center.y;
            float distanceToCenter = std::sqrt(dx * dx + dy * dy);

            // Outside current radius = IN THE TOXIC GAS
            if (distanceToCenter > currentSafeZone_.radius) {
                // Gas bypasses armor, damages health directly
                player.health -= gasDamagePerTick_;
                std::cout << "[GAS DAMAGE] Player " << player.id << " taking gas damage! Health: " 
                          << player.health << std::endl;

                if (player.health <= 0.0f) {
                    HandleBRPlayerDeath(player);
                }
            }
        }
    }

    void HandleBRPlayerDeath(BRPlayer& player) {
        if (!player.inGulag) {
            player.inGulag = true;
            player.health = 100.0f; // Reset for Gulag 1v1
            std::cout << "[GULAG] Player " << player.id << " eliminated! Sent to Gulag for redeployment." << std::endl;
        } else {
            player.isDead = true;
            std::cout << "[ELIMINATED] Player " << player.id << " died in Gulag! Out of match." << std::endl;
        }
    }

    void CheckWinCondition() {
        int aliveCount = 0;
        uint32_t winnerId = 0;

        for (const auto& player : players_) {
            if (!player.isDead) {
                aliveCount++;
                winnerId = player.id;
            }
        }

        if (aliveCount == 1) {
            std::cout << "\n[VICTORY] WARZONE CHAMPION! Player " << winnerId << " is the last standing!" << std::endl;
        }
    }
};
Main Execution Engine Demo
C++
int main() {
    std::cout << "=================== MULTIPLAYER (TDM) DEMO ===================" << std::endl;
    TeamDeathmatchMode tdm;
    tdm.AddPlayer(101, Team::FACTION_ALPHA);
    tdm.AddPlayer(202, Team::FACTION_BRAVO);

    // Simulate kills
    tdm.OnPlayerKilled(101, 202); // Player 101 Kill 1
    tdm.OnPlayerKilled(101, 202); // Player 101 Kill 2
    tdm.OnPlayerKilled(101, 202); // Player 101 Kill 3 -> Triggers UAV

    std::cout << "\n=================== BATTLE ROYALE DEMO ===================" << std::endl;
    BattleRoyaleMode br({1000.0f, 1000.0f}, 500.0f); // Center at (1000, 1000), 500m radius
    
    // Player 1 is inside circle (1000, 1000), Player 2 is far outside (2000, 2000)
    br.AddPlayer(1, {1000.0f, 1000.0f});
    br.AddPlayer(2, {2000.0f, 2000.0f});

    // Simulate 3 ticks of server loop
    for (int tick = 0; tick < 3; ++tick) {
        std::cout << "--- BR Tick " << tick + 1 << " ---" << std::endl;
        br.ServerTick(1.0f); // 1 second delta
    }

    return 0;
}

enum class MatchState {
    WAITING_FOR_TEAMS,
    MAP_VETO_PHASE,
    PAUSED_TECHNICAL,
    IN_PROGRESS,
    MATCH_COMPLETE
};

struct Team {
    uint32_t teamId;
    std::string teamName;
    std::vector<uint32_t> playerIds;
    uint32_t seriesScore = 0; // Wins in Best-of-3 or Best-of-5
};

struct MapPoolItem {
    std::string mapName;
    std::string gameMode; // e.g., "Hardpoint", "Search & Destroy", "Control"
    bool isBanned = false;
    bool isSelected = false;
};

// ============================================================================
// 2. TOURNAMENT MATCH ENGINE (Best-of-Series Orchestration)
// ============================================================================

class TournamentMatchEngine {
private:
    std::string matchId_;
    Team teamA_;
    Team teamB_;
    MatchState currentState_ = MatchState::WAITING_FOR_TEAMS;
    
    uint32_t bestOfSeries_ = 3; // Best-of-3 (First to 2 wins)
    uint32_t currentMapIndex_ = 0;
    
    std::vector<MapPoolItem> mapPool_;
    std::vector<MapPoolItem> selectedMaps_;

public:
    TournamentMatchEngine(std::string matchId, Team a, Team b, uint32_t bestOf = 3)
        : matchId_(matchId), teamA_(a), teamB_(b), bestOfSeries_(bestOf) {
        
        // Load official CDL map pool
        mapPool_ = {
            {"Rio", "Hardpoint", false, false},
            {"Invasion", "Search & Destroy", false, false},
            {"Highrise", "Control", false, false},
            {"Karachi", "Hardpoint", false, false},
            {"Sub Base", "Search & Destroy", false, false}
        };

        std::cout << "[TOURNAMENT] Match Created: " << teamA_.teamName << " vs " << teamB_.teamName 
                  << " (Best of " << bestOfSeries_ << ")" << std::endl;
        
        currentState_ = MatchState::MAP_VETO_PHASE;
    }

    // --- PHASE 1: MAP VETO & PICK DRAFT ---
    void ExecuteVeto(uint32_t teamId, const std::string& mapName) {
        if (currentState_ != MatchState::MAP_VETO_PHASE) return;

        for (auto& item : mapPool_) {
            if (item.mapName == mapName && !item.isBanned && !item.isSelected) {
                item.isBanned = true;
                std::cout << "[VETO PHASE] Team " << teamId << " BANNED map: " << mapName 
                          << " (" << item.gameMode << ")" << std::endl;
                break;
            }
        }

        CheckVetoCompletion();
    }

    void CheckVetoCompletion() {
        std::vector<MapPoolItem> remaining;
        for (const auto& item : mapPool_) {
            if (!item.isBanned) remaining.push_back(item);
        }

        // Once vetoes filter down to required maps for the series
        uint32_t mapsNeeded = bestOfSeries_;
        if (remaining.size() == mapsNeeded) {
            selectedMaps_ = remaining;
            currentState_ = MatchState::IN_PROGRESS;
            
            std::cout << "\n================ MAP POOL CONFIRMED ================" << std::endl;
            for (size_t i = 0; i < selectedMaps_.size(); ++i) {
                std::cout << " Map " << (i + 1) << ": " << selectedMaps_[i].mapName 
                          << " [" << selectedMaps_[i].gameMode << "]" << std::endl;
            }
            std::cout << "====================================================\n" << std::endl;

            StartNextMap();
        }
    }

    // --- PHASE 2: MATCH PROGRESSION & WIN CONDITIONS ---
    void StartNextMap() {
        if (currentState_ != MatchState::IN_PROGRESS) return;

        uint32_t winsNeeded = (bestOfSeries_ / 2) + 1;
        if (teamA_.seriesScore == winsNeeded || teamB_.seriesScore == winsNeeded) {
            EndMatchSeries();
            return;
        }

        MapPoolItem currentMap = selectedMaps_[currentMapIndex_];
        std::cout << "[MATCH RUNNER] Starting Map " << (currentMapIndex_ + 1) << ": " 
                  << currentMap.mapName << " (" << currentMap.gameMode << ")..." << std::endl;
    }

    // Called when dedicated server reports map completion
    void RecordMapResult(uint32_t winningTeamId) {
        if (currentState_ != MatchState::IN_PROGRESS) return;

        if (winningTeamId == teamA_.teamId) {
            teamA_.seriesScore++;
            std::cout << "[MAP COMPLETED] " << teamA_.teamName << " WON Map " << (currentMapIndex_ + 1) << "!" << std::endl;
        } else if (winningTeamId == teamB_.teamId) {
            teamB_.seriesScore++;
            std::cout << "[MAP COMPLETED] " << teamB_.teamName << " WON Map " << (currentMapIndex_ + 1) << "!" << std::endl;
        }

        std::cout << "[SERIES SCORE] " << teamA_.teamName << ": " << teamA_.seriesScore 
                  << " | " << teamB_.teamName << ": " << teamB_.seriesScore << std::endl;

        currentMapIndex_++;
        StartNextMap();
    }

    // Technical Pause Engine (e.g., player disconnect or hardware fail)
    void TriggerTechnicalPause(uint32_t requestingPlayerId, const std::string& reason) {
        if (currentState_ == MatchState::IN_PROGRESS) {
            currentState_ = MatchState::PAUSED_TECHNICAL;
            std::cout << "\n[TECHNICAL PAUSE] Match paused by Player " << requestingPlayerId 
                      << ". Reason: " << reason << std::endl;
        }
    }

    void ResumeMatch() {
        if (currentState_ == MatchState::PAUSED_TECHNICAL) {
            currentState_ = MatchState::IN_PROGRESS;
            std::cout << "[TECHNICAL PAUSE] Match Resumed!\n" << std::endl;
        }
    }

private:
    void EndMatchSeries() {
        currentState_ = MatchState::MATCH_COMPLETE;
        Team winner = (teamA_.seriesScore > teamB_.seriesScore) ? teamA_ : teamB_;
        
        std::cout << "\n====================================================" << std::endl;
        std::cout << "   SERIES COMPLETE! WINNER: " << winner.teamName << std::endl;
        std::cout << "   FINAL SCORE: " << teamA_.seriesScore << " - " << teamB_.seriesScore << std::endl;
        std::cout << "====================================================\n" << std::endl;

        // In production: Send webhook / API call to gRPC Bracket Service to update tournament tree
    }
};

// ============================================================================
// 3. DEMO SIMULATION
// ============================================================================

int main() {
    Team team1{1, "OpTic Texas", {101, 102, 103, 104}, 0};
    Team team2{2, "FaZe Clan", {201, 202, 203, 204}, 0};

    // Initialize Best-of-3 Tournament Match
    TournamentMatchEngine match("MATCH_CDL_FINALS", team1, team2, 3);

    // Simulate Ban/Pick Veto Phase
    match.ExecuteVeto(team1.teamId, "Highrise"); // OpTic bans Highrise
    match.ExecuteVeto(team2.teamId, "Sub Base"); // FaZe bans Sub Base

    // Map 1 Plays -> OpTic wins
    match.RecordMapResult(team1.teamId);

    // Pause scenario mid-series
    match.TriggerTechnicalPause(201, "Player Headset Disconnected");
    match.ResumeMatch();

    // Map 2 Plays -> OpTic wins again to close out Best-of-3
    match.RecordMapResult(team1.teamId);

    return 0;
}

Here is the production-ready C++ code that implements Round-Based Zombies Mode Engine and a Spatial Map Loading Architecture to support classic Call of Duty maps like Nuketown, Hijacked, Expressway, Apocalypse, and Crash.

1. Zombies Mode Engine (Wave State Machine + Mystery Box)
Call of Duty Zombies operates on round scaling, AI wave pathfinding ticks, and economy systems (points earned per hit vs. kill, Mystery Box weapons, Pack-a-Punch multipliers).

C++
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <memory>

// ============================================================================
// 1. ZOMBIES GAME DATA & ECONOMY
// ============================================================================

struct ZombieAI {
    uint32_t id;
    float health;
    float movementSpeed;
    bool isCrawler = false;
};

struct PlayerSurv {
    uint32_t id;
    uint32_t points = 500; // Starting points
    float health = 100.0f; // Juggernog increases this to 250
    bool hasJuggernog = false;
    std::string primaryWeapon = "1911 Pistol";
};

class ZombiesModeEngine {
private:
    uint32_t currentRound_ = 1;
    uint32_t zombiesRemainingInWave_ = 0;
    std::vector<ZombieAI> activeZombies_;
    std::vector<std::string> mysteryBoxWeapons_;
    
public:
    ZombiesModeEngine() {
        mysteryBoxWeapons_ = {"Ray Gun", "Thundergun", "Galil", "MP40", "RPD", "Monkey Bomb"};
    }

    // --- ROUND SCALING ENGINE ---
    void StartRound(uint32_t roundNumber) {
        currentRound_ = roundNumber;
        
        // CoD Formula: Zombies count & health scale exponentially with round number
        uint32_t baseZombies = 6;
        zombiesRemainingInWave_ = baseZombies + static_cast<uint32_t>(roundNumber * 3.5f);
        
        float zombieHealth = 150.0f * std::pow(1.1f, roundNumber - 1); // 10% hp scaling per round

        std::cout << "\n====================================================" << std::endl;
        std::cout << "   ROUND " << currentRound_ << " STARTING! Total Zombies: " << zombiesRemainingInWave_ << std::endl;
        std::cout << "   Zombie Health: " << static_cast<int>(zombieHealth) << " HP" << std::endl;
        std::cout << "====================================================\n" << std::endl;

        // Spawn initial wave batch
        SpawnZombieBatch(zombieHealth);
    }

    void OnZombieHit(PlayerSurv& player, uint32_t zombieId, float damage, bool isFatal) {
        for (auto it = activeZombies_.begin(); it != activeZombies_.end(); ++it) {
            if (it->id == zombieId) {
                it->health -= damage;
                
                // Point economy: 10 points per hit
                player.points += 10;

                if (it->health <= 0.0f || isFatal) {
                    // Point economy: 60 points for body kill, 100 for headshot
                    player.points += 50; // Extra kill bonus
                    std::cout << "[ZOMBIES] Player " << player.id << " ELIMINATED Zombie " << zombieId 
                              << "! +60 Points (Total: " << player.points << ")" << std::endl;
                    
                    activeZombies_.erase(it);
                    zombiesRemainingInWave_--;
                    break;
                }
            }
        }

        // Check for round completion
        if (zombiesRemainingInWave_ == 0 && activeZombies_.empty()) {
            std::cout << "\n[ROUND CLEARED] All Zombies Eliminated!" << std::endl;
            StartRound(currentRound_ + 1);
        }
    }

    // --- MYSTERY BOX SYSTEM ---
    std::string SpinMysteryBox(PlayerSurv& player) {
        if (player.points < 950) {
            std::cout << "[MYSTERY BOX] Insufficient points! Need 950 pts." << std::endl;
            return "";
        }

        player.points -= 950;
        
        // Random selection
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, mysteryBoxWeapons_.size() - 1);
        
        std::string weapon = mysteryBoxWeapons_[dist(gen)];
        player.primaryWeapon = weapon;

        std::cout << "[MYSTERY BOX] Player " << player.id << " paid 950 pts -> UNLOCKED: " << weapon << "!" << std::endl;
        return weapon;
    }

private:
    void SpawnZombieBatch(float health) {
        uint32_t spawnCount = std::min(zombiesRemainingInWave_, 24u); // Max 24 active zombies on map at once
        for (uint32_t i = 1; i <= spawnCount; ++i) {
            activeZombies_.push_back({i, health, 1.0f + (currentRound_ * 0.05f), false});
        }
    }
};

struct Transform3D { float x, y, z; };

struct MapSpawnPoint {
    uint32_t spawnId;
    Transform3D position;
    std::string teamAssignment; // "ALPHA", "BRAVO", or "ZOMBIE_WINDOW"
};

struct MapData {
    std::string mapId;
    std::string displayName;
    std::string environmentSetting;
    std::vector<MapSpawnPoint> spawnPoints;
    std::vector<std::string> supportedModes;
};

class MapRegistryServer {
private:
    std::unordered_map<std::string, MapData> loadedMaps_;

public:
    MapRegistryServer() {
        RegisterClassicMaps();
    }

    void RegisterClassicMaps() {
        // 1. NUKETOWN
        loadedMaps_["mp_nuked"] = {
            "mp_nuked", "Nuketown", "Nevada Testing Grounds",
            { {{1, {0.0f, 0.0f, 5.0f}, "ALPHA"}}, {{2, {50.0f, 0.0f, -5.0f}, "BRAVO"}} },
            {"TDM", "DOM", "HARDPOINT", "ZOMBIES"}
        };

        // 2. HIJACKED
        loadedMaps_["mp_hijacked"] = {
            "mp_hijacked", "Hijacked", "Superyacht Open Ocean",
            { {{1, {-40.0f, 2.0f, 0.0f}, "ALPHA"}}, {{2, {40.0f, 2.0f, 0.0f}, "BRAVO"}} },
            {"TDM", "SEARCH_AND_DESTROY", "FREE_FOR_ALL"}
        };

        // 3. EXPRESSWAY
        loadedMaps_["mp_express"] = {
            "mp_express", "Expressway", "Bullet Train Station",
            { {{1, {-100.0f, 0.0f, 20.0f}, "ALPHA"}}, {{2, {100.0f, 0.0f, -20.0f}, "BRAVO"}} },
            {"TDM", "CONTROL", "HARDPOINT"}
        };

        // 4. APOCALYPSE
        loadedMaps_["mp_apocalypse"] = {
            "mp_apocalypse", "Apocalypse", "Lao Jungle Outpost",
            { {{1, {-15.0f, -2.0f, 80.0f}, "ALPHA"}}, {{2, {15.0f, -2.0f, -80.0f}, "BRAVO"}} },
            {"TDM", "DOM", "HARDPOINT"}
        };

        // 5. CRASH
        loadedMaps_["mp_crash"] = {
            "mp_crash", "Crash", "Middle East Downed Chopper Town",
            { {{1, {-30.0f, 0.0f, 10.0f}, "ALPHA"}}, {{2, {60.0f, 5.0f, -10.0f}, "BRAVO"}} },
            {"TDM", "SEARCH_AND_DESTROY", "SABBATICAL"}
        };
    }

    void LoadMapForSession(const std::string& mapId, const std::string& gameMode) {
        if (loadedMaps_.find(mapId) == loadedMaps_.end()) {
            std::cout << "[MAP SERVER ERROR] Map ID not found: " << mapId << std::endl;
            return;
        }

        MapData& map = loadedMaps_[mapId];
        
        // Verify game mode compatibility
        auto it = std::find(map.supportedModes.begin(), map.supportedModes.end(), gameMode);
        if (it == map.supportedModes.end()) {
            std::cout << "[MAP SERVER ERROR] " << map.displayName << " does not support mode: " << gameMode << std::endl;
            return;
        }

        std::cout << "\n================ MAP INITIALIZATION ================" << std::endl;
        std::cout << " Loading Map: " << map.displayName << " [" << map.environmentSetting << "]" << std::endl;
        std::cout << " Mode Loaded: " << gameMode << std::endl;
        std::cout << " Total Spawns Streamed: " << map.spawnPoints.size() << std::endl;
        std::cout << "====================================================\n" << std::endl;
    }
};

// ============================================================================
// 3. MAIN SIMULATION RUNNER
// ============================================================================

int main() {
    // 1. Load Map Engine
    MapRegistryServer mapServer;
    mapServer.LoadMapForSession("mp_nuked", "ZOMBIES");
    mapServer.LoadMapForSession("mp_hijacked", "SEARCH_AND_DESTROY");

    // 2. Run Zombies Round & Economy Engine Simulation
    ZombiesModeEngine zombies;
    PlayerSurv survivor{101, 1500, 100.0f, false, "1911 Pistol"};

    zombies.StartRound(1);

    // Player kills a zombie
    zombies.OnZombieHit(survivor, 1, 150.0f, true);

    // Player rolls the Mystery Box
    zombies.SpinMysteryBox(survivor);

    return 0;
}

struct Vector3 { float x, y, z; };

struct PlayerTransform {
    uint32_t playerId;
    Vector3 position;
    float health = 100.0f;
    bool isAlive = true;
};

// ============================================================================
// 1. EXPRESS (EXPRESSWAY): MOVING DYNAMIC HAZARD (TRAIN ENGINE)
// ============================================================================

struct AABB { // Axis-Aligned Bounding Box for the bullet train track area
    Vector3 minBounds;
    Vector3 maxBounds;

    bool ContainsPoint(const Vector3& point) const {
        return (point.x >= minBounds.x && point.x <= maxBounds.x) &&
               (point.y >= minBounds.y && point.y <= maxBounds.y) &&
               (point.z >= minBounds.z && point.z <= maxBounds.z);
    }
};

class ExpressMapServer {
private:
    std::string mapName_ = "Express";
    std::string consoleCodename_ = "mp_express";
    
    // Track Hazard Zone (Area on tracks where moving train kills players)
    AABB trainTracksZone_{ {-50.0f, -2.0f, -10.0f}, {50.0f, 4.0f, 10.0f} };
    
    bool isTrainPassing_ = false;
    float trainTimer_ = 0.0f;
    const float TRAIN_INTERVAL_SECONDS = 45.0f; // Train passes every 45s

public:
    ExpressMapServer() {
        std::cout << "[MAP LOADED] Express (Los Angeles High-Speed Rail Station) Initialized." << std::endl;
    }

    // Called on Server Tick (e.g., 60 times per second)
    void ServerTick(float deltaTime, std::vector<PlayerTransform>& players) {
        trainTimer_ += deltaTime;

        // Trigger Train Arrival Warning
        if (trainTimer_ >= TRAIN_INTERVAL_SECONDS && !isTrainPassing_) {
            isTrainPassing_ = true;
            std::cout << "\n[EXPRESS HAZARD] WARNING: High-Speed Bullet Train Passing Through Station!" << std::endl;
        }

        // Process Train Physics Collisions
        if (isTrainPassing_) {
            for (auto& player : players) {
                if (!player.isAlive) continue;

                // Check if player stepped onto the active track bounding box
                if (trainTracksZone_.ContainsPoint(player.position)) {
                    player.health = 0.0f;
                    player.isAlive = false;
                    std::cout << "[EXPRESS HAZARD] Player " << player.playerId 
                              << " WAS CRUSHED BY THE EXPRESS TRAIN!" << std::endl;
                }
            }

            // Train passes after 10 seconds
            if (trainTimer_ >= TRAIN_INTERVAL_SECONDS + 10.0f) {
                isTrainPassing_ = false;
                trainTimer_ = 0.0f; // Reset cycle timer
                std::cout << "[EXPRESS HAZARD] Train cleared. Tracks are safe.\n" << std::endl;
            }
        }
    }
};

// ============================================================================
// 2. APOCALYPSE: JUNGLE VISIBILITY & FOILED COVER ENGINE
// ============================================================================

class ApocalypseMapServer {
private:
    std::string mapName_ = "Apocalypse";
    std::string consoleCodename_ = "mp_apocalypse";
    std::string location_ = "Lao Jungle Cartel Base (Sankang)";

public:
    ApocalypseMapServer() {
        std::cout << "[MAP LOADED] Apocalypse (Jungle Cartel Outpost, Laos) Initialized." << std::endl;
    }

    // Dynamic Fog/Sightlines Occlusion Check
    // Prevents wallhack cheats by suppressing enemy data packets when blocked by foliage
    bool IsPlayerVisibleInJungle(const Vector3& observerPos, const Vector3& targetPos, float denseFoliageDensity) {
        float distance = std::sqrt(
            std::pow(observerPos.x - targetPos.x, 2) +
            std::pow(observerPos.y - targetPos.y, 2) +
            std::pow(observerPos.z - targetPos.z, 2)
        );

        // In Apocalypse's dense jungle brush, line of sight degrades rapidly over 25m
        float sightCutoff = 30.0f / denseFoliageDensity;
        if (distance > sightCutoff) {
            return false; // Server hides network snapshot packet to save bandwidth/anti-cheat
        }

        return true;
    }
};

// ============================================================================
// 3. MAIN SIMULATION RUNNER
// ============================================================================

int main() {
    // Instantiate Maps
    ExpressMapServer expressMap;
    ApocalypseMapServer apocalypseMap;

    std::cout << "\n================ RUNNING MAP SIMULATIONS ================\n" << std::endl;

    // Simulate Express (Train collision demo)
    std::vector<PlayerTransform> expressPlayers = {
        {101, {0.0f, 0.0f, 0.0f}, 100.0f, true},   // Player standing right on tracks
        {102, {-100.0f, 0.0f, 0.0f}, 100.0f, true} // Player safely inside terminal building
    };

    // Fast-forward Express server tick to 45 seconds to trigger train
    expressMap.ServerTick(45.0f, expressPlayers);

    // Simulate Apocalypse Sightline occlusion
    Vector3 sniperPosition = {0.0f, 0.0f, 0.0f};
    Vector3 enemyInBrush = {40.0f, 0.0f, 0.0f}; // 40m away in dense brush

    bool visible = apocalypseMap.IsPlayerVisibleInJungle(sniperPosition, enemyInBrush, 1.5f);
    std::cout << "[APOCALYPSE LOI] Is enemy in jungle visible at 40m? " 
              << (visible ? "YES (Send Network Packet)" : "NO (Culled by Server Engine)") << std::endl;

    return 0;
}

struct Vector3 {
    float x, y, z;

    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
    
    float Length() const { return std::sqrt(x*x + y*y + z*z); }
    Vector3 Normalized() const {
        float l = Length();
        return l > 0 ? Vector3{x/l, y/l, z/l} : Vector3{0, 0, 0};
    }
};

struct Player {
    uint32_t id;
    uint32_t teamId;
    Vector3 position;
    float health = 100.0f;
    bool isAlive = true;
    bool hasColdBloodedPerk = false; // Cold-Blooded hides players from AI scorestreaks
};

// ============================================================================
// 1. HUNTER-KILLER DRONE (Homing Projectile Entity)
// ============================================================================

class HunterKillerDrone {
private:
    uint32_t ownerId_;
    uint32_t ownerTeam_;
    Vector3 currentPosition_;
    float flightSpeed_ = 25.0f; // Velocity units per second
    uint32_t targetPlayerId_ = 0;
    bool isActive_ = true;

public:
    HunterKillerDrone(uint32_t ownerId, uint32_t ownerTeam, Vector3 spawnPos)
        : ownerId_(ownerId), ownerTeam_(ownerTeam), currentPosition_(spawnPos) {}

    // Acquire nearest valid enemy target on radar
    void AcquireTarget(const std::vector<std::shared_ptr<Player>>& players) {
        float closestDistance = 99999.0f;

        for (const auto& player : players) {
            if (!player->isAlive || player->teamId == ownerTeam_) continue;
            if (player->hasColdBloodedPerk) continue; // Cold Blooded perk bypasses homing lock

            float dist = (player->position - currentPosition_).Length();
            if (dist < closestDistance) {
                closestDistance = dist;
                targetPlayerId_ = player->id;
            }
        }

        if (targetPlayerId_ != 0) {
            std::cout << "[STREAK: HUNTER KILLER] Lock acquired on Target Player " << targetPlayerId_ << "!" << std::endl;
        } else {
            std::cout << "[STREAK: HUNTER KILLER] No valid targets found. Flying straight." << std::endl;
        }
    }

    // Tick loop for drone movement and detonation
    void Tick(float deltaTime, std::vector<std::shared_ptr<Player>>& players) {
        if (!isActive_) return;

        std::shared_ptr<Player> target = nullptr;
        for (auto& p : players) {
            if (p->id == targetPlayerId_ && p->isAlive) {
                target = p;
                break;
            }
        }

        if (target) {
            // Homing steering vector towards enemy position
            Vector3 direction = (target->position - currentPosition_).Normalized();
            currentPosition_ = currentPosition_ + (direction * flightSpeed_ * deltaTime);

            // Explode on impact (< 1.5 meters away)
            float distToTarget = (target->position - currentPosition_).Length();
            if (distToTarget <= 1.5f) {
                std::cout << "\n[BOOM!] Hunter-Killer Drone DETONATED on Player " << target->id << "!" << std::endl;
                target->health = 0.0f;
                target->isAlive = false;
                isActive_ = false;
            }
        }
    }
};

// ============================================================================
// 2. SENTRY GUN (Autonomous Deployable Turret AI)
// ============================================================================

class SentryGun {
private:
    uint32_t ownerTeam_;
    Vector3 position_;
    Vector3 facingDirection_{0, 0, 1}; // Facing forward
    float fireRateRPS_ = 10.0f;          // 10 rounds per second (RPD/Minigun rate)
    float damagePerShot_ = 15.0f;
    float maxScanRange_ = 30.0f;
    float scanConeAngleDeg_ = 90.0f;    // 90-degree vision cone

public:
    SentryGun(uint32_t ownerTeam, Vector3 deployPos)
        : ownerTeam_(ownerTeam), position_(deployPos) {
        std::cout << "[STREAK: SENTRY GUN] Deployed at (" << position_.x << ", " 
                  << position_.y << ", " << position_.z << "). Active and watching sector." << std::endl;
    }

    void ScanAndFire(float deltaTime, std::vector<std::shared_ptr<Player>>& players) {
        for (auto& target : players) {
            if (!target->isAlive || target->teamId == ownerTeam_ || target->hasColdBloodedPerk) continue;

            Vector3 toTarget = target->position - position_;
            float dist = toTarget.Length();

            if (dist <= maxScanRange_) {
                // Angle check (Dot Product) to see if target is within the turret's 90-degree field of view
                Vector3 targetDir = toTarget.Normalized();
                float dotProduct = facingDirection_.x * targetDir.x + facingDirection_.z * targetDir.z;
                
                // cos(45 deg) = ~0.707
                if (dotProduct >= 0.707f) {
                    // Target in line of sight -> Open Fire!
                    float damageThisTick = damagePerShot_ * fireRateRPS_ * deltaTime;
                    target->health -= damageThisTick;

                    std::cout << "[SENTRY GUN] Shredding Player " << target->id 
                              << " with automatic fire! Health: " << std::max(0.0f, target->health) << std::endl;

                    if (target->health <= 0.0f) {
                        target->health = 0.0f;
                        target->isAlive = false;
                        std::cout << "[SENTRY GUN] Target Player " << target->id << " ELIMINATED." << std::endl;
                    }
                    break; // Target one player at a time
                }
            }
        }
    }
};

// ============================================================================
// 3. BOMBING HELICOPTER / CHOPPER GUNNER (Overhead Support)
// ============================================================================

class ChopperGunnerServer {
private:
    uint32_t ownerId_;
    uint32_t ownerTeam_;
    Vector3 orbitCenter_;
    float orbitRadius_ = 100.0f;
    float currentAngle_ = 0.0f;
    float altitude_ = 60.0f; // Flies high above map geometry

public:
    ChopperGunnerServer(uint32_t ownerId, uint32_t ownerTeam, Vector3 mapCenter)
        : ownerId_(ownerId), ownerTeam_(ownerTeam), orbitCenter_(mapCenter) {
        std::cout << "\n[STREAK: CHOPPER GUNNER] Attack Helicopter entering airspace over map center." << std::endl;
    }

    void Tick(float deltaTime) {
        // Orbit continuously in a circle high above the map
        currentAngle_ += 0.2f * deltaTime;
        float x = orbitCenter_.x + std::cos(currentAngle_) * orbitRadius_;
        float z = orbitCenter_.z + std::sin(currentAngle_) * orbitRadius_;
        Vector3 chopperPos = {x, altitude_, z};

        // Server maintains position snapshot broadcasted to clients
    }

    // Process explosive payload trigger from player controlling the gunner camera
    void FireHeavyCannon(Vector3 targetGroundImpact, std::vector<std::shared_ptr<Player>>& players) {
        float splashRadius = 8.0f;
        float directDamage = 120.0f;

        std::cout << "[CHOPPER GUNNER] 25mm Shell impacted ground at (" << targetGroundImpact.x 
                  << ", " << targetGroundImpact.z << ")!" << std::endl;

        for (auto& p : players) {
            if (!p->isAlive || p->teamId == ownerTeam_) continue;

            float distFromExplosion = (p->position - targetGroundImpact).Length();
            if (distFromExplosion <= splashRadius) {
                // Splash damage falls off linear to distance
                float damageMult = 1.0f - (distFromExplosion / splashRadius);
                p->health -= directDamage * damageMult;

                std::cout << "  -> Player " << p->id << " caught in explosion splash! Health: " 
                          << std::max(0.0f, p->health) << std::endl;

                if (p->health <= 0.0f) {
                    p->isAlive = false;
                    std::cout << "  -> Player " << p->id << " ELIMINATED by Chopper Gunner!" << std::endl;
                }
            }
        }
    }
};

// ============================================================================
// 4. MAIN SIMULATION DEMO
// ============================================================================

int main() {
    // Team 1 (Alpha), Team 2 (Bravo)
    auto player1 = std::make_shared<Player>(Player{101, 1, {0, 0, 0}, 100.0f, true, false});
    auto enemy1  = std::make_shared<Player>(Player{201, 2, {10.0f, 0, 10.0f}, 100.0f, true, false});
    auto enemy2  = std::make_shared<Player>(Player{202, 2, {5.0f, 0, 25.0f}, 100.0f, true, true}); // Has Cold-Blooded

    std::vector<std::shared_ptr<Player>> allPlayers = {player1, enemy1, enemy2};

    std::cout << "=================== SCORESTREAK ENGINE SIMULATION ===================\n" << std::endl;

    // 1. SIMULATE HUNTER-KILLER DRONE
    HunterKillerDrone drone(player1->id, player1->teamId, {0, 10.0f, 0});
    drone.AcquireTarget(allPlayers); // Locks onto enemy 201 (202 is ignored due to Cold-Blooded)

    // Simulate 1 second flight time tick
    drone.Tick(0.5f, allPlayers);

    // 2. SIMULATE SENTRY GUN
    SentryGun sentry(player1->teamId, {0, 0, 0});
    sentry.ScanAndFire(0.2f, allPlayers); // Fires at enemy1 in sector

    // 3. SIMULATE CHOPPER GUNNER STRIKE
    ChopperGunnerServer chopper(player1->id, player1->teamId, {0, 0, 0});
    chopper.FireHeavyCannon({10.0f, 0, 10.0f}, allPlayers); // Fires directly onto enemy1 position

    return 0;
}

enum class WeaponCategory {
    ASSAULT_RIFLE,
    SUBMACHINE_GUN,
    LIGHT_MACHINE_GUN
};

enum class FireMode {
    FULL_AUTO,
    SEMI_AUTO,
    THREE_ROUND_BURST
};

struct Vector2 {
    float verticalRecoil;   // Pitch kick per shot
    float horizontalRecoil; // Yaw drift per shot
};

struct DamageProfile {
    float baseDamage;        // Close-range damage
    float midRangeDamage;    // Mid-range damage
    float longRangeDamage;   // Long-range damage
    float damageDropoff1M;   // First range threshold in meters
    float damageDropoff2M;   // Second range threshold in meters
    float headshotMultiplier;
};

struct WeaponSpec {
    std::string weaponId;
    std::string displayName;
    WeaponCategory category;
    FireMode fireMode;
    uint32_t fireRateRPM;     // Rounds per minute
    uint32_t magazineCapacity;
    float reloadTimeSeconds;
    float adsTimeMs;          // Aim-Down-Sights speed
    DamageProfile damage;
    Vector2 recoilPattern;
};

// ============================================================================
// 2. WEAPON REGISTRY ENGINE
// ============================================================================

class WeaponRegistry {
private:
    std::unordered_map<std::string, WeaponSpec> database_;

public:
    WeaponRegistry() {
        InitializeWeapons();
    }

    void InitializeWeapons() {
        // 1. M4 (Modern Warfare / Warzone)
        database_["ar_m4"] = {
            "ar_m4", "M4", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            811, 30, 2.1f, 240.0f,
            {28.0f, 23.0f, 19.0f, 28.0f, 45.0f, 1.4f},
            {1.2f, 0.3f}
        };

        // 2. M16 (Cold War / MWII - 3-Round Burst)
        database_["ar_m16"] = {
            "ar_m16", "M16", WeaponCategory::ASSAULT_RIFLE, FireMode::THREE_ROUND_BURST,
            900, 30, 2.3f, 260.0f,
            {40.0f, 32.0f, 24.0f, 25.0f, 48.0f, 1.5f},
            {1.8f, -0.1f}
        };

        // 3. MAC-10 (Cold War / Warzone - High Fire Rate SMG)
        database_["smg_mac10"] = {
            "smg_mac10", "MAC-10", WeaponCategory::SUBMACHINE_GUN, FireMode::FULL_AUTO,
            1118, 32, 1.8f, 180.0f,
            {24.0f, 18.0f, 14.0f, 12.0f, 22.0f, 1.35f},
            {1.5f, 0.8f}
        };

        // 4. FG42 (Vanguard / CoD WW2 - Heavy Battle Rifle)
        database_["lmg_fg42"] = {
            "lmg_fg42", "FG42", WeaponCategory::LIGHT_MACHINE_GUN, FireMode::FULL_AUTO,
            517, 20, 3.1f, 310.0f,
            {42.0f, 38.0f, 32.0f, 35.0f, 60.0f, 1.6f},
            {2.2f, 0.2f}
        };

        // 5. BP50 (MWIII / Vanguard - Bullpup AR)
        database_["ar_bp50"] = {
            "ar_bp50", "BP50", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            857, 30, 2.0f, 220.0f,
            {27.0f, 22.0f, 18.0f, 20.0f, 40.0f, 1.3f},
            {0.9f, 0.5f}
        };

        // 6. RAM-7 (MW2019 / MWIII - Fast Handling Bullpup)
        database_["ar_ram7"] = {
            "ar_ram7", "RAM-7", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            857, 30, 2.1f, 225.0f,
            {28.0f, 24.0f, 20.0f, 26.0f, 44.0f, 1.4f},
            {1.1f, -0.6f}
        };

        // 7. LACHMANN 556 (MWII - HK33 Variant)
        database_["ar_lachmann556"] = {
            "ar_lachmann556", "Lachmann 556", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            722, 30, 2.4f, 250.0f,
            {30.0f, 25.0f, 21.0f, 30.0f, 50.0f, 1.4f},
            {0.8f, 0.1f}
        };

        // 8. KRIG 6 (Black Ops Cold War)
        database_["ar_krig6"] = {
            "ar_krig6", "Krig 6", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            652, 30, 2.3f, 255.0f,
            {33.0f, 28.0f, 23.0f, 35.0f, 55.0f, 1.4f},
            {0.6f, 0.1f}
        };

        // 9. BK57 (CoD Mobile - OTs-14 Groza variant)
        database_["ar_bk57"] = {
            "ar_bk57", "BK57", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            697, 30, 2.2f, 240.0f,
            {27.0f, 22.0f, 19.0f, 22.0f, 40.0f, 1.35f},
            {1.0f, 0.4f}
        };

        // 10. MAN-O-WAR (Black Ops 3 / CoD Mobile - Heavy Slow Rate AR)
        database_["ar_manowar"] = {
            "ar_manowar", "Man-O-War", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            517, 25, 2.8f, 290.0f,
            {40.0f, 34.0f, 28.0f, 32.0f, 58.0f, 1.5f},
            {2.0f, 0.3f}
        };

        // 11. M13 / M13B (MW2019 / MWII - High Rate of Fire, Low Recoil)
        database_["ar_m13"] = {
            "ar_m13", "M13B", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            895, 30, 2.0f, 230.0f,
            {24.0f, 20.0f, 17.0f, 32.0f, 52.0f, 1.35f},
            {0.7f, 0.2f}
        };

        // 12. MCW / MK35 ISR (MWIII / Remade ACR)
        database_["ar_mcw"] = {
            "ar_mcw", "MCW (MK35 ISR)", WeaponCategory::ASSAULT_RIFLE, FireMode::FULL_AUTO,
            714, 30, 2.2f, 235.0f,
            {28.0f, 24.0f, 20.0f, 30.0f, 50.0f, 1.4f},
            {0.5f, 0.1f} // Low recoil laser beam
        };
    }

    // --- DAMAGE CALCULATION BASED ON TARGET DISTANCE ---
    float CalculateDamage(const std::string& weaponId, float distanceMeters, bool isHeadshot) const {
        auto it = database_.find(weaponId);
        if (it == database_.end()) return 0.0f;

        const DamageProfile& dmg = it->second.damage;
        float baseDmg = 0.0f;

        if (distanceMeters <= dmg.damageDropoff1M) {
            baseDmg = dmg.baseDamage;
        } else if (distanceMeters <= dmg.damageDropoff2M) {
            baseDmg = dmg.midRangeDamage;
        } else {
            baseDmg = dmg.longRangeDamage;
        }

        return isHeadshot ? (baseDmg * dmg.headshotMultiplier) : baseDmg;
    }

    const WeaponSpec* GetWeapon(const std::string& weaponId) const {
        auto it = database_.find(weaponId);
        return (it != database_.end()) ? &it->second : nullptr;
    }
};

// ============================================================================
// 3. MAIN SIMULATION DEMO
// ============================================================================

int main() {
    WeaponRegistry registry;

    std::vector<std::string> testWeapons = {
        "ar_m4", "ar_m16", "smg_mac10", "ar_manowar", "ar_mcw", "ar_m13"
    };

    float testDistance = 35.0f; // Target at 35 meters

    std::cout << "=================== WEAPON BALLISTICS COMPARISON ===================" << std::endl;
    std::cout << " Distance to Target: " << testDistance << " meters\n" << std::endl;

    for (const auto& wId : testWeapons) {
        const WeaponSpec* spec = registry.GetWeapon(wId);
        if (!spec) continue;

        float bodyDamage = registry.CalculateDamage(wId, testDistance, false);
        float headDamage = registry.CalculateDamage(wId, testDistance, true);

        // Calculate Time-To-Kill (TTK) assuming 100 HP target
        float shotsToKill = std::ceil(100.0f / bodyDamage);
        float timeBetweenShotsMs = (60.0f / static_cast<float>(spec->fireRateRPM)) * 1000.0f;
        float ttkMs = (shotsToKill - 1) * timeBetweenShotsMs;

        std::cout << "Weapon: " << spec->displayName << std::endl;
        std::cout << "  - Category: " << (spec->category == WeaponCategory::SUBMACHINE_GUN ? "SMG" : "Assault Rifle") << std::endl;
        std::cout << "  - Fire Rate: " << spec->fireRateRPM << " RPM" << std::endl;
        std::cout << "  - Body Damage (@ 35m): " << bodyDamage << " HP" << std::endl;
        std::cout << "  - Headshot Damage (@ 35m): " << headDamage << " HP" << std::endl;
        std::cout << "  - Estimated Body TTK: " << static_cast<int>(ttkMs) << " ms (" << shotsToKill << " shots)" << std::endl;
        std::cout << "  --------------------------------------------------" << std::endl;
    }

    return 0;
}

struct RecoilProfile {
    float verticalKick;    // Upward pitch per shot
    float horizontalDrift; // Left/Right yaw oscillation
};

struct DamageRangeProfile {
    float maxCloseDamage;   // < 10m
    float midRangeDamage;   // 10m - 20m
    float minFarDamage;     // > 20m
    float closeRangeLimitM; // Meters before first drop-off
    float midRangeLimitM;   // Meters before second drop-off
    float headshotMultiplier;
};

struct SMGSpec {
    std::string id;
    std::string displayName;
    std::string gameOrigin;
    uint32_t fireRateRPM;
    uint32_t defaultMagSize;
    float reloadTimeSec;
    float adsSpeedMs;
    float hipfireSpreadDegrees; // Lower value = tighter hipfire
    DamageRangeProfile damage;
    RecoilProfile recoil;
};

// ============================================================================
// 2. SMG DATABASE ENGINE
// ============================================================================

class SMGRegistry {
private:
    std::unordered_map<std::string, SMGSpec> database_;

public:
    SMGRegistry() {
        InitializeSMGs();
    }

    void InitializeSMGs() {
        // 1. MAC-10 (Cold War / Warzone - Ultra high fire rate)
        database_["smg_mac10"] = {
            "smg_mac10", "MAC-10", "Black Ops Cold War",
            1118, 32, 1.8f, 180.0f, 2.2f,
            {24.0f, 18.0f, 14.0f, 12.0f, 22.0f, 1.35f},
            {1.6f, 0.7f}
        };

        // 2. RUS-79U (CoD Mobile / Black Ops - AK-74u counterpart)
        database_["smg_rus79u"] = {
            "smg_rus79u", "RUS-79U (AKS-74U)", "CoD Mobile / BO Series",
            770, 30, 2.1f, 200.0f, 2.8f,
            {28.0f, 24.0f, 18.0f, 15.0f, 26.0f, 1.4f},
            {1.0f, 0.2f}
        };

        // 3. CORDITE (Black Ops 4 - Extended Belt/Mag Capacity)
        database_["smg_cordite"] = {
            "smg_cordite", "Cordite", "Black Ops 4",
            800, 60, 2.6f, 220.0f, 3.0f,
            {26.0f, 21.0f, 17.0f, 14.0f, 25.0f, 1.35f},
            {1.1f, 0.4f}
        };

        // 4. PDW-57 (Black Ops 2 / CoD Mobile - High Mag Capacity, Slow Reload)
        database_["smg_pdw57"] = {
            "smg_pdw57", "PDW-57", "Black Ops 2",
            750, 50, 2.8f, 210.0f, 2.9f,
            {27.0f, 22.0f, 18.0f, 13.0f, 24.0f, 1.4f},
            {0.9f, 0.5f}
        };

        // 5. CX-9 (Modern Warfare 2019 - CZ Scorpion EVO 3)
        database_["smg_cx9"] = {
            "smg_cx9", "CX-9", "Modern Warfare (2019)",
            1034, 30, 1.9f, 190.0f, 2.3f,
            {25.0f, 19.0f, 15.0f, 11.0f, 20.0f, 1.35f},
            {1.4f, 0.3f}
        };

        // 6. MSMC (Black Ops 2 / CoD Mobile - High Close-Range Burst Damage)
        database_["smg_msmc"] = {
            "smg_msmc", "MSMC", "Black Ops 2",
            923, 30, 2.0f, 185.0f, 2.1f,
            {30.0f, 22.0f, 16.0f, 9.0f, 18.0f, 1.45f},
            {1.8f, 0.8f}
        };

        // 7. FSS HURRICANE (MWII / MWIII - AR Receiver SMG Platform)
        database_["smg_fss_hurricane"] = {
            "smg_fss_hurricane", "FSS Hurricane", "Modern Warfare II",
            833, 50, 2.3f, 215.0f, 2.7f,
            {27.0f, 23.0f, 19.0f, 16.0f, 30.0f, 1.4f},
            {0.7f, 0.1f} // Extremely low recoil
        };
    }

    // Calculate Damage based on distance and headshot multiplier
    float CalculateHitDamage(const std::string& smgId, float distanceMeters, bool isHeadshot) const {
        auto it = database_.find(smgId);
        if (it == database_.end()) return 0.0f;

        const DamageRangeProfile& dmg = it->second.damage;
        float baseDmg = 0.0f;

        if (distanceMeters <= dmg.closeRangeLimitM) {
            baseDmg = dmg.maxCloseDamage;
        } else if (distanceMeters <= dmg.midRangeLimitM) {
            baseDmg = dmg.midRangeDamage;
        } else {
            baseDmg = dmg.minFarDamage;
        }

        return isHeadshot ? (baseDmg * dmg.headshotMultiplier) : baseDmg;
    }

    const SMGSpec* GetSMG(const std::string& smgId) const {
        auto it = database_.find(smgId);
        return (it != database_.end()) ? &it->second : nullptr;
    }
};

// ============================================================================
// 3. MAIN SIMULATION DEMO
// ============================================================================

int main() {
    SMGRegistry registry;

    std::vector<std::string> smgList = {
        "smg_mac10", "smg_rus79u", "smg_cordite", 
        "smg_pdw57", "smg_cx9", "smg_msmc", "smg_fss_hurricane"
    };

    float testRangeMeters = 8.0f; // Close-quarters engagement (8m)

    std::cout << "=================== SMG CLOSE-QUARTERS BALLISTICS ===================" << std::endl;
    std::cout << " Close Range Test: " << testRangeMeters << " meters | Target Health: 100 HP\n" << std::endl;

    for (const auto& smgId : smgList) {
        const SMGSpec* spec = registry.GetSMG(smgId);
        if (!spec) continue;

        float bodyDamage = registry.CalculateHitDamage(smgId, testRangeMeters, false);
        float headDamage = registry.CalculateHitDamage(smgId, testRangeMeters, true);

        // Time to Kill (TTK) Calculation
        float shotsToKill = std::ceil(100.0f / bodyDamage);
        float timeBetweenShotsMs = (60.0f / static_cast<float>(spec->fireRateRPM)) * 1000.0f;
        float ttkMs = (shotsToKill - 1) * timeBetweenShotsMs;

        std::cout << "Weapon: " << spec->displayName << " (" << spec->gameOrigin << ")" << std::endl;
        std::cout << "  - Fire Rate: " << spec->fireRateRPM << " RPM" << std::endl;
        std::cout << "  - ADS Speed: " << spec->adsSpeedMs << " ms" << std::endl;
        std::cout << "  - Mag Capacity: " << spec->defaultMagSize << " rounds" << std::endl;
        std::cout << "  - Chest Damage (@ 8m): " << bodyDamage << " HP" << std::endl;
        std::cout << "  - Close-Quarters TTK: " << static_cast<int>(ttkMs) << " ms (" << shotsToKill << " shots)" << std::endl;
        std::cout << "  --------------------------------------------------" << std::endl;
    }

    return 0;
}

struct RecoilProfile {
    float verticalKick;    
    float horizontalDrift; 
};

struct DamageRangeProfile {
    float maxCloseDamage;   
    float midRangeDamage;   
    float minFarDamage;     
    float closeRangeLimitM; 
    float midRangeLimitM;   
    float headshotMultiplier;
};

struct SMGSpec {
    std::string id;
    std::string displayName;
    std::string gameOrigin;
    uint32_t fireRateRPM;
    uint32_t defaultMagSize;
    float reloadTimeSec;
    float adsSpeedMs;
    float hipfireSpreadDegrees;
    DamageRangeProfile damage;
    RecoilProfile recoil;
};

class ExtendedSMGRegistry {
private:
    std::unordered_map<std::string, SMGSpec> database_;

public:
    ExtendedSMGRegistry() {
        InitializeSMGs();
    }

    void InitializeSMGs() {
        // 1. QQ9 / MP5 (Franchise Staple - High RPM Burst Mobility)
        database_["smg_qq9"] = {
            "smg_qq9", "QQ9 (MP5)", "CoD Mobile / Modern Warfare",
            833, 30, 2.0f, 190.0f, 2.4f,
            {28.0f, 22.0f, 17.0f, 11.0f, 20.0f, 1.4f},
            {1.3f, 0.4f}
        };

        // 2. FENNEC / KRISS VECTOR (Highest RPM SMG in Franchise)
        database_["smg_fennec"] = {
            "smg_fennec", "Fennec (Vector)", "MW 2019 / CoD Mobile",
            1111, 33, 1.7f, 175.0f, 2.0f,
            {23.0f, 17.0f, 13.0f, 8.0f, 16.0f, 1.3f},
            {2.1f, 0.9f} // High vertical kick
        };

        // 3. CBR4 / P90 (High Ammo Capacity & Laser Recoil Control)
        database_["smg_cbr4"] = {
            "smg_cbr4", "CBR4 (P90)", "CoD Mobile / Modern Warfare",
            800, 50, 2.5f, 210.0f, 2.6f,
            {26.0f, 22.0f, 18.0f, 14.0f, 28.0f, 1.35f},
            {0.5f, 0.1f} // Laser beam recoil
        };

        // 4. SWITCHBLADE X9 (High Strafing Speed & Close-Quarters Meta)
        database_["smg_switchblade"] = {
            "smg_switchblade", "Switchblade X9", "Black Ops 4 / CoD Mobile",
            909, 30, 1.8f, 180.0f, 2.1f,
            {29.0f, 22.0f, 16.0f, 10.0f, 22.0f, 1.4f},
            {1.1f, 0.3f}
        };

        // 5. VMP (Black Ops 3 - High Fire Rate & Control)
        database_["smg_vmp"] = {
            "smg_vmp", "VMP", "Black Ops 3 / CoD Mobile",
            900, 40, 2.1f, 185.0f, 2.3f,
            {27.0f, 22.0f, 17.0f, 13.0f, 25.0f, 1.4f},
            {1.2f, 0.5f}
        };

        // 6. PP19 BIZON (Helical Mag / Extremely High Ammo Capacity)
        database_["smg_bizon"] = {
            "smg_bizon", "PP19 Bizon", "MW 2019 / CoD Mobile",
            652, 64, 2.4f, 220.0f, 2.8f,
            {30.0f, 26.0f, 21.0f, 16.0f, 32.0f, 1.35f},
            {0.6f, 0.1f}
        };

        // 7. STRIKER 45 / UMP45 (High Range, Slow Fire Rate Puncher)
        database_["smg_striker45"] = {
            "smg_striker45", "Striker 45 (UMP)", "MW 2019 / MWIII",
            600, 25, 2.2f, 230.0f, 2.9f,
            {34.0f, 28.0f, 22.0f, 15.0f, 30.0f, 1.5f},
            {0.8f, 0.2f}
        };

        // 8. LC10 (Cold War / CoD Mobile)
        database_["smg_lc10"] = {
            "smg_lc10", "LC10", "Black Ops Cold War",
            800, 34, 2.0f, 200.0f, 2.5f,
            {27.0f, 23.0f, 18.0f, 15.0f, 28.0f, 1.4f},
            {0.6f, 0.2f}
        };
    }

    float CalculateHitDamage(const std::string& smgId, float distanceMeters, bool isHeadshot) const {
        auto it = database_.find(smgId);
        if (it == database_.end()) return 0.0f;

        const DamageRangeProfile& dmg = it->second.damage;
        float baseDmg = 0.0f;

        if (distanceMeters <= dmg.closeRangeLimitM) {
            baseDmg = dmg.maxCloseDamage;
        } else if (distanceMeters <= dmg.midRangeLimitM) {
            baseDmg = dmg.midRangeDamage;
        } else {
            baseDmg = dmg.minFarDamage;
        }

        return isHeadshot ? (baseDmg * dmg.headshotMultiplier) : baseDmg;
    }

    const SMGSpec* GetSMG(const std::string& smgId) const {
        auto it = database_.find(smgId);
        return (it != database_.end()) ? &it->second : nullptr;
    }
};

int main() {
    ExtendedSMGRegistry registry;

    std::vector<std::string> smgList = {
        "smg_qq9", "smg_fennec", "smg_cbr4", 
        "smg_switchblade", "smg_vmp", "smg_bizon", "smg_striker45", "smg_lc10"
    };

    float testRangeMeters = 10.0f; 

    std::cout << "=================== EXTENDED SMG METRICS (@ 10 METERS) ===================" << std::endl;

    for (const auto& smgId : smgList) {
        const SMGSpec* spec = registry.GetSMG(smgId);
        if (!spec) continue;

        float bodyDamage = registry.CalculateHitDamage(smgId, testRangeMeters, false);
        float shotsToKill = std::ceil(100.0f / bodyDamage);
        float timeBetweenShotsMs = (60.0f / static_cast<float>(spec->fireRateRPM)) * 1000.0f;
        float ttkMs = (shotsToKill - 1) * timeBetweenShotsMs;

        std::cout << "Weapon: " << spec->displayName << std::endl;
        std::cout << "  - Origin: " << spec->gameOrigin << std::endl;
        std::cout << "  - Fire Rate: " << spec->fireRateRPM << " RPM" << std::endl;
        std::cout << "  - Mag Size: " << spec->defaultMagSize << " rounds" << std::endl;
        std::cout << "  - Body Damage: " << bodyDamage << " HP" << std::endl;
        std::cout << "  - Close TTK: " << static_cast<int>(ttkMs) << " ms (" << shotsToKill << " shots)" << std::endl;
        std::cout << "  --------------------------------------------------" << std::endl;
    }

    return 0;
}

enum class WeaponClass {
    SNIPER_RIFLE,
    LIGHT_MACHINE_GUN,
    SHOTGUN,
    MARKSMAN_RIFLE
};

struct HitboxMultipliers {
    float head = 1.5f;
    float upperTorso = 1.2f;
    float lowerTorso = 1.0f;
    float limbs = 0.9f;
};

struct MasterWeaponSpec {
    std::string id;
    std::string name;
    WeaponClass category;
    uint32_t fireRateRPM;
    uint32_t magazineCapacity;
    float baseDamage;
    float adsSpeedMs;
    float pelletCount; // > 1 for Shotguns
    HitboxMultipliers multipliers;
};

class FullArsenalRegistry {
private:
    std::unordered_map<std::string, MasterWeaponSpec> registry_;

public:
    FullArsenalRegistry() {
        InitializeRemainingWeapons();
    }

    void InitializeRemainingWeapons() {
        // ====================================================================
        // 1. SNIPER RIFLES (High damage, one-shot torso potential, slow ADS)
        // ====================================================================
        
        // DL Q33 (CheyTac M200 - CoD Mobile flagship sniper)
        registry_["sniper_dlq33"] = {
            "sniper_dlq33", "DL Q33", WeaponClass::SNIPER_RIFLE,
            50, 6, 150.0f, 480.0f, 1,
            {2.0f, 1.2f, 1.0f, 0.9f} // 1-shot kill to chest and head
        };

        // Locus (Black Ops 3 - Fast ADS Bolt Action)
        registry_["sniper_locus"] = {
            "sniper_locus", "Locus", WeaponClass::SNIPER_RIFLE,
            57, 8, 142.0f, 440.0f, 1,
            {2.0f, 1.25f, 0.95f, 0.85f}
        };

        // HDR (Modern Warfare / Warzone - Extreme bullet velocity)
        registry_["sniper_hdr"] = {
            "sniper_hdr", "HDR", WeaponClass::SNIPER_RIFLE,
            45, 5, 175.0f, 520.0f, 1,
            {2.2f, 1.3f, 1.1f, 1.0f}
        };

        // Kilo Bolt-Action (Kar98k - Quick scoping favorite)
        registry_["marksman_kilo"] = {
            "marksman_kilo", "Kilo Bolt-Action (Kar98k)", WeaponClass::MARKSMAN_RIFLE,
            62, 5, 130.0f, 370.0f, 1,
            {2.0f, 1.2f, 0.9f, 0.8f}
        };

        // ====================================================================
        // 2. LIGHT MACHINE GUNS (LMGs - Massive Mags, High Wall Penetration)
        // ====================================================================

        // Chopper (KAC ChainSAW - High Hipfire Spraying LMG)
        registry_["lmg_chopper"] = {
            "lmg_chopper", "Chopper", WeaponClass::LIGHT_MACHINE_GUN,
            750, 100, 31.0f, 450.0f, 1,
            {1.4f, 1.0f, 1.0f, 0.9f}
        };

        // Holger 26 (MW / CoD Mobile - Hybrid AR/LMG)
        registry_["lmg_holger26"] = {
            "lmg_holger26", "Holger 26", WeaponClass::LIGHT_MACHINE_GUN,
            714, 100, 31.0f, 390.0f, 1,
            {1.4f, 1.0f, 1.0f, 0.9f}
        };

        // RPD (Classic Belt-fed LMG)
        registry_["lmg_rpd"] = {
            "lmg_rpd", "RPD", WeaponClass::LIGHT_MACHINE_GUN,
            666, 100, 34.0f, 420.0f, 1,
            {1.4f, 1.0f, 1.0f, 0.9f}
        };

        // ====================================================================
        // 3. SHOTGUNS (Multi-pellet spread, point-blank lethal)
        // ====================================================================

        // KRM-262 (CoD Mobile / BO3 - High-range pump shotgun)
        registry_["shotgun_krm262"] = {
            "shotgun_krm262", "KRM-262", WeaponClass::SHOTGUN,
            58, 8, 28.0f, 300.0f, 12, // 12 pellets * 28 damage = 336 max damage close
            {1.2f, 1.0f, 1.0f, 0.8f}
        };

        // BY15 (Winchester 1200 - Tight pellet spread)
        registry_["shotgun_by15"] = {
            "shotgun_by15", "BY15", WeaponClass::SHOTGUN,
            70, 8, 25.0f, 280.0f, 12,
            {1.2f, 1.0f, 1.0f, 0.8f}
        };

        // R9-0 (Modern Warfare - Double Tap Pump Shotgun)
        registry_["shotgun_r90"] = {
            "shotgun_r90", "R9-0", WeaponClass::SHOTGUN,
            240, 14, 22.0f, 290.0f, 8,
            {1.2f, 1.0f, 1.0f, 0.8f}
        };

        // ====================================================================
        // 4. MARKSMAN RIFLES (Semi-Auto Precision)
        // ====================================================================

        // SKS (Semi-Auto 2-Tap Kill)
        registry_["marksman_sks"] = {
            "marksman_sks", "SKS", WeaponClass::MARKSMAN_RIFLE,
            333, 20, 60.0f, 290.0f, 1,
            {1.75f, 1.0f, 1.0f, 0.9f} // 1-shot headshot or 2-shot body
        };
    }

    // Process a hit scenario considering body region multipliers
    float CalculateCalculatedHit(const std::string& weaponId, const std::string& hitRegion) const {
        auto it = registry_.find(weaponId);
        if (it == registry_.end()) return 0.0f;

        const MasterWeaponSpec& w = it->second;
        float mult = w.multipliers.lowerTorso;

        if (hitRegion == "head") mult = w.multipliers.head;
        else if (hitRegion == "upper_torso") mult = w.multipliers.upperTorso;
        else if (hitRegion == "limb") mult = w.multipliers.limbs;

        // Total potential damage per shot/trigger pull
        return w.baseDamage * mult * w.pelletCount;
    }

    const MasterWeaponSpec* Get(const std::string& id) const {
        auto it = registry_.find(id);
        return (it != registry_.end()) ? &it->second : nullptr;
    }
};

// ============================================================================
// MAIN SYSTEM DEMO
// ============================================================================

int main() {
    FullArsenalRegistry system;

    std::vector<std::string> testIds = {
        "sniper_dlq33", "lmg_chopper", "shotgun_krm262", "marksman_sks"
    };

    std::cout << "================ FULL ARSENAL MISC WEAPON SIMULATION ================\n" << std::endl;

    for (const auto& id : testIds) {
        const auto* spec = system.Get(id);
        if (!spec) continue;

        float upperChestDamage = system.CalculateCalculatedHit(id, "upper_torso");
        float headshotDamage = system.CalculateCalculatedHit(id, "head");

        std::cout << "Weapon: " << spec->name << std::endl;
        std::cout << "  - ADS Speed: " << spec->adsSpeedMs << " ms" << std::endl;
        std::cout << "  - Mag Capacity: " << spec->magazineCapacity << " rounds" << std::endl;
        std::cout << "  - Upper Chest Damage: " << upperChestDamage << " HP (" 
                  << (upperChestDamage >= 100.0f ? "1-SHOT KILL" : "Multi-shot required") << ")" << std::endl;
        std::cout << "  - Headshot Damage: " << headshotDamage << " HP" << std::endl;
        std::cout << "  --------------------------------------------------" << std::endl;
    }

    return 0;
}

enum class ReloadState {
    READY_TO_FIRE,      // Weapon is operational and ready to shoot
    MAG_OUT,            // Ejecting depleted magazine
    MAG_IN,             // Inserting fresh magazine
    CHAMBERING_ROUND,   // Pulling bolt/charging handle (tactical reload vs empty)
    CANCELLED           // Reload interrupted by sprint, aim, or weapon switch
};

enum class ReloadType {
    TACTICAL,  // Mag had remaining rounds -> +1 round left in chamber
    EMPTY      // Mag was empty -> chamber is empty, must rack charging handle
};

struct WeaponAmmoState {
    uint32_t currentMagAmmo = 0;
    uint32_t maxMagCapacity = 30;
    uint32_t reserveAmmo = 90;
    bool hasChamberedRound = false; // "1 in the chamber" mechanic
};

// ============================================================================
// 2. NETWORK RELOAD STATE MACHINE
// ============================================================================

class ServerWeaponFSM {
private:
    uint32_t ownerPlayerId_;
    std::string weaponName_;
    WeaponAmmoState ammo_;
    
    ReloadState currentState_ = ReloadState::READY_TO_FIRE;
    ReloadType currentReloadType_ = ReloadType::TACTICAL;

    // Timers (in seconds)
    float stateTimer_ = 0.0f;
    float magOutDuration_ = 0.8f;
    float magInDuration_ = 0.7f;
    float chamberDuration_ = 0.6f;

public:
    ServerWeaponFSM(uint32_t playerId, const std::string& name, WeaponAmmoState initialAmmo)
        : ownerPlayerId_(playerId), weaponName_(name), ammo_(initialAmmo) {}

    // --- USER INPUT INTERRUPTIONS ---
    void TriggerReload() {
        if (currentState_ != ReloadState::READY_TO_FIRE) return;
        if (ammo_.reserveAmmo == 0) {
            std::cout << "[SERVER] Cannot reload: Reserve ammo depleted!" << std::endl;
            return;
        }

        uint32_t maxAllowed = ammo_.hasChamberedRound ? ammo_.maxMagCapacity + 1 : ammo_.maxMagCapacity;
        if (ammo_.currentMagAmmo >= maxAllowed) {
            std::cout << "[SERVER] Magazine already full." << std::endl;
            return;
        }

        // Determine if this is a Tactical Reload (+1 in chamber) or Empty Reload
        if (ammo_.currentMagAmmo > 0) {
            currentReloadType_ = ReloadType::TACTICAL;
            ammo_.hasChamberedRound = true; // Retain current round in chamber
            std::cout << "\n[RELOAD INITIATED] Type: TACTICAL (+1 Chambered)" << std::endl;
        } else {
            currentReloadType_ = ReloadType::EMPTY;
            ammo_.hasChamberedRound = false;
            std::cout << "\n[RELOAD INITIATED] Type: EMPTY MAG (Must chamber round)" << std::endl;
        }

        currentState_ = ReloadState::MAG_OUT;
        stateTimer_ = 0.0f;
        BroadcastStateToClients();
    }

    // Interrupted by Sprint or Aim Down Sights (ADS)
    void InterruptReload(const std::string& reason) {
        if (currentState_ == ReloadState::READY_TO_FIRE) return;

        std::cout << "[CANCEL] Reload interrupted by " << reason << "! Reverting state." << std::endl;
        currentState_ = ReloadState::READY_TO_FIRE;
        stateTimer_ = 0.0f;
        
        // Mag was not inserted yet -> Ammo state is untouched
        BroadcastStateToClients();
    }

    // --- SERVER TICK LOOP (Called every frame/tick) ---
    void Tick(float deltaTime) {
        if (currentState_ == ReloadState::READY_TO_FIRE) return;

        stateTimer_ += deltaTime;

        switch (currentState_) {
            case ReloadState::MAG_OUT:
                if (stateTimer_ >= magOutDuration_) {
                    currentState_ = ReloadState::MAG_IN;
                    stateTimer_ = 0.0f;
                    std::cout << "  -> Stage 1 Complete: Ejected Old Magazine." << std::endl;
                    BroadcastStateToClients();
                }
                break;

            case ReloadState::MAG_IN:
                if (stateTimer_ >= magInDuration_) {
                    // Commit ammo transfer server-side
                    CommitAmmoTransfer();

                    if (currentReloadType_ == ReloadType::EMPTY) {
                        // Needs extra step to chamber the first round
                        currentState_ = ReloadState::CHAMBERING_ROUND;
                        stateTimer_ = 0.0f;
                        std::cout << "  -> Stage 2 Complete: Fresh Mag Inserted. Rack Charging Handle..." << std::endl;
                    } else {
                        // Tactical reload completes immediately
                        currentState_ = ReloadState::READY_TO_FIRE;
                        stateTimer_ = 0.0f;
                        std::cout << "  -> Stage 2 Complete: Tactical Reload Complete!" << std::endl;
                    }
                    BroadcastStateToClients();
                }
                break;

            case ReloadState::CHAMBERING_ROUND:
                if (stateTimer_ >= chamberDuration_) {
                    ammo_.hasChamberedRound = true;
                    currentState_ = ReloadState::READY_TO_FIRE;
                    stateTimer_ = 0.0f;
                    std::cout << "  -> Stage 3 Complete: Round Chambered! Weapon Ready to Fire." << std::endl;
                    BroadcastStateToClients();
                }
                break;

            default:
                break;
        }
    }

private:
    // --- AUTHORITATIVE AMMO CALCULATION ---
    void CommitAmmoTransfer() {
        uint32_t targetCapacity = ammo_.maxMagCapacity;

        // "1 in the chamber" mechanic allows 30 + 1 rounds if tactical reloading
        if (currentReloadType_ == ReloadType::TACTICAL && ammo_.hasChamberedRound) {
            targetCapacity += 1;
        }

        uint32_t neededAmmo = targetCapacity - ammo_.currentMagAmmo;
        uint32_t transferredAmmo = std::min(neededAmmo, ammo_.reserveAmmo);

        ammo_.currentMagAmmo += transferredAmmo;
        ammo_.reserveAmmo -= transferredAmmo;

        std::cout << "  [AMMO SYNC] Mag Ammo: " << ammo_.currentMagAmmo 
                  << " | Reserve Left: " << ammo_.reserveAmmo << std::endl;
    }

    void BroadcastStateToClients() {
        // Sends network RPC packet to sync player animation and HUD
    }

public:
    void PrintStatus() const {
        std::cout << "\n--- WEAPON STATUS ---" << std::endl;
        std::cout << "Weapon: " << weaponName_ << std::endl;
        std::cout << "Mag: " << ammo_.currentMagAmmo << " / " << ammo_.maxMagCapacity 
                  << (ammo_.hasChamberedRound ? " (+1 Chambered)" : "") << std::endl;
        std::cout << "Reserves: " << ammo_.reserveAmmo << std::endl;
        std::cout << "---------------------\n" << std::endl;
    }
};

// ============================================================================
// 3. MAIN SIMULATION DEMO
// ============================================================================

int main() {
    // Player starts with empty mag (0 rounds) and 90 reserve rounds
    ServerWeaponFSM weapon(101, "M4 Assault Rifle", {0, 30, 90, false});

    weapon.PrintStatus();

    // 1. Trigger Empty Reload (Full sequence: Mag Out -> Mag In -> Chamber Round)
    weapon.TriggerReload();

    // Simulate Server Ticks over 2.5 seconds (0.1s delta per step)
    for (int i = 0; i < 25; ++i) {
        weapon.Tick(0.1f);
    }

    weapon.PrintStatus();

    // Fire 5 rounds
    std::cout << "Player fires 5 rounds..." << std::endl;
    ServerWeaponFSM tacticalWeapon(101, "M4 Assault Rifle", {25, 30, 60, true});

    // 2. Trigger Tactical Reload (Mag Out -> Mag In -> Ready, +1 in chamber)
    tacticalWeapon.TriggerReload();

    for (int i = 0; i < 18; ++i) {
        tacticalWeapon.Tick(0.1f);
    }

    tacticalWeapon.PrintStatus();

    return 0;
}

enum class SurfaceMaterial {
    WOOD,
    DRYWALL,
    CONCRETE,
    STEEL_PLATE
};

struct MaterialProperties {
    float densityFactor;       // Resistance coefficient
    float maxPenetrationDepth; // Max meters bullet can travel through
};

class BallisticsPenetrationEngine {
public:
    static MaterialProperties GetMaterialProps(SurfaceMaterial mat) {
        switch (mat) {
            case SurfaceMaterial::DRYWALL:     return { 0.2f, 1.2f };
            case SurfaceMaterial::WOOD:        return { 0.4f, 0.8f };
            case SurfaceMaterial::CONCRETE:    return { 0.8f, 0.35f };
            case SurfaceMaterial::STEEL_PLATE: return { 1.5f, 0.15f };
        }
        return { 1.0f, 0.5f };
    }

    // Calculates damage reduction after passing through cover
    static float CalculateWallbangDamage(float incomingDamage, 
                                        float bulletClassPower, // LMG/Sniper = 1.0, SMG = 0.3
                                        SurfaceMaterial material, 
                                        float wallThicknessMeters) {
        MaterialProperties props = GetMaterialProps(material);

        // Check if bullet can physically breach the barrier
        float maxBreach = props.maxPenetrationDepth * bulletClassPower;
        if (wallThicknessMeters > maxBreach) {
            std::cout << "  [PENETRATION] Bullet stopped inside surface!" << std::endl;
            return 0.0f; // Stopped by barrier
        }

        // Damage dropoff percentage based on density and thickness
        float lossMultiplier = (wallThicknessMeters / maxBreach) * props.densityFactor;
        float remainingDamage = incomingDamage * (1.0f - std::min(lossMultiplier, 0.95f));

        std::cout << "  [WALLBANG] Penetrated " << wallThicknessMeters << "m of material. "
                  << "Damage reduced from " << incomingDamage << " to " << remainingDamage << std::endl;

        return remainingDamage;
    }
};

struct Vector2D {
    float x;
    float y;
};

class ProceduralSwayEngine {
private:
    float swayTime_ = 0.0f;

public:
    // Generates periodic Lissajous curve for breathing sway
    Vector2D CalculateIdleSway(float deltaTime, float swayIntensity, bool isHoldingBreath) {
        swayTime_ += deltaTime;

        if (isHoldingBreath) {
            swayIntensity *= 0.05f; // Reduces sway by 95%
        }

        // Oscillate horizontal (X) and vertical (Y) camera offset
        float offsetX = std::sin(swayTime_ * 1.5f) * 0.2f * swayIntensity;
        float offsetY = std::cos(swayTime_ * 3.0f) * 0.3f * swayIntensity;

        return { offsetX, offsetY };
    }
};

// ============================================================================
// MAIN SIMULATION DEMO
// ============================================================================

int main() {
    std::cout << "=================== WEAPON LOGIC DEMO ===================\n" << std::endl;

    // 1. Test Gunsmith Loadout Encoder
    GunsmithEncoder encoder;
    std::vector<uint32_t> myAttachments = { 12, 45, 102, 14, 88 }; // 5 attachment IDs
    std::string shareCode = encoder.GenerateShareCode("RPD", myAttachments);

    std::cout << "1. GUNSMITH SERIALIZATION:" << std::endl;
    std::cout << "  - Generated Share Code: " << shareCode << std::endl;

    auto decoded = encoder.DecodeShareCode(shareCode);
    std::cout << "  - Decoded Attachment IDs: ";
    for (auto id : decoded) std::cout << id << " ";
    std::cout << "\n\n";

    // 2. Test Bullet Wall Penetration
    std::cout << "2. BULLET PENETRATION (WALLBANG) TESTS:" << std::endl;
    // RPD / LMG through 0.2m Wood
    std::cout << "Test A: RPD firing through 0.2m Wood:" << std::endl;
    BallisticsPenetrationEngine::CalculateWallbangDamage(34.0f, 1.0f, SurfaceMaterial::WOOD, 0.2f);

    // SMG through 0.3m Concrete
    std::cout << "Test B: MAC-10 firing through 0.3m Concrete:" << std::endl;
    BallisticsPenetrationEngine::CalculateWallbangDamage(24.0f, 0.3f, SurfaceMaterial::CONCRETE, 0.3f);

    // 3. Test Procedural Idle Sway
    std::cout << "\n3. SNIPER IDLE SWAY SIMULATION:" << std::endl;
    ProceduralSwayEngine swayEngine;
    Vector2D normalSway = swayEngine.CalculateIdleSway(1.2f, 1.5f, false);
    Vector2D breathHoldSway = swayEngine.CalculateIdleSway(1.2f, 1.5f, true);

    std::cout << "  - Standard ADS Sway Offset: X=" << normalSway.x << ", Y=" << normalSway.y << std::endl;
    std::cout << "  - Hold Breath Sway Offset: X=" << breathHoldSway.x << ", Y=" << breathHoldSway.y << std::endl;

    return 0;
}

enum class SkinRarity { Common, Rare, Epic, Legendary, Mythic };

// Operator Skin metadata
struct OperatorSkin {
    uint32_t skinID;
    std::string skinName;
    std::string operatorBaseName; // e.g., "Charly", "Ghost", "Outrider"
    SkinRarity rarity;
    std::string modelMeshPath;
    std::string texturePath;
};

// Player Cosmetic Inventory
class PlayerCosmeticInventory {
private:
    uint64_t playerUID;
    std::unordered_map<uint32_t, OperatorSkin> unlockedSkins;
    uint32_t equippedOperatorSkinID = 0; // Default base skin ID

public:
    explicit PlayerCosmeticInventory(uint64_t uid) : playerUID(uid) {}

    bool UnlockSkin(const OperatorSkin& skin) {
        if (unlockedSkins.find(skin.skinID) != unlockedSkins.end()) {
            std::cout << "[Inventory] Player " << playerUID << " already owns " << skin.skinName << "!\n";
            return false;
        }
        unlockedSkins[skin.skinID] = skin;
        std::cout << "[Inventory] Successfully unlocked skin: " << skin.skinName 
                  << " for Operator (" << skin.operatorBaseName << ")\n";
        return true;
    }

    bool EquipSkin(uint32_t skinID) {
        if (unlockedSkins.find(skinID) == unlockedSkins.end()) {
            std::cout << "[Inventory] Cannot equip: Skin ID " << skinID << " is locked!\n";
            return false;
        }
        equippedOperatorSkinID = skinID;
        std::cout << "[Inventory] Equipped Skin: " << unlockedSkins[skinID].skinName << "\n";
        return true;
    }

    void DisplayOwnedSkins() const {
        std::cout << "\n--- Owned Operator Skins for UID " << playerUID << " ---\n";
        for (const auto& [id, skin] : unlockedSkins) {
            std::cout << " ID: " << skin.skinID << " | " << skin.operatorBaseName 
                      << " - " << skin.skinName;
            if (id == equippedOperatorSkinID) std::cout << " [EQUIPPED]";
            std::cout << "\n";
        }
    }
};

// Skin Redemption Engine
class RedemptionCodeEngine {
private:
    std::unordered_map<std::string, OperatorSkin> codeDatabase;

public:
    void RegisterCode(const std::string& code, const OperatorSkin& skin) {
        codeDatabase[code] = skin;
    }

    bool RedeemCode(const std::string& code, PlayerCosmeticInventory& inventory) {
        auto it = codeDatabase.find(code);
        if (it == codeDatabase.end()) {
            std::cout << "[Redeem Engine] Error: Code '" << code << "' is invalid or expired.\n";
            return false;
        }
        std::cout << "[Redeem Engine] Valid Code Processed!\n";
        return inventory.UnlockSkin(it->second);
    }
};

int main() {
    // 1. Initialize Skins
    OperatorSkin charlyAngel = {1001, "Angel of Death", "Charly", SkinRarity::Epic, "models/charly_angel.fbx", "tex/charly_angel.png"};
    OperatorSkin parkSpirit = {1002, "Spirit", "Helen Park", SkinRarity::Rare, "models/park_spirit.fbx", "tex/park_spirit.png"};
    OperatorSkin outriderDark = {1003, "Dark Strobe", "Outrider", SkinRarity::Epic, "models/outrider_dark.fbx", "tex/outrider_dark.png"};

    // 2. Setup Redemption Engine
    RedemptionCodeEngine engine;
    engine.RegisterCode("CHARLY-DEATH-2026", charlyAngel);
    engine.RegisterCode("CULCZBZ9BX", parkSpirit);
    engine.RegisterCode("DAMGZBZMWW", outriderDark);

    // 3. Simulate Player Profile
    PlayerCosmeticInventory player1(987654321012345678ULL);

    // 4. Redeem Codes
    engine.RedeemCode("CHARLY-DEATH-2026", player1);
    engine.RedeemCode("CULCZBZ9BX", player1);
    engine.RedeemCode("INVALIDCODE123", player1); // Test invalid code

    // 5. Equip Skin & View Inventory
    player1.EquipSkin(1001);
    player1.DisplayOwnedSkins();

    return 0;
}

struct Matrix4x4 {
    float m[4][4] = {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    };

    static Matrix4x4 Identity() { return Matrix4x4(); }
};

struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

// ============================================================================
// 1. DYNAMIC MATERIAL & TEXTURE SWAPPING SYSTEM
// ============================================================================

struct MaterialTextures {
    std::string albedoMapPath;   // Diffuse / Base Color
    std::string normalMapPath;   // Surface Bump / Detail
    std::string metallicMapPath; // Metallic / Roughness PBR map
    uint32_t gpuTextureID_Albedo = 0;
};

class Material {
private:
    std::string materialName_;
    MaterialTextures activeTextures_;

public:
    explicit Material(const std::string& name) : materialName_(name) {}

    // Swaps skin textures dynamically on the fly
    void SwapTextures(const MaterialTextures& newTextures) {
        activeTextures_ = newTextures;
        // In OpenGL/DirectX, this would bind new GPU texture IDs to sampler uniforms
        std::cout << "[Material System] Texture Swap Applied to '" << materialName_ << "':\n"
                  << "   - Albedo Map:   " << activeTextures_.albedoMapPath << "\n"
                  << "   - Normal Map:   " << activeTextures_.normalMapPath << "\n"
                  << "   - Metallic Map: " << activeTextures_.metallicMapPath << "\n";
    }

    const MaterialTextures& GetTextures() const { return activeTextures_; }
};

// ============================================================================
// 2. SKELETAL RIGGING & ANIMATION ENGINE (Linear Blend Skinning)
// ============================================================================

constexpr int MAX_BONE_INFLUENCES = 4;
constexpr int MAX_BONES = 128;

// A single vertex on the operator mesh with bone weights
struct SkinnedVertex {
    Vector3 position;
    Vector3 normal;
    float uv[2];
    int boneIDs[MAX_BONE_INFLUENCES] = {0, 0, 0, 0};
    float weights[MAX_BONE_INFLUENCES] = {1.0f, 0.0f, 0.0f, 0.0f}; // Must sum to 1.0
};

// Individual Joint / Bone in the skeleton hierarchy
struct Joint {
    int id;
    std::string name;
    int parentID; // -1 if root joint
    Matrix4x4 localTransform;
    Matrix4x4 inverseBindPose; // Offset matrix from mesh bind pose
};

class SkeletonRig {
private:
    std::vector<Joint> joints_;
    std::vector<Matrix4x4> finalBoneMatrices_;

public:
    SkeletonRig() {
        finalBoneMatrices_.resize(MAX_BONES, Matrix4x4::Identity());
    }

    void AddJoint(int id, const std::string& name, int parentID) {
        Joint j;
        j.id = id;
        j.name = name;
        j.parentID = parentID;
        j.localTransform = Matrix4x4::Identity();
        j.inverseBindPose = Matrix4x4::Identity();
        joints_.push_back(j);
    }

    // Calculates World-space transforms for all joints down the hierarchy
    void UpdateHierarchyTransforms() {
        for (size_t i = 0; i < joints_.size(); ++i) {
            if (joints_[i].parentID == -1) {
                // Root Joint (Pelvis)
                finalBoneMatrices_[i] = joints_[i].localTransform;
            } else {
                // Child Joint inherits parent's transformation matrix
                // finalMatrix = parentMatrix * localTransform * inverseBindPose
                int parentIdx = joints_[i].parentID;
                // Simplified matrix multiply simulation:
                finalBoneMatrices_[i] = joints_[i].localTransform; 
            }
        }
    }

    const std::vector<Matrix4x4>& GetFinalBoneMatrices() const {
        return finalBoneMatrices_;
    }

    size_t GetJointCount() const { return joints_.size(); }
};

// ============================================================================
// 3. SKINNED MESH & RENDER COMPONENT
// ============================================================================

class SkinnedMeshComponent {
private:
    std::string meshName_;
    std::vector<SkinnedVertex> vertices_;
    std::shared_ptr<Material> material_;
    SkeletonRig skeleton_;

public:
    SkinnedMeshComponent(const std::string& name, std::shared_ptr<Material> mat)
        : meshName_(name), material_(mat) {}

    void SetSkeleton(const SkeletonRig& rig) {
        skeleton_ = rig;
    }

    void AddVertex(const SkinnedVertex& v) {
        vertices_.push_back(v);
    }

    // Compute CPU/GPU Linear Blend Skinning for a vertex
    Vector3 CalculateSkinnedVertexPosition(const SkinnedVertex& vertex) {
        Vector3 skinnedPos = {0.0f, 0.0f, 0.0f};
        const auto& boneMatrices = skeleton_.GetFinalBoneMatrices();

        for (int i = 0; i < MAX_BONE_INFLUENCES; ++i) {
            float weight = vertex.weights[i];
            if (weight > 0.0f) {
                int boneID = vertex.boneIDs[i];
                // In actual HLSL/GLSL vertex shader:
                // skinnedPos += (BoneMatrix[boneID] * vec4(pos, 1.0)) * weight;
                skinnedPos.x += vertex.position.x * weight;
                skinnedPos.y += vertex.position.y * weight;
                skinnedPos.z += vertex.position.z * weight;
            }
        }
        return skinnedPos;
    }

    void UpdateAndRender(float deltaTime) {
        // 1. Update Skeleton Joints from Animation Frame
        skeleton_.UpdateHierarchyTransforms();

        // 2. Perform vertex skinning deformation
        std::cout << "[Render Pipeline] Deforming Skinned Mesh '" << meshName_ 
                  << "' across " << vertices_.size() << " vertices & " 
                  << skeleton_.GetJointCount() << " joints.\n";
    }

    std::shared_ptr<Material> GetMaterial() const { return material_; }
};

// ============================================================================
// MAIN SIMULATION DEMO
// ============================================================================

int main() {
    std::cout << "================ CHARACTER RIG & TEXTURE SWAP DEMO ================\n\n";

    // 1. Create Base Material for Operator (Charly)
    auto charlyMaterial = std::make_shared<Material>("mat_charly_body");
    
    // Initial Base Skin Textures
    MaterialTextures defaultSkin = {
        "textures/charly_base_albedo.png",
        "textures/charly_base_normal.png",
        "textures/charly_base_metallic.png",
        101
    };
    charlyMaterial->SwapTextures(defaultSkin);

    // 2. Build Skeleton Rig Hierarchy (Shared across all Charly skins)
    SkeletonRig charlyRig;
    charlyRig.AddJoint(0, "Root_Pelvis", -1);
    charlyRig.AddJoint(1, "Spine_01", 0);
    charlyRig.AddJoint(2, "Clavicle_L", 1);
    charlyRig.AddJoint(3, "Arm_Upper_L", 2);
    charlyRig.AddJoint(4, "Hand_L", 3);

    // 3. Construct Operator Skinned Mesh
    SkinnedMeshComponent charlyMesh("mesh_charly_v2", charlyMaterial);
    charlyMesh.SetSkeleton(charlyRig);

    // Add sample skinned vertex (Weighted 70% to Upper Arm, 30% to Clavicle)
    SkinnedVertex v1;
    v1.position = {0.5f, 1.2f, 0.0f};
    v1.boneIDs[0] = 3; v1.weights[0] = 0.7f; // Arm_Upper_L
    v1.boneIDs[1] = 2; v1.weights[1] = 0.3f; // Clavicle_L
    charlyMesh.AddVertex(v1);

    // Render Initial Frame
    charlyMesh.UpdateAndRender(0.016f);

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "EVENT: Player changes skin to 'Charly - Angel of Death'\n";
    std::cout << "------------------------------------------------------------\n";

    // 4. Perform Dynamic Texture Swap (No mesh reloading or re-rigging required)
    MaterialTextures angelOfDeathSkin = {
        "textures/charly_angel_of_death_albedo.png",
        "textures/charly_angel_of_death_normal.png",
        "textures/charly_angel_of_death_metallic.png",
        205
    };
    charlyMaterial->SwapTextures(angelOfDeathSkin);

    // Render Frame with new textures applied
    charlyMesh.UpdateAndRender(0.016f);

    return 0;
}

struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        };
    }
};

struct Quaternion {
    float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

    // Spherical Linear Interpolation (Slerp) for smooth rotation blending
    static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

        // Ensure shortest path
        Quaternion target = q2;
        if (dot < 0.0f) {
            dot = -dot;
            target = {-q2.x, -q2.y, -q2.z, -q2.w};
        }

        if (dot > 0.9995f) { // Linear interpolation if quaternions are almost identical
            return {
                q1.x + (target.x - q1.x) * t,
                q1.y + (target.y - q1.y) * t,
                q1.z + (target.z - q1.z) * t,
                q1.w + (target.w - q1.w) * t
            };
        }

        float theta_0 = std::acos(dot);
        float theta = theta_0 * t;
        float sin_theta = std::sin(theta);
        float sin_theta_0 = std::sin(theta_0);

        float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
        float s1 = sin_theta / sin_theta_0;

        return {
            (s0 * q1.x) + (s1 * target.x),
            (s0 * q1.y) + (s1 * target.y),
            (s0 * q1.z) + (s1 * target.z),
            (s0 * q1.w) + (s1 * target.w)
        };
    }
};

struct Transform {
    Vector3 translation;
    Quaternion rotation;
    Vector3 scale = {1.0f, 1.0f, 1.0f};

    // Blends two local joint transforms using Lerp + Slerp
    static Transform Blend(const Transform& a, const Transform& b, float factor) {
        Transform result;
        result.translation = Vector3::Lerp(a.translation, b.translation, factor);
        result.rotation = Quaternion::Slerp(a.rotation, b.rotation, factor);
        result.scale = Vector3::Lerp(a.scale, b.scale, factor);
        return result;
    }
};

// ============================================================================
// 2. ANIMATION CLIPS & KEYFRAME DATA
// ============================================================================

struct Keyframe {
    float timeStamp; // Seconds
    Transform localTransform;
};

struct BoneTrack {
    int jointID;
    std::vector<Keyframe> keyframes;

    // Evaluates local joint transform at time 't'
    Transform Sample(float timeSeconds) const {
        if (keyframes.empty()) return Transform();
        if (keyframes.size() == 1 || timeSeconds <= keyframes.front().timeStamp) 
            return keyframes.front().localTransform;
        if (timeSeconds >= keyframes.back().timeStamp) 
            return keyframes.back().localTransform;

        // Locate active keyframe interval
        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (timeSeconds >= keyframes[i].timeStamp && timeSeconds <= keyframes[i+1].timeStamp) {
                float segmentDuration = keyframes[i+1].timeStamp - keyframes[i].timeStamp;
                float progress = (timeSeconds - keyframes[i].timeStamp) / segmentDuration;
                
                return Transform::Blend(keyframes[i].localTransform, keyframes[i+1].localTransform, progress);
            }
        }
        return keyframes.back().localTransform;
    }
};

class AnimationClip {
public:
    std::string name;
    float durationSeconds;
    bool isLooping = true;
    std::unordered_map<int, BoneTrack> tracks; // JointID -> Track

    Pose Evaluate(float timeSeconds, size_t totalJoints) const {
        std::vector<Transform> poseTransforms(totalJoints);
        
        // Wrap time for looping animations
        float animTime = isLooping ? std::fmod(timeSeconds, durationSeconds) : std::min(timeSeconds, durationSeconds);

        for (size_t j = 0; j < totalJoints; ++j) {
            auto it = tracks.find(static_cast<int>(j));
            if (it != tracks.end()) {
                poseTransforms[j] = it->second.Sample(animTime);
            } else {
                poseTransforms[j] = Transform(); // Identity default
            }
        }
        return { poseTransforms };
    }
};

struct Pose {
    std::vector<Transform> jointTransforms; // Local transforms for each joint
};

// ============================================================================
// 3. ANIMATION STATE MACHINE (FSM) WITH CROSS-FADING
// ============================================================================

enum class AnimState {
    IDLE,
    RUNNING,
    RELOADING
};

class AnimationStateMachine {
private:
    std::unordered_map<AnimState, std::shared_ptr<AnimationClip>> animations_;
    
    AnimState currentState_ = AnimState::IDLE;
    AnimState targetState_ = AnimState::IDLE;
    
    float currentAnimTime_ = 0.0f;
    float targetAnimTime_ = 0.0f;
    
    bool isBlending_ = false;
    float blendDuration_ = 0.3f; // 300ms cross-fade duration
    float blendTimer_ = 0.0f;    // Current blend time [0.0 -> blendDuration_]

    size_t jointCount_ = 0;

public:
    explicit AnimationStateMachine(size_t jointCount) : jointCount_(jointCount) {}

    void RegisterAnimation(AnimState state, std::shared_ptr<AnimationClip> clip) {
        animations_[state] = clip;
    }

    void TransitionTo(AnimState newState, float crossFadeDuration = 0.3f) {
        if (newState == currentState_ && !isBlending_) return;

        targetState_ = newState;
        targetAnimTime_ = 0.0f;
        blendDuration_ = crossFadeDuration;
        blendTimer_ = 0.0f;
        isBlending_ = true;

        std::cout << "  [ASM Transition] Triggered blend to state (" 
                  << static_cast<int>(newState) << ") over " 
                  << crossFadeDuration << "s\n";
    }

    Pose Update(float deltaTime) {
        currentAnimTime_ += deltaTime;

        if (!isBlending_) {
            // Single State Evaluation
            auto clip = animations_[currentState_];
            return clip ? clip->Evaluate(currentAnimTime_, jointCount_) : Pose{ std::vector<Transform>(jointCount_) };
        }

        // --- DUAL-POSE BLENDING DURING TRANSITION ---
        targetAnimTime_ += deltaTime;
        blendTimer_ += deltaTime;

        float blendFactor = std::min(blendTimer_ / blendDuration_, 1.0f);

        Pose sourcePose = animations_[currentState_]->Evaluate(currentAnimTime_, jointCount_);
        Pose targetPose = animations_[targetState_]->Evaluate(targetAnimTime_, jointCount_);

        // Blend poses per joint
        Pose finalPose;
        finalPose.jointTransforms.resize(jointCount_);

        for (size_t j = 0; j < jointCount_; ++j) {
            finalPose.jointTransforms[j] = Transform::Blend(
                sourcePose.jointTransforms[j],
                targetPose.jointTransforms[j],
                blendFactor
            );
        }

        // Complete Blend
        if (blendFactor >= 1.0f) {
            std::cout << "  [ASM Transition] Cross-fade complete! Active state is now " 
                      << static_cast<int>(targetState_) << "\n";
            currentState_ = targetState_;
            currentAnimTime_ = targetAnimTime_;
            isBlending_ = false;
        }

        return finalPose;
    }

    AnimState GetCurrentState() const { return currentState_; }
};

// ============================================================================
// MAIN SIMULATION DEMO
// ============================================================================

int main() {
    constexpr size_t TOTAL_BONE_JOINTS = 5; // e.g., Pelvis, Spine, Shoulder, Elbow, Wrist
    AnimationStateMachine asmEngine(TOTAL_BONE_JOINTS);

    // 1. Construct Mock Running Animation
    auto runClip = std::make_shared<AnimationClip>();
    runClip->name = "Clip_Run_Forward";
    runClip->durationSeconds = 1.0f;
    
    // Joint 3 (Shoulder_R) swinging back and forth during run
    BoneTrack runShoulderTrack;
    runShoulderTrack.jointID = 3;
    runShoulderTrack.keyframes = {
        { 0.0f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.1f, 0.99f} } },
        { 0.5f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, -0.3f, 0.95f} } },
        { 1.0f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.1f, 0.99f} } }
    };
    runClip->tracks[3] = runShoulderTrack;

    // 2. Construct Mock Reloading Animation
    auto reloadClip = std::make_shared<AnimationClip>();
    reloadClip->name = "Clip_Reload_Tactical";
    reloadClip->durationSeconds = 1.8f;
    reloadClip->isLooping = false;

    // Joint 3 (Shoulder_R) bringing weapon to body during reload
    BoneTrack reloadShoulderTrack;
    reloadShoulderTrack.jointID = 3;
    reloadShoulderTrack.keyframes = {
        { 0.0f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } },
        { 0.9f, { {0.1f, 1.2f, 0.3f}, {0.2f, 0.1f, 0.5f, 0.83f} } },
        { 1.8f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }
    };
    reloadClip->tracks[3] = reloadShoulderTrack;

    // 3. Register Clips
    asmEngine.RegisterAnimation(AnimState::RUNNING, runClip);
    asmEngine.RegisterAnimation(AnimState::RELOADING, reloadClip);

    // Start in Running State
    asmEngine.TransitionTo(AnimState::RUNNING, 0.0f); // Instant transition

    std::cout << "================ ANIMATION BLENDING ENGINE SIMULATION ================\n\n";

    float frameDelta = 0.05f; // 20 FPS step for logging clarity

    // Simulate 0.5 seconds of Running
    std::cout << "1. Running Phase (0.0s - 0.5s):\n";
    for (int i = 0; i < 10; ++i) {
        Pose p = asmEngine.Update(frameDelta);
        std::cout << "   Frame " << i << " | Shoulder Pos Z: " 
                  << p.jointTransforms[3].translation.z 
                  << " | Rot W: " << p.jointTransforms[3].rotation.w << "\n";
    }

    // Trigger Reload (Cross-fade over 0.25s)
    std::cout << "\n2. Player presses RELOAD key -> Initiating Cross-Fade Transition:\n";
    asmEngine.TransitionTo(AnimState::RELOADING, 0.25f);

    // Simulate blending period and follow-up reload
    for (int i = 0; i < 10; ++i) {
        Pose p = asmEngine.Update(frameDelta);
        std::cout << "   Blend Frame " << i << " | Shoulder Pos Z: " 
                  << p.jointTransforms[3].translation.z 
                  << " | Rot W: " << p.jointTransforms[3].rotation.w << "\n";
    }

    return 0;
}

struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        };
    }
};

struct Quaternion {
    float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

    // Spherical Linear Interpolation (Slerp) for smooth rotation blending
    static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

        // Ensure shortest path
        Quaternion target = q2;
        if (dot < 0.0f) {
            dot = -dot;
            target = {-q2.x, -q2.y, -q2.z, -q2.w};
        }

        if (dot > 0.9995f) { // Linear interpolation if quaternions are almost identical
            return {
                q1.x + (target.x - q1.x) * t,
                q1.y + (target.y - q1.y) * t,
                q1.z + (target.z - q1.z) * t,
                q1.w + (target.w - q1.w) * t
            };
        }

        float theta_0 = std::acos(dot);
        float theta = theta_0 * t;
        float sin_theta = std::sin(theta);
        float sin_theta_0 = std::sin(theta_0);

        float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
        float s1 = sin_theta / sin_theta_0;

        return {
            (s0 * q1.x) + (s1 * target.x),
            (s0 * q1.y) + (s1 * target.y),
            (s0 * q1.z) + (s1 * target.z),
            (s0 * q1.w) + (s1 * target.w)
        };
    }
};

struct Transform {
    Vector3 translation;
    Quaternion rotation;
    Vector3 scale = {1.0f, 1.0f, 1.0f};

    // Blends two local joint transforms using Lerp + Slerp
    static Transform Blend(const Transform& a, const Transform& b, float factor) {
        Transform result;
        result.translation = Vector3::Lerp(a.translation, b.translation, factor);
        result.rotation = Quaternion::Slerp(a.rotation, b.rotation, factor);
        result.scale = Vector3::Lerp(a.scale, b.scale, factor);
        return result;
    }
};

// ============================================================================
// 2. ANIMATION CLIPS & KEYFRAME DATA
// ============================================================================

struct Keyframe {
    float timeStamp; // Seconds
    Transform localTransform;
};

struct BoneTrack {
    int jointID;
    std::vector<Keyframe> keyframes;

    // Evaluates local joint transform at time 't'
    Transform Sample(float timeSeconds) const {
        if (keyframes.empty()) return Transform();
        if (keyframes.size() == 1 || timeSeconds <= keyframes.front().timeStamp) 
            return keyframes.front().localTransform;
        if (timeSeconds >= keyframes.back().timeStamp) 
            return keyframes.back().localTransform;

        // Locate active keyframe interval
        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (timeSeconds >= keyframes[i].timeStamp && timeSeconds <= keyframes[i+1].timeStamp) {
                float segmentDuration = keyframes[i+1].timeStamp - keyframes[i].timeStamp;
                float progress = (timeSeconds - keyframes[i].timeStamp) / segmentDuration;
                
                return Transform::Blend(keyframes[i].localTransform, keyframes[i+1].localTransform, progress);
            }
        }
        return keyframes.back().localTransform;
    }
};

class AnimationClip {
public:
    std::string name;
    float durationSeconds;
    bool isLooping = true;
    std::unordered_map<int, BoneTrack> tracks; // JointID -> Track

    Pose Evaluate(float timeSeconds, size_t totalJoints) const {
        std::vector<Transform> poseTransforms(totalJoints);
        
        // Wrap time for looping animations
        float animTime = isLooping ? std::fmod(timeSeconds, durationSeconds) : std::min(timeSeconds, durationSeconds);

        for (size_t j = 0; j < totalJoints; ++j) {
            auto it = tracks.find(static_cast<int>(j));
            if (it != tracks.end()) {
                poseTransforms[j] = it->second.Sample(animTime);
            } else {
                poseTransforms[j] = Transform(); // Identity default
            }
        }
        return { poseTransforms };
    }
};

struct Pose {
    std::vector<Transform> jointTransforms; // Local transforms for each joint
};

// ============================================================================
// 3. ANIMATION STATE MACHINE (FSM) WITH CROSS-FADING
// ============================================================================

enum class AnimState {
    IDLE,
    RUNNING,
    RELOADING
};

class AnimationStateMachine {
private:
    std::unordered_map<AnimState, std::shared_ptr<AnimationClip>> animations_;
    
    AnimState currentState_ = AnimState::IDLE;
    AnimState targetState_ = AnimState::IDLE;
    
    float currentAnimTime_ = 0.0f;
    float targetAnimTime_ = 0.0f;
    
    bool isBlending_ = false;
    float blendDuration_ = 0.3f; // 300ms cross-fade duration
    float blendTimer_ = 0.0f;    // Current blend time [0.0 -> blendDuration_]

    size_t jointCount_ = 0;

public:
    explicit AnimationStateMachine(size_t jointCount) : jointCount_(jointCount) {}

    void RegisterAnimation(AnimState state, std::shared_ptr<AnimationClip> clip) {
        animations_[state] = clip;
    }

    void TransitionTo(AnimState newState, float crossFadeDuration = 0.3f) {
        if (newState == currentState_ && !isBlending_) return;

        targetState_ = newState;
        targetAnimTime_ = 0.0f;
        blendDuration_ = crossFadeDuration;
        blendTimer_ = 0.0f;
        isBlending_ = true;

        std::cout << "  [ASM Transition] Triggered blend to state (" 
                  << static_cast<int>(newState) << ") over " 
                  << crossFadeDuration << "s\n";
    }

    Pose Update(float deltaTime) {
        currentAnimTime_ += deltaTime;

        if (!isBlending_) {
            // Single State Evaluation
            auto clip = animations_[currentState_];
            return clip ? clip->Evaluate(currentAnimTime_, jointCount_) : Pose{ std::vector<Transform>(jointCount_) };
        }

        // --- DUAL-POSE BLENDING DURING TRANSITION ---
        targetAnimTime_ += deltaTime;
        blendTimer_ += deltaTime;

        float blendFactor = std::min(blendTimer_ / blendDuration_, 1.0f);

        Pose sourcePose = animations_[currentState_]->Evaluate(currentAnimTime_, jointCount_);
        Pose targetPose = animations_[targetState_]->Evaluate(targetAnimTime_, jointCount_);

        // Blend poses per joint
        Pose finalPose;
        finalPose.jointTransforms.resize(jointCount_);

        for (size_t j = 0; j < jointCount_; ++j) {
            finalPose.jointTransforms[j] = Transform::Blend(
                sourcePose.jointTransforms[j],
                targetPose.jointTransforms[j],
                blendFactor
            );
        }

        // Complete Blend
        if (blendFactor >= 1.0f) {
            std::cout << "  [ASM Transition] Cross-fade complete! Active state is now " 
                      << static_cast<int>(targetState_) << "\n";
            currentState_ = targetState_;
            currentAnimTime_ = targetAnimTime_;
            isBlending_ = false;
        }

        return finalPose;
    }

    AnimState GetCurrentState() const { return currentState_; }
};

// ============================================================================
// MAIN SIMULATION DEMO
// ============================================================================

int main() {
    constexpr size_t TOTAL_BONE_JOINTS = 5; // e.g., Pelvis, Spine, Shoulder, Elbow, Wrist
    AnimationStateMachine asmEngine(TOTAL_BONE_JOINTS);

    // 1. Construct Mock Running Animation
    auto runClip = std::make_shared<AnimationClip>();
    runClip->name = "Clip_Run_Forward";
    runClip->durationSeconds = 1.0f;
    
    // Joint 3 (Shoulder_R) swinging back and forth during run
    BoneTrack runShoulderTrack;
    runShoulderTrack.jointID = 3;
    runShoulderTrack.keyframes = {
        { 0.0f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.1f, 0.99f} } },
        { 0.5f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, -0.3f, 0.95f} } },
        { 1.0f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.1f, 0.99f} } }
    };
    runClip->tracks[3] = runShoulderTrack;

    // 2. Construct Mock Reloading Animation
    auto reloadClip = std::make_shared<AnimationClip>();
    reloadClip->name = "Clip_Reload_Tactical";
    reloadClip->durationSeconds = 1.8f;
    reloadClip->isLooping = false;

    // Joint 3 (Shoulder_R) bringing weapon to body during reload
    BoneTrack reloadShoulderTrack;
    reloadShoulderTrack.jointID = 3;
    reloadShoulderTrack.keyframes = {
        { 0.0f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } },
        { 0.9f, { {0.1f, 1.2f, 0.3f}, {0.2f, 0.1f, 0.5f, 0.83f} } },
        { 1.8f, { {0.2f, 1.4f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }
    };
    reloadClip->tracks[3] = reloadShoulderTrack;

    // 3. Register Clips
    asmEngine.RegisterAnimation(AnimState::RUNNING, runClip);
    asmEngine.RegisterAnimation(AnimState::RELOADING, reloadClip);

    // Start in Running State
    asmEngine.TransitionTo(AnimState::RUNNING, 0.0f); // Instant transition

    std::cout << "================ ANIMATION BLENDING ENGINE SIMULATION ================\n\n";

    float frameDelta = 0.05f; // 20 FPS step for logging clarity

    // Simulate 0.5 seconds of Running
    std::cout << "1. Running Phase (0.0s - 0.5s):\n";
    for (int i = 0; i < 10; ++i) {
        Pose p = asmEngine.Update(frameDelta);
        std::cout << "   Frame " << i << " | Shoulder Pos Z: " 
                  << p.jointTransforms[3].translation.z 
                  << " | Rot W: " << p.jointTransforms[3].rotation.w << "\n";
    }

    // Trigger Reload (Cross-fade over 0.25s)
    std::cout << "\n2. Player presses RELOAD key -> Initiating Cross-Fade Transition:\n";
    asmEngine.TransitionTo(AnimState::RELOADING, 0.25f);

    // Simulate blending period and follow-up reload
    for (int i = 0; i < 10; ++i) {
        Pose p = asmEngine.Update(frameDelta);
        std::cout << "   Blend Frame " << i << " | Shoulder Pos Z: " 
                  << p.jointTransforms[3].translation.z 
                  << " | Rot W: " << p.jointTransforms[3].rotation.w << "\n";
    }

    return 0;
}

struct SurroundingStyleProfile {
    std::string styleName;
    float brightness = 1.0f;  // [-0.5 to 0.5]
    float contrast   = 1.0f;  // [0.5 to 2.0]
    float saturation = 1.0f;  // [0.0 to 2.0]
    float temperature= 0.0f;  // [-1.0 (Cool) to 1.0 (Warm)]
    float gamma      = 1.0f;  // [0.5 to 2.2]
};

class PostProcessingManager {
private:
    std::unordered_map<std::string, SurroundingStyleProfile> presets_;
    SurroundingStyleProfile activeProfile_;

public:
    PostProcessingManager() {
        InitializePresets();
    }

    void InitializePresets() {
        // 1. DEFAULT
        presets_["DEFAULT"] = { "Default", 1.0f, 1.0f, 1.0f, 0.0f, 1.0f };

        // 2. DYNAMIC (High Saturation & Brightness for enemy visibility)
        presets_["DYNAMIC"] = { "Dynamic", 1.12f, 1.25f, 1.45f, -0.05f, 0.95f };

        // 3. REALISTIC (Higher Contrast, Cooler Tones)
        presets_["REALISTIC"] = { "Realistic", 0.95f, 1.35f, 0.85f, -0.20f, 1.1f };

        // 4. SOFT (Lower Contrast, Soft Warmth)
        presets_["SOFT"] = { "Soft", 1.05f, 0.85f, 0.90f, 0.15f, 1.05f };

        // 5. NOSTALGIC (Vintage Warmth, Retro Color Balance)
        presets_["NOSTALGIC"] = { "Nostalgic", 0.98f, 1.15f, 0.75f, 0.40f, 1.15f };
    }

    bool SetSurroundingStyle(const std::string& styleKey) {
        auto it = presets_.find(styleKey);
        if (it == presets_.end()) {
            std::cout << "[PostProcess] Error: Unknown style '" << styleKey << "'\n";
            return false;
        }

        activeProfile_ = it->second;
        std::cout << "\n==================================================\n"
                  << "[PostProcess] Applied Surrounding Style: " << activeProfile_.styleName << "\n"
                  << "  - Brightness: " << activeProfile_.brightness << "\n"
                  << "  - Contrast:   " << activeProfile_.contrast << "\n"
                  << "  - Saturation: " << activeProfile_.saturation << "\n"
                  << "  - Warmth:     " << activeProfile_.temperature << "\n"
                  << "==================================================\n";
        
        BindShaderUniforms();
        return true;
    }

    void BindShaderUniforms() const {
        // In OpenGL/Vulkan, send uniforms to shader:
        // glUniform1f(glGetUniformLocation(shaderID, "u_Brightness"), activeProfile_.brightness);
        // glUniform1f(glGetUniformLocation(shaderID, "u_Contrast"), activeProfile_.contrast);
        // glUniform1f(glGetUniformLocation(shaderID, "u_Saturation"), activeProfile_.saturation);
        // glUniform1f(glGetUniformLocation(shaderID, "u_Temperature"), activeProfile_.temperature);
    }
};

int main() {
    PostProcessingManager engine;

    // Cycle through environment surrounding styles
    engine.SetSurroundingStyle("DEFAULT");
    engine.SetSurroundingStyle("DYNAMIC");   // Competitive preset
    engine.SetSurroundingStyle("REALISTIC");
    engine.SetSurroundingStyle("NOSTALGIC");

    return 0;
}
GLSL Post-Processing Fragment Shader (surrounding_filter.frag)
OpenGL Shading Language
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_SceneTexture; // Rendered Framebuffer

// Filter Uniforms from C++ Engine
uniform float u_Brightness;
uniform float u_Contrast;
uniform float u_Saturation;
uniform float u_Temperature;

vec3 ApplyTemperature(vec3 color, float temp) {
    // Shifts color towards blue (cool) or orange (warm)
    color.r += temp * 0.1;
    color.b -= temp * 0.1;
    return clamp(color, 0.0, 1.0);
}

void main() {
    vec3 color = texture(u_SceneTexture, TexCoords).rgb;

    // 1. Apply Brightness
    color *= u_Brightness;

    // 2. Apply Contrast
    color = (color - 0.5) * u_Contrast + 0.5;

    // 3. Apply Saturation
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = mix(vec3(luminance), color, u_Saturation);

    // 4. Apply Temperature / Tone Tint
    color = ApplyTemperature(color, u_Temperature);

    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}

enum class MedalRarity { Common, Rare, Epic, Legendary };

struct MedalCallout {
    std::string id;
    std::string title;
    int xpBonus;
    MedalRarity rarity;
    std::string soundFX;
};

struct PlayerKillEvent {
    uint64_t killerUID;
    uint64_t victimUID;
    float timestampSeconds;
    bool isHeadshot;
    bool isMelee;
    bool wasAvenged;
};

class KillFeedEngine {
private:
    uint64_t localPlayerUID_;
    int currentLifeStreak_ = 0;
    float lastKillTimestamp_ = -10.0f;
    int rapidKillCount_ = 0;
    
    // Configurable Rapid-Kill Time Window (seconds)
    const float MULTI_KILL_WINDOW = 3.0f;
    
    // Player tracking for Revenge / Avenger logic
    uint64_t lastKilledByUID_ = 0;
    std::unordered_map<uint64_t, float> recentTeammateKillers_; // Victim -> Killer

public:
    explicit KillFeedEngine(uint64_t playerUID) : localPlayerUID_(playerUID) {}

    // Called whenever a kill occurs in the match world
    void OnKillOccurred(const PlayerKillEvent& event) {
        if (event.killerUID != localPlayerUID_) {
            // Track if local player was killed
            if (event.victimUID == localPlayerUID_) {
                lastKilledByUID_ = event.killerUID;
                currentLifeStreak_ = 0; // Reset streak on death
            }
            return;
        }

        // --- LOCAL PLAYER SCORED A KILL ---
        currentLifeStreak_++;
        float currentTime = event.timestampSeconds;

        // 1. Evaluate Multi-Kill Timing Window
        if (currentTime - lastKillTimestamp_ <= MULTI_KILL_WINDOW) {
            rapidKillCount_++;
        } else {
            rapidKillCount_ = 1; // Reset multi-kill chain
        }
        lastKillTimestamp_ = currentTime;

        // 2. Evaluate Rapid Kill Medals
        EvaluateRapidKillMedal(rapidKillCount_);

        // 3. Evaluate Streak Medals
        EvaluateStreakMedal(currentLifeStreak_);

        // 4. Evaluate Contextual Medals (Revenge / Avenger)
        if (event.victimUID == lastKilledByUID_ && lastKilledByUID_ != 0) {
            TriggerCalloutPopup({"MEDAL_REVENGE", "REVENGE!", 100, MedalRarity::Rare, "sfx_revenge.wav"});
            lastKilledByUID_ = 0; // Clear payback state
        }

        if (event.isHeadshot) {
            TriggerCalloutPopup({"MEDAL_HEADSHOT", "HEADSHOT", 50, MedalRarity::Common, "sfx_headshot.wav"});
        }
    }

private:
    void EvaluateRapidKillMedal(int count) {
        switch (count) {
            case 2: TriggerCalloutPopup({"MEDAL_DOUBLE", "DOUBLE KILL", 100, MedalRarity::Common, "sfx_double.wav"}); break;
            case 3: TriggerCalloutPopup({"MEDAL_TRIPLE", "TRIPLE KILL", 250, MedalRarity::Rare, "sfx_triple.wav"}); break;
            case 4: TriggerCalloutPopup({"MEDAL_FURY", "FURY KILL!", 500, MedalRarity::Epic, "sfx_fury.wav"}); break;
            case 5: TriggerCalloutPopup({"MEDAL_FRENZY", "FRENZY KILL!!", 750, MedalRarity::Epic, "sfx_frenzy.wav"}); break;
            default:
                if (count >= 6) {
                    TriggerCalloutPopup({"MEDAL_SUPER", "SUPER KILL!!!", 1000, MedalRarity::Legendary, "sfx_super.wav"});
                }
                break;
        }
    }

    void EvaluateStreakMedal(int streak) {
        if (streak == 5) TriggerCalloutPopup({"MEDAL_BLOODTHIRSTY", "BLOODTHIRSTY (5 Kills)", 250, MedalRarity::Rare, "sfx_streak5.wav"});
        else if (streak == 10) TriggerCalloutPopup({"MEDAL_MERCILESS", "MERCILESS (10 Kills)", 500, MedalRarity::Epic, "sfx_streak10.wav"});
        else if (streak == 20) TriggerCalloutPopup({"MEDAL_RELENTLESS", "RELENTLESS (20 Kills)", 1000, MedalRarity::Epic, "sfx_streak20.wav"});
        else if (streak == 30) TriggerCalloutPopup({"MEDAL_NUCLEAR", "NUCLEAR (30 Kills)", 2000, MedalRarity::Legendary, "sfx_nuke_earned.wav"});
    }

    void TriggerCalloutPopup(const MedalCallout& callout) {
        std::cout << " >>> [HUD POPUP TRIGGERED] " << callout.title 
                  << " | +" << callout.xpBonus << " XP"
                  << " | Sound: " << callout.soundFX << " <<<\n";
    }
};

int main() {
    uint64_t myPlayerID = 998877;
    KillFeedEngine engine(myPlayerID);

    std::cout << "================ SIMULATING MULTI-KILL EVENT QUEUE ================\n\n";

    // Simulate dying to enemy 505
    engine.OnKillOccurred({505, myPlayerID, 1.0f, false, false, false});

    std::cout << "\n--- Respawning and Going on a Rapid Streak ---\n";
    
    // Kill 1: Revenge kill on player 505 (at t = 5.0s)
    engine.OnKillOccurred({myPlayerID, 505, 5.0f, true, false, false});

    // Kill 2: Rapid kill (at t = 6.2s)
    engine.OnKillOccurred({myPlayerID, 102, 6.2f, false, false, false});

    // Kill 3: Rapid kill (at t = 7.1s)
    engine.OnKillOccurred({myPlayerID, 103, 7.1f, true, false, false});

    // Kill 4: Rapid kill (at t = 8.0s) -> Triggers Fury Kill
    engine.OnKillOccurred({myPlayerID, 104, 8.0f, false, false, false});

    return 0;
}

enum class AttachmentSlot { MUZZLE, BARREL, OPTIC, STOCK, LASER, UNDERBARREL, MAGAZINE, REAR_GRIP, PERK };

struct WeaponStats {
    float damage = 30.0f;
    float rangeMeters = 25.0f;
    float fireRateRpm = 600.0f;
    float adsTimeMs = 250.0f;       // Aim Down Sights speed
    float sprintToFireMs = 150.0f;
    float recoilControl = 100.0f;   // Base 100%
    float movementSpeed = 1.0f;     // Base multiplier
    uint32_t magazineSize = 30;
};

struct Attachment {
    std::string name;
    AttachmentSlot slot;
    // Modifiers (1.0 = neutral, >1.0 = increase, <1.0 = decrease)
    float adsTimeModifier = 1.0f;
    float rangeModifier = 1.0f;
    float recoilModifier = 1.0f;
    float movementModifier = 1.0f;
    int magCapacityDelta = 0;
};

class GunsmithEngine {
public:
    static WeaponStats CalculateFinalStats(const WeaponStats& base, const std::vector<Attachment>& attachments) {
        if (attachments.size() > 5) {
            std::cerr << "[GUNSMITH ERROR] Maximum 5 attachments allowed!" << std::endl;
        }

        WeaponStats finalStats = base;

        for (const auto& att : attachments) {
            finalStats.adsTimeMs *= att.adsTimeModifier;
            finalStats.rangeMeters *= att.rangeModifier;
            finalStats.recoilControl *= att.recoilModifier;
            finalStats.movementSpeed *= att.movementModifier;
            finalStats.magazineSize += att.magCapacityDelta;
        }

        return finalStats;
    }
};

enum class AbilityState { CHARGING, READY, ACTIVE, COOLDOWN };

class OperatorSkillComponent {
private:
    std::string skillName_;
    float currentCharge_ = 0.0f;
    float maxCharge_ = 100.0f; // Charged via passive time + combat performance
    float activeDurationSeconds_ = 10.0f;
    float activeTimer_ = 0.0f;
    AbilityState state_ = AbilityState::CHARGING;

public:
    OperatorSkillComponent(std::string name) : skillName_(name) {}

    void OnKillOrScore(float scoreGained) {
        if (state_ != AbilityState::CHARGING) return;
        
        currentCharge_ += scoreGained * 0.15f; // Convert match performance into charge
        if (currentCharge_ >= maxCharge_) {
            currentCharge_ = maxCharge_;
            state_ = AbilityState::READY;
            std::cout << "[OPERATOR SKILL] " << skillName_ << " IS READY!" << std::endl;
        }
    }

    bool Activate() {
        if (state_ != AbilityState::READY) return false;
        state_ = AbilityState::ACTIVE;
        activeTimer_ = activeDurationSeconds_;
        std::cout << "[OPERATOR SKILL] ACTIVATED: " << skillName_ << std::endl;
        return true;
    }

    void Tick(float deltaTime) {
        if (state_ == AbilityState::CHARGING) {
            currentCharge_ += deltaTime * 2.0f; // Passive baseline tick charge
            if (currentCharge_ >= maxCharge_) {
                currentCharge_ = maxCharge_;
                state_ = AbilityState::READY;
            }
        } else if (state_ == AbilityState::ACTIVE) {
            activeTimer_ -= deltaTime;
            if (activeTimer_ <= 0.0f) {
                state_ = AbilityState::CHARGING;
                currentCharge_ = 0.0f;
                std::cout << "[OPERATOR SKILL] " << skillName_ << " EXPIRED." << std::endl;
            }
        }
    }
};

struct ClientInputPacket {
    uint32_t sequenceNumber;
    uint32_t playerId;
    float moveForward;
    float moveRight;
    float deltaTime;
    float rotationY;
};

struct ServerCorrectionPacket {
    uint32_t acknowledgedSequenceNumber;
    Vector3 correctedPosition;
    bool requiresSnap;
};

struct SoundEvent {
    uint32_t sourcePlayerId;
    Vector3 location;
    float radiusMeters;
    enum Type { FOOTSTEP, GUNFIRE, SLIDE, VEHICLE } type;
};

class SoundBroadcaster {
public:
    static void BroadcastSound(const SoundEvent& event, const std::unordered_map<uint32_t, Vector3>& allPlayers) {
        for (const auto& [id, pos] : allPlayers) {
            if (id == event.sourcePlayerId) continue;

            float dx = pos.x - event.location.x;
            float dz = pos.z - event.location.z;
            float distSq = dx*dx + dz*dz;

            if (distSq <= event.radiusMeters * event.radiusMeters) {
                // Send minimap indicator notification packet to this client
            }
        }
    }
};

enum class StreakType { UAV = 400, COUNTER_UAV = 600, CARE_PACKAGE = 700, STEALTH_CHOPPER = 1000, VTOL = 1600 };

struct PlayerPerks {
    bool hasHardline = false;    // +25% score toward streaks
    bool hasGhost = false;       // Hidden from enemy UAVs
    bool hasColdBlooded = false; // Immune to AI scorestreaks
};

class ScorestreakSystem {
private:
    uint32_t currentScore_ = 0;
    PlayerPerks perks_;
    std::unordered_map<StreakType, bool> unlockedStreaks_;

public:
    ScorestreakSystem(PlayerPerks perks) : perks_(perks) {
        unlockedStreaks_[StreakType::UAV] = false;
        unlockedStreaks_[StreakType::COUNTER_UAV] = false;
        unlockedStreaks_[StreakType::STEALTH_CHOPPER] = false;
        unlockedStreaks_[StreakType::VTOL] = false;
    }

    void OnMatchEventScore(uint32_t baseScore) {
        // Apply Hardline multiplier
        uint32_t finalScore = perks_.hasHardline ? static_cast<uint32_t>(baseScore * 1.25f) : baseScore;
        currentScore_ += finalScore;

        std::cout << "[SCORESTREAK] Score Earned: +" << finalScore << " | Total Score: " << currentScore_ << std::endl;

        // Check streak thresholds
        CheckUnlocks();
    }

    bool DeployStreak(StreakType type) {
        if (unlockedStreaks_[type]) {
            unlockedStreaks_[type] = false; // Consume streak
            std::cout << "[SCORESTREAK DEPLOYED] Streak type " << static_cast<int>(type) << " executed!" << std::endl;
            return true;
        }
        std::cout << "[SCORESTREAK ERROR] Streak not yet unlocked!" << std::endl;
        return false;
    }

private:
    void CheckUnlocks() {
        auto check = [this](StreakType streak) {
            uint32_t cost = static_cast<uint32_t>(streak);
            if (currentScore_ >= cost && !unlockedStreaks_[streak]) {
                unlockedStreaks_[streak] = true;
                std::cout << "  ===> [UNLOCKED SCORESTREAK] Cost: " << cost << " pts!" << std::endl;
            }
        };

        check(StreakType::UAV);
        check(StreakType::COUNTER_UAV);
        check(StreakType::STEALTH_CHOPPER);
        check(StreakType::VTOL);
    }
};

enum class StanceState { STANDING, CROUCHING, PRONE, SLIDING, JUMPING };

struct CharacterMovementState {
    StanceState stance = StanceState::STANDING;
    bool isSprinting = false;
    float currentSpeed = 0.0f;
    float slideTimerSeconds = 0.0f;
    
    const float BASE_WALK_SPEED = 4.0f;
    const float SPRINT_SPEED = 6.5f;
    const float SLIDE_INITIAL_BOOST = 9.0f;
    const float MAX_SLIDE_DURATION = 0.8f;
};

class MovementValidationEngine {
public:
    static void ProcessMovementInput(CharacterMovementState& state, bool sprintPressed, bool crouchPressed, float deltaTime) {
        // Handle Slide Execution
        if (state.stance == StanceState::SLIDING) {
            state.slideTimerSeconds -= deltaTime;
            // Friction slowdown during slide
            state.currentSpeed = std::max(state.BASE_WALK_SPEED, state.currentSpeed - (5.0f * deltaTime));

            if (state.slideTimerSeconds <= 0.0f || !crouchPressed) {
                // Exit slide into crouch or slide-cancel back to standing sprint
                state.stance = crouchPressed ? StanceState::CROUCHING : StanceState::STANDING;
                std::cout << "[MOVEMENT] Slide finished/canceled. Transitioned to " 
                          << (crouchPressed ? "CROUCHING" : "STANDING") << std::endl;
            }
            return;
        }

        // Trigger Slide: Must be sprinting and press crouch
        if (sprintPressed && crouchPressed && state.stance == StanceState::STANDING) {
            state.stance = StanceState::SLIDING;
            state.slideTimerSeconds = state.MAX_SLIDE_DURATION;
            state.currentSpeed = state.SLIDE_INITIAL_BOOST;
            std::cout << "[MOVEMENT] Executing SLIDE! Boost Speed: " << state.currentSpeed << " m/s" << std::endl;
            return;
        }

        // Standard stance speeds
        if (sprintPressed && state.stance == StanceState::STANDING) {
            state.isSprinting = true;
            state.currentSpeed = state.SPRINT_SPEED;
        } else {
            state.isSprinting = false;
            state.currentSpeed = state.BASE_WALK_SPEED;
        }
    }
};

struct BRAMArmorComponent {
    float baseHealth = 100.0f;
    float armorPlates = 150.0f; // Max 3 plates @ 50 HP each = 150 HP
    const float MAX_ARMOR = 150.0f;

    void ApplyArmorPlate() {
        if (armorPlates >= MAX_ARMOR) return;
        
        armorPlates = std::min(MAX_ARMOR, armorPlates + 50.0f);
        std::cout << "[BR ARMOR] Inserted Plate! Current Armor: " << armorPlates << " / 150.0" << std::endl;
    }

    void TakeDamage(float incomingDamage, bool bypassesArmor = false) {
        if (bypassesArmor || armorPlates <= 0.0f) {
            // Direct damage to health (e.g., Gas, Fall Damage)
            baseHealth -= incomingDamage;
            std::cout << "[COMBAT DAMAGE] Direct HP Hit! Remaining HP: " << std::max(0.0f, baseHealth) << std::endl;
        } else {
            // Damage absorbed by Armor first
            float leftoverDamage = incomingDamage - armorPlates;
            armorPlates = std::max(0.0f, armorPlates - incomingDamage);

            std::cout << "[COMBAT DAMAGE] Armor absorbed hit! Remaining Armor: " << armorPlates;

            if (leftoverDamage > 0.0f) {
                baseHealth -= leftoverDamage;
                std::cout << " | Overspill HP Hit! Remaining HP: " << std::max(0.0f, baseHealth);
            }
            std::cout << std::endl;
        }
    }
};

class AutoFireAccessibilitySystem {
private:
    float targetLockTimer_ = 0.0f;
    const float REQUIRED_LOCK_TIME_SEC = 0.15f; // 150ms lock-on delay before firing

public:
    bool EvaluateAutoFire(bool RaycastHitsEnemy, float deltaTime) {
        if (RaycastHitsEnemy) {
            targetLockTimer_ += deltaTime;
            if (targetLockTimer_ >= REQUIRED_LOCK_TIME_SEC) {
                // Auto-trigger weapon fire packet
                return true;
            }
        } else {
            targetLockTimer_ = 0.0f; // Reset lock on target lost
        }
        return false;
    }
};

int main() {
    std::cout << "=================== 1. SCORESTREAK DEMO ===================" << std::endl;
    PlayerPerks p1Perks{true, false, false}; // Equipped with Hardline
    ScorestreakSystem streakEngine(p1Perks);

    streakEngine.OnMatchEventScore(100); // Kills
    streakEngine.OnMatchEventScore(200); // Objective Capture
    streakEngine.OnMatchEventScore(200); // Double Kill -> Triggers UAV & Counter-UAV
    streakEngine.DeployStreak(StreakType::UAV);

    std::cout << "\n=================== 2. ADVANCED MOVEMENT DEMO ===================" << std::endl;
    CharacterMovementState moveState;
    // Simulate Sprint -> Slide -> Cancel sequence
    MovementValidationEngine::ProcessMovementInput(moveState, true, false, 0.016f); // Sprinting
    MovementValidationEngine::ProcessMovementInput(moveState, true, true, 0.016f);  // Press Crouch while Sprinting = Slide
    MovementValidationEngine::ProcessMovementInput(moveState, true, false, 0.016f); // Release Crouch = Cancel Slide

    std::cout << "\n=================== 3. BR ARMOR SYSTEM DEMO ===================" << std::endl;
    BRAMArmorComponent brPlayer;
    brPlayer.TakeDamage(80.0f);  // Absorbed by armor
    brPlayer.TakeDamage(100.0f); // Breaks armor and spills 30 HP onto health
    brPlayer.ApplyArmorPlate();  // Inserts plate (+50 Armor)

    std::cout << "\n=================== 4. AUTO-FIRE ACCESSIBILITY DEMO ===================" << std::endl;
    AutoFireAccessibilitySystem autoFire;
    // Simulate aiming over an enemy for 200ms across 2 ticks
    bool shouldShoot = autoFire.EvaluateAutoFire(true, 0.100f);
    std::cout << "[AUTO-FIRE] T1 Auto-fire status: " << (shouldShoot ? "FIRING" : "LOCKED_ON") << std::endl;
    shouldShoot = autoFire.EvaluateAutoFire(true, 0.100f);
    std::cout << "[AUTO-FIRE] T2 Auto-fire status: " << (shouldShoot ? "FIRING" : "LOCKED_ON") << std::endl;

    return 0;
}

enum class ItemRarity {
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,       // Purple
    LEGENDARY,  // Orange/Gold
    MYTHIC      // Red / Dynamic
};

enum class PopupType {
    TOP_UP_STORE,
    REWARD_UNLOCK,
    SPECIAL_SCHEME
};

// --- Top-Up / Store Pack Structure ---
struct TopUpPackage {
    std::string packageId;
    uint32_t codPoints;
    uint32_t bonusPoints;
    float priceUSD;
    std::string badgeTag; // e.g., "POPULAR", "BEST VALUE", "+20% EXTRA"
    bool isLimitedTimeOffer = false;
};

// --- Prize / Reward Structure ---
struct RewardItem {
    std::string itemId;
    std::string name;
    std::string category; // "Weapon Blueprint", "Operator Skin", "Vehicle Camo"
    ItemRarity rarity;
    std::string iconAssetPath;
    uint32_t duplicateCreditValue = 0; // Granted if player already owns item
};

// --- Event / Scheme Promo Structure ---
struct SchemePromo {
    std::string schemeId;
    std::string title;
    std::string description;
    float originalPriceUSD;
    float discountedPriceUSD;
    uint64_t expirationTimestamp; // Epoch time in seconds
    std::vector<RewardItem> bundledItems;
};

// ============================================================================
// 2. SERVER PAYLOAD SERIALIZATION ENGINE
// ============================================================================

class PopupPayloadSerializer {
public:
    static std::string RarityToString(ItemRarity rarity) {
        switch (rarity) {
            case ItemRarity::COMMON:    return "COMMON";
            case ItemRarity::UNCOMMON:  return "UNCOMMON";
            case ItemRarity::RARE:      return "RARE";
            case ItemRarity::EPIC:      return "EPIC";
            case ItemRarity::LEGENDARY: return "LEGENDARY";
            case ItemRarity::MYTHIC:    return "MYTHIC";
            default:                    return "UNKNOWN";
        }
    }

    // Serializes Top-Up Store Package list to JSON payload
    static std::string SerializeTopUpStore(const std::vector<TopUpPackage>& packages) {
        std::string json = "{\n  \"popup_type\": \"TOP_UP_STORE\",\n  \"packages\": [\n";
        for (size_t i = 0; i < packages.size(); ++i) {
            const auto& p = packages[i];
            json += "    {\n";
            json += "      \"id\": \"" + p.packageId + "\",\n";
            json += "      \"cp\": " + std::to_string(p.codPoints) + ",\n";
            json += "      \"bonus\": " + std::to_string(p.bonusPoints) + ",\n";
            json += "      \"price_usd\": " + std::to_string(p.priceUSD) + ",\n";
            json += "      \"badge\": \"" + p.badgeTag + "\"\n";
            json += "    }" + std::string(i < packages.size() - 1 ? "," : "") + "\n";
        }
        json += "  ]\n}";
        return json;
    }

    // Serializes Reward Unlock Popup payload
    static std::string SerializeRewardUnlock(const RewardItem& item) {
        std::string json = "{\n  \"popup_type\": \"REWARD_UNLOCK\",\n";
        json += "  \"item_id\": \"" + item.itemId + "\",\n";
        json += "  \"name\": \"" + item.name + "\",\n";
        json += "  \"category\": \"" + item.category + "\",\n";
        json += "  \"rarity\": \"" + RarityToString(item.rarity) + "\",\n";
        json += "  \"asset_path\": \"" + item.iconAssetPath + "\",\n";
        json += "  \"duplicate_credits\": " + std::to_string(item.duplicateCreditValue) + "\n";
        json += "}";
        return json;
    }

    // Serializes Scheme / Special Offer payload
    static std::string SerializeSchemeOffer(const SchemePromo& scheme) {
        std::string json = "{\n  \"popup_type\": \"SPECIAL_SCHEME\",\n";
        json += "  \"scheme_id\": \"" + scheme.schemeId + "\",\n";
        json += "  \"title\": \"" + scheme.title + "\",\n";
        json += "  \"description\": \"" + scheme.description + "\",\n";
        json += "  \"original_price\": " + std::to_string(scheme.originalPriceUSD) + ",\n";
        json += "  \"discount_price\": " + std::to_string(scheme.discountedPriceUSD) + ",\n";
        json += "  \"expires_at\": " + std::to_string(scheme.expirationTimestamp) + ",\n";
        json += "  \"bundle_count\": " + std::to_string(scheme.bundledItems.size()) + "\n";
        json += "}";
        return json;
    }
};

// ============================================================================
// 3. CLIENT UI STATE ENGINE & POPUP CONTROLLER
// ============================================================================

class ClientPopupController {
private:
    bool isPopupActive_ = false;
    PopupType currentType_;
    std::string activePayload_;

public:
    void TriggerTopUpPopup(const std::vector<TopUpPackage>& packages) {
        isPopupActive_ = true;
        currentType_ = PopupType::TOP_UP_STORE;
        activePayload_ = PopupPayloadSerializer::SerializeTopUpStore(packages);
        RenderUI();
    }

    void TriggerRewardPopup(const RewardItem& item) {
        isPopupActive_ = true;
        currentType_ = PopupType::REWARD_UNLOCK;
        activePayload_ = PopupPayloadSerializer::SerializeRewardUnlock(item);
        RenderUI();
    }

    void TriggerSchemePopup(const SchemePromo& scheme) {
        isPopupActive_ = true;
        currentType_ = PopupType::SPECIAL_SCHEME;
        activePayload_ = PopupPayloadSerializer::SerializeSchemeOffer(scheme);
        RenderUI();
    }

    void DismissPopup() {
        if (!isPopupActive_) return;
        std::cout << "\n[UI ENGINE] Closing current active popup overlay.\n";
        isPopupActive_ = false;
    }

    void ProcessUserPurchase(const std::string& packageId) {
        if (currentType_ != PopupType::TOP_UP_STORE && currentType_ != PopupType::SPECIAL_SCHEME) {
            std::cout << "[ERROR] Cannot execute purchase outside of Store/Scheme popup!\n";
            return;
        }
        std::cout << "\n[PAYMENT SYSTEM] Executing purchase transaction for ID: " << packageId << std::endl;
        std::cout << "[PAYMENT SYSTEM] Payment Gateway Response: SUCCESS (200 OK)" << std::endl;
        DismissPopup();
    }

private:
    void RenderUI() const {
        std::cout << "\n======================================================\n";
        switch (currentType_) {
            case PopupType::TOP_UP_STORE:
                std::cout << "               STORE: GET COD POINTS                  \n";
                break;
            case PopupType::REWARD_UNLOCK:
                std::cout << "               ITEM UNLOCKED / PRIZE                  \n";
                break;
            case PopupType::SPECIAL_SCHEME:
                std::cout << "           SPECIAL EVENT / SCHEME PROMO               \n";
                break;
        }
        std::cout << "======================================================\n";
        std::cout << activePayload_ << std::endl;
        std::cout << "======================================================\n";
    }
};

// ============================================================================
// 4. MAIN DEMO EXECUTION
// ============================================================================

int main() {
    ClientPopupController uiController;

    // ------------------------------------------------------------------------
    // 1. TOP-UP STORE POPUP DEMO
    // ------------------------------------------------------------------------
    std::cout << "\n>>> DEMO 1: TRIGGERING TOP-UP POPUP <<<\n";
    std::vector<TopUpPackage> storePacks = {
        {"cp_pack_80",    80,   0,    0.99f,  ""},
        {"cp_pack_420",   400,  20,   4.99f,  "POPULAR"},
        {"cp_pack_880",   800,  80,   9.99f,  "+10% EXTRA"},
        {"cp_pack_2400",  2000, 400,  24.99f, "BEST VALUE"},
        {"cp_pack_5000",  4000, 1000, 49.99f, "+25% EXTRA"}
    };

    uiController.TriggerTopUpPopup(storePacks);
    uiController.ProcessUserPurchase("cp_pack_2400");

    // ------------------------------------------------------------------------
    // 2. PRIZE / REWARD POPUP DEMO
    // ------------------------------------------------------------------------
    std::cout << "\n>>> DEMO 2: TRIGGERING REWARD UNLOCK POPUP <<<\n";
    RewardItem legendaryM4{
        "wpn_m4_legendary_01",
        "M4 - Court Jester",
        "Weapon Blueprint",
        ItemRarity::LEGENDARY,
        "UI/Textures/Weapons/M4_CourtJester.png",
        500 // 500 Credits if duplicate
    };

    uiController.TriggerRewardPopup(legendaryM4);
    uiController.DismissPopup();

    // ------------------------------------------------------------------------
    // 3. SPECIAL SCHEME / PROMO EVENT POPUP DEMO
    // ------------------------------------------------------------------------
    std::cout << "\n>>> DEMO 3: TRIGGERING SCHEME EVENT POPUP <<<\n";
    
    // Calculate expiration timestamp (24 hours from current time)
    auto now = std::chrono::system_clock::now();
    auto expiresAt = std::chrono::duration_cast<std::chrono::seconds>(
        (now + std::chrono::hours(24)).time_since_epoch()
    ).count();

    SchemePromo battlePassBundle{
        "scheme_bp_bundle_s5",
        "SEASON 5 GROUND FORCE SCHEME",
        "Get Season Pass + 25 Tier Skips & Legendary AK-47 Blueprint at 50% Off!",
        19.99f,
        9.99f,
        static_cast<uint64_t>(expiresAt),
        { legendaryM4 }
    };

    uiController.TriggerSchemePopup(battlePassBundle);
    uiController.ProcessUserPurchase("scheme_bp_bundle_s5");

    return 0;
}

struct Color {
    float r, g, b, a;
    static Color Yellow()    { return {0.95f, 0.67f, 0.00f, 1.0f}; }
    static Color CardDark()  { return {0.11f, 0.12f, 0.15f, 0.95f}; }
    static Color Epic()      { return {0.64f, 0.00f, 1.00f, 1.0f}; }
    static Color Legendary() { return {1.00f, 0.60f, 0.00f, 1.0f}; }
    static Color White()     { return {1.00f, 1.00f, 1.00f, 1.0f}; }
    static Color Red()       { return {0.90f, 0.20f, 0.20f, 1.0f}; }
};

struct Rect {
    float x, y, width, height;
    bool Contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

// Virtual Canvas Interface simulating Unreal Slate / OpenGL Canvas / Direct2D
class UIContext {
public:
    virtual void DrawRect(const Rect& rect, const Color& color) = 0;
    virtual void DrawText(const std::string& text, float x, float y, float size, const Color& color) = 0;
    virtual void DrawButton(const Rect& rect, const std::string& label, const Color& bg, const Color& textCol, bool isHovered) = 0;
    virtual void DrawGlowCircle(float centerX, float centerY, float radius, const Color& color) = 0;
    virtual ~UIContext() = default;
};

// Console Graphics Debugger Simulating HW Acceleration Canvas
class MockRenderCanvas : public UIContext {
public:
    void DrawRect(const Rect& rect, const Color& color) override {
        std::cout << "  [DRAW RECT] Pos(" << rect.x << "," << rect.y << ") Size(" << rect.width << "x" << rect.height 
                  << ") Color(" << color.r << "," << color.g << "," << color.b << ")\n";
    }

    void DrawText(const std::string& text, float x, float y, float size, const Color& color) override {
        std::cout << "  [DRAW TEXT] \"" << text << "\" at (" << x << "," << y << ") Size:" << size << "\n";
    }

    void DrawButton(const Rect& rect, const std::string& label, const Color& bg, const Color& textCol, bool isHovered) override {
        std::cout << "  [DRAW BUTTON] \"" << label << "\" Pos(" << rect.x << "," << rect.y << ") " 
                  << (isHovered ? "[HOVERED]" : "[NORMAL]") << "\n";
    }

    void DrawGlowCircle(float centerX, float centerY, float radius, const Color& color) override {
        std::cout << "  [DRAW RARITY GLOW] Center(" << centerX << "," << centerY << ") Radius(" << radius << ")\n";
    }
};

// ============================================================================
// 2. FRONTEND WIDGET BASE CLASS & MOUSE EVENT HANDLING
// ============================================================================

class UIWidget {
protected:
    Rect bounds_;
    bool isVisible_ = true;

public:
    UIWidget(Rect bounds) : bounds_(bounds) {}
    virtual void Render(UIContext* canvas) = 0;
    virtual bool OnMouseClick(float mouseX, float mouseY) = 0;
    void SetVisible(bool visible) { isVisible_ = visible; }
    bool IsVisible() const { return isVisible_; }
    virtual ~UIWidget() = default;
};

// ============================================================================
// 3. KEY IMPLEMENTATION 1: TOP-UP STORE OVERLAY FRONTEND
// ============================================================================

struct StoreCardData {
    std::string id;
    std::string cpAmount;
    std::string price;
    std::string badge;
};

class TopUpStoreWidget : public UIWidget {
private:
    std::vector<StoreCardData> cards_;

public:
    TopUpStoreWidget(Rect bounds) : UIWidget(bounds) {
        // Initialize CP Store Grid Data
        cards_ = {
            {"cp_80", "80 CP", "$0.99", ""},
            {"cp_420", "420 CP", "$4.99", "POPULAR"},
            {"cp_880", "880 CP", "$9.99", "+10% EXTRA"},
            {"cp_2400", "2,400 CP", "$24.99", "BEST VALUE"}
        };
    }

    void Render(UIContext* canvas) override {
        if (!isVisible_) return;

        // Overlay Background Dimming & Main Modal Panel
        canvas->DrawRect(bounds_, Color::CardDark());
        canvas->DrawText("GET CALL OF DUTY POINTS", bounds_.x + 20, bounds_.y + 20, 24, Color::Yellow());

        // Grid Layout Calculation (4 Columns)
        float cardWidth = 140.0f;
        float cardHeight = 180.0f;
        float spacing = 15.0f;
        float startX = bounds_.x + 20.0f;
        float startY = bounds_.y + 60.0f;

        for (size_t i = 0; i < cards_.size(); ++i) {
            float x = startX + i * (cardWidth + spacing);
            Rect cardRect{x, startY, cardWidth, cardHeight};

            // Draw Card Base
            canvas->DrawRect(cardRect, {0.18f, 0.20f, 0.25f, 1.0f});

            // Draw Badge Overlay if exists
            if (!cards_[i].badge.empty()) {
                Rect badgeRect{x + 20, startY - 10, 100, 20};
                canvas->DrawRect(badgeRect, Color::Yellow());
                canvas->DrawText(cards_[i].badge, x + 25, startY - 5, 10, {0,0,0,1});
            }

            // Draw CP Text and Purchase Button
            canvas->DrawText(cards_[i].cpAmount, x + 30, startY + 60, 18, Color::White());
            Rect btnRect{x + 10, startY + cardHeight - 40, cardWidth - 20, 30};
            canvas->DrawButton(btnRect, cards_[i].price, Color::Yellow(), {0,0,0,1}, false);
        }
    }

    bool OnMouseClick(float mouseX, float mouseY) override {
        if (!isVisible_ || !bounds_.Contains(mouseX, mouseY)) return false;
        std::cout << "[EVENT HANDLER] Top-Up Overlay clicked at (" << mouseX << "," << mouseY << ")\n";
        return true;
    }
};

// ============================================================================
// 4. KEY IMPLEMENTATION 2: REWARD / PRIZE UNLOCKED FRONTEND
// ============================================================================

class RewardUnlockedWidget : public UIWidget {
private:
    std::string itemName_;
    std::string category_;
    Color rarityColor_;
    float animationTime_ = 0.0f;

public:
    RewardUnlockedWidget(Rect bounds, std::string name, std::string cat, Color rarity)
        : UIWidget(bounds), itemName_(name), category_(cat), rarityColor_(rarity) {}

    void UpdateAnimation(float deltaTime) {
        animationTime_ += deltaTime; // Drives pulse/scale effects
    }

    void Render(UIContext* canvas) override {
        if (!isVisible_) return;

        // Modal Box
        canvas->DrawRect(bounds_, Color::CardDark());

        // Header Title
        canvas->DrawText("ITEM UNLOCKED", bounds_.x + (bounds_.width/2) - 60, bounds_.y + 30, 16, {0.7f, 0.7f, 0.7f, 1.0f});

        // Animated Radial Glow Effect for Rarity Accent
        float glowRadius = 80.0f + std::sin(animationTime_ * 3.0f) * 5.0f;
        canvas->DrawGlowCircle(bounds_.x + bounds_.width/2, bounds_.y + 130, glowRadius, rarityColor_);

        // Render Item Blueprint Name
        canvas->DrawText(itemName_, bounds_.x + 40, bounds_.y + 220, 22, rarityColor_);
        canvas->DrawText(category_, bounds_.x + 40, bounds_.y + 250, 12, Color::White());

        // Claim Button
        Rect claimBtn{bounds_.x + (bounds_.width/2) - 60, bounds_.y + bounds_.height - 50, 120, 35};
        canvas->DrawButton(claimBtn, "CLAIM", rarityColor_, Color::White(), true);
    }

    bool OnMouseClick(float mouseX, float mouseY) override {
        if (!isVisible_) return false;
        Rect claimBtn{bounds_.x + (bounds_.width/2) - 60, bounds_.y + bounds_.height - 50, 120, 35};
        if (claimBtn.Contains(mouseX, mouseY)) {
            std::cout << "[EVENT HANDLER] Claim Button Pressed! Dismissing Popup.\n";
            SetVisible(false);
            return true;
        }
        return false;
    }
};

// ============================================================================
// 5. KEY IMPLEMENTATION 3: SPECIAL SCHEME / PROMO BANNER FRONTEND
// ============================================================================

class SchemePromoWidget : public UIWidget {
private:
    std::string promoTitle_;
    std::string discountText_;

public:
    SchemePromoWidget(Rect bounds, std::string title, std::string discount)
        : UIWidget(bounds), promoTitle_(title), discountText_(discount) {}

    void Render(UIContext* canvas) override {
        if (!isVisible_) return;

        // Banner Base
        canvas->DrawRect(bounds_, {0.15f, 0.08f, 0.05f, 0.98f}); // Deep Amber Tint

        // Title
        canvas->DrawText(promoTitle_, bounds_.x + 20, bounds_.y + 25, 20, Color::Red());
        canvas->DrawText("LIMITED TIME SPECIAL OFFER!", bounds_.x + 20, bounds_.y + 55, 12, Color::White());

        // Pricing Tag
        canvas->DrawText(discountText_, bounds_.x + 20, bounds_.y + 90, 26, Color::Yellow());

        // Purchase CTA Button
        Rect buyBtn{bounds_.x + bounds_.width - 150, bounds_.y + 40, 130, 40};
        canvas->DrawButton(buyBtn, "BUY NOW", Color::Red(), Color::White(), false);
    }

    bool OnMouseClick(float mouseX, float mouseY) override {
        if (!isVisible_ || !bounds_.Contains(mouseX, mouseY)) return false;
        std::cout << "[EVENT HANDLER] Special Scheme Banner Interacted.\n";
        return true;
    }
};

// ============================================================================
// 6. MAIN ENGINE EXECUTION LOOP
// ============================================================================

int main() {
    MockRenderCanvas engineCanvas;

    // 1. Instantiating UI Front-end Overlays
    TopUpStoreWidget topUpOverlay({50.0f, 50.0f, 680.0f, 280.0f});
    RewardUnlockedWidget rewardOverlay({250.0f, 80.0f, 320.0f, 320.0f}, "M4 - COURT JESTER", "EPIC WEAPON BLUEPRINT", Color::Epic());
    SchemePromoWidget schemeBanner({100.0f, 420.0f, 580.0f, 130.0f}, "SEASON 5 BATTLE PASS SCHEME", "$9.99 (50% OFF)");

    // 2. Engine Frame Loop Render Step
    std::cout << "=================== FRONTEND UI FRAME RENDER TICK ===================\n";
    
    std::cout << "\n--- Rendering Top-Up Store Component ---\n";
    topUpOverlay.Render(&engineCanvas);

    std::cout << "\n--- Rendering Reward Unlocked Component (Tick 1) ---\n";
    rewardOverlay.UpdateAnimation(0.016f); // 16ms Frame Step
    rewardOverlay.Render(&engineCanvas);

    std::cout << "\n--- Rendering Special Scheme Banner Component ---\n";
    schemeBanner.Render(&engineCanvas);

    // 3. User Input Dispatch Event Demo
    std::cout << "\n=================== INPUT DISPATCH EVENT DEMO ===================\n";
    rewardOverlay.OnMouseClick(400.0f, 365.0f); // Simulate clicking the CLAIM button

    return 0;
}

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 UV;

uniform mat4 uModel;
uniform mat4 uProjection;

void main() {
    // Offset UVs to center [-0.5, 0.5] for symmetrical radial distance calculations
    UV = aTexCoords - vec2(0.5);
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}

in vec2 UV;
out vec4 FragColor;

// Uniforms
uniform vec3 uGlowColor;      // Rarity RGB color
uniform float uTime;          // Game time for pulsing animation
uniform float uGlowRadius;    // Base radius spread (0.1 - 0.5)
uniform float uGlowIntensity; // Brightness multiplier

void main() {
    // Calculate distance from center [0.0, 0.5]
    float dist = length(UV);

    // Pulse wave calculation using sin() harmonic
    float pulse = 1.0 + 0.15 * sin(uTime * 3.5);
    float adjustedRadius = uGlowRadius * pulse;

    // Smooth exponential radial attenuation
    float alpha = exp(-dist * dist / (2.0 * adjustedRadius * adjustedRadius));
    
    // Hard cutoff at edges to prevent quad clipping artifacts
    alpha *= smoothstep(0.5, 0.35, dist);

    // Combine base glow color with animated alpha intensity
    vec3 finalColor = uGlowColor * uGlowIntensity;
    FragColor = vec4(finalColor, alpha);
}

class RarityGlowRenderer {
private:
    GLuint shaderProgram_ = 0;
    GLuint quadVAO_ = 0;
    GLuint quadVBO_ = 0;

    // Uniform locations
    GLint locModel_ = -1;
    GLint locProj_ = -1;
    GLint locColor_ = -1;
    GLint locTime_ = -1;
    GLint locRadius_ = -1;
    GLint locIntensity_ = -1;

public:
    RarityGlowRenderer() = default;

    bool Initialize(const char* vertSrc, const char* fragSrc) {
        // Compile Shaders
        GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertSrc);
        GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

        if (!vertShader || !fragShader) return false;

        shaderProgram_ = glCreateProgram();
        glAttachShader(shaderProgram_, vertShader);
        glAttachShader(shaderProgram_, fragShader);
        glLinkProgram(shaderProgram_);

        GLint success;
        glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
            std::cerr << "[GLSL LINK ERROR] " << infoLog << std::endl;
            return false;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        // Cache Uniform Locations
        locModel_     = glGetUniformLocation(shaderProgram_, "uModel");
        locProj_      = glGetUniformLocation(shaderProgram_, "uProjection");
        locColor_     = glGetUniformLocation(shaderProgram_, "uGlowColor");
        locTime_      = glGetUniformLocation(shaderProgram_, "uTime");
        locRadius_    = glGetUniformLocation(shaderProgram_, "uGlowRadius");
        locIntensity_ = glGetUniformLocation(shaderProgram_, "uGlowIntensity");

        SetupQuadGeometry();
        return true;
    }

    void RenderGlow(float centerX, float centerY, float width, float height, 
                    const glm::vec3& rgbColor, float timeSeconds, 
                    const glm::mat4& projectionMatrix) 
    {
        glUseProgram(shaderProgram_);

        // Enable Alpha Blending for Glow Smoothness
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for vibrant glow effect

        // Model Matrix: Translate to target center and scale
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        // Pass Uniforms to GPU
        glUniformMatrix4fv(locProj_, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(locModel_, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(locColor_, rgbColor.r, rgbColor.g, rgbColor.b);
        glUniform1f(locTime_, timeSeconds);
        glUniform1f(locRadius_, 0.18f);     // Base falloff radius
        glUniform1f(locIntensity_, 1.8f);   // Brightness multiplier

        // Draw Quad
        glBindVertexArray(quadVAO_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        // Restore default blend mode
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    ~RarityGlowRenderer() {
        if (quadVAO_) glDeleteVertexArrays(1, &quadVAO_);
        if (quadVBO_) glDeleteBuffers(1, &quadVBO_);
        if (shaderProgram_) glDeleteProgram(shaderProgram_);
    }

private:
    void SetupQuadGeometry() {
        // Quad Vertices: Position (XY) + UVs (ST)
        float quadVertices[] = {
            // Pos        // UV
            -0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO_);
        glGenBuffers(1, &quadVBO_);

        glBindVertexArray(quadVAO_);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Pos Attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // UV Attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    GLuint CompileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "[GLSL COMPILE ERROR] " << infoLog << std::endl;
            return 0;
        }
        return shader;
    }
};

const glm::vec3 COLOR_RARE      = glm::vec3(0.00f, 0.45f, 0.90f); // Blue
const glm::vec3 COLOR_EPIC      = glm::vec3(0.64f, 0.00f, 1.00f); // Purple
const glm::vec3 COLOR_LEGENDARY = glm::vec3(1.00f, 0.60f, 0.00f); // Gold / Orange
const glm::vec3 COLOR_MYTHIC    = glm::vec3(0.90f, 0.10f, 0.15f); // Red

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 UV;

uniform mat4 uModel;
uniform mat4 uProjection;

void main() {
    UV = aTexCoords - vec2(0.5); // Center UVs to [-0.5, 0.5]
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform vec3 uGlowColor;
uniform float uTime;
uniform float uGlowRadius;
uniform float uGlowIntensity;

void main() {
    float dist = length(UV);

    // Harmonic pulse effect
    float pulse = 1.0 + 0.15 * sin(uTime * 3.5);
    float adjustedRadius = uGlowRadius * pulse;

    // Smooth radial attenuation
    float alpha = exp(-dist * dist / (2.0 * adjustedRadius * adjustedRadius));
    alpha *= smoothstep(0.5, 0.35, dist);

    vec3 finalColor = uGlowColor * uGlowIntensity;
    FragColor = vec4(finalColor, alpha);
}
)";

// ============================================================================
// 2. RARITY GLOW RENDERER CLASS
// ============================================================================

class RarityGlowRenderer {
private:
    GLuint shaderProgram_ = 0;
    GLuint quadVAO_ = 0;
    GLuint quadVBO_ = 0;

    GLint locModel_ = -1;
    GLint locProj_ = -1;
    GLint locColor_ = -1;
    GLint locTime_ = -1;
    GLint locRadius_ = -1;
    GLint locIntensity_ = -1;

public:
    RarityGlowRenderer() = default;

    bool Initialize(const char* vertSrc, const char* fragSrc) {
        GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertSrc);
        GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

        if (!vertShader || !fragShader) return false;

        shaderProgram_ = glCreateProgram();
        glAttachShader(shaderProgram_, vertShader);
        glAttachShader(shaderProgram_, fragShader);
        glLinkProgram(shaderProgram_);

        GLint success;
        glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
            std::cerr << "[GLSL LINK ERROR] " << infoLog << std::endl;
            return false;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        // Cache uniform locations
        locModel_     = glGetUniformLocation(shaderProgram_, "uModel");
        locProj_      = glGetUniformLocation(shaderProgram_, "uProjection");
        locColor_     = glGetUniformLocation(shaderProgram_, "uGlowColor");
        locTime_      = glGetUniformLocation(shaderProgram_, "uTime");
        locRadius_    = glGetUniformLocation(shaderProgram_, "uGlowRadius");
        locIntensity_ = glGetUniformLocation(shaderProgram_, "uGlowIntensity");

        SetupQuadGeometry();
        return true;
    }

    void RenderGlow(float centerX, float centerY, float width, float height, 
                    const glm::vec3& rgbColor, float timeSeconds, 
                    const glm::mat4& projectionMatrix) 
    {
        glUseProgram(shaderProgram_);

        // Enable Additive Alpha Blending for vibrant radial glows
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        glUniformMatrix4fv(locProj_, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(locModel_, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(locColor_, rgbColor.r, rgbColor.g, rgbColor.b);
        glUniform1f(locTime_, timeSeconds);
        glUniform1f(locRadius_, 0.18f);
        glUniform1f(locIntensity_, 1.8f);

        glBindVertexArray(quadVAO_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    ~RarityGlowRenderer() {
        if (quadVAO_) glDeleteVertexArrays(1, &quadVAO_);
        if (quadVBO_) glDeleteBuffers(1, &quadVBO_);
        if (shaderProgram_) glDeleteProgram(shaderProgram_);
    }

private:
    void SetupQuadGeometry() {
        float quadVertices[] = {
            -0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO_);
        glGenBuffers(1, &quadVBO_);

        glBindVertexArray(quadVAO_);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    GLuint CompileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "[GLSL COMPILE ERROR] " << infoLog << std::endl;
            return 0;
        }
        return shader;
    }
};

// ============================================================================
// 3. MAIN RUNNABLE ENTRY POINT
// ============================================================================

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CODM Rarity Glow Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize Glow Renderer
    RarityGlowRenderer glowRenderer;
    if (!glowRenderer.Initialize(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return -1;
    }

    // Define 2D Orthographic Projection Matrix
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), 
                                      0.0f, static_cast<float>(SCREEN_HEIGHT), 
                                      -1.0f, 1.0f);

    // CODM Rarity Preset Colors
    const glm::vec3 COLOR_LEGENDARY = glm::vec3(1.00f, 0.60f, 0.00f); // Gold / Orange
    const glm::vec3 COLOR_EPIC      = glm::vec3(0.64f, 0.00f, 1.00f); // Purple

    std::cout << "[SUCCESS] OpenGL window initialized. Rendering item rarity glow...\n";

    // Main Render Loop
    while (!glfwWindowShouldClose(window)) {
        // Process ESC to exit
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Clear Screen with Dark Background
        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float currentTime = static_cast<float>(glfwGetTime());

        // Render Animated Legendary Glow at screen center (640, 360)
        glowRenderer.RenderGlow(
            640.0f, 360.0f, // Center X, Center Y
            400.0f, 400.0f, // Width, Height
            COLOR_LEGENDARY,
            currentTime,
            projection
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void Render(UIContext* canvas) override {
    if (!isVisible_) return;

    // 1. Draw UI Background Frame
    canvas->DrawRect(bounds_, Color::CardDark());

    // 2. PASS TO OPENGL GLOW SHADER
    // Translates UI coordinates to screen center for the glowing item aura
    float centerX = bounds_.x + (bounds_.width / 2.0f);
    float centerY = bounds_.y + 130.0f;
    
    // Calls our live OpenGL shader renderer!
    glowRenderer.RenderGlow(
        centerX, centerY, 
        250.0f, 250.0f, // Quad dimensions for glow aura
        glm::vec3(rarityColor_.r, rarityColor_.g, rarityColor_.b), 
        animationTime_, 
        projectionMatrix
    );

    // 3. Draw Weapon Text Over the Glow Aura
    canvas->DrawText(itemName_, bounds_.x + 40, bounds_.y + 220, 22, rarityColor_);
}

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
#include <deque>
#include <random>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <array>

// If compiling with Boost.Asio, include: <boost/asio.hpp>
// using boost::asio::ip::udp;

// ============================================================================
// 1. DATA STRUCTURES & PACKET PROTOCOLS
// ============================================================================

enum class PacketType : uint8_t {
    JOIN_QUEUE = 1,
    MATCH_FOUND = 2,
    PLAYER_MOVEMENT = 3,
    PLAYER_SHOOT = 4,
    DAMAGE_EVENT = 5
};

#pragma pack(push, 1)
struct Header {
    PacketType type;
    uint32_t playerId;
};

struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
    
    float lengthSquared() const { return x*x + y*y + z*z; }
    
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }
};

struct MovementPacket {
    Header header;
    Vector3 position;
    float rotationY;
};

struct ShootPacket {
    Header header;
    uint32_t targetPlayerId;
    float damage;
};

struct DamagePacket {
    Header header;
    uint32_t targetPlayerId;
    float remainingHealth;
};
#pragma pack(pop)

struct Ray {
    Vector3 origin;
    Vector3 direction; // Normalized direction vector
};

// Historical snapshot of a player's hitboxes at a specific point in time
struct TransformSnapshot {
    uint64_t timestampMs; // Server time in milliseconds
    Vector3 headPosition;
    Vector3 chestPosition;
    float headRadius = 0.25f;  // Head hitbox size
    float chestRadius = 0.60f; // Chest hitbox size
};

enum class EntityType { PLAYER, BOT_AI, CAMPAIGN_NPC, ZOMBIE_AI, ENVIRONMENT_OBJECT };
enum class Team { NEUTRAL, FACTION_ALPHA, FACTION_BRAVO, HOSTILE_AI };

// Unified Entity Structure for Players, Bots, Campaign NPCs, and Zombies
struct GameEntity {
    uint32_t id;
    std::string name;
    EntityType type;
    Team team = Team::NEUTRAL;
    
    Vector3 position;
    float rotationY = 0.0f;
    
    float health = 100.0f;
    float maxHealth = 100.0f;
    float armor = 0.0f;
    bool isAlive = true;
    
    uint32_t score = 0;
    uint32_t kills = 0;
    uint32_t deaths = 0;
    uint32_t points = 500; // Zombies economy points
    bool hasJuggernog = false;
    std::string primaryWeapon = "1911 Pistol";

    TransformSnapshot currentTransform;
    std::deque<TransformSnapshot> historyBuffer; // Ring buffer for lag comp
};

struct MatchRoom {
    std::string roomId;
    std::unordered_map<uint32_t, std::shared_ptr<GameEntity>> players;
};

// ============================================================================
// 2. RAYCAST & LAG COMPENSATION SYSTEM
// ============================================================================

class PhysicsGeom {
public:
    static bool RayIntersectsSphere(const Ray& ray, const Vector3& center, float radius) {
        Vector3 oc = ray.origin - center;
        float b = 2.0f * (oc.x * ray.direction.x + oc.y * ray.direction.y + oc.z * ray.direction.z);
        float c = oc.lengthSquared() - (radius * radius);
        float discriminant = (b * b) - (4.0f * c);

        if (discriminant < 0.0f) return false;
        
        float t = (-b - std::sqrt(discriminant)) / 2.0f;
        return t >= 0.0f;
    }
};

class LagCompensationSystem {
private:
    static constexpr size_t MAX_HISTORY_SNAPSHOTS = 60; // 1 second at 60 FPS
    static constexpr uint64_t MAX_REWIND_MS = 1000;

public:
    void RecordPlayerTransform(GameEntity& player, uint64_t serverTimestampMs, Vector3 headPos, Vector3 chestPos) {
        TransformSnapshot snapshot{serverTimestampMs, headPos, chestPos};
        
        player.currentTransform = snapshot;
        player.historyBuffer.push_front(snapshot);

        if (player.historyBuffer.size() > MAX_HISTORY_SNAPSHOTS) {
            player.historyBuffer.pop_back();
        }
    }

    TransformSnapshot GetRewoundTransform(const GameEntity& target, uint64_t shotTimestampMs) {
        if (target.historyBuffer.empty()) return target.currentTransform;

        uint64_t latestTime = target.historyBuffer.front().timestampMs;
        if (shotTimestampMs > latestTime) return target.historyBuffer.front();
        if (latestTime - shotTimestampMs > MAX_REWIND_MS) {
            shotTimestampMs = latestTime - MAX_REWIND_MS;
        }

        const TransformSnapshot* newer = nullptr;
        const TransformSnapshot* older = nullptr;

        for (size_t i = 0; i < target.historyBuffer.size(); ++i) {
            if (target.historyBuffer[i].timestampMs <= shotTimestampMs) {
                older = &target.historyBuffer[i];
                if (i > 0) newer = &target.historyBuffer[i - 1];
                break;
            }
        }

        if (!older) return target.historyBuffer.back();
        if (!newer) return *older;

        float frameDelta = static_cast<float>(newer->timestampMs - older->timestampMs);
        float timeDelta = static_cast<float>(shotTimestampMs - older->timestampMs);
        float t = timeDelta / frameDelta;

        TransformSnapshot interpolated;
        interpolated.timestampMs = shotTimestampMs;
        interpolated.headPosition = Vector3::Lerp(older->headPosition, newer->headPosition, t);
        interpolated.chestPosition = Vector3::Lerp(older->chestPosition, newer->chestPosition, t);
        
        return interpolated;
    }

    bool ValidateHitScan(GameEntity& shooter, GameEntity& target, uint64_t clientRenderTimeMs, const Ray& shotRay, std::string& outHitZone) {
        TransformSnapshot targetRewoundState = GetRewoundTransform(target, clientRenderTimeMs);

        if (PhysicsGeom::RayIntersectsSphere(shotRay, targetRewoundState.headPosition, targetRewoundState.headRadius)) {
            outHitZone = "HEADSHOT";
            return true;
        }

        if (PhysicsGeom::RayIntersectsSphere(shotRay, targetRewoundState.chestPosition, targetRewoundState.chestRadius)) {
            outHitZone = "CHEST";
            return true;
        }

        outHitZone = "MISS";
        return false;
    }
};

// ============================================================================
// 3. SPATIAL MAP STREAMING ARCHITECTURE
// ============================================================================

struct MapChunk {
    std::string chunkId;
    Vector3 centerPoint;
    float boundingRadius;
    bool isLoaded = false;
};

struct GameMap {
    std::string mapName;
    std::vector<MapChunk> chunks;
};

class SpatialMapLoader {
private:
    std::unordered_map<std::string, GameMap> mapRegistry_;
    std::string activeMap_;
    float streamingRadius_ = 150.0f;

public:
    SpatialMapLoader() {
        InitializeMapDatabase();
    }

    void LoadMap(const std::string& mapName) {
        if (mapRegistry_.find(mapName) != mapRegistry_.end()) {
            activeMap_ = mapName;
            std::cout << "\n[SPATIAL LOADER] Map Selected: " << mapName << std::endl;
            std::cout << "[SPATIAL LOADER] Pre-loading origin chunks..." << std::endl;
        }
    }

    void UpdateStreamingZones(const Vector3& playerPos) {
        if (activeMap_.empty()) return;

        auto& map = mapRegistry_[activeMap_];
        for (auto& chunk : map.chunks) {
            float dx = playerPos.x - chunk.centerPoint.x;
            float dy = playerPos.y - chunk.centerPoint.y;
            float dz = playerPos.z - chunk.centerPoint.z;
            float distSq = dx*dx + dy*dy + dz*dz;

            bool insideRadius = distSq <= (streamingRadius_ * streamingRadius_);

            if (insideRadius && !chunk.isLoaded) {
                chunk.isLoaded = true;
                std::cout << "  [STREAMING] LOADED Chunk: " << chunk.chunkId 
                          << " on " << map.mapName << std::endl;
            } else if (!insideRadius && chunk.isLoaded) {
                chunk.isLoaded = false;
                std::cout << "  [STREAMING] UNLOADED Chunk: " << chunk.chunkId 
                          << " (Out of bounds)" << std::endl;
            }
        }
    }

private:
    void InitializeMapDatabase() {
        mapRegistry_["Nuketown"] = {"Nuketown", {
            {"Nuketown_House_Alpha", {0.0f, 0.0f, 0.0f}, 50.0f},
            {"Nuketown_House_Bravo", {80.0f, 0.0f, 0.0f}, 50.0f},
            {"Nuketown_CulDeSac",    {40.0f, 0.0f, 0.0f}, 60.0f}
        }};

        mapRegistry_["Hijacked"] = {"Hijacked", {
            {"Hijacked_Bow",        {0.0f, 0.0f, 0.0f},   70.0f},
            {"Hijacked_EngineRoom", {0.0f, -20.0f, 80.0f}, 50.0f},
            {"Hijacked_Stern",      {0.0f, 0.0f, 160.0f}, 70.0f}
        }};

        mapRegistry_["Expressway"] = {"Expressway", {
            {"Expressway_Overpass", {0.0f, 10.0f, 0.0f},   100.0f},
            {"Expressway_Station",  {200.0f, 0.0f, 0.0f},  120.0f}
        }};

        mapRegistry_["Apocalypse"] = {"Apocalypse", {
            {"Apocalypse_Jungle_A", {0.0f, 0.0f, 0.0f},    100.0f},
            {"Apocalypse_Temple",   {250.0f, 0.0f, 0.0f},  100.0f}
        }};

        mapRegistry_["Crash"] = {"Crash", {
            {"Crash_Helicopter_Site", {0.0f, 0.0f, 0.0f},   80.0f},
            {"Crash_Hotel_Building",  {120.0f, 0.0f, 0.0f},  60.0f}
        }};
    }
};

// ============================================================================
// 4. ABSTRACT GAME MODE INTERFACE & MODULAR MODES
// ============================================================================

class IGameMode {
public:
    virtual ~IGameMode() = default;
    
    virtual void OnModeInit() = 0;
    virtual void OnEntitySpawned(std::shared_ptr<GameEntity> entity) = 0;
    virtual void OnEntityKilled(uint32_t attackerId, uint32_t victimId) = 0;
    virtual void UpdateTick(float deltaTime) = 0;
    virtual bool IsMatchOver() const = 0;
    virtual std::string GetModeName() const = 0;
};

// --- CAMPAIGN MODE ---
class CampaignMode : public IGameMode {
private:
    uint32_t currentObjectiveIndex_ = 0;
    std::vector<std::string> objectives_ = {
        "Secure the LZ",
        "Breach the Compound",
        "Extract HVT"
    };
    bool matchOver_ = false;

public:
    void OnModeInit() override {
        std::cout << "[CAMPAIGN] Objective 1: " << objectives_[0] << std::endl;
    }

    void OnEntitySpawned(std::shared_ptr<GameEntity> entity) override {
        if (entity->type == EntityType::CAMPAIGN_NPC) {
            std::cout << "[CAMPAIGN] Spawned Mission NPC: " << entity->name << std::endl;
        }
    }

    void OnEntityKilled(uint32_t attackerId, uint32_t victimId) override {
        std::cout << "[CAMPAIGN] Target eliminated. Progressing mission objectives..." << std::endl;
        AdvanceObjective();
    }

    void UpdateTick(float deltaTime) override {}

    void AdvanceObjective() {
        currentObjectiveIndex_++;
        if (currentObjectiveIndex_ < objectives_.size()) {
            std::cout << "[CAMPAIGN] NEW OBJECTIVE: " << objectives_[currentObjectiveIndex_] << std::endl;
        } else {
            std::cout << "[CAMPAIGN] MISSION COMPLETE!" << std::endl;
            matchOver_ = true;
        }
    }

    bool IsMatchOver() const override { return matchOver_; }
    std::string GetModeName() const override { return "Single-Player Campaign"; }
};

// --- MULTIPLAYER MODE (TDM) ---
class TeamDeathmatchMode : public IGameMode {
private:
    uint32_t scoreLimit_ = 5;
    uint32_t teamAlphaScore_ = 0;
    uint32_t teamBravoScore_ = 0;
    bool matchOver_ = false;

public:
    void OnModeInit() override {
        std::cout << "[TDM] Match started. First to " << scoreLimit_ << " kills wins." << std::endl;
    }

    void OnEntitySpawned(std::shared_ptr<GameEntity> entity) override {}

    void OnEntityKilled(uint32_t attackerId, uint32_t victimId) override {
        teamAlphaScore_++;
        std::cout << "[TDM] Score -> Alpha: " << teamAlphaScore_ << " | Bravo: " << teamBravoScore_ << std::endl;

        if (teamAlphaScore_ >= scoreLimit_ || teamBravoScore_ >= scoreLimit_) {
            matchOver_ = true;
            std::cout << "[TDM] VICTORY! Match score limit reached." << std::endl;
        }
    }

    void UpdateTick(float deltaTime) override {}
    bool IsMatchOver() const override { return matchOver_; }
    std::string GetModeName() const override { return "Multiplayer (TDM)"; }
};

// --- BATTLE ROYALE MODE ---
class BattleRoyaleMode : public IGameMode {
private:
    float zoneRadius_ = 1000.0f;
    uint32_t remainingPlayers_ = 0;
    bool matchOver_ = false;

public:
    void OnModeInit() override {
        std::cout << "[BATTLE ROYALE] Infiltration started. Initial Zone Radius: " << zoneRadius_ << "m" << std::endl;
    }

    void OnEntitySpawned(std::shared_ptr<GameEntity> entity) override {
        if (entity->type == EntityType::PLAYER) {
            remainingPlayers_++;
            entity->armor = 150.0f; // 3 Armor plates
        }
    }

    void OnEntityKilled(uint32_t attackerId, uint32_t victimId) override {
        if (remainingPlayers_ > 0) remainingPlayers_--;
        std::cout << "[BATTLE ROYALE] Players remaining: " << remainingPlayers_ << std::endl;

        if (remainingPlayers_ <= 1) {
            matchOver_ = true;
            std::cout << "[BATTLE ROYALE] MATCH OVER! Champion Crowned." << std::endl;
        }
    }

    void UpdateTick(float deltaTime) override {
        if (zoneRadius_ > 100.0f) {
            zoneRadius_ -= 10.0f * deltaTime;
        }
    }

    bool IsMatchOver() const override { return matchOver_; }
    std::string GetModeName() const override { return "Battle Royale"; }
};

// --- ZOM BIES MODE ---
class ZombiesMode : public IGameMode {
private:
    uint32_t currentRound_ = 1;
    uint32_t zombiesRemainingInWave_ = 0;
    std::vector<std::string> mysteryBoxWeapons_ = {"Ray Gun", "Thundergun", "Galil", "MP40", "RPD", "Monkey Bomb"};

public:
    void OnModeInit() override {
        StartRound(1);
    }

    void OnEntitySpawned(std::shared_ptr<GameEntity> entity) override {}

    void StartRound(uint32_t roundNumber) {
        currentRound_ = roundNumber;
        uint32_t baseZombies = 6;
        zombiesRemainingInWave_ = baseZombies + static_cast<uint32_t>(roundNumber * 3.5f);
        
        float zombieHealth = (roundNumber < 10) 
            ? 150.0f + (roundNumber - 1) * 100.0f 
            : 950.0f * std::pow(1.1f, roundNumber - 9);

        std::cout << "\n[ZOM BIES] ROUND " << currentRound_ << " STARTED!" << std::endl;
        std::cout << " Wave Count: " << zombiesRemainingInWave_ 
                  << " | Health per Zombie: " << static_cast<uint32_t>(zombieHealth) << " HP" << std::endl;
    }

    void OnEntityKilled(uint32_t attackerId, uint32_t victimId) override {
        if (zombiesRemainingInWave_ > 0) zombiesRemainingInWave_--;

        if (zombiesRemainingInWave_ == 0) {
            std::cout << "[ZOM BIES] Wave cleared! Progressing..." << std::endl;
            StartRound(currentRound_ + 1);
        }
    }

    void SpinMysteryBox(GameEntity& player) {
        constexpr uint32_t BOX_COST = 950;
        if (player.points < BOX_COST) {
            std::cout << "[MYSTERY BOX] Insufficient points!" << std::endl;
            return;
        }

        player.points -= BOX_COST;
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, mysteryBoxWeapons_.size() - 1);

        player.primaryWeapon = mysteryBoxWeapons_[dist(rng)];
        std::cout << "[MYSTERY BOX] Player " << player.id << " pulled: *** " << player.primaryWeapon << " ***!" << std::endl;
    }

    void PurchaseJuggernog(GameEntity& player) {
        constexpr uint32_t JUGG_COST = 2500;
        if (player.points >= JUGG_COST && !player.hasJuggernog) {
            player.points -= JUGG_COST;
            player.hasJuggernog = true;
            player.health = 250.0f;
            std::cout << "[PERK-A-COLA] Player " << player.id << " acquired Juggernog! HP boosted to 250." << std::endl;
        }
    }

    void UpdateTick(float deltaTime) override {}
    bool IsMatchOver() const override { return false; }
    std::string GetModeName() const override { return "Round-Based Zombies"; }
};

// ============================================================================
// 5. TOURNAMENT MATCH ENGINE (CDL Map Veto & Series Orchestration)
// ============================================================================

enum class MatchState {
    WAITING_FOR_TEAMS,
    MAP_VETO_PHASE,
    PAUSED_TECHNICAL,
    IN_PROGRESS,
    MATCH_COMPLETE
};

struct TournamentTeam {
    uint32_t teamId;
    std::string teamName;
    std::vector<uint32_t> playerIds;
    uint32_t seriesScore = 0;
};

struct MapPoolItem {
    std::string mapName;
    std::string gameMode;
    bool isBanned = false;
    bool isSelected = false;
};

class TournamentMatchEngine {
private:
    std::string matchId_;
    TournamentTeam teamA_;
    TournamentTeam teamB_;
    MatchState currentState_ = MatchState::WAITING_FOR_TEAMS;
    
    uint32_t bestOfSeries_ = 3;
    uint32_t currentMapIndex_ = 0;
    
    std::vector<MapPoolItem> mapPool_;
    std::vector<MapPoolItem> selectedMaps_;

public:
    TournamentMatchEngine(std::string matchId, TournamentTeam a, TournamentTeam b, uint32_t bestOf = 3)
        : matchId_(matchId), teamA_(a), teamB_(b), bestOfSeries_(bestOf) {
        
        mapPool_ = {
            {"Rio", "Hardpoint", false, false},
            {"Invasion", "Search & Destroy", false, false},
            {"Highrise", "Control", false, false},
            {"Karachi", "Hardpoint", false, false},
            {"Sub Base", "Search & Destroy", false, false}
        };

        std::cout << "\n[TOURNAMENT] Match Created: " << teamA_.teamName << " vs " << teamB_.teamName 
                  << " (Best of " << bestOfSeries_ << ")" << std::endl;
        
        currentState_ = MatchState::MAP_VETO_PHASE;
    }

    void ExecuteVeto(uint32_t teamId, const std::string& mapName) {
        if (currentState_ != MatchState::MAP_VETO_PHASE) return;

        for (auto& item : mapPool_) {
            if (item.mapName == mapName && !item.isBanned && !item.isSelected) {
                item.isBanned = true;
                std::cout << "[VETO] Team " << teamId << " BANNED map: " << mapName 
                          << " (" << item.gameMode << ")" << std::endl;
                break;
            }
        }

        CheckVetoCompletion();
    }

    void CheckVetoCompletion() {
        std::vector<MapPoolItem> remaining;
        for (const auto& item : mapPool_) {
            if (!item.isBanned) remaining.push_back(item);
        }

        if (remaining.size() == bestOfSeries_) {
            selectedMaps_ = remaining;
            currentState_ = MatchState::IN_PROGRESS;
            
            std::cout << "\n================ MAP POOL CONFIRMED ================" << std::endl;
            for (size_t i = 0; i < selectedMaps_.size(); ++i) {
                std::cout << " Map " << (i + 1) << ": " << selectedMaps_[i].mapName 
                          << " [" << selectedMaps_[i].gameMode << "]" << std::endl;
            }
            std::cout << "====================================================\n" << std::endl;

            StartNextMap();
        }
    }

    void StartNextMap() {
        if (currentState_ != MatchState::IN_PROGRESS) return;

        uint32_t winsNeeded = (bestOfSeries_ / 2) + 1;
        if (teamA_.seriesScore == winsNeeded || teamB_.seriesScore == winsNeeded) {
            EndMatchSeries();
            return;
        }

        MapPoolItem currentMap = selectedMaps_[currentMapIndex_];
        std::cout << "[MATCH RUNNER] Starting Map " << (currentMapIndex_ + 1) << ": " 
                  << currentMap.mapName << " (" << currentMap.gameMode << ")..." << std::endl;
    }

    void RecordMapResult(uint32_t winningTeamId) {
        if (currentState_ != MatchState::IN_PROGRESS) return;

        if (winningTeamId == teamA_.teamId) {
            teamA_.seriesScore++;
            std::cout << "[MAP COMPLETED] " << teamA_.teamName << " WON Map " << (currentMapIndex_ + 1) << "!" << std::endl;
        } else if (winningTeamId == teamB_.teamId) {
            teamB_.seriesScore++;
            std::cout << "[MAP COMPLETED] " << teamB_.teamName << " WON Map " << (currentMapIndex_ + 1) << "!" << std::endl;
        }

        std::cout << "[SERIES SCORE] " << teamA_.teamName << ": " << teamA_.seriesScore 
                  << " | " << teamB_.teamName << ": " << teamB_.seriesScore << std::endl;

        currentMapIndex_++;
        StartNextMap();
    }

    void TriggerTechnicalPause(uint32_t requestingPlayerId, const std::string& reason) {
        if (currentState_ == MatchState::IN_PROGRESS) {
            currentState_ = MatchState::PAUSED_TECHNICAL;
            std::cout << "\n[TECHNICAL PAUSE] Paused by Player " << requestingPlayerId 
                      << ". Reason: " << reason << std::endl;
        }
    }

    void ResumeMatch() {
        if (currentState_ == MatchState::PAUSED_TECHNICAL) {
            currentState_ = MatchState::IN_PROGRESS;
            std::cout << "[TECHNICAL PAUSE] Match Resumed!\n" << std::endl;
        }
    }

private:
    void EndMatchSeries() {
        currentState_ = MatchState::MATCH_COMPLETE;
        TournamentTeam winner = (teamA_.seriesScore > teamB_.seriesScore) ? teamA_ : teamB_;
        
        std::cout << "\n====================================================" << std::endl;
        std::cout << "   SERIES COMPLETE! WINNER: " << winner.teamName << std::endl;
        std::cout << "   FINAL SCORE: " << teamA_.seriesScore << " - " << teamB_.seriesScore << std::endl;
        std::cout << "====================================================\n" << std::endl;
    }
};

// ============================================================================
// 6. UNIVERSAL GAME ENGINE CORE
// ============================================================================

class UnifiedEngine {
private:
    std::unordered_map<uint32_t, std::shared_ptr<GameEntity>> entityRegistry_;
    std::unique_ptr<IGameMode> activeMode_;
    uint32_t nextEntityId_ = 1000;

public:
    void SetGameMode(std::unique_ptr<IGameMode> mode) {
        activeMode_ = std::move(mode);
        entityRegistry_.clear();
        std::cout << "\n====================================================" << std::endl;
        std::cout << " [ENGINE] Initializing Mode: " << activeMode_->GetModeName() << std::endl;
        std::cout << "====================================================\n" << std::endl;
        activeMode_->OnModeInit();
    }

    std::shared_ptr<GameEntity> SpawnEntity(const std::string& name, EntityType type, Team team, Vector3 pos) {
        auto entity = std::make_shared<GameEntity>();
        entity->id = nextEntityId_++;
        entity->name = name;
        entity->type = type;
        entity->team = team;
        entity->position = pos;

        entityRegistry_[entity->id] = entity;
        if (activeMode_) {
            activeMode_->OnEntitySpawned(entity);
        }
        return entity;
    }

    void ApplyDamage(uint32_t attackerId, uint32_t victimId, float damage) {
        if (entityRegistry_.find(victimId) == entityRegistry_.end()) return;
        
        auto victim = entityRegistry_[victimId];
        if (!victim->isAlive) return;

        if (victim->armor > 0.0f) {
            float armorDamage = std::min(victim->armor, damage);
            victim->armor -= armorDamage;
            damage -= armorDamage;
        }

        victim->health -= damage;

        if (victim->health <= 0.0f) {
            victim->health = 0.0f;
            victim->isAlive = false;
            victim->deaths++;
            
            std::cout << "[COMBAT] " << victim->name << " was eliminated!" << std::endl;

            if (activeMode_) {
                activeMode_->OnEntityKilled(attackerId, victimId);
            }
        }
    }

    void Tick(float deltaTime) {
        if (activeMode_ && !activeMode_->IsMatchOver()) {
            activeMode_->UpdateTick(deltaTime);
        }
    }

    std::shared_ptr<GameEntity> GetEntity(uint32_t id) {
        if (entityRegistry_.find(id) != entityRegistry_.end()) {
            return entityRegistry_[id];
        }
        return nullptr;
    }

    IGameMode* GetActiveMode() { return activeMode_.get(); }
};

// ============================================================================
// 7. MAIN INTEGRATED EXECUTION
// ============================================================================

int main() {
    UnifiedEngine engine;
    LagCompensationSystem lagComp;
    SpatialMapLoader mapLoader;

    // ------------------------------------------------------------------------
    // 1. CAMPAIGN MODE SIMULATION
    // ------------------------------------------------------------------------
    engine.SetGameMode(std::make_unique<CampaignMode>());
    
    auto hero = engine.SpawnEntity("Soap MacTavish", EntityType::PLAYER, Team::FACTION_ALPHA, {0, 0, 0});
    auto enemyLeader = engine.SpawnEntity("Enemy Commander", EntityType::CAMPAIGN_NPC, Team::HOSTILE_AI, {10, 0, 10});

    // Advance objectives by defeating NPCs
    engine.ApplyDamage(hero->id, enemyLeader->id, 100.0f);
    engine.ApplyDamage(hero->id, enemyLeader->id, 100.0f);
    engine.ApplyDamage(hero->id, enemyLeader->id, 100.0f);

    // ------------------------------------------------------------------------
    // 2. MULTIPLAYER (TDM) & TOURNAMENT SYSTEM SIMULATION
    // ------------------------------------------------------------------------
    engine.SetGameMode(std::make_unique<TeamDeathmatchMode>());

    TournamentTeam t1{1, "OpTic Texas", {101, 102, 103, 104}, 0};
    TournamentTeam t2{2, "FaZe Clan", {201, 202, 203, 204}, 0};

    TournamentMatchEngine tournament("MATCH_CDL_FINALS", t1, t2, 3);
    tournament.ExecuteVeto(t1.teamId, "Highrise");
    tournament.ExecuteVeto(t2.teamId, "Sub Base");

    auto p1 = engine.SpawnEntity("OpTic_Scump", EntityType::PLAYER, Team::FACTION_ALPHA, {0, 0, 0});
    auto p2 = engine.SpawnEntity("FaZe_Simp", EntityType::PLAYER, Team::FACTION_BRAVO, {5, 0, 5});

    // Lag compensation check during shooter exchange
    for (uint64_t tick = 0; tick <= 5; ++tick) {
        lagComp.RecordPlayerTransform(*p2, tick * 16, {tick * 1.5f, 1.8f, 10.0f}, {tick * 1.5f, 1.0f, 10.0f});
    }

    Ray shotRay{{3.0f, 1.8f, 0.0f}, {0.0f, 0.0f, 1.0f}};
    std::string hitZone;
    if (lagComp.ValidateHitScan(*p1, *p2, 32, shotRay, hitZone)) {
        std::cout << "[LAG COMP] Shot verified as: " << hitZone << std::endl;
        engine.ApplyDamage(p1->id, p2->id, 100.0f);
    }

    tournament.RecordMapResult(t1.teamId);
    tournament.TriggerTechnicalPause(201, "Headset Disconnected");
    tournament.ResumeMatch();
    tournament.RecordMapResult(t1.teamId);

    // ------------------------------------------------------------------------
    // 3. BATTLE ROYALE SIMULATION
    // ------------------------------------------------------------------------
    engine.SetGameMode(std::make_unique<BattleRoyaleMode>());

    auto brPlayer1 = engine.SpawnEntity("DropMaster", EntityType::PLAYER, Team::NEUTRAL, {100, 0, 100});
    auto brPlayer2 = engine.SpawnEntity("LootGoblin", EntityType::PLAYER, Team::NEUTRAL, {200, 0, 200});

    engine.Tick(1.0f);
    engine.ApplyDamage(brPlayer1->id, brPlayer2->id, 250.0f); // Penetrates armor + health

    // ------------------------------------------------------------------------
    // 4. ZOM BIES MODE SIMULATION
    // ------------------------------------------------------------------------
    engine.SetGameMode(std::make_unique<ZombiesMode>());
    auto survivor = engine.SpawnEntity("Survivor", EntityType::PLAYER, Team::FACTION_ALPHA, {0, 0, 0});

    auto zombiesModePtr = static_cast<ZombiesMode*>(engine.GetActiveMode());
    survivor->points = 3500;
    zombiesModePtr->SpinMysteryBox(*survivor);
    zombiesModePtr->PurchaseJuggernog(*survivor);

    // ------------------------------------------------------------------------
    // 5. SPATIAL MAP STREAMING DEMO
    // ------------------------------------------------------------------------
    mapLoader.LoadMap("Hijacked");
    std::cout << "\n-- Moving player across Hijacked map --" << std::endl;
    mapLoader.UpdateStreamingZones({0.0f, 0.0f, 0.0f});   // Bow zone
    mapLoader.UpdateStreamingZones({0.0f, 0.0f, 160.0f}); // Stern zone

    return 0;
}

#include <iostream>
#include <vector>
#include <memory>

// Base Class for Any COD Mobile Mode
class IGameMode {
public:
    virtual void InitializeMatch() = 0;
    virtual void OnPlayerKill(uint32_t attackerId, uint32_t victimId) = 0;
    virtual void UpdateLogic(float deltaTime) = 0;
    virtual bool IsMatchOver() const = 0;
    virtual ~IGameMode() = default;
};

// --- MULTIPLAYER (Team Deathmatch) ---
class TeamDeathmatchMode : public IGameMode {
private:
    uint32_t team1Kills = 0;
    uint32_t team2Kills = 0;
    const uint32_t SCORE_LIMIT = 50;

public:
    void InitializeMatch() override {
        std::cout << "[MP] Spawning 5v5 Players. Target Score: " << SCORE_LIMIT << std::endl;
    }

    void OnPlayerKill(uint32_t attackerId, uint32_t victimId) override {
        team1Kills++; // Example scoring
        std::cout << "[MP] Player " << attackerId << " killed Player " << victimId 
                  << " | Score: " << team1Kills << "/" << SCORE_LIMIT << std::endl;
    }

    void UpdateLogic(float deltaTime) override {
        // Handle match timers, scorestreak checks, objective ticks
    }

    bool IsMatchOver() const override {
        return team1Kills >= SCORE_LIMIT || team2Kills >= SCORE_LIMIT;
    }
};

// --- BATTLE ROYALE ---
class BattleRoyaleMode : public IGameMode {
private:
    uint32_t playersRemaining_ = 100;
    float safeZoneRadius_ = 1000.0f;

public:
    void InitializeMatch() override {
        std::cout << "[BR] 100 Players dropped from jump plane. Safe zone initialized." << std::endl;
    }

    void OnPlayerKill(uint32_t attackerId, uint32_t victimId) override {
        playersRemaining_--;
        std::cout << "[BR] Player eliminated. " << playersRemaining_ << " players remain." << std::endl;
    }

    void UpdateLogic(float deltaTime) override {
        // Shrink the safe zone over time
        safeZoneRadius_ -= 0.5f * deltaTime;
    }

    bool IsMatchOver() const override {
        return playersRemaining_ <= 1;
    }
};

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

// ============================================================================
// 1. DATA STRUCTURES & NETWORK PROTOCOLS
// ============================================================================

enum class PacketType : uint8_t {
    JOIN_QUEUE = 1,
    MATCH_FOUND = 2,
    PLAYER_MOVEMENT = 3,
    PLAYER_SHOOT = 4,
    DAMAGE_EVENT = 5,
    OBJECTIVE_UPDATE = 6
};

enum class GameModeType {
    CAMPAIGN,
    MULTIPLAYER_TDM,
    MULTIPLAYER_SND,
    BATTLE_ROYALE
};

#pragma pack(push, 1)
struct Header {
    PacketType type;
    uint32_t playerId;
};

struct Vector3 {
    float x, y, z;

    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
    float lengthSquared() const { return x*x + y*y + z*z; }

    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * t;
    }
};

struct MovementPacket {
    Header header;
    Vector3 position;
    float rotationY;
};

struct ShootPacket {
    Header header;
    uint32_t targetPlayerId;
    float damage;
};

struct DamagePacket {
    Header header;
    uint32_t targetPlayerId;
    float remainingHealth;
};
#pragma pack(pop)

struct Ray {
    Vector3 origin;
    Vector3 direction;
};

struct TransformSnapshot {
    uint64_t timestampMs;
    Vector3 headPosition;
    Vector3 chestPosition;
    float headRadius = 0.25f;
    float chestRadius = 0.60f;
};

struct Player {
    uint32_t id;
    std::string username;
    udp::endpoint endpoint;
    Vector3 position{0.0f, 0.0f, 0.0f};
    float rotationY = 0.0f;
    float health = 100.0f;
    
    // Lag Compensation History
    TransformSnapshot currentTransform;
    std::deque<TransformSnapshot> historyBuffer;
};

// ============================================================================
// 2. LAG COMPENSATION & HIT VALIDATION ENGINE
// ============================================================================

class PhysicsGeom {
public:
    static bool RayIntersectsSphere(const Ray& ray, const Vector3& center, float radius) {
        Vector3 oc = ray.origin - center;
        float b = 2.0f * (oc.x * ray.direction.x + oc.y * ray.direction.y + oc.z * ray.direction.z);
        float c = oc.lengthSquared() - (radius * radius);
        float discriminant = (b * b) - (4.0f * c);
        if (discriminant < 0.0f) return false;
        float t = (-b - std::sqrt(discriminant)) / 2.0f;
        return t >= 0.0f;
    }
};

class LagCompensationSystem {
private:
    static constexpr size_t MAX_HISTORY_SNAPSHOTS = 60;
    static constexpr uint64_t MAX_REWIND_MS = 1000;

public:
    void RecordPlayerTransform(Player& player, uint64_t serverTimestampMs, Vector3 headPos, Vector3 chestPos) {
        TransformSnapshot snapshot{serverTimestampMs, headPos, chestPos};
        player.currentTransform = snapshot;
        player.historyBuffer.push_front(snapshot);

        if (player.historyBuffer.size() > MAX_HISTORY_SNAPSHOTS) {
            player.historyBuffer.pop_back();
        }
    }

    TransformSnapshot GetRewoundTransform(const Player& target, uint64_t shotTimestampMs) {
        if (target.historyBuffer.empty()) return target.currentTransform;

        uint64_t latestTime = target.historyBuffer.front().timestampMs;
        if (shotTimestampMs > latestTime) return target.historyBuffer.front();
        if (latestTime - shotTimestampMs > MAX_REWIND_MS) {
            shotTimestampMs = latestTime - MAX_REWIND_MS;
        }

        const TransformSnapshot* newer = nullptr;
        const TransformSnapshot* older = nullptr;

        for (size_t i = 0; i < target.historyBuffer.size(); ++i) {
            if (target.historyBuffer[i].timestampMs <= shotTimestampMs) {
                older = &target.historyBuffer[i];
                if (i > 0) newer = &target.historyBuffer[i - 1];
                break;
            }
        }

        if (!older) return target.historyBuffer.back();
        if (!newer) return *older;

        float frameDelta = static_cast<float>(newer->timestampMs - older->timestampMs);
        float timeDelta = static_cast<float>(shotTimestampMs - older->timestampMs);
        float t = (frameDelta > 0.0f) ? (timeDelta / frameDelta) : 0.0f;

        TransformSnapshot interpolated;
        interpolated.timestampMs = shotTimestampMs;
        interpolated.headPosition = Vector3::Lerp(older->headPosition, newer->headPosition, t);
        interpolated.chestPosition = Vector3::Lerp(older->chestPosition, newer->chestPosition, t);
        return interpolated;
    }

    bool ValidateHitScan(const Player& target, uint64_t clientRenderTimeMs, const Ray& shotRay, std::string& outHitZone) {
        TransformSnapshot rewoundState = GetRewoundTransform(target, clientRenderTimeMs);

        if (PhysicsGeom::RayIntersectsSphere(shotRay, rewoundState.headPosition, rewoundState.headRadius)) {
            outHitZone = "HEADSHOT";
            return true;
        }
        if (PhysicsGeom::RayIntersectsSphere(shotRay, rewoundState.chestPosition, rewoundState.chestRadius)) {
            outHitZone = "BODYSHOT";
            return true;
        }
        outHitZone = "MISS";
        return false;
    }
};

// ============================================================================
// 3. CAMPAIGN MODE ENGINE (multi-Player / Co-op Story)
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

struct Objective {
    std::string id;
    std::string description;
    bool isCompleted = false;
};

struct EnemyAI {
    uint32_t id;
    Vector3 position;
    float health = 100.0f;
    std::string state = "PATROL"; // PATROL, ENGAGE, COVER
};

class CampaignEngine {
private:
    std::string currentMissionName_;
    std::vector<Objective> missionObjectives_;
    std::vector<EnemyAI> activeEnemies_;
    bool isMissionFailed_ = false;

public:
    void LoadMission(const std::string& missionName) {
        currentMissionName_ = missionName;
        missionObjectives_.clear();
        activeEnemies_.clear();

        std::cout << "\n[CAMPAIGN] Loading Story Mission: " << currentMissionName_ << std::endl;

        if (missionName == "Operation Blackout") {
            missionObjectives_.push_back({"obj_1", "Infiltrate the Radar Base", false});
            missionObjectives_.push_back({"obj_2", "Upload Virus to Server Tower", false});
            missionObjectives_.push_back({"obj_3", "Exfiltrate via Chopper", false});

            activeEnemies_.push_back({501, {10.0f, 0.0f, 20.0f}, 100.0f, "PATROL"});
            activeEnemies_.push_back({502, {15.0f, 0.0f, 25.0f}, 100.0f, "PATROL"});
        }
    }

    void CompleteObjective(const std::string& objId) {
        for (auto& obj : missionObjectives_) {
            if (obj.id == objId && !obj.isCompleted) {
                obj.isCompleted = true;
                std::cout << "[CAMPAIGN OBJECTIVE COMPLETE] " << obj.description << std::endl;
            }
        }
    }

    void UpdateAIState(float playerDistance, uint32_t enemyId) {
        for (auto& enemy : activeEnemies_) {
            if (enemy.id == enemyId) {
                if (playerDistance < 15.0f) {
                    enemy.state = "ENGAGE";
                    std::cout << "[CAMPAIGN AI] Enemy " << enemyId << " spotted player! State: ENGAGING" << std::endl;
                }
            }
        }
    }
};

// ============================================================================
// 4. MULTIPLAYER ENGINE (Competitive Systems & CDL Rulesets)
// ============================================================================

enum class Team { NONE, ALPHA, BRAVO };

struct MPPlayer {
    uint32_t id;
    Team team = Team::NONE;
    uint32_t kills = 0;
    uint32_t deaths = 0;
    uint32_t streak = 0;
};

class MultiplayerEngine {
private:
    uint32_t scoreLimit_ = 75;
    std::unordered_map<Team, uint32_t> teamScores_;
    std::unordered_map<uint32_t, std::shared_ptr<MPPlayer>> mpPlayers_;

public:
    MultiplayerEngine() {
        teamScores_[Team::ALPHA] = 0;
        teamScores_[Team::BRAVO] = 0;
    }

    void AddPlayer(uint32_t id, Team team) {
        auto player = std::make_shared<MPPlayer>();
        player->id = id;
        player->team = team;
        mpPlayers_[id] = player;
    }

    void OnPlayerKilled(uint32_t killerId, uint32_t victimId) {
        auto killer = mpPlayers_[killerId];
        auto victim = mpPlayers_[victimId];

        if (!killer || !victim || killer->team == victim->team) return; // Prevent friendly fire points

        killer->kills++;
        killer->streak++;
        victim->deaths++;
        victim->streak = 0;

        teamScores_[killer->team]++;

        std::cout << "[MULTIPLAYER TDM] Player " << killerId << " killed Player " << victimId 
                  << " | Score [Alpha: " << teamScores_[Team::ALPHA] 
                  << " - Bravo: " << teamScores_[Team::BRAVO] << "]" << std::endl;

        // Killstreak evaluation
        if (killer->streak == 3) std::cout << "  -> [STREAK UNLOCKED] UAV Available!" << std::endl;
        if (killer->streak == 5) std::cout << "  -> [STREAK UNLOCKED] Cruise Missile Ready!" << std::endl;
        if (killer->streak == 10) std::cout << "  -> [STREAK UNLOCKED] Chopper Gunner Online!" << std::endl;
    }
};

// ============================================================================
// 5. SPATIAL MAP STREAMING ARCHITECTURE
// ============================================================================

struct MapNode {
    std::string mapName;
    std::string modeSupport; // "Campaign, Multiplayer, BR"
    uint32_t maxPlayers;
    float boundaryRadius;
};

class SpatialMapLoader {
private:
    std::unordered_map<std::string, MapNode> mapRegistry_;
    std::string activeMap_ = "";

public:
    SpatialMapLoader() {
        mapRegistry_["Nuketown"]   = {"Nuketown", "Multiplayer", 12, 250.0f};
        mapRegistry_["Hijacked"]   = {"Hijacked", "Multiplayer", 12, 180.0f};
        mapRegistry_["Expressway"] = {"Expressway", "Multiplayer", 12, 320.0f};
        mapRegistry_["Crash"]      = {"Crash", "Multiplayer, Campaign", 16, 350.0f};
        mapRegistry_["Urzikstan"]  = {"Urzikstan", "Campaign, Battle Royale", 100, 4000.0f};
    }

    bool LoadMap(const std::string& mapName) {
        if (mapRegistry_.find(mapName) == mapRegistry_.end()) {
            std::cerr << "[MAP LOADER ERROR] Map " << mapName << " not found!" << std::endl;
            return false;
        }

        activeMap_ = mapName;
        MapNode& node = mapRegistry_[mapName];

        std::cout << "\n================ MAP STREAMING ENGINE ================" << std::endl;
        std::cout << " Loaded Map: " << node.mapName << std::endl;
        std::cout << " Supported Modes: " << node.modeSupport << std::endl;
        std::cout << " Boundary Radius: " << node.boundaryRadius << "m" << std::endl;
        std::cout << "======================================================\n" << std::endl;
        return true;
    }
};

// ============================================================================
// 6. MAIN SERVER CORE RUNNER
// ============================================================================

class GameServer {
private:
    boost::asio::io_context io_context_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;

public:
    GameServer(unsigned short port)
        : socket_(io_context_, udp::endpoint(udp::v4(), port)) {
        std::cout << "[NETWORK] Server active on UDP Port " << port << std::endl;
    }
};

int main() {
    std::cout << "====================================================" << std::endl;
    std::cout << "  AAA FPS ENGINE CORE (Campaign & Multiplayer)     " << std::endl;
    std::cout << "====================================================" << std::endl;

    // 1. Map Loading Setup
    SpatialMapLoader mapLoader;
    mapLoader.LoadMap("Crash");

    // 2. Campaign Mode Simulation
    CampaignEngine campaign;
    campaign.LoadMission("Operation Blackout");
    campaign.UpdateAIState(10.0f, 501); // Enemy 501 spots player nearby
    campaign.CompleteObjective("obj_1");

    // 3. Multiplayer Mode Simulation
    std::cout << "\n--- STARTING MULTIPLAYER MATCH ---" << std::endl;
    MultiplayerEngine mp;
    mp.AddPlayer(101, Team::ALPHA);
    mp.AddPlayer(202, Team::BRAVO);

    mp.OnPlayerKilled(101, 202);
    mp.OnPlayerKilled(101, 202);
    mp.OnPlayerKilled(101, 202); // 3-kill streak triggers UAV

    // 4. Lag-Compensated Combat Hit Verification Demo
    std::cout << "\n--- LAG COMPENSATION HITSCAN CHECK ---" << std::endl;
    LagCompensationSystem lagComp;
    Player shooter{101};
    Player target{202};

    // Record historical positions for target moving across frame
    lagComp.RecordPlayerTransform(target, 0,  {0.0f, 1.8f, 10.0f}, {0.0f, 1.0f, 10.0f});
    lagComp.RecordPlayerTransform(target, 32, {3.0f, 1.8f, 10.0f}, {3.0f, 1.0f, 10.0f});

    // Shooter fires ray at coordinates where target was visible on client screen at 32ms
    Ray shotRay{{3.0f, 1.8f, 0.0f}, {0.0f, 0.0f, 1.0f}};
    std::string hitZone;

    if (lagComp.ValidateHitScan(target, 32, shotRay, hitZone)) {
        std::cout << "[HITSCAN VALIDATED] Server Rewind Confirmed: " << hitZone << "!" << std::endl;
    }

    return 0;
}

#include "raylib.h"
#include <math.h>

int main() {
    // 1. Initialize Window/Canvas
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "3D Tactical Lobby - C++ Wasm");

    // 2. Define 3D Camera Settings
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 3.0f, 6.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };   // Looking at center podium
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };       // Up vector
    camera.fovy = 45.0f;                             // Field of view
    camera.projection = CAMERA_PERSPECTIVE;

    float rotationAngle = 0.0f;

    SetTargetFPS(60); // Target 60 FPS in browser

    // Main Game/Render Loop
    while (!WindowShouldClose()) {
        // --- UPDATE ---
        rotationAngle += 0.8f; // Rotate central operator platform

        // --- DRAW ---
        BeginDrawing();
            ClearBackground((Color){ 5, 8, 17, 255 }); // Dark Tactical Background

            // Begin 3D Rendering Context
            BeginMode3D(camera);

                // Draw Grid Floor (Holographic Tactical Grid)
                DrawGrid(20, 1.0f);

                // Draw Central Pedestal / Podium
                DrawCylinder((Vector3){ 0.0f, -0.1f, 0.0f }, 2.5f, 2.5f, 0.2f, 32, (Color){ 17, 24, 39, 255 });
                DrawCylinderWires((Vector3){ 0.0f, -0.1f, 0.0f }, 2.55f, 2.55f, 0.22f, 16, (Color){ 0, 240, 255, 255 });

                // Draw Central Rotating Operator Model (Constructed from 3D Shapes)
                // Torso & Head
                DrawCube((Vector3){ 0.0f, 1.2f, 0.0f }, 0.8f, 1.0f, 0.5f, (Color){ 55, 65, 81, 255 });
                DrawSphere((Vector3){ 0.0f, 2.0f, 0.0f }, 0.35f, (Color){ 31, 41, 55, 255 });
                
                // Glowing Cyber Visor
                DrawCube((Vector3){ 0.0f, 2.05f, 0.3f }, 0.4f, 0.1f, 0.1f, (Color){ 0, 240, 255, 255 });

                // Rotating Tactical Weapon/Shield Marker
                Vector3 orbitPos = {
                    cosf(rotationAngle * 0.02f) * 1.5f,
                    1.2f,
                    sinf(rotationAngle * 0.02f) * 1.5f
                };
                DrawCube(orbitPos, 0.2f, 0.2f, 0.2f, (Color){ 255, 69, 0, 255 });

            EndMode3D();

            // --- 2D HUD OVERLAY (UI drawn over 3D scene) ---
            DrawText("OPERATOR LOBBY (C++ / WebAssembly)", 20, 20, 22, (Color){ 0, 240, 255, 255 });
            DrawText("LEVEL 150 | MASTER V", 20, 50, 16, RAYWHITE);

            // Draw "DEPLOY / START MATCH" Button UI Box
            DrawRectangleLines(screenWidth - 260, screenHeight - 90, 220, 60, (Color){ 255, 69, 0, 255 });
            DrawRectangle(screenWidth - 258, screenHeight - 88, 216, 56, (Color){ 255, 69, 0, 100 });
            DrawText("START MATCH", screenWidth - 225, screenHeight - 70, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class MeshBuilder {
public:
    static std::vector<Vertex> MakeCube(float w, float h, float d) {
        std::vector<Vertex> v;
        float hw = w / 2, hh = h / 2, hd = d / 2;
        v.push_back({{-hw, -hh, -hd}, {0, 0, -1}, {0, 0}});
        v.push_back({{hw, -hh, -hd}, {0, 0, -1}, {1, 0}});
        v.push_back({{hw, hh, -hd}, {0, 0, -1}, {1, 1}});
        v.push_back({{-hw, hh, -hd}, {0, 0, -1}, {0, 1}});
        return v;
    }

    static std::vector<Vertex> MakeCylinder(float radius, float length, int segments) {
        std::vector<Vertex> v;
        for (int i = 0; i < segments; ++i) {
            float theta = (float)i / segments * 2.0f * 3.14159f;
            float x = radius * cos(theta);
            float y = radius * sin(theta);
            v.push_back({{x, y, 0}, {x, y, 0}, {0, 0}});
            v.push_back({{x, y, length}, {x, y, 0}, {1, 0}});
        }
        return v;
    }
};

class WeaponModel {
protected:
    GLuint vao, vbo;
    std::vector<Vertex> vertices;

public:
    virtual void Build() = 0;

    void Upload() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void Draw() {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
};

class AssaultRifleModel : public WeaponModel {
public:
    void Build() override {
        auto body = MeshBuilder::MakeCube(1.0f, 0.3f, 0.5f);
        auto barrel = MeshBuilder::MakeCylinder(0.05f, 0.8f, 16);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());
    }
};


class SMGModel : public WeaponModel {
public:
    void Build() override {
        auto body = MeshBuilder::MakeCube(0.8f, 0.25f, 0.4f);
        auto barrel = MeshBuilder::MakeCylinder(0.04f, 0.6f, 12);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());
    }
};

class LMGModel : public WeaponModel {
public:
    void Build() override {
        auto body = MeshBuilder::MakeCube(1.2f, 0.35f, 0.6f);
        auto barrel = MeshBuilder::MakeCylinder(0.06f, 1.0f, 20);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());
    }
};

class SniperModel : public WeaponModel {
public:
    void Build() override {
        auto body = MeshBuilder::MakeCube(1.4f, 0.25f, 0.5f);
        auto barrel = MeshBuilder::MakeCylinder(0.05f, 1.5f, 24);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());
    }
};

class ShotgunModel : public WeaponModel {
public:
    void Build() override {
        auto body = MeshBuilder::MakeCube(1.0f, 0.3f, 0.5f);
        auto barrel = MeshBuilder::MakeCylinder(0.07f, 0.9f, 18);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());
    }
};

class PistolModel : public WeaponModel {
public:
    void Build() override {
        auto body = MeshBuilder::MakeCube(0.5f, 0.25f, 0.3f);
        auto barrel = MeshBuilder::MakeCylinder(0.03f, 0.4f, 10);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());
    }
};

class LauncherModel : public WeaponModel {
public:
    void Build() override {
        auto tube = MeshBuilder::MakeCylinder(0.1f, 1.5f, 24);
        auto warhead = MeshBuilder::MakeCylinder(0.15f, 0.3f, 24);
        vertices.insert(vertices.end(), tube.begin(), tube.end());
        vertices.insert(vertices.end(), warhead.begin(), warhead.end());
    }
};

class MeleeModel : public WeaponModel {
public:
    void Build() override {
        auto blade = MeshBuilder::MakeCube(0.05f, 0.5f, 0.02f);
        auto handle = MeshBuilder::MakeCube(0.08f, 0.2f, 0.05f);
        vertices.insert(vertices.end(), blade.begin(), blade.end());
        vertices.insert(vertices.end(), handle.begin(), handle.end());
    }
};

enum class WeaponCategory { ASSAULT, SMG, LMG, SNIPER, SHOTGUN, PISTOL, LAUNCHER, MELEE };

class WeaponFactory {
public:
    static std::unique_ptr<WeaponModel> Create(WeaponCategory type) {
        switch (type) {
            case WeaponCategory::ASSAULT: return std::make_unique<AssaultRifleModel>();
            case WeaponCategory::SMG: return std::make_unique<SMGModel>();
            case WeaponCategory::LMG: return std::make_unique<LMGModel>();
            case WeaponCategory::SNIPER: return std::make_unique<SniperModel>();
            case WeaponCategory::SHOTGUN: return std::make_unique<ShotgunModel>();
            case WeaponCategory::PISTOL: return std::make_unique<PistolModel>();
            case WeaponCategory::LAUNCHER: return std::make_unique<LauncherModel>();
            case WeaponCategory::MELEE: return std::make_unique<MeleeModel>();
        }
        return nullptr;
    }
};

int main() {
    auto m4a1 = WeaponFactory::Create(WeaponCategory::ASSAULT);
    m4a1->Build();
    m4a1->Upload();

    auto mp5 = WeaponFactory::Create(WeaponCategory::SMG);
    mp5->Build();
    mp5->Upload();

    auto ax50 = WeaponFactory::Create(WeaponCategory::SNIPER);
    ax50->Build();
    ax50->Upload();

    std::cout << "[ENGINE] All weapon models built and uploaded to GPU." << std::endl;
    return 0;
}

#version 330 core
layout(location = 0) in vec3 aPos;      // Position
layout(location = 1) in vec3 aNormal;   // Normal
layout(location = 2) in vec2 aTexCoord; // Texture coordinates

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D diffuseMap;   // Polymer texture
uniform sampler2D metalMap;     // Metal texture
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Basic Phong lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // Specular highlight
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // Mix metal and polymer textures
    vec4 polymerColor = texture(diffuseMap, TexCoord);
    vec4 metalColor = texture(metalMap, TexCoord);
    vec4 baseColor = mix(polymerColor, metalColor, 0.5);

    vec3 lighting = (0.2 + diff + spec) * baseColor.rgb;
    FragColor = vec4(lighting, 1.0);
}

// Example: M4A1 body cube with texture mapping
vertices.push_back({{-0.5f, -0.2f, -0.5f}, {0,0,-1}, {0.0f, 0.0f}});
vertices.push_back({{0.5f, -0.2f, -0.5f}, {0,0,-1}, {1.0f, 0.0f}});
vertices.push_back({{0.5f,  0.2f, -0.5f}, {0,0,-1}, {1.0f, 1.0f}});
vertices.push_back({{-0.5f,  0.2f, -0.5f}, {0,0,-1}, {0.0f, 1.0f}});

GLuint polymerTex = LoadTexture("Textures/M4A1_polymer.png");
GLuint metalTex   = LoadTexture("Textures/M4A1_metal.png");

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, polymerTex);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, metalTex);

class Attachment {
public:
    virtual void Build(std::vector<Vertex>& vertices) = 0;
};

class Scope : public Attachment {
public:
    void Build(std::vector<Vertex>& vertices) override {
        auto tube = MeshBuilder::MakeCylinder(0.05f, 0.3f, 16);
        vertices.insert(vertices.end(), tube.begin(), tube.end());
    }
};

class Silencer : public Attachment {
public:
    void Build(std::vector<Vertex>& vertices) override {
        auto tube = MeshBuilder::MakeCylinder(0.06f, 0.5f, 20);
        vertices.insert(vertices.end(), tube.begin(), tube.end());
    }
};

class Grip : public Attachment {
public:
    void Build(std::vector<Vertex>& vertices) override {
        auto handle = MeshBuilder::MakeCube(0.1f, 0.3f, 0.1f);
        vertices.insert(vertices.end(), handle.begin(), handle.end());
    }
};

class AssaultRifleModel : public WeaponModel {
private:
    std::vector<std::unique_ptr<Attachment>> attachments;
public:
    void AddAttachment(std::unique_ptr<Attachment> att) {
        attachments.push_back(std::move(att));
    }

    void Build() override {
        auto body = MeshBuilder::MakeCube(1.0f, 0.3f, 0.5f);
        auto barrel = MeshBuilder::MakeCylinder(0.05f, 0.8f, 16);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());

        for (auto& att : attachments) {
            att->Build(vertices);
        }
    }
};

int main() {
    AssaultRifleModel m4a1;
    m4a1.AddAttachment(std::make_unique<Scope>());
    m4a1.AddAttachment(std::make_unique<Silencer>());
    m4a1.AddAttachment(std::make_unique<Grip>());
    m4a1.Build();
    m4a1.Upload();

    std::cout << "[ENGINE] M4A1 built with scope, silencer, and grip." << std::endl;
    return 0;
}

GLuint polymerTex = LoadTexture("Textures/M4A1_polymer.png");
GLuint metalTex   = LoadTexture("Textures/M4A1_metal.png");
glUniform1i(glGetUniformLocation(shaderProgram, "diffuseMap"), 0);
glUniform1i(glGetUniformLocation(shaderProgram, "metalMap"), 1);

AssaultRifleModel m4a1;
m4a1.AddAttachment(std::make_unique<Scope>());
m4a1.AddAttachment(std::make_unique<Silencer>());
m4a1.AddAttachment(std::make_unique<Grip>());
m4a1.Build();
m4a1.Upload();

int main() {
    // Initialize OpenGL context here...

    // Create weapons
    auto m4a1 = WeaponFactory::Create(WeaponCategory::ASSAULT);
    m4a1->Build();
    m4a1->Upload();

    auto mp5 = WeaponFactory::Create(WeaponCategory::SMG);
    mp5->Build();
    mp5->Upload();

    auto ax50 = WeaponFactory::Create(WeaponCategory::SNIPER);
    ax50->Build();
    ax50->Upload();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind shader
        glUseProgram(shaderProgram);

        // Draw weapons
        m4a1->Draw();
        mp5->Draw();
        ax50->Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

class AssaultRifleModel : public WeaponModel {
private:
    GLuint polymerTex, metalTex;
    std::vector<std::unique_ptr<Attachment>> attachments;

public:
    AssaultRifleModel() {
        polymerTex = LoadTexture("Textures/Assault_polymer.png");
        metalTex   = LoadTexture("Textures/Assault_metal.png");
    }

    void AddAttachment(std::unique_ptr<Attachment> att) {
        attachments.push_back(std::move(att));
    }

    void Build() override {
        auto body = MeshBuilder::MakeCube(1.0f, 0.3f, 0.5f);
        auto barrel = MeshBuilder::MakeCylinder(0.05f, 0.8f, 16);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());

        for (auto& att : attachments) att->Build(vertices);
    }

    void BindTextures() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, polymerTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metalTex);
    }
};

class SMGModel : public WeaponModel {
private:
    GLuint polymerTex, metalTex;
    std::vector<std::unique_ptr<Attachment>> attachments;

public:
    SMGModel() {
        polymerTex = LoadTexture("Textures/SMG_polymer.png");
        metalTex   = LoadTexture("Textures/SMG_metal.png");
    }

    void AddAttachment(std::unique_ptr<Attachment> att) { attachments.push_back(std::move(att)); }

    void Build() override {
        auto body = MeshBuilder::MakeCube(0.8f, 0.25f, 0.4f);
        auto barrel = MeshBuilder::MakeCylinder(0.04f, 0.6f, 12);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());

        for (auto& att : attachments) att->Build(vertices);
    }

    void BindTextures() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, polymerTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metalTex);
    }
};

class ExtendedMag : public Attachment {
public:
    void Build(std::vector<Vertex>& vertices) override {
        auto mag = MeshBuilder::MakeCube(0.2f, 0.5f, 0.1f);
        vertices.insert(vertices.end(), mag.begin(), mag.end());
    }
};

class RedDotSight : public Attachment {
public:
    void Build(std::vector<Vertex>& vertices) override {
        auto sight = MeshBuilder::MakeCube(0.1f, 0.1f, 0.1f);
        vertices.insert(vertices.end(), sight.begin(), sight.end());
    }
};

int main() {
    AssaultRifleModel m4a1;
    m4a1.AddAttachment(std::make_unique<Scope>());
    m4a1.AddAttachment(std::make_unique<Silencer>());
    m4a1.AddAttachment(std::make_unique<Grip>());
    m4a1.Build();
    m4a1.Upload();
    m4a1.BindTextures();

    SMGModel mp5;
    mp5.AddAttachment(std::make_unique<RedDotSight>());
    mp5.Build();
    mp5.Upload();
    mp5.BindTextures();

    std::cout << "[ENGINE] Weapons built with default textures and attachments." << std::endl;
    return 0;
}

enum class WeaponAnimation { FIRE, RELOAD, INSPECT };

class WeaponAnimator {
public:
    void Play(WeaponAnimation anim) {
        switch (anim) {
            case WeaponAnimation::FIRE:
                std::cout << "[ANIM] Playing fire animation..." << std::endl;
                // Trigger barrel recoil, muzzle flash
                break;
            case WeaponAnimation::RELOAD:
                std::cout << "[ANIM] Playing reload animation..." << std::endl;
                // Trigger magazine detach/attach, hand movement
                break;
            case WeaponAnimation::INSPECT:
                std::cout << "[ANIM] Playing inspect animation..." << std::endl;
                // Rotate weapon, show details
                break;
        }
    }
};

class AssaultRifleModel : public WeaponModel {
private:
    WeaponAnimator animator;
    GLuint polymerTex, metalTex;
    std::vector<std::unique_ptr<Attachment>> attachments;

public:
    AssaultRifleModel() {
        polymerTex = LoadTexture("Textures/Assault_polymer.png");
        metalTex   = LoadTexture("Textures/Assault_metal.png");
    }

    void AddAttachment(std::unique_ptr<Attachment> att) {
        attachments.push_back(std::move(att));
    }

    void Build() override {
        auto body = MeshBuilder::MakeCube(1.0f, 0.3f, 0.5f);
        auto barrel = MeshBuilder::MakeCylinder(0.05f, 0.8f, 16);
        vertices.insert(vertices.end(), body.begin(), body.end());
        vertices.insert(vertices.end(), barrel.begin(), barrel.end());

        for (auto& att : attachments) att->Build(vertices);
    }

    void PlayAnimation(WeaponAnimation anim) {
        animator.Play(anim);
    }
};

int main() {
    AssaultRifleModel m4a1;
    m4a1.AddAttachment(std::make_unique<Scope>());
    m4a1.AddAttachment(std::make_unique<Silencer>());
    m4a1.Build();
    m4a1.Upload();

    // Gameplay events
    m4a1.PlayAnimation(WeaponAnimation::FIRE);
    m4a1.PlayAnimation(WeaponAnimation::RELOAD);
    m4a1.PlayAnimation(WeaponAnimation::INSPECT);

    return 0;
}

struct WeaponState {
    int ammoInMag;
    int magSize;
    int reserveAmmo;
    bool isReloading = false;
};

class AssaultRifleModel : public WeaponModel {
private:
    WeaponAnimator animator;
    WeaponState state;

public:
    AssaultRifleModel() {
        state.magSize = 30;
        state.ammoInMag = 30;
        state.reserveAmmo = 90;
    }

    void Fire() {
        if (state.isReloading) return;
        if (state.ammoInMag > 0) {
            state.ammoInMag--;
            animator.Play(WeaponAnimation::FIRE);
            std::cout << "[GAME] Fired! Ammo left: " << state.ammoInMag << std::endl;
        } else {
            std::cout << "[GAME] Empty magazine! Reload required." << std::endl;
        }
    }

    void Reload() {
        if (state.isReloading) return;
        if (state.reserveAmmo > 0) {
            state.isReloading = true;
            animator.Play(WeaponAnimation::RELOAD);
            int needed = state.magSize - state.ammoInMag;
            int toLoad = std::min(needed, state.reserveAmmo);
            state.ammoInMag += toLoad;
            state.reserveAmmo -= toLoad;
            state.isReloading = false;
            std::cout << "[GAME] Reloaded. Ammo in mag: " << state.ammoInMag 
                      << " | Reserve: " << state.reserveAmmo << std::endl;
        }
    }

    void Inspect() {
        animator.Play(WeaponAnimation::INSPECT);
        std::cout << "[GAME] Inspecting weapon... Input paused." << std::endl;
    }
};

int main() {
    AssaultRifleModel m4a1;
    m4a1.Build();
    m4a1.Upload();

    bool running = true;
    while (running) {
        char input;
        std::cin >> input;

        switch (input) {
            case 'f': // fire
                m4a1.Fire();
                break;
            case 'r': // reload
                m4a1.Reload();
                break;
            case 'i': // inspect
                m4a1.Inspect();
                break;
            case 'q': // quit
                running = false;
                break;
        }
    }

    return 0;
}

enum class PacketType : uint8_t {
    FIRE_EVENT = 10,
    RELOAD_EVENT = 11,
    INSPECT_EVENT = 12
};

struct WeaponActionPacket {
    Header header;       // includes playerId + type
    uint32_t weaponId;   // which gun
    uint32_t ammoInMag;  // current ammo
    uint32_t reserveAmmo;
};

class AssaultRifleModel : public WeaponModel {
private:
    WeaponAnimator animator;
    WeaponState state;
    udp::socket& socket;
    udp::endpoint serverEndpoint;

public:
    AssaultRifleModel(udp::socket& sock, udp::endpoint serverEp)
        : socket(sock), serverEndpoint(serverEp) {
        state.magSize = 30;
        state.ammoInMag = 30;
        state.reserveAmmo = 90;
    }

    void Fire(uint32_t playerId) {
        if (state.ammoInMag > 0) {
            state.ammoInMag--;
            animator.Play(WeaponAnimation::FIRE);

            WeaponActionPacket pkt;
            pkt.header = {PacketType::FIRE_EVENT, playerId};
            pkt.weaponId = 1; // M4A1
            pkt.ammoInMag = state.ammoInMag;
            pkt.reserveAmmo = state.reserveAmmo;

            socket.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), serverEndpoint);
        }
    }

    void Reload(uint32_t playerId) {
        animator.Play(WeaponAnimation::RELOAD);
        int needed = state.magSize - state.ammoInMag;
        int toLoad = std::min(needed, state.reserveAmmo);
        state.ammoInMag += toLoad;
        state.reserveAmmo -= toLoad;

        WeaponActionPacket pkt;
        pkt.header = {PacketType::RELOAD_EVENT, playerId};
        pkt.weaponId = 1;
        pkt.ammoInMag = state.ammoInMag;
        pkt.reserveAmmo = state.reserveAmmo;

        socket.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), serverEndpoint);
    }

    void Inspect(uint32_t playerId) {
        animator.Play(WeaponAnimation::INSPECT);

        WeaponActionPacket pkt;
        pkt.header = {PacketType::INSPECT_EVENT, playerId};
        pkt.weaponId = 1;
        pkt.ammoInMag = state.ammoInMag;
        pkt.reserveAmmo = state.reserveAmmo;

        socket.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), serverEndpoint);
    }
};

void handle_packet(std::size_t bytes_transferred) {
    Header* header = reinterpret_cast<Header*>(recv_buffer_.data());

    switch (header->type) {
        case PacketType::FIRE_EVENT:
        case PacketType::RELOAD_EVENT:
        case PacketType::INSPECT_EVENT: {
            auto action = *reinterpret_cast<WeaponActionPacket*>(recv_buffer_.data());
            broadcast_action(action);
            break;
        }
    }
}

void broadcast_action(const WeaponActionPacket& pkt) {
    std::string room_id = player_to_room_map_[pkt.header.playerId];
    auto room = active_rooms_[room_id];

    for (const auto& [id, p] : room->players) {
        socket_.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), p->endpoint);
    }
}

while (running) {
    char input;
    std::cin >> input;

    switch (input) {
        case 'f': m4a1.Fire(playerId); break;
        case 'r': m4a1.Reload(playerId); break;
        case 'i': m4a1.Inspect(playerId); break;
    }
}
#include <iostream>
#include <unordered_map>
#include <string>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum class Action { FIRE, RELOAD, ZOOM };

class InputManager {
private:
    std::unordered_map<Action, int> keyBindings;
public:
    InputManager() {
        keyBindings[Action::FIRE] = GLFW_KEY_F;
        keyBindings[Action::RELOAD] = GLFW_KEY_R;
        keyBindings[Action::ZOOM] = GLFW_KEY_Z;
    }

    void SetKey(Action action, int key) { keyBindings[action] = key; }
    int GetKey(Action action) const { return keyBindings.at(action); }

    void HandleInput(GLFWwindow* window) {
        if (glfwGetKey(window, keyBindings[Action::FIRE]) == GLFW_PRESS)
            std::cout << "Firing weapon!\n";
        if (glfwGetKey(window, keyBindings[Action::RELOAD]) == GLFW_PRESS)
            std::cout << "Reloading...\n";
        if (glfwGetKey(window, keyBindings[Action::ZOOM]) == GLFW_PRESS)
            std::cout << "Zooming scope...\n";
    }
};

void DrawConsole(InputManager& input) {
    ImGui::Begin("Customizable Console");
    ImGui::Text("Rebind Controls:");
    static int fireKey = input.GetKey(Action::FIRE);
    static int reloadKey = input.GetKey(Action::RELOAD);
    static int zoomKey = input.GetKey(Action::ZOOM);

    ImGui::InputInt("Fire Key", &fireKey);
    ImGui::InputInt("Reload Key", &reloadKey);
    ImGui::InputInt("Zoom Key", &zoomKey);

    if (ImGui::Button("Apply")) {
        input.SetKey(Action::FIRE, fireKey);
        input.SetKey(Action::RELOAD, reloadKey);
        input.SetKey(Action::ZOOM, zoomKey);
    }
    ImGui::End();
}

#include <iostream>
#include <string>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class ChatSystem {
private:
    udp::socket socket_;
    udp::endpoint serverEndpoint_;
public:
    ChatSystem(boost::asio::io_context& io, const std::string& host, int port)
        : socket_(io, udp::endpoint(udp::v4(), 0)) {
        serverEndpoint_ = udp::endpoint(boost::asio::ip::address::from_string(host), port);
    }

    void SendMessage(const std::string& msg, uint32_t playerId) {
        std::string packet = "[Player " + std::to_string(playerId) + "]: " + msg;
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void ReceiveMessages() {
        char buffer[512];
        udp::endpoint sender;
        while (true) {
            size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender);
            std::cout << std::string(buffer, len) << std::endl;
        }
    }
};

#include <portaudio.h>

void CaptureAudio() {
    PaStream* stream;
    Pa_Initialize();
    Pa_OpenDefaultStream(&stream, 1, 0, paInt16, 44100, 256, nullptr, nullptr);
    Pa_StartStream(stream);

    short buffer[256];
    while (true) {
        Pa_ReadStream(stream, buffer, 256);
        // Encode with Opus, send via UDP
    }
}

#include <boost/asio.hpp>
#include <unordered_map>
#include <iostream>
using boost::asio::ip::udp;

class ChatServer {
    udp::socket socket_;
    std::unordered_map<std::string, udp::endpoint> players;
public:
    ChatServer(boost::asio::io_context& io, int port)
        : socket_(io, udp::endpoint(udp::v4(), port)) {}

    void Start() {
        while (true) {
            char buffer[1024];
            udp::endpoint sender;
            size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender);

            std::string msg(buffer, len);
            players[sender.address().to_string()] = sender;

            for (auto& [id, ep] : players) {
                socket_.send_to(boost::asio::buffer(msg), ep);
            }
        }
    }
};

#include <boost/asio.hpp>
#include <portaudio.h>
#include <opus/opus.h>
using boost::asio::ip::udp;

class ChatClient {
    udp::socket socket_;
    udp::endpoint serverEndpoint_;
public:
    ChatClient(boost::asio::io_context& io, const std::string& host, int port)
        : socket_(io, udp::endpoint(udp::v4(), 0)) {
        serverEndpoint_ = udp::endpoint(boost::asio::ip::address::from_string(host), port);
    }

    void SendText(const std::string& msg) {
        socket_.send_to(boost::asio::buffer(msg), serverEndpoint_);
    }

    void CaptureAndSendVoice() {
        PaStream* stream;
        Pa_Initialize();
        Pa_OpenDefaultStream(&stream, 1, 0, paInt16, 48000, 256, nullptr, nullptr);
        Pa_StartStream(stream);

        short buffer[256];
        while (true) {
            Pa_ReadStream(stream, buffer, 256);
            // Encode with Opus here
            socket_.send_to(boost::asio::buffer(buffer, sizeof(buffer)), serverEndpoint_);
        }
    }

    void ReceiveLoop() {
        char buffer[1024];
        udp::endpoint sender;
        while (true) {
            size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender);
            std::cout << std::string(buffer, len) << std::endl;
            // If voice packet → decode with Opus and play via PortAudio
        }
    }
};

struct PlayerInfo {
    uint32_t id;
    std::string name;
    int team;          // 0 = unassigned, 1 = Team A, 2 = Team B
    bool ready = false;
    udp::endpoint endpoint;
};

struct Lobby {
    std::string lobbyId;
    std::unordered_map<uint32_t, PlayerInfo> players;
};

class LobbyServer {
    std::unordered_map<std::string, Lobby> lobbies;
    udp::socket socket_;
public:
    LobbyServer(boost::asio::io_context& io, int port)
        : socket_(io, udp::endpoint(udp::v4(), port)) {}

    void HandlePacket(const std::string& msg, udp::endpoint sender) {
        // Parse packet: JOIN, CHAT, READY, TEAM_SELECT
        // Update lobby state
        // Broadcast updated state to all players in lobby
    }

    void BroadcastLobbyState(const Lobby& lobby) {
        for (auto& [id, p] : lobby.players) {
            std::string state = "Lobby " + lobby.lobbyId +
                " | Player " + p.name +
                " | Team " + std::to_string(p.team) +
                " | Ready: " + (p.ready ? "Yes" : "No");
            socket_.send_to(boost::asio::buffer(state), p.endpoint);
        }
    }
};

class LobbyClient {
    udp::socket socket_;
    udp::endpoint serverEndpoint_;
    uint32_t playerId;
    std::string playerName;
public:
    LobbyClient(boost::asio::io_context& io, const std::string& host, int port)
        : socket_(io, udp::endpoint(udp::v4(), 0)) {
        serverEndpoint_ = udp::endpoint(boost::asio::ip::address::from_string(host), port);
    }

    void JoinLobby(const std::string& lobbyId) {
        std::string packet = "JOIN|" + lobbyId + "|" + playerName;
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void SendChat(const std::string& msg) {
        std::string packet = "CHAT|" + playerName + "|" + msg;
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void SelectTeam(int team) {
        std::string packet = "TEAM_SELECT|" + std::to_string(team);
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void ToggleReady(bool ready) {
        std::string packet = "READY|" + (ready ? "1" : "0");
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void ReceiveLoop() {
        char buffer[1024];
        udp::endpoint sender;
        while (true) {
            size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender);
            std::cout << std::string(buffer, len) << std::endl;
        }
    }
};

enum class MatchState { PRE_MATCH, IN_MATCH, POST_MATCH };

struct Match {
    std::string lobbyId;
    MatchState state = MatchState::PRE_MATCH;
    std::unordered_map<uint32_t, PlayerInfo> players;
    int teamAScore = 0;
    int teamBScore = 0;
};

class MatchManager {
private:
    std::unordered_map<std::string, Match> matches;
public:
    void TransitionTo(const std::string& lobbyId, MatchState newState) {
        auto& match = matches[lobbyId];
        match.state = newState;
        BroadcastState(match);
    }

    void BroadcastState(const Match& match) {
        for (auto& [id, p] : match.players) {
            std::string msg = "Match " + match.lobbyId +
                " | State: " + StateToString(match.state) +
                " | TeamA: " + std::to_string(match.teamAScore) +
                " | TeamB: " + std::to_string(match.teamBScore);
            socket_.send_to(boost::asio::buffer(msg), p.endpoint);
        }
    }

    std::string StateToString(MatchState s) {
        switch (s) {
            case MatchState::PRE_MATCH: return "Lobby";
            case MatchState::IN_MATCH: return "In-Game";
            case MatchState::POST_MATCH: return "Results";
        }
        return "Unknown";
    }
};

class MatchClient {
public:
    void HandleServerMessage(const std::string& msg) {
        if (msg.find("State: Lobby") != std::string::npos) {
            std::cout << "[UI] Lobby screen active.\n";
        } else if (msg.find("State: In-Game") != std::string::npos) {
            std::cout << "[UI] Gameplay started!\n";
        } else if (msg.find("State: Results") != std::string::npos) {
            std::cout << "[UI] Showing results screen.\n";
        }
    }
};

struct PlayerStats {
    int kills = 0;
    int assists = 0;
    int deaths = 0;
};

struct PlayerInfo {
    uint32_t id;
    std::string name;
    int team;
    bool ready = false;
    PlayerStats stats;
    udp::endpoint endpoint;
};

void RegisterKill(uint32_t killerId, uint32_t victimId, uint32_t assisterId = 0) {
    auto& killer = match.players[killerId];
    auto& victim = match.players[victimId];

    killer.stats.kills++;
    victim.stats.deaths++;

    if (assisterId != 0) {
        match.players[assisterId].stats.assists++;
    }
}

void BroadcastResults(const Match& match) {
    for (auto& [id, p] : match.players) {
        std::string result = "Results for " + p.name +
            " | Kills: " + std::to_string(p.stats.kills) +
            " | Assists: " + std::to_string(p.stats.assists) +
            " | Deaths: " + std::to_string(p.stats.deaths);
        socket_.send_to(boost::asio::buffer(result), p.endpoint);
    }
}

class ResultsUI {
public:
    void ShowPlayerResult(const std::string& msg) {
        std::cout << "[RESULTS] " << msg << std::endl;
        // Extend with ImGui table for kills/assists/deaths
    }
};
enum class RewardType { XP, CREDITS, SKIN_UNLOCK };

struct Reward {
    RewardType type;
    int amount;
    std::string description;
};

class RewardSystem {
public:
    void ClaimReward(uint32_t playerId, const Reward& reward) {
        // Update backend player profile
        std::cout << "[REWARD] Player " << playerId << " claimed: " 
                  << reward.description << " (" << reward.amount << ")" << std::endl;

        // In production: update DB, send to client
    }
};

enum class KillPopupType { SINGLE, DOUBLE, TRIPLE, FURY };

class KillPopupManager {
public:
    void ShowPopup(uint32_t playerId, KillPopupType type) {
        switch (type) {
            case KillPopupType::SINGLE:
                std::cout << "[POPUP] Player " << playerId << " scored a Kill!" << std::endl;
                break;
            case KillPopupType::DOUBLE:
                std::cout << "[POPUP] DOUBLE KILL!" << std::endl;
                break;
            case KillPopupType::TRIPLE:
                std::cout << "[POPUP] TRIPLE KILL!" << std::endl;
                break;
            case KillPopupType::FURY:
                std::cout << "[POPUP] FURY KILL! Rampage!" << std::endl;
                break;
        }
    }
};

int main() {
    RewardSystem rewards;
    KillPopupManager popups;

    uint32_t playerId = 101;

    // Example: player gets a triple kill
    popups.ShowPopup(playerId, KillPopupType::TRIPLE);

    // Example: player claims a reward
    Reward xpReward{RewardType::XP, 500, "Match Victory XP"};
    rewards.ClaimReward(playerId, xpReward);

    Reward creditsReward{RewardType::CREDITS, 200, "Daily Login Credits"};
    rewards.ClaimReward(playerId, creditsReward);

    return 0;
}

struct PopupPacket {
    Header header; // includes playerId + type
    KillPopupType popupType;
};

struct RewardPacket {
    Header header;
    RewardType rewardType;
    int amount;
};

enum class KillstreakType { UAV, CHOPPER_GUNNER, NUKE };

class KillstreakManager {
private:
    std::unordered_map<uint32_t, int> playerKillCounts;
    RewardSystem& rewardSystem;
    KillPopupManager& popupManager;

public:
    KillstreakManager(RewardSystem& rs, KillPopupManager& pm)
        : rewardSystem(rs), popupManager(pm) {}

    void RegisterKill(uint32_t playerId) {
        playerKillCounts[playerId]++;
        int kills = playerKillCounts[playerId];

        // Trigger streaks
        if (kills == 3) {
            TriggerKillstreak(playerId, KillstreakType::UAV);
        } else if (kills == 7) {
            TriggerKillstreak(playerId, KillstreakType::CHOPPER_GUNNER);
        } else if (kills == 25) {
            TriggerKillstreak(playerId, KillstreakType::NUKE);
        }
    }

    void TriggerKillstreak(uint32_t playerId, KillstreakType type) {
        switch (type) {
            case KillstreakType::UAV:
                popupManager.ShowPopup(playerId, KillPopupType::DOUBLE);
                rewardSystem.ClaimReward(playerId, {RewardType::XP, 200, "UAV Unlocked"});

                int main() {
    RewardSystem rewards;
    KillPopupManager popups;
    KillstreakManager streaks(rewards, popups);

    uint32_t playerId = 101;

    // Simulate kills
    streaks.RegisterKill(playerId); // 1 kill
    streaks.RegisterKill(playerId); // 2 kills
    streaks.RegisterKill(playerId); // 3 kills → UAV
    streaks.RegisterKill(playerId); // 4
    streaks.RegisterKill(playerId); // 5
    streaks.RegisterKill(playerId); // 6
    streaks.RegisterKill(playerId); // 7 kills → Chopper Gunner

    // Simulate ultimate streak
    for (int i = 8; i <= 25; i++) streaks.RegisterKill(playerId); // 25 kills → Nuke
}

enum class PacketType : uint8_t {
    KILLSTREAK_EVENT = 20
};

struct KillstreakPacket {
    Header header;          // includes playerId + type
    KillstreakType streak;  // UAV, CHOPPER_GUNNER, NUKE
};

void broadcast_killstreak(const KillstreakPacket& pkt) {
    std::string room_id = player_to_room_map_[pkt.header.playerId];
    auto room = active_rooms_[room_id];

    for (const auto& [id, p] : room->players) {
        socket_.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), p->endpoint);
    }
}

void TriggerKillstreak(uint32_t playerId, KillstreakType type) {
    KillstreakPacket pkt;
    pkt.header = {PacketType::KILLSTREAK_EVENT, playerId};
    pkt.streak = type;

    // Broadcast to all players
    broadcast_killstreak(pkt);

    // Local popup + reward
    switch (type) {
        case KillstreakType::UAV:
            popupManager.ShowPopup(playerId, KillPopupType::DOUBLE);
            rewardSystem.ClaimReward(playerId, {RewardType::XP, 200, "UAV Unlocked"});
            break;
        case KillstreakType::CHOPPER_GUNNER:
            popupManager.ShowPopup(playerId, KillPopupType::TRIPLE);
            rewardSystem.ClaimReward(playerId, {RewardType::XP, 500, "Chopper Gunner Unlocked"});
            break;
        case KillstreakType::NUKE:
            popupManager.ShowPopup(playerId, KillPopupType::FURY);
            rewardSystem.ClaimReward(playerId, {RewardType::XP, 1000, "Nuke Deployed"});
            break;
    }
}

void handle_killstreak_packet(const KillstreakPacket& pkt) {
    switch (pkt.streak) {
        case KillstreakType::UAV:
            std::cout << "[CLIENT] UAV activated by Player " << pkt.header.playerId << std::endl;
            break;
        case KillstreakType::CHOPPER_GUNNER:
            std::cout << "[CLIENT] Chopper Gunner incoming!" << std::endl;
            break;
        case KillstreakType::NUKE:
            std::cout << "[CLIENT] NUKE launched! Match ending..." << std::endl;
            break;
    }
}

struct ActionPacket {
    Header header;       // includes playerId + type
    uint64_t timestamp;  // client time in ms
    WeaponAnimation action;
};

struct PlayerStateSnapshot {
    uint64_t timestamp;
    glm::vec3 position;
    glm::vec3 velocity;
};

std::unordered_map<uint32_t, std::deque<PlayerStateSnapshot>> playerHistory;

void process_action(const ActionPacket& pkt) {
    auto& history = playerHistory[pkt.header.playerId];

    // Find closest snapshot to action timestamp
    auto it = std::find_if(history.begin(), history.end(),
        [&](const PlayerStateSnapshot& snap) {
            return snap.timestamp >= pkt.timestamp;
        });

    if (it != history.end()) {
        // Rewind to snapshot
        PlayerStateSnapshot rewindState = *it;
        std::cout << "[NET] Rewinding Player " << pkt.header.playerId
                  << " to timestamp " << pkt.timestamp << std::endl;

        // Validate hit or action in rewind state
        validate_action(pkt, rewindState);

        // Fast‑forward back to current state
        PlayerStateSnapshot currentState = history.back();
        apply_action(pkt, currentState);
    }
}

void trigger_killstreak(uint32_t playerId, KillstreakType type, uint64_t timestamp) {
    // Broadcast with timestamp
    KillstreakPacket pkt;
    pkt.header = {PacketType::KILLSTREAK_EVENT, playerId};
    pkt.streak = type;
    pkt.header.timestamp = timestamp;

    broadcast_killstreak(pkt);

    std::cout << "[NET] Killstreak " << (int)type 
              << " synced at timestamp " << timestamp << std::endl;
}

// Client-side prediction
void ClientFire() {
    // Predict instantly
    localAmmo--;
    PlayLocalAnimation(WeaponAnimation::FIRE);

    // Send packet to server
    ActionPacket pkt;
    pkt.header = {PacketType::FIRE_EVENT, playerId};
    pkt.timestamp = GetCurrentTime();
    pkt.action = WeaponAnimation::FIRE;
    socket.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), serverEndpoint);
}

void handle_action_packet(const ActionPacket& pkt) {
    auto& state = playerStates[pkt.header.playerId];

    if (pkt.action == WeaponAnimation::FIRE) {
        if (state.ammoInMag > 0) {
            state.ammoInMag--;
            // authoritative update
            broadcast_state(pkt.header.playerId, state);
        }
    }
}

void OnServerStateUpdate(uint32_t playerId, const WeaponState& serverState) {
    auto& localState = localPlayerState;

    if (localState.ammoInMag != serverState.ammoInMag) {
        std::cout << "[RECONCILE] Correcting ammo count. Local: "
                  << localState.ammoInMag << " → Server: "
                  << serverState.ammoInMag << std::endl;
        localState = serverState;
    }
}

struct GameState {
    uint64_t timestamp;
    WeaponState weapon;
    glm::vec3 playerPosition;
};

std::deque<GameState> stateHistory;

void ApplyInput(ActionPacket pkt) {
    // Predict instantly
    simulate_action(pkt);

    // Save state snapshot
    GameState snapshot;
    snapshot.timestamp = pkt.timestamp;
    snapshot.weapon = localWeaponState;
    snapshot.playerPosition = localPlayerPosition;
    stateHistory.push_back(snapshot);

    // Send to server
    socket.send_to(boost::asio::buffer(&pkt, sizeof(pkt)), serverEndpoint);
}

void OnServerCorrection(const GameState& authoritativeState) {
    // Find snapshot at server timestamp
    auto it = std::find_if(stateHistory.begin(), stateHistory.end(),
        [&](const GameState& s){ return s.timestamp == authoritativeState.timestamp; });

    if (it != stateHistory.end()) {
        // Rollback
        localWeaponState = authoritativeState.weapon;
        localPlayerPosition = authoritativeState.playerPosition;

        std::cout << "[ROLLBACK] Correcting to server state at " 
                  << authoritativeState.timestamp << std::endl;

        // Reapply all inputs since then
        for (auto iter = it; iter != stateHistory.end(); ++iter) {
            simulate_action_from_snapshot(*iter);
        }
    }
}

void OnKillstreakCorrection(const KillstreakPacket& pkt) {
    std::cout << "[ROLLBACK] Killstreak corrected: " << (int)pkt.streak 
              << " at timestamp " << pkt.header.timestamp << std::endl;
    trigger_killstreak(pkt.header.playerId, pkt.streak, pkt.header.timestamp);
}

bool validate_action(const ActionPacket& pkt, const PlayerState& state) {
    // Example: firing with no ammo
    if (pkt.action == WeaponAnimation::FIRE && state.ammoInMag <= 0) {
        std::cout << "[ANTI-CHEAT] Invalid FIRE action: no ammo." << std::endl;
        return false;
    }

    // Example: reload while already full
    if (pkt.action == WeaponAnimation::RELOAD && state.ammoInMag == state.magSize) {
        std::cout << "[ANTI-CHEAT] Invalid RELOAD action: magazine already full." << std::endl;
        return false;
    }

    return true;
}

struct SecureHeader {
    PacketType type;
    uint32_t playerId;
    uint64_t timestamp;
    uint32_t checksum; // simple hash of payload
};

bool verify_checksum(const SecureHeader& header, const void* payload, size_t size) {
    uint32_t calc = hash(payload, size);
    return calc == header.checksum;
}

if (distance(newPos, oldPos) > maxSpeed * deltaTime) {
    std::cout << "[ANTI-CHEAT] Speed hack detected for Player " << playerId << std::endl;
}

void enforce_cheat_policy(uint32_t playerId) {
    std::cout << "[ANTI-CHEAT] Player " << playerId << " flagged. Action rejected." << std::endl;
    // In production: increment violation counter, kick if repeated
}

enum class LogType { INFO, WARNING, ERROR, CHEAT, PERFORMANCE };

struct LogEvent {
    LogType type;
    uint64_t timestamp;
    uint32_t playerId;
    std::string message;
};

class Logger {
public:
    void Log(const LogEvent& event) {
        std::cout << "[" << to_string(event.type) << "] "
                  << "Player " << event.playerId << " | "
                  << event.timestamp << " | "
                  << event.message << std::endl;
        // In production: write to file or DB
    }

private:
    std::string to_string(LogType t) {
        switch (t) {
            case LogType::INFO: return "INFO";
            case LogType::WARNING: return "WARN";
            case LogType::ERROR: return "ERROR";
            case LogType::CHEAT: return "CHEAT";
            case LogType::PERFORMANCE: return "PERF";
        }
        return "UNKNOWN";
    }
};

struct PlayerMetrics {
    int kills = 0;
    int deaths = 0;
    int rewardsClaimed = 0;
    int cheatFlags = 0;
    double avgPing = 0.0;
};

class Analytics {
private:
    std::unordered_map<uint32_t, PlayerMetrics> metrics;

public:
    void RecordKill(uint32_t playerId) { metrics[playerId].kills++; }
    void RecordDeath(uint32_t playerId) { metrics[playerId].deaths++; }
    void RecordReward(uint32_t playerId) { metrics[playerId].rewardsClaimed++; }
    void RecordCheat(uint32_t playerId) { metrics[playerId].cheatFlags++; }
    void RecordPing(uint32_t playerId, double ping) {
        metrics[playerId].avgPing = (metrics[playerId].avgPing + ping) / 2.0;
    }

    void Report(uint32_t playerId) {
        auto& m = metrics[playerId];
        std::cout << "[ANALYTICS] Player " << playerId
                  << " | Kills: " << m.kills
                  << " | Deaths: " << m.deaths
                  << " | Rewards: " << m.rewardsClaimed
                  << " | CheatFlags: " << m.cheatFlags
                  << " | AvgPing: " << m.avgPing << "ms" << std::endl;
    }
};

void enforce_cheat_policy(uint32_t playerId, Logger& logger, Analytics& analytics) {
    logger.Log({LogType::CHEAT, GetCurrentTime(), playerId, "Cheat attempt detected"});
    analytics.RecordCheat(playerId);
    // Optionally kick/ban after threshold
}

int main() {
    Logger logger;
    Analytics analytics;

    uint32_t playerId = 101;

    // Simulate events
    analytics.RecordKill(playerId);
    analytics.RecordReward(playerId);
    enforce_cheat_policy(playerId, logger, analytics);

    analytics.Report(playerId);
}

struct PlayerProfile {
    uint32_t id;
    std::string name;
    int xp = 0;
    int rank = 1;
    std::vector<std::string> unlocks;
};

int CalculateXP(const PlayerStats& stats, bool wonMatch) {
    int xp = stats.kills * 100 + stats.assists * 50 + stats.deaths * 10;
    if (wonMatch) xp += 500; // bonus for winning
    return xp;
}

void ApplyRewards(Match& match, bool teamAWon) {
    for (auto& [id, p] : match.players) {
        bool won = (p.team == 1 && teamAWon) || (p.team == 2 && !teamAWon);
        int gainedXP = CalculateXP(p.stats, won);

        playerProfiles[id].xp += gainedXP;

        // Rank progression: every 1000 XP = +1 rank
        if (playerProfiles[id].xp >= playerProfiles[id].rank * 1000) {
            playerProfiles[id].rank++;
            playerProfiles[id].unlocks.push_back("New Skin at Rank " + std::to_string(playerProfiles[id].rank));
        }

        std::string rewardMsg = "Player " + p.name +
            " | XP Gained: " + std::to_string(gainedXP) +
            " | Total XP: " + std::to_string(playerProfiles[id].xp) +
            " | Rank: " + std::to_string(playerProfiles[id].rank) +
            " | Unlocks: " + (playerProfiles[id].unlocks.empty() ? "None" : playerProfiles[id].unlocks.back());

        socket_.send_to(boost::asio::buffer(rewardMsg), p.endpoint);
    }
}

class RewardsUI {
public:
    void ShowRewardMessage(const std::string& msg) {
        std::cout << "[REWARD] " << msg << std::endl;
        // Extend with ImGui overlay: XP bar, rank badge, unlock icons
    }
};

void DrawXPOverlay(PlayerProfile& profile, int gainedXP) {
    ImGui::Begin("Match Results & Rewards");

    ImGui::Text("XP Progression");
    float progress = (float)profile.xp / (profile.rank * 1000);
    ImGui::ProgressBar(progress, ImVec2(400, 30));

    ImGui::Text("+%d XP Earned!", gainedXP);
    ImGui::Text("Total XP: %d", profile.xp);
    ImGui::Text("Rank: %d", profile.rank);

    if (!profile.unlocks.empty()) {
        ImGui::Text("Latest Unlock: %s", profile.unlocks.back().c_str());
    }

    if (ImGui::Button("Continue")) {
        // Transition to next screen
    }

    ImGui::End();
}

void DrawRankBadge(PlayerProfile& profile) {
    ImGui::Begin("Rank Badge");

    // Determine badge texture based on rank
    std::string badgeTexture;
    if (profile.rank < 10) badgeTexture = "Textures/Badge_Bronze.png";
    else if (profile.rank < 20) badgeTexture = "Textures/Badge_Silver.png";
    else if (profile.rank < 30) badgeTexture = "Textures/Badge_Gold.png";
    else if (profile.rank < 40) badgeTexture = "Textures/Badge_Platinum.png";
    else badgeTexture = "Textures/Badge_Diamond.png";

    // Display badge image
    ImGui::Text("Rank %d", profile.rank);
    ImGui::Image((void*)(intptr_t)LoadTexture(badgeTexture.c_str()), ImVec2(128, 128));

    ImGui::Text("XP: %d", profile.xp);
    ImGui::Text("Next Rank at %d XP", profile.rank * 1000);

    ImGui::End();
}

void DrawLeaderboard(const std::vector<PlayerProfile>& profiles) {
    ImGui::Begin("Match Leaderboard");

    ImGui::Text("🏆 Match Results");
    ImGui::Separator();

    ImGui::Columns(5, "Leaderboard");
    ImGui::Text("Player"); ImGui::NextColumn();
    ImGui::Text("Rank"); ImGui::NextColumn();
    ImGui::Text("Kills"); ImGui::NextColumn();
    ImGui::Text("Assists"); ImGui::NextColumn();
    ImGui::Text("Deaths"); ImGui::NextColumn();
    ImGui::Separator();

    for (const auto& p : profiles) {
        ImGui::Text("%s", p.name.c_str()); ImGui::NextColumn();

        // Load badge texture based on rank
        std::string badgeTexture;
        if (p.rank < 10) badgeTexture = "Textures/Badge_Bronze.png";
        else if (p.rank < 20) badgeTexture = "Textures/Badge_Silver.png";
        else if (p.rank < 30) badgeTexture = "Textures/Badge_Gold.png";
        else if (p.rank < 40) badgeTexture = "Textures/Badge_Platinum.png";
        else badgeTexture = "Textures/Badge_Diamond.png";

        ImGui::Image((void*)(intptr_t)LoadTexture(badgeTexture.c_str()), ImVec2(64, 64));
        ImGui::NextColumn();

        ImGui::Text("%d", p.stats.kills); ImGui::NextColumn();
        ImGui::Text("%d", p.stats.assists); ImGui::NextColumn();
        ImGui::Text("%d", p.stats.deaths); ImGui::NextColumn();
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::End();
}

struct PlayerProfile {
    uint32_t id;
    std::string name;
    int xp = 0;
    int rank = 1;
    std::vector<std::string> unlocks;

    // Lifetime stats
    int totalKills = 0;
    int totalAssists = 0;
    int totalDeaths = 0;
};

void SaveLeaderboardResults(const Match& match) {
    for (auto& [id, p] : match.players) {
        auto& profile = playerProfiles[id];

        profile.totalKills   += p.stats.kills;
        profile.totalAssists += p.stats.assists;
        profile.totalDeaths  += p.stats.deaths;

        int gainedXP = CalculateXP(p.stats, /*wonMatch*/ true); 
        profile.xp += gainedXP;

        if (profile.xp >= profile.rank * 1000) {
            profile.rank++;
            profile.unlocks.push_back("New Badge at Rank " + std::to_string(profile.rank));
        }

        std::cout << "[PROFILE] Saved results for " << profile.name
                  << " | Total Kills: " << profile.totalKills
                  << " | Assists: " << profile.totalAssists
                  << " | Deaths: " << profile.totalDeaths
                  << " | XP: " << profile.xp
                  << " | Rank: " << profile.rank << std::endl;
    }
}

void BroadcastProfile(const PlayerProfile& profile, udp::endpoint ep) {
    std::string msg = "Profile Update | " + profile.name +
        " | Rank: " + std::to_string(profile.rank) +
        " | XP: " + std::to_string(profile.xp) +
        " | Total Kills: " + std::to_string(profile.totalKills) +
        " | Assists: " + std::to_string(profile.totalAssists) +
        " | Deaths: " + std::to_string(profile.totalDeaths);
    socket_.send_to(boost::asio::buffer(msg), ep);
}

struct GlobalEntry {
    uint32_t id;
    std::string name;
    int rank;
    int xp;
    int totalKills;
    int totalAssists;
    int totalDeaths;
};

class GlobalLeaderboardManager {
private:
    std::vector<GlobalEntry> leaderboard;
public:
    void UpdatePlayer(const PlayerProfile& profile) {
        auto it = std::find_if(leaderboard.begin(), leaderboard.end(),
            [&](const GlobalEntry& e){ return e.id == profile.id; });

        if (it != leaderboard.end()) {
            it->xp = profile.xp;
            it->rank = profile.rank;
            it->totalKills = profile.totalKills;
            it->totalAssists = profile.totalAssists;
            it->totalDeaths = profile.totalDeaths;
        } else {
            leaderboard.push_back({profile.id, profile.name, profile.rank,
                                   profile.xp, profile.totalKills,
                                   profile.totalAssists, profile.totalDeaths});
        }

        // Sort by XP or rank
        std::sort(leaderboard.begin(), leaderboard.end(),
            [](const GlobalEntry& a, const GlobalEntry& b) {
                return a.xp > b.xp;
            });
    }

    void BroadcastLeaderboard(udp::socket& socket, const udp::endpoint& ep) {
        std::string msg = "GLOBAL LEADERBOARD:\n";
        for (const auto& e : leaderboard) {
            msg += e.name + " | Rank " + std::to_string(e.rank) +
                   " | XP " + std::to_string(e.xp) +
                   " | Kills " + std::to_string(e.totalKills) +
                   " | Assists " + std::to_string(e.totalAssists) +
                   " | Deaths " + std::to_string(e.totalDeaths) + "\n";
        }
        socket.send_to(boost::asio::buffer(msg), ep);
    }
};

void DrawGlobalLeaderboard(const std::vector<GlobalEntry>& leaderboard) {
    ImGui::Begin("Global Leaderboard");
    ImGui::Columns(6, "Leaderboard");
    ImGui::Text("Player"); ImGui::NextColumn();
    ImGui::Text("Rank"); ImGui::NextColumn();
    ImGui::Text("XP"); ImGui::NextColumn();
    ImGui::Text("Kills"); ImGui::NextColumn();
    ImGui::Text("Assists"); ImGui::NextColumn();
    ImGui::Text("Deaths"); ImGui::NextColumn();
    ImGui::Separator();

    for (const auto& e : leaderboard) {
        ImGui::Text("%s", e.name.c_str()); ImGui::NextColumn();
        ImGui::Text("%d", e.rank); ImGui::NextColumn();
        ImGui::Text("%d", e.xp); ImGui::NextColumn();
        ImGui::Text("%d", e.totalKills); ImGui::NextColumn();
        ImGui::Text("%d", e.totalAssists); ImGui::NextColumn();
        ImGui::Text("%d", e.totalDeaths); ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::End();
}

struct Season {
    int seasonId;
    std::string name;
    std::string startDate;
    std::string endDate;
};

class SeasonManager {
private:
    Season currentSeason;
public:
    SeasonManager() {
        currentSeason = {1, "Season One", "2026-07-01", "2026-09-30"};
    }

    bool IsSeasonOver(const std::string& currentDate) {
        return currentDate > currentSeason.endDate;
    }

    void ResetLeaderboard(GlobalLeaderboardManager& lb) {
        lb.Clear(); // wipe stats for new season
        currentSeason.seasonId++;
        currentSeason.name = "Season " + std::to_string(currentSeason.seasonId);
        // assign new start/end dates
    }
};

enum class Tier { BRONZE, SILVER, GOLD, PLATINUM, DIAMOND, LEGENDARY };

Tier CalculateTier(int xp) {
    if (xp < 1000) return Tier::BRONZE;
    if (xp < 5000) return Tier::SILVER;
    if (xp < 10000) return Tier::GOLD;
    if (xp < 20000) return Tier::PLATINUM;
    if (xp < 50000) return Tier::DIAMOND;
    return Tier::LEGENDARY;
}

std::string GetSeasonReward(Tier tier) {
    switch (tier) {
        case Tier::BRONZE: return "Bronze Emblem";
        case Tier::SILVER: return "Silver Weapon Skin";
        case Tier::GOLD: return "Gold Outfit";
        case Tier::PLATINUM: return "Platinum Banner";
        case Tier::DIAMOND: return "Diamond Character Skin";
        case Tier::LEGENDARY: return "Legendary Exclusive Weapon";
    }
    return "No Reward";
}

void GrantSeasonRewards(PlayerProfile& profile) {
    Tier tier = CalculateTier(profile.xp);
    std::string reward = GetSeasonReward(tier);
    profile.unlocks.push_back(reward);
    std::cout << "[REWARD] " << profile.name << " unlocked: " << reward << std::endl;
}

struct HallOfFameEntry {
    int seasonId;
    std::string playerName;
    int rank;
    int xp;
    int totalKills;
    std::string reward;
};

class HallOfFameManager {
private:
    std::vector<HallOfFameEntry> hallOfFame;
public:
    void AddTopPlayers(const std::vector<GlobalEntry>& leaderboard, int seasonId) {
        // Take top 3 players for Hall of Fame
        for (int i = 0; i < std::min(3, (int)leaderboard.size()); i++) {
            const auto& e = leaderboard[i];
            hallOfFame.push_back({seasonId, e.name, e.rank, e.xp, e.totalKills,
                                  GetSeasonReward(CalculateTier(e.xp))});
        }
    }

    void BroadcastHallOfFame(udp::socket& socket, const udp::endpoint& ep) {
        std::string msg = "HALL OF FAME:\n";
        for (const auto& entry : hallOfFame) {
            msg += "Season " + std::to_string(entry.seasonId) + " | " + entry.playerName +
                   " | Rank " + std::to_string(entry.rank) +
                   " | XP " + std::to_string(entry.xp) +
                   " | Kills " + std::to_string(entry.totalKills) +
                   " | Reward: " + entry.reward + "\n";
        }
        socket.send_to(boost::asio::buffer(msg), ep);
    }
};

void DrawHallOfFame(const std::vector<HallOfFameEntry>& hof) {
    ImGui::Begin("Hall of Fame");
    ImGui::Text("🏅 Past Season Champions");
    ImGui::Separator();

    ImGui::Columns(5, "HallOfFame");
    ImGui::Text("Season"); ImGui::NextColumn();
    ImGui::Text("Player"); ImGui::NextColumn();
    ImGui::Text("Rank"); ImGui::NextColumn();
    ImGui::Text("XP"); ImGui::NextColumn();
    ImGui::Text("Reward"); ImGui::NextColumn();
    ImGui::Separator();

    for (const auto& e : hof) {
        ImGui::Text("%d", e.seasonId); ImGui::NextColumn();
        ImGui::Text("%s", e.playerName.c_str()); ImGui::NextColumn();
        ImGui::Text("%d", e.rank); ImGui::NextColumn();
        ImGui::Text("%d", e.xp); ImGui::NextColumn();
        ImGui::Text("%s", e.reward.c_str()); ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::End();
}

std::string GetExclusiveTitle(int position) {
    switch (position) {
        case 1: return "Season Champion";
        case 2: return "Elite Warrior";
        case 3: return "Top Contender";
        default: return "Competitor";
    }
}

void AddTopPlayersWithTitles(const std::vector<GlobalEntry>& leaderboard, int seasonId) {
    for (int i = 0; i < std::min(3, (int)leaderboard.size()); i++) {
        const auto& e = leaderboard[i];
        std::string title = GetExclusiveTitle(i + 1);

        hallOfFame.push_back({
            seasonId, e.name, e.rank, e.xp, e.totalKills,
            GetSeasonReward(CalculateTier(e.xp)) + " | Title: " + title
        });
    }
}

void DrawHallOfFame(const std::vector<HallOfFameEntry>& hof) {
    ImGui::Begin("Hall of Fame");
    ImGui::Text("🏅 Past Season Champions");
    ImGui::Separator();

    ImGui::Columns(6, "HallOfFame");
    ImGui::Text("Season"); ImGui::NextColumn();
    ImGui::Text("Player"); ImGui::NextColumn();
    ImGui::Text("Rank"); ImGui::NextColumn();
    ImGui::Text("XP"); ImGui::NextColumn();
    ImGui::Text("Reward"); ImGui::NextColumn();
    ImGui::Text("Title"); ImGui::NextColumn();
    ImGui::Separator();

    for (const auto& e : hof) {
        ImGui::Text("%d", e.seasonId); ImGui::NextColumn();
        ImGui::Text("%s", e.playerName.c_str()); ImGui::NextColumn();
        ImGui::Text("%d", e.rank); ImGui::NextColumn();
        ImGui::Text("%d", e.xp); ImGui::NextColumn();
        ImGui::Text("%s", e.reward.c_str()); ImGui::NextColumn();
        ImGui::Text("%s", e.reward.find("Title:") != std::string::npos 
                     ? e.reward.substr(e.reward.find("Title:")).c_str() 
                     : ""); ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::End();
}

void DrawPlayerWithTitle(const PlayerProfile& profile) {
    ImGui::Begin("Player Display");

    // Player name
    ImGui::Text("%s", profile.name.c_str());

    // Dynamic title display
    if (!profile.title.empty()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "[%s]", profile.title.c_str());
    }

    // Rank badge beside title
    std::string badgeTexture = GetBadgeTexture(profile.rank);
    ImGui::Image((void*)(intptr_t)LoadTexture(badgeTexture.c_str()), ImVec2(64, 64));

    ImGui::End();
}

struct PlayerInfo {
    uint32_t id;
    std::string name;
    int team = 0;      // 0 = unassigned, 1 = Team A, 2 = Team B
    bool ready = false;
    udp::endpoint endpoint;
};

struct Lobby {
    std::string lobbyId;
    std::unordered_map<uint32_t, PlayerInfo> players;
};

class LobbyServer {
    std::unordered_map<std::string, Lobby> lobbies;
    udp::socket socket_;
public:
    LobbyServer(boost::asio::io_context& io, int port)
        : socket_(io, udp::endpoint(udp::v4(), port)) {}

    void HandlePacket(const std::string& msg, udp::endpoint sender) {
        // Parse packet: JOIN, CHAT, TEAM_SELECT, READY
        // Update lobby state accordingly
        // Broadcast updated lobby state
    }

    void BroadcastLobbyState(const Lobby& lobby) {
        for (auto& [id, p] : lobby.players) {
            std::string state = "Lobby " + lobby.lobbyId +
                " | Player " + p.name +
                " | Team " + std::to_string(p.team) +
                " | Ready: " + (p.ready ? "Yes" : "No");
            socket_.send_to(boost::asio::buffer(state), p.endpoint);
        }
    }
};

class LobbyClient {
    udp::socket socket_;
    udp::endpoint serverEndpoint_;
    uint32_t playerId;
    std::string playerName;
public:
    LobbyClient(boost::asio::io_context& io, const std::string& host, int port)
        : socket_(io, udp::endpoint(udp::v4(), 0)) {
        serverEndpoint_ = udp::endpoint(boost::asio::ip::address::from_string(host), port);
    }

    void JoinLobby(const std::string& lobbyId) {
        std::string packet = "JOIN|" + lobbyId + "|" + playerName;
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void SelectTeam(int team) {
        std::string packet = "TEAM_SELECT|" + std::to_string(team);
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void ToggleReady(bool ready) {
        std::string packet = "READY|" + (ready ? "1" : "0");
        socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
    }

    void ReceiveLoop() {
        char buffer[1024];
        udp::endpoint sender;
        while (true) {
            size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender);
            std::cout << std::string(buffer, len) << std::endl;
        }
    }
};

void HandlePacket(const std::string& msg, udp::endpoint sender) {
    if (msg.rfind("CHAT|", 0) == 0) {
        // Handle text chat
    } else if (msg.rfind("VOICE|", 0) == 0) {
        // Handle voice packet
        for (auto& [id, p] : lobby.players) {
            socket_.send_to(boost::asio::buffer(msg), p.endpoint);
        }
    }
}

void SendVoice(short* buffer, size_t len) {
    // Encode with Opus
    std::string packet = "VOICE|" + std::string((char*)buffer, len * sizeof(short));
    socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
}

void ReceiveLoop() {
    char buffer[2048];
    udp::endpoint sender;
    while (true) {
        size_t len = socket_.receive_from(boost::asio::buffer(buffer), sender);
        std::string msg(buffer, len);

        if (msg.rfind("VOICE|", 0) == 0) {
            // Decode with Opus and play via PortAudio
        } else if (msg.rfind("CHAT|", 0) == 0) {
            std::cout << msg << std::endl;
        }
    }
}

void StartCountdown(Match& match, int seconds) {
    for (int i = seconds; i >= 0; --i) {
        std::string msg = "COUNTDOWN|" + std::to_string(i);
        for (auto& [id, p] : match.players) {
            socket_.send_to(boost::asio::buffer(msg), p.endpoint);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    TransitionTo(match.lobbyId, MatchState::IN_MATCH);
}

void HandleServerMessage(const std::string& msg) {
    if (msg.rfind("COUNTDOWN|", 0) == 0) {
        int seconds = std::stoi(msg.substr(10));
        ShowCountdownOverlay(seconds);
    }
}

void ShowCountdownOverlay(int seconds) {
    ImGui::Begin("Match Countdown");
    ImGui::Text("Match starting in %d...", seconds);
    ImGui::ProgressBar((float)(seconds) / 10.0f, ImVec2(300, 30));
    ImGui::End();
}

struct Lobby {
    std::string lobbyId;
    std::unordered_map<uint32_t, PlayerInfo> players;
    bool countdownActive = false;
};

void HandleTeamSelect(Lobby& lobby, uint32_t playerId, int team) {
    if (lobby.countdownActive) {
        // Reject team change
        auto& p = lobby.players[playerId];
        std::string msg = "TEAM_CHANGE_DENIED|Countdown active";
        socket_.send_to(boost::asio::buffer(msg), p.endpoint);
        return;
    }
    lobby.players[playerId].team = team;
    BroadcastLobbyState(lobby);
}

void StartCountdown(Lobby& lobby, int seconds) {
    lobby.countdownActive = true; // lock teams
    for (int i = seconds; i >= 0; --i) {
        std::string msg = "COUNTDOWN|" + std::to_string(i);
        for (auto& [id, p] : lobby.players) {
            socket_.send_to(boost::asio::buffer(msg), p.endpoint);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // Transition to match start
    TransitionTo(lobby.lobbyId, MatchState::IN_MATCH);
}

void HandleServerMessage(const std::string& msg) {
    if (msg.rfind("TEAM_CHANGE_DENIED", 0) == 0) {
        std::cout << "[LOBBY] Cannot change team during countdown.\n";
    }
}

struct MapVote {
    std::string mapName;
    int votes = 0;
};

class MapVotingSystem {
    std::vector<MapVote> maps;
    std::unordered_map<uint32_t, std::string> playerVotes;
public:
    MapVotingSystem(std::vector<std::string> candidateMaps) {
        for (auto& m : candidateMaps) maps.push_back({m, 0});
    }

    void CastVote(uint32_t playerId, const std::string& mapName) {
        // Prevent duplicate voting
        if (playerVotes.find(playerId) != playerVotes.end()) return;
        playerVotes[playerId] = mapName;

        for (auto& mv : maps) {
            if (mv.mapName == mapName) {
                mv.votes++;
                break;
            }
        }
    }

    std::string GetWinningMap() {
        auto winner = std::max_element(maps.begin(), maps.end(),
            [](const MapVote& a, const MapVote& b){ return a.votes < b.votes; });
        return winner->mapName;
    }

    void BroadcastResults(udp::socket& socket, const std::vector<PlayerInfo>& players) {
        std::string msg = "Map Voting Results:\n";
        for (auto& mv : maps) {
            msg += mv.mapName + " - " + std::to_string(mv.votes) + " votes\n";
        }
        msg += "Winning Map: " + GetWinningMap();

        for (auto& p : players) {
            socket.send_to(boost::asio::buffer(msg), p.endpoint);
        }
    }
};

void ShowMapVotingUI(const std::vector<std::string>& maps) {
    ImGui::Begin("Map Voting");
    ImGui::Text("Vote for the next map:");

    for (auto& map : maps) {
        if (ImGui::Button(map.c_str())) {
            std::string packet = "VOTE|" + map;
            socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
        }
    }

    ImGui::End();
}

void CastVote(uint32_t playerId, const std::string& mapName, Lobby& lobby) {
    if (playerVotes.find(playerId) != playerVotes.end()) return; // prevent duplicate
    playerVotes[playerId] = mapName;

    for (auto& mv : maps) {
        if (mv.mapName == mapName) {
            mv.votes++;
            break;
        }
    }

    // Broadcast live tally
    std::string msg = "LIVE_TALLY|";
    for (auto& mv : maps) {
        msg += mv.mapName + ":" + std::to_string(mv.votes) + ",";
    }

    for (auto& [id, p] : lobby.players) {
        socket_.send_to(boost::asio::buffer(msg), p.endpoint);
    }
}

void HandleServerMessage(const std::string& msg) {
    if (msg.rfind("LIVE_TALLY|", 0) == 0) {
        std::string data = msg.substr(11);
        currentTally = ParseTally(data); // mapName → votes
    }
}

void ShowLiveVoteTallyUI(const std::map<std::string,int>& tally) {
    ImGui::Begin("Map Voting");
    ImGui::Text("Vote for the next map:");

    for (auto& [map, votes] : tally) {
        ImGui::Text("%s", map.c_str());
        ImGui::SameLine();
        ImGui::ProgressBar((float)votes / (float)totalPlayers, ImVec2(200, 20));
        ImGui::SameLine();
        ImGui::Text("%d votes", votes);
    }

    ImGui::End();
}

struct MapVote {
    std::string mapName;
    int votes = 0;
    int firstToFiveOrder = -1; // track which map hit 5 first
};

class MapVotingSystem {
    std::vector<MapVote> maps;
    std::unordered_map<uint32_t, std::string> playerVotes;
    int orderCounter = 0; // increments when a map reaches 5 votes
public:
    MapVotingSystem(std::vector<std::string> candidateMaps) {
        for (auto& m : candidateMaps) maps.push_back({m, 0, -1});
    }

    void CastVote(uint32_t playerId, const std::string& mapName) {
        if (playerVotes.find(playerId) != playerVotes.end()) return; // prevent duplicate
        playerVotes[playerId] = mapName;

        for (auto& mv : maps) {
            if (mv.mapName == mapName && mv.votes < 5) {
                mv.votes++;
                if (mv.votes == 5 && mv.firstToFiveOrder == -1) {
                    mv.firstToFiveOrder = ++orderCounter;
                }
                break;
            }
        }
    }

    std::string GetWinningMap() {
        // Find highest votes
        int maxVotes = 0;
        for (auto& mv : maps) maxVotes = std::max(maxVotes, mv.votes);

        // Collect maps with max votes
        std::vector<MapVote> tied;
        for (auto& mv : maps) {
            if (mv.votes == maxVotes) tied.push_back(mv);
        }

        // Tie-breaker: map that reached 5 votes first
        if (tied.size() > 1 && maxVotes == 5) {
            auto winner = *std::min_element(tied.begin(), tied.end(),
                [](const MapVote& a, const MapVote& b){
                    return a.firstToFiveOrder < b.firstToFiveOrder;
                });
            return winner.mapName;
        }

        // Normal case: highest votes wins
        return tied.front().mapName;
    }
};

void ShowLiveVoteTallyUI(const std::vector<MapVote>& maps) {
    ImGui::Begin("Map Voting");
    ImGui::Text("Vote for the next map (max 5 votes per map):");

    for (auto& mv : maps) {
        ImGui::Text("%s", mv.mapName.c_str());
        ImGui::SameLine();
        ImGui::ProgressBar((float)mv.votes / 5.0f, ImVec2(200, 20));
        ImGui::SameLine();
        ImGui::Text("%d/5 votes", mv.votes);
    }

    ImGui::End();
}

class MapVotingSystem {
    std::vector<MapVote> maps;
    std::unordered_map<uint32_t, std::string> playerVotes;
    int orderCounter = 0;
    bool votingActive = false;

public:
    MapVotingSystem(std::vector<std::string> candidateMaps) {
        for (auto& m : candidateMaps) maps.push_back({m, 0, -1});
    }

    void StartVotingPhase(int durationSeconds, Lobby& lobby) {
        votingActive = true;
        for (int i = durationSeconds; i >= 0; --i) {
            std::string msg = "VOTE_COUNTDOWN|" + std::to_string(i);
            for (auto& [id, p] : lobby.players) {
                socket_.send_to(boost::asio::buffer(msg), p.endpoint);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        votingActive = false;
        std::string winner = GetWinningMap();
        BroadcastWinner(winner, lobby);
    }

    void CastVote(uint32_t playerId, const std::string& mapName) {
        if (!votingActive) return; // ignore votes after countdown
        if (playerVotes.find(playerId) != playerVotes.end()) return;

        playerVotes[playerId] = mapName;
        for (auto& mv : maps) {
            if (mv.mapName == mapName && mv.votes < 5) {
                mv.votes++;
                if (mv.votes == 5 && mv.firstToFiveOrder == -1) {
                    mv.firstToFiveOrder = ++orderCounter;
                }
                break;
            }
        }
        BroadcastLiveTally();
    }

    std::string GetWinningMap() {
        int maxVotes = 0;
        for (auto& mv : maps) maxVotes = std::max(maxVotes, mv.votes);

        std::vector<MapVote> tied;
        for (auto& mv : maps) if (mv.votes == maxVotes) tied.push_back(mv);

        if (tied.size() > 1 && maxVotes == 5) {
            auto winner = *std::min_element(tied.begin(), tied.end(),
                [](const MapVote& a, const MapVote& b){
                    return a.firstToFiveOrder < b.firstToFiveOrder;
                });
            return winner.mapName;
        }
        return tied.front().mapName;
    }

    void BroadcastWinner(const std::string& winner, Lobby& lobby) {
        std::string msg = "VOTING_ENDED|Winning Map: " + winner;
        for (auto& [id, p] : lobby.players) {
            socket_.send_to(boost::asio::buffer(msg), p.endpoint);
        }
    }
};

void HandleServerMessage(const std::string& msg) {
    if (msg.rfind("VOTE_COUNTDOWN|", 0) == 0) {
        int seconds = std::stoi(msg.substr(14));
        ShowCountdownOverlay(seconds);
        if (seconds == 0) votingLocked = true; // lock UI
    } else if (msg.rfind("LIVE_TALLY|", 0) == 0) {
        currentTally = ParseTally(msg.substr(11));
    } else if (msg.rfind("VOTING_ENDED|", 0) == 0) {
        std::cout << msg << std::endl; // show winner
    }
}

void ShowCountdownOverlay(int seconds) {
    ImGui::Begin("Voting Countdown");
    ImGui::Text("Voting ends in %d seconds...", seconds);
    ImGui::ProgressBar((float)seconds / 30.0f, ImVec2(300, 30)); // example 30s timer
    ImGui::End();
}

void ShowMapVotingUI(const std::map<std::string,int>& tally) {
    ImGui::Begin("Map Voting");
    if (votingLocked) {
        ImGui::Text("Voting has ended.");
    } else {
        for (auto& [map, votes] : tally) {
            if (ImGui::Button(map.c_str())) {
                std::string packet = "VOTE|" + map;
                socket_.send_to(boost::asio::buffer(packet), serverEndpoint_);
            }
            ImGui::SameLine();
            ImGui::Text("%d/5 votes", votes);
        }
    }
    ImGui::End();
}


void RunMapName() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Crashes Map - gameXpro+");

    // Define shapes
    sf::RectangleShape building(sf::Vector2f(100, 100));
    building.setFillColor(sf::Color::Blue);
    building.setPosition(200, 200);

    sf::CircleShape crashSite(40);
    crashSite.setFillColor(sf::Color::Red);
    crashSite.setPosition(400, 300);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(building);
        window.draw(crashSite);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Crossfiring Map - gameXpro+");

    // Load textures
    sf::Texture roadTexture;
    if (!roadTexture.loadFromFile("road.png")) return -1;

    sf::Texture buildingTexture;
    if (!buildingTexture.loadFromFile("building.png")) return -1;

    sf::Texture spawnTexture;
    if (!spawnTexture.loadFromFile("spawn.png")) return -1;

    // Road (main street)
    sf::RectangleShape road(sf::Vector2f(800, 100));
    road.setTexture(&roadTexture);
    road.setPosition(100, 300);

    // Buildings along the street
    sf::RectangleShape building1(sf::Vector2f(150, 200));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(150, 200));
    building2.setTexture(&buildingTexture);
    building2.setPosition(750, 100);

    sf::RectangleShape building3(sf::Vector2f(150, 200));
    building3.setTexture(&buildingTexture);
    building3.setPosition(100, 450);

    sf::RectangleShape building4(sf::Vector2f(150, 200));
    building4.setTexture(&buildingTexture);
    building4.setPosition(750, 450);

    // Spawn points
    sf::CircleShape spawnA(30);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 320);

    sf::CircleShape spawnB(30);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(820, 320);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(road);
        window.draw(building1);
        window.draw(building2);
        window.draw(building3);
        window.draw(building4);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Shoot Range Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("ground.png");

    sf::Texture wallTexture;
    wallTexture.loadFromFile("wall.png");

    sf::Texture targetTexture;
    targetTexture.loadFromFile("target.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Shooting lanes
    sf::RectangleShape lane1(sf::Vector2f(1000, 80));
    lane1.setTexture(&wallTexture);
    lane1.setPosition(100, 200);

    sf::RectangleShape lane2(sf::Vector2f(1000, 80));
    lane2.setTexture(&wallTexture);
    lane2.setPosition(100, 400);

    // Targets
    sf::CircleShape target1(30);
    target1.setTexture(&targetTexture);
    target1.setPosition(500, 220);

    sf::CircleShape target2(30);
    target2.setTexture(&targetTexture);
    target2.setPosition(800, 420);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 220);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(120, 420);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(lane1);
        window.draw(lane2);
        window.draw(target1);
        window.draw(target2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void RunMapName() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Shipments@1994 Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("ground.png");

    sf::Texture containerTexture;
    containerTexture.loadFromFile("container.png");

    sf::Texture crateTexture;
    crateTexture.loadFromFile("crate.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1000, 800));
    ground.setTexture(&groundTexture);

    // Containers
    sf::RectangleShape container1(sf::Vector2f(200, 100));
    container1.setTexture(&containerTexture);
    container1.setPosition(100, 200);

    sf::RectangleShape container2(sf::Vector2f(200, 100));
    container2.setTexture(&containerTexture);
    container2.setPosition(700, 200);

    sf::RectangleShape container3(sf::Vector2f(200, 100));
    container3.setTexture(&containerTexture);
    container3.setPosition(100, 500);

    sf::RectangleShape container4(sf::Vector2f(200, 100));
    container4.setTexture(&containerTexture);
    container4.setPosition(700, 500);

    // Crates
    sf::RectangleShape crate1(sf::Vector2f(80, 80));
    crate1.setTexture(&crateTexture);
    crate1.setPosition(450, 300);

    sf::RectangleShape crate2(sf::Vector2f(80, 80));
    crate2.setTexture(&crateTexture);
    crate2.setPosition(450, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 220);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(820, 520);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(container1);
        window.draw(container2);
        window.draw(container3);
        window.draw(container4);
        window.draw(crate1);
        window.draw(crate2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Hijacked Map - gameXpro+");

    // Load textures
    sf::Texture deckTexture;
    deckTexture.loadFromFile("deck.png");

    sf::Texture cabinTexture;
    cabinTexture.loadFromFile("cabin.png");

    sf::Texture poolTexture;
    poolTexture.loadFromFile("pool.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn.png");

    // Deck (base ground)
    sf::RectangleShape deck(sf::Vector2f(1200, 800));
    deck.setTexture(&deckTexture);

    // Cabins
    sf::RectangleShape cabin1(sf::Vector2f(200, 150));
    cabin1.setTexture(&cabinTexture);
    cabin1.setPosition(100, 100);

    sf::RectangleShape cabin2(sf::Vector2f(200, 150));
    cabin2.setTexture(&cabinTexture);
    cabin2.setPosition(900, 100);

    sf::RectangleShape cabin3(sf::Vector2f(200, 150));
    cabin3.setTexture(&cabinTexture);
    cabin3.setPosition(100, 550);

    sf::RectangleShape cabin4(sf::Vector2f(200, 150));
    cabin4.setTexture(&cabinTexture);
    cabin4.setPosition(900, 550);

    // Central pool
    sf::RectangleShape pool(sf::Vector2f(300, 200));
    pool.setTexture(&poolTexture);
    pool.setPosition(450, 300);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(deck);
        window.draw(cabin1);
        window.draw(cabin2);
        window.draw(cabin3);
        window.draw(cabin4);
        window.draw(pool);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Nukedtown Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("ground.png");

    sf::Texture houseTexture;
    houseTexture.loadFromFile("house.png");

    sf::Texture vehicleTexture;
    vehicleTexture.loadFromFile("vehicle.png");

    sf::Texture fenceTexture;
    fenceTexture.loadFromFile("fence.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Houses
    sf::RectangleShape house1(sf::Vector2f(250, 200));
    house1.setTexture(&houseTexture);
    house1.setPosition(100, 100);

    sf::RectangleShape house2(sf::Vector2f(250, 200));
    house2.setTexture(&houseTexture);
    house2.setPosition(850, 100);

    // Central bus
    sf::RectangleShape bus(sf::Vector2f(200, 80));
    bus.setTexture(&vehicleTexture);
    bus.setPosition(500, 300);

    // Central truck
    sf::RectangleShape truck(sf::Vector2f(150, 80));
    truck.setTexture(&vehicleTexture);
    truck.setPosition(520, 420);

    // Fences/backyards
    sf::RectangleShape fence1(sf::Vector2f(250, 50));
    fence1.setTexture(&fenceTexture);
    fence1.setPosition(100, 320);

    sf::RectangleShape fence2(sf::Vector2f(250, 50));
    fence2.setTexture(&fenceTexture);
    fence2.setPosition(850, 320);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(house1);
        window.draw(house2);
        window.draw(bus);
        window.draw(truck);
        window.draw(fence1);
        window.draw(fence2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Killonhousees Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("ground.png");

    sf::Texture wallTexture;
    wallTexture.loadFromFile("wall.png");

    sf::Texture crateTexture;
    crateTexture.loadFromFile("crate.png");

    sf::Texture barrelTexture;
    barrelTexture.loadFromFile("barrel.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Warehouse walls
    sf::RectangleShape wallLeft(sf::Vector2f(100, 800));
    wallLeft.setTexture(&wallTexture);
    wallLeft.setPosition(0, 0);

    sf::RectangleShape wallRight(sf::Vector2f(100, 800));
    wallRight.setTexture(&wallTexture);
    wallRight.setPosition(1100, 0);

    // Central crates
    sf::RectangleShape crate1(sf::Vector2f(100, 100));
    crate1.setTexture(&crateTexture);
    crate1.setPosition(500, 300);

    sf::RectangleShape crate2(sf::Vector2f(100, 100));
    crate2.setTexture(&crateTexture);
    crate2.setPosition(600, 400);

    // Barrels
    sf::CircleShape barrel1(40);
    barrel1.setTexture(&barrelTexture);
    barrel1.setPosition(700, 350);

    sf::CircleShape barrel2(40);
    barrel2.setTexture(&barrelTexture);
    barrel2.setPosition(400, 350);

    // Spawn zones
    sf::CircleShape spawnA(50);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 350);

    sf::CircleShape spawnB(50);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1020, 350);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(wallLeft);
        window.draw(wallRight);
        window.draw(crate1);
        window.draw(crate2);
        window.draw(barrel1);
        window.draw(barrel2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Standoff Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("ground.png");

    sf::Texture buildingTexture;
    buildingTexture.loadFromFile("building.png");

    sf::Texture plazaTexture;
    plazaTexture.loadFromFile("plaza.png");

    sf::Texture vehicleTexture;
    vehicleTexture.loadFromFile("vehicle.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Central plaza
    sf::RectangleShape plaza(sf::Vector2f(300, 200));
    plaza.setTexture(&plazaTexture);
    plaza.setPosition(450, 300);

    // Buildings
    sf::RectangleShape building1(sf::Vector2f(200, 150));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(200, 150));
    building2.setTexture(&buildingTexture);
    building2.setPosition(900, 100);

    sf::RectangleShape building3(sf::Vector2f(200, 150));
    building3.setTexture(&buildingTexture);
    building3.setPosition(100, 550);

    sf::RectangleShape building4(sf::Vector2f(200, 150));
    building4.setTexture(&buildingTexture);
    building4.setPosition(900, 550);

    // Vehicles
    sf::RectangleShape car(sf::Vector2f(100, 50));
    car.setTexture(&vehicleTexture);
    car.setPosition(550, 250);

    sf::RectangleShape truck(sf::Vector2f(150, 70));
    truck.setTexture(&vehicleTexture);
    truck.setPosition(550, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(plaza);
        window.draw(building1);
        window.draw(building2);
        window.draw(building3);
        window.draw(building4);
        window.draw(car);
        window.draw(truck);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Standoff Halloweenings Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("halloweenings_ground.png");

    sf::Texture hauntedBuildingTexture;
    hauntedBuildingTexture.loadFromFile("haunted_building.png");

    sf::Texture plazaTexture;
    plazaTexture.loadFromFile("plaza_halloween.png");

    sf::Texture vehicleTexture;
    vehicleTexture.loadFromFile("rusty_vehicle.png");

    sf::Texture pumpkinTexture;
    pumpkinTexture.loadFromFile("pumpkin.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Central plaza
    sf::RectangleShape plaza(sf::Vector2f(300, 200));
    plaza.setTexture(&plazaTexture);
    plaza.setPosition(450, 300);

    // Haunted buildings
    sf::RectangleShape building1(sf::Vector2f(200, 150));
    building1.setTexture(&hauntedBuildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(200, 150));
    building2.setTexture(&hauntedBuildingTexture);
    building2.setPosition(900, 100);

    sf::RectangleShape building3(sf::Vector2f(200, 150));
    building3.setTexture(&hauntedBuildingTexture);
    building3.setPosition(100, 550);

    sf::RectangleShape building4(sf::Vector2f(200, 150));
    building4.setTexture(&hauntedBuildingTexture);
    building4.setPosition(900, 550);

    // Vehicles
    sf::RectangleShape car(sf::Vector2f(100, 50));
    car.setTexture(&vehicleTexture);
    car.setPosition(550, 250);

    sf::RectangleShape truck(sf::Vector2f(150, 70));
    truck.setTexture(&vehicleTexture);
    truck.setPosition(550, 500);

    // Halloween props (pumpkins)
    sf::CircleShape pumpkin1(30);
    pumpkin1.setTexture(&pumpkinTexture);
    pumpkin1.setPosition(480, 320);

    sf::CircleShape pumpkin2(30);
    pumpkin2.setTexture(&pumpkinTexture);
    pumpkin2.setPosition(720, 320);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(plaza);
        window.draw(building1);
        window.draw(building2);
        window.draw(building3);
        window.draw(building4);
        window.draw(car);
        window.draw(truck);
        window.draw(pumpkin1);
        window.draw(pumpkin2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
// Load Halloween textures
sf::Texture groundTexture;
groundTexture.loadFromFile("halloween_ground.png"); // cracked cobblestone or spooky dirt

sf::Texture hauntedBuildingTexture;
hauntedBuildingTexture.loadFromFile("haunted_building.png"); // boarded windows, cobwebs

sf::Texture plazaTexture;
plazaTexture.loadFromFile("plaza_halloween.png"); // eerie tiles with lanterns

sf::Texture vehicleTexture;
vehicleTexture.loadFromFile("rusty_vehicle.png"); // abandoned cars with graffiti

sf::Texture pumpkinTexture;
pumpkinTexture.loadFromFile("pumpkin.png"); // jack-o’-lanterns

sf::Texture spawnTexture;
spawnTexture.loadFromFile("spawn_glow.png"); // glowing spawn zones
sf::RectangleShape building1(sf::Vector2f(200, 150));
building1.setTexture(&hauntedBuildingTexture);
building1.setPosition(100, 100);

sf::CircleShape pumpkin1(30);
pumpkin1.setTexture(&pumpkinTexture);
pumpkin1.setPosition(480, 320);
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Standoff Halloween Map - COD Mobile");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("halloween_ground.png");

    sf::Texture hauntedBuildingTexture;
    hauntedBuildingTexture.loadFromFile("haunted_building.png");

    sf::Texture plazaTexture;
    plazaTexture.loadFromFile("plaza_halloween.png");

    sf::Texture vehicleTexture;
    vehicleTexture.loadFromFile("rusty_vehicle.png");

    sf::Texture pumpkinTexture;
    pumpkinTexture.loadFromFile("pumpkin.png");

    sf::Texture tombstoneTexture;
    tombstoneTexture.loadFromFile("tombstone.png");

    sf::Texture lanternTexture;
    lanternTexture.loadFromFile("lantern.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Central plaza
    sf::RectangleShape plaza(sf::Vector2f(300, 200));
    plaza.setTexture(&plazaTexture);
    plaza.setPosition(450, 300);

    // Haunted buildings
    sf::RectangleShape building1(sf::Vector2f(200, 150));
    building1.setTexture(&hauntedBuildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(200, 150));
    building2.setTexture(&hauntedBuildingTexture);
    building2.setPosition(900, 100);

    // Vehicles
    sf::RectangleShape car(sf::Vector2f(100, 50));
    car.setTexture(&vehicleTexture);
    car.setPosition(550, 250);

    // Pumpkins
    sf::CircleShape pumpkin1(30);
    pumpkin1.setTexture(&pumpkinTexture);
    pumpkin1.setPosition(480, 320);

    // Tombstones
    sf::RectangleShape tombstone1(sf::Vector2f(40, 60));
    tombstone1.setTexture(&tombstoneTexture);
    tombstone1.setPosition(600, 350);

    sf::RectangleShape tombstone2(sf::Vector2f(40, 60));
    tombstone2.setTexture(&tombstoneTexture);
    tombstone2.setPosition(700, 370);

    // Lanterns
    sf::CircleShape lantern1(20);
    lantern1.setTexture(&lanternTexture);
    lantern1.setPosition(450, 500);

    sf::CircleShape lantern2(20);
    lantern2.setTexture(&lanternTexture);
    lantern2.setPosition(750, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(plaza);
        window.draw(building1);
        window.draw(building2);
        window.draw(car);
        window.draw(pumpkin1);
        window.draw(tombstone1);
        window.draw(tombstone2);
        window.draw(lantern1);
        window.draw(lantern2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Nuketown Russian Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture;
    groundTexture.loadFromFile("snow_ground.png");

    sf::Texture houseTexture;
    houseTexture.loadFromFile("russian_house.png");

    sf::Texture vehicleTexture;
    vehicleTexture.loadFromFile("snow_vehicle.png");

    sf::Texture fenceTexture;
    fenceTexture.loadFromFile("snow_fence.png");

    sf::Texture spawnTexture;
    spawnTexture.loadFromFile("spawn_glow.png");

    sf::Texture propTexture;
    propTexture.loadFromFile("snow_pile.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Houses
    sf::RectangleShape house1(sf::Vector2f(250, 200));
    house1.setTexture(&houseTexture);
    house1.setPosition(100, 100);

    sf::RectangleShape house2(sf::Vector2f(250, 200));
    house2.setTexture(&houseTexture);
    house2.setPosition(850, 100);

    // Central bus
    sf::RectangleShape bus(sf::Vector2f(200, 80));
    bus.setTexture(&vehicleTexture);
    bus.setPosition(500, 300);

    // Central truck
    sf::RectangleShape truck(sf::Vector2f(150, 80));
    truck.setTexture(&vehicleTexture);
    truck.setPosition(520, 420);

    // Fences/backyards
    sf::RectangleShape fence1(sf::Vector2f(250, 50));
    fence1.setTexture(&fenceTexture);
    fence1.setPosition(100, 320);

    sf::RectangleShape fence2(sf::Vector2f(250, 50));
    fence2.setTexture(&fenceTexture);
    fence2.setPosition(850, 320);

    // Winter props (snow piles)
    sf::RectangleShape snowPile(sf::Vector2f(60, 40));
    snowPile.setTexture(&propTexture);
    snowPile.setPosition(600, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(house1);
        window.draw(house2);
        window.draw(bus);
        window.draw(truck);
        window.draw(fence1);
        window.draw(fence2);
        window.draw(snowPile);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Nuketown Russia - Snowy Atmosphere");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("snow_ground.png");
    sf::Texture houseTexture; houseTexture.loadFromFile("russian_house.png");
    sf::Texture vehicleTexture; vehicleTexture.loadFromFile("snow_vehicle.png");
    sf::Texture fenceTexture; fenceTexture.loadFromFile("snow_fence.png");
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");
    sf::Texture propTexture; propTexture.loadFromFile("snow_pile.png");
    sf::Texture lamppostTexture; lamppostTexture.loadFromFile("lamppost.png");
    sf::Texture bannerTexture; bannerTexture.loadFromFile("banner.png");
    sf::Texture fogTexture; fogTexture.loadFromFile("fog.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Houses
    sf::RectangleShape house1(sf::Vector2f(250, 200));
    house1.setTexture(&houseTexture);
    house1.setPosition(100, 100);

    sf::RectangleShape house2(sf::Vector2f(250, 200));
    house2.setTexture(&houseTexture);
    house2.setPosition(850, 100);

    // Vehicles
    sf::RectangleShape bus(sf::Vector2f(200, 80));
    bus.setTexture(&vehicleTexture);
    bus.setPosition(500, 300);

    sf::RectangleShape truck(sf::Vector2f(150, 80));
    truck.setTexture(&vehicleTexture);
    truck.setPosition(520, 420);

    // Fences
    sf::RectangleShape fence1(sf::Vector2f(250, 50));
    fence1.setTexture(&fenceTexture);
    fence1.setPosition(100, 320);

    sf::RectangleShape fence2(sf::Vector2f(250, 50));
    fence2.setTexture(&fenceTexture);
    fence2.setPosition(850, 320);

    // Props (snow piles)
    sf::RectangleShape snowPile(sf::Vector2f(60, 40));
    snowPile.setTexture(&propTexture);
    snowPile.setPosition(600, 500);

    // Lampposts
    sf::RectangleShape lamppost(sf::Vector2f(20, 120));
    lamppost.setTexture(&lamppostTexture);
    lamppost.setPosition(400, 200);

    // Banners
    sf::RectangleShape banner(sf::Vector2f(150, 40));
    banner.setTexture(&bannerTexture);
    banner.setPosition(500, 100);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    // Lighting effect (soft glow)
    sf::CircleShape lightGlow(100);
    lightGlow.setFillColor(sf::Color(255, 200, 100, 80)); // orange glow with transparency
    lightGlow.setPosition(380, 150);

    // Fog overlay
    sf::RectangleShape fog(sf::Vector2f(1200, 800));
    fog.setTexture(&fogTexture);
    fog.setFillColor(sf::Color(255, 255, 255, 100)); // semi-transparent fog

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(house1);
        window.draw(house2);
        window.draw(bus);
        window.draw(truck);
        window.draw(fence1);
        window.draw(fence2);
        window.draw(snowPile);
        window.draw(lamppost);
        window.draw(banner);
        window.draw(spawnA);
        window.draw(spawnB);
        window.draw(lightGlow);
        window.draw(fog); // drawn last for overlay
        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Raidex Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("raidex_ground.png"); // stone courtyard
    sf::Texture houseTexture; houseTexture.loadFromFile("raidex_house.png");    // villa walls
    sf::Texture poolTexture; poolTexture.loadFromFile("pool.png");            // water
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Mansion (main villa)
    sf::RectangleShape mansion(sf::Vector2f(400, 250));
    mansion.setTexture(&houseTexture);
    mansion.setPosition(100, 100);

    // Courtyard pool
    sf::RectangleShape pool(sf::Vector2f(200, 100));
    pool.setTexture(&poolTexture);
    pool.setPosition(600, 300);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(mansion);
        window.draw(pool);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Standoff Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("standoff_ground.png");
    sf::Texture buildingTexture; buildingTexture.loadFromFile("standoff_building.png");
    sf::Texture plazaTexture; plazaTexture.loadFromFile("plaza.png");
    sf::Texture vehicleTexture; vehicleTexture.loadFromFile("standoff_vehicle.png");
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Central plaza
    sf::RectangleShape plaza(sf::Vector2f(300, 200));
    plaza.setTexture(&plazaTexture);
    plaza.setPosition(450, 300);

    // Buildings
    sf::RectangleShape building1(sf::Vector2f(200, 150));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(200, 150));
    building2.setTexture(&buildingTexture);
    building2.setPosition(900, 100);

    // Vehicle
    sf::RectangleShape car(sf::Vector2f(100, 50));
    car.setTexture(&vehicleTexture);
    car.setPosition(550, 250);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(plaza);
        window.draw(building1);
        window.draw(building2);
        window.draw(car);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Take it on Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("takeoff_ground.png"); // concrete base
    sf::Texture rocketTexture; rocketTexture.loadFromFile("rocket.png");         // launch rocket
    sf::Texture buildingTexture; buildingTexture.loadFromFile("hangar.png");     // industrial hangar
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Rocket
    sf::RectangleShape rocket(sf::Vector2f(100, 300));
    rocket.setTexture(&rocketTexture);
    rocket.setPosition(500, 200);

    // Hangar building
    sf::RectangleShape hangar(sf::Vector2f(300, 200));
    hangar.setTexture(&buildingTexture);
    hangar.setPosition(100, 100);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(rocket);
        window.draw(hangar);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "cold Raid Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("snow_ground.png"); 
    sf::Texture houseTexture; houseTexture.loadFromFile("winter_villa.png");   
    sf::Texture poolTexture; poolTexture.loadFromFile("frozen_pool.png");      
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Mansion (snow-covered villa)
    sf::RectangleShape mansion(sf::Vector2f(400, 250));
    mansion.setTexture(&houseTexture);
    mansion.setPosition(100, 100);

    // Frozen pool
    sf::RectangleShape pool(sf::Vector2f(200, 100));
    pool.setTexture(&poolTexture);
    pool.setPosition(600, 300);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(mansion);
        window.draw(pool);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Cartridge Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("cage_ground.png");     // concrete floor
    sf::Texture wallTexture; wallTexture.loadFromFile("graffiti_wall.png");       // graffiti walls
    sf::Texture propTexture; propTexture.loadFromFile("steel_barrel.png");        // barrels/cover
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Walls
    sf::RectangleShape wall1(sf::Vector2f(300, 200));
    wall1.setTexture(&wallTexture);
    wall1.setPosition(100, 100);

    sf::RectangleShape wall2(sf::Vector2f(300, 200));
    wall2.setTexture(&wallTexture);
    wall2.setPosition(800, 100);

    // Props (barrels)
    sf::RectangleShape barrel(sf::Vector2f(60, 80));
    barrel.setTexture(&propTexture);
    barrel.setPosition(550, 350);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(wall1);
        window.draw(wall2);
        window.draw(barrel);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Meltondowns Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("meltdown_ground.png"); // industrial concrete
    sf::Texture towerTexture; towerTexture.loadFromFile("cooling_tower.png");     // nuclear cooling towers
    sf::Texture pipeTexture; pipeTexture.loadFromFile("pipes.png");               // industrial pipes
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Cooling towers
    sf::RectangleShape tower1(sf::Vector2f(200, 300));
    tower1.setTexture(&towerTexture);
    tower1.setPosition(200, 200);

    sf::RectangleShape tower2(sf::Vector2f(200, 300));
    tower2.setTexture(&towerTexture);
    tower2.setPosition(800, 200);

    // Pipes
    sf::RectangleShape pipes(sf::Vector2f(400, 80));
    pipes.setTexture(&pipeTexture);
    pipes.setPosition(400, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(tower1);
        window.draw(tower2);
        window.draw(pipes);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Rust Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("desert_ground.png");   // sandy base
    sf::Texture scaffoldTexture; scaffoldTexture.loadFromFile("scaffold.png");    // oil rig scaffolding
    sf::Texture ladderTexture; ladderTexture.loadFromFile("ladder.png");          // ladders
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Central scaffolding
    sf::RectangleShape scaffold(sf::Vector2f(300, 400));
    scaffold.setTexture(&scaffoldTexture);
    scaffold.setPosition(450, 200);

    // Ladder
    sf::RectangleShape ladder(sf::Vector2f(40, 200));
    ladder.setTexture(&ladderTexture);
    ladder.setPosition(700, 300);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(scaffold);
        window.draw(ladder);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Saloon Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("desert_ground.png");   // dusty desert
    sf::Texture saloonTexture; saloonTexture.loadFromFile("saloon.png");          // wooden saloon
    sf::Texture propTexture; propTexture.loadFromFile("barrels.png");             // barrels/props
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Saloon building
    sf::RectangleShape saloon(sf::Vector2f(300, 200));
    saloon.setTexture(&saloonTexture);
    saloon.setPosition(450, 300);

    // Props (barrels)
    sf::RectangleShape barrels(sf::Vector2f(80, 60));
    barrels.setTexture(&propTexture);
    barrels.setPosition(600, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(saloon);
        window.draw(barrels);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Tunisia Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("sand_ground.png");       // sandy streets
    sf::Texture buildingTexture; buildingTexture.loadFromFile("mediterranean_building.png"); // arches, stone walls
    sf::Texture propTexture; propTexture.loadFromFile("market_stall.png");          // stalls/props
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Buildings
    sf::RectangleShape building1(sf::Vector2f(250, 200));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(250, 200));
    building2.setTexture(&buildingTexture);
    building2.setPosition(850, 100);

    // Market stall
    sf::RectangleShape stall(sf::Vector2f(150, 100));
    stall.setTexture(&propTexture);
    stall.setPosition(500, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(building1);
        window.draw(building2);
        window.draw(stall);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Gulag Showers Map - gameXpro+");

    // Load textures
    sf::Texture ()groundTexture; groundTexture.loadFromFile("tile_ground.png");       // tiled floor
    sf::Texture wallTexture; wallTexture.loadFromFile("prison_wall.png");           // concrete prison walls
    sf::Texture pipeTexture; pipeTexture.loadFromFile("pipes.png");                 // industrial pipes
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Walls
    sf::RectangleShape wall1(sf::Vector2f(300, 200));
    wall1.setTexture(&wallTexture);
    wall1.setPosition(100, 100);

    sf::RectangleShape wall2(sf::Vector2f(300, 200));
    wall2.setTexture(&wallTexture);
    wall2.setPosition(800, 100);

    // Pipes
    sf::RectangleShape pipes(sf::Vector2f(400, 80));
    pipes.setTexture(&pipeTexture);
    pipes.setPosition(400, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(wall1);
        window.draw(wall2);
        window.draw(pipes);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Highrise Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("rooftop_ground.png");   // rooftop surface
    sf::Texture buildingTexture; buildingTexture.loadFromFile("office_building.png"); // skyscraper walls
    sf::Texture craneTexture; craneTexture.loadFromFile("crane.png");              // rooftop crane
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Office building section
    sf::RectangleShape office(sf::Vector2f(300, 200));
    office.setTexture(&buildingTexture);
    office.setPosition(100, 100);

    // Rooftop crane
    sf::RectangleShape crane(sf::Vector2f(200, 150));
    crane.setTexture(&craneTexture);
    crane.setPosition(700, 300);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(office);
        window.draw(crane);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Terminalizatoin Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("terminal_ground.png");   // airport flooring
    sf::Texture loungeTexture; loungeTexture.loadFromFile("airport_lounge.png");    // lounge area
    sf::Texture airplaneTexture; airplaneTexture.loadFromFile("airplane.png");      // airplane parked
    sf::Texture gateTexture; gateTexture.loadFromFile("boarding_gate.png");         // boarding gate
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Airport lounge
    sf::RectangleShape lounge(sf::Vector2f(300, 200));
    lounge.setTexture(&loungeTexture);
    lounge.setPosition(100, 100);

    // Boarding gate
    sf::RectangleShape gate(sf::Vector2f(200, 150));
    gate.setTexture(&gateTexture);
    gate.setPosition(800, 100);

    // Airplane
    sf::RectangleShape airplane(sf::Vector2f(400, 200));
    airplane.setTexture(&airplaneTexture);
    airplane.setPosition(400, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(lounge);
        window.draw(gate);
        window.draw(airplane);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Pine Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("pine_ground.png");     // forest floor
    sf::Texture treeTexture; treeTexture.loadFromFile("pine_tree.png");           // pine trees
    sf::Texture crateTexture; crateTexture.loadFromFile("training_crate.png");    // crates for cover
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Trees
    sf::RectangleShape tree1(sf::Vector2f(80, 200));
    tree1.setTexture(&treeTexture);
    tree1.setPosition(200, 150);

    sf::RectangleShape tree2(sf::Vector2f(80, 200));
    tree2.setTexture(&treeTexture);
    tree2.setPosition(900, 150);

    // Training crates
    sf::RectangleShape crate(sf::Vector2f(100, 80));
    crate.setTexture(&crateTexture);
    crate.setPosition(550, 350);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(tree1);
        window.draw(tree2);
        window.draw(crate);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "King Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("warehouse_ground.png"); // concrete floor
    sf::Texture wallTexture; wallTexture.loadFromFile("warehouse_wall.png");       // warehouse walls
    sf::Texture crateTexture; crateTexture.loadFromFile("training_crate.png");     // crates for cover
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Warehouse walls
    sf::RectangleShape wall1(sf::Vector2f(300, 200));
    wall1.setTexture(&wallTexture);
    wall1.setPosition(100, 100);

    sf::RectangleShape wall2(sf::Vector2f(300, 200));
    wall2.setTexture(&wallTexture);
    wall2.setPosition(800, 100);

    // Training crates
    sf::RectangleShape crate1(sf::Vector2f(100, 80));
    crate1.setTexture(&crateTexture);
    crate1.setPosition(500, 300);

    sf::RectangleShape crate2(sf::Vector2f(100, 80));
    crate2.setTexture(&crateTexture);
    crate2.setPosition(600, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(wall1);
        window.draw(wall2);
        window.draw(crate1);
        window.draw(crate2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Hackney Yard Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("yard_ground.png");       // industrial concrete
    sf::Texture containerTexture; containerTexture.loadFromFile("container.png");   // shipping containers
    sf::Texture warehouseTexture; warehouseTexture.loadFromFile("warehouse.png");   // warehouse building
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Warehouse
    sf::RectangleShape warehouse(sf::Vector2f(300, 200));
    warehouse.setTexture(&warehouseTexture);
    warehouse.setPosition(100, 100);

    // Containers
    sf::RectangleShape container1(sf::Vector2f(200, 100));
    container1.setTexture(&containerTexture);
    container1.setPosition(500, 300);

    sf::RectangleShape container2(sf::Vector2f(200, 100));
    container2.setTexture(&containerTexture);
    container2.setPosition(800, 500);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(warehouse);
        window.draw(container1);
        window.draw(container2);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Express Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("station_ground.png");    // tiled station floor
    sf::Texture trainTexture; trainTexture.loadFromFile("train.png");               // train cars
    sf::Texture platformTexture; platformTexture.loadFromFile("platform.png");      // station platform
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Train
    sf::RectangleShape train(sf::Vector2f(400, 150));
    train.setTexture(&trainTexture);
    train.setPosition(400, 300);

    // Platform
    sf::RectangleShape platform(sf::Vector2f(300, 100));
    platform.setTexture(&platformTexture);
    platform.setPosition(100, 100);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(train);
        window.draw(platform);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Slums Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("slums_ground.png");       // dusty street
    sf::Texture buildingTexture; buildingTexture.loadFromFile("slums_building.png"); // graffiti walls
    sf::Texture propTexture; propTexture.loadFromFile("market_cart.png");            // carts/props
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Buildings
    sf::RectangleShape building1(sf::Vector2f(250, 200));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(250, 200));
    building2.setTexture(&buildingTexture);
    building2.setPosition(850, 100);

    // Market cart
    sf::RectangleShape cart(sf::Vector2f(120, 80));
    cart.setTexture(&propTexture);
    cart.setPosition(500, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(building1);
        window.draw(building2);
        window.draw(cart);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Monastery Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("stone_ground.png");       // stone courtyard
    sf::Texture monasteryTexture; monasteryTexture.loadFromFile("monastery.png");    // stone church
    sf::Texture propTexture; propTexture.loadFromFile("statue.png");                 // courtyard statue
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Monastery building
    sf::RectangleShape monastery(sf::Vector2f(400, 250));
    monastery.setTexture(&monasteryTexture);
    monastery.setPosition(100, 100);

    // Courtyard statue
    sf::RectangleShape statue(sf::Vector2f(100, 150));
    statue.setTexture(&propTexture);
    statue.setPosition(600, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(monastery);
        window.draw(statue);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Breachons Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("breach_ground.png");     // rubble-strewn streets
    sf::Texture buildingTexture; buildingTexture.loadFromFile("collapsed_building.png"); // destroyed buildings
    sf::Texture propTexture; propTexture.loadFromFile("sandbags.png");              // cover props
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Collapsed buildings
    sf::RectangleShape building1(sf::Vector2f(250, 200));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(250, 200));
    building2.setTexture(&buildingTexture);
    building2.setPosition(850, 100);

    // Sandbag cover
    sf::RectangleShape sandbags(sf::Vector2f(150, 80));
    sandbags.setTexture(&propTexture);
    sandbags.setPosition(500, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(building1);
        window.draw(building2);
        window.draw(sandbags);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Aniyah Incursion Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("aniyah_ground.png");     // palace courtyard
    sf::Texture palaceTexture; palaceTexture.loadFromFile("palace.png");            // palace building
    sf::Texture wreckTexture; wreckTexture.loadFromFile("wreckage.png");            // destroyed vehicles
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Palace building
    sf::RectangleShape palace(sf::Vector2f(400, 250));
    palace.setTexture(&palaceTexture);
    palace.setPosition(100, 100);

    // Wrecked vehicle
    sf::RectangleShape wreck(sf::Vector2f(200, 100));
    wreck.setTexture(&wreckTexture);
    wreck.setPosition(600, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(palace);
        window.draw(wreck);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Shoot House Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("shoot_house_ground.png"); // dirt training ground
    sf::Texture wallTexture; wallTexture.loadFromFile("plywood_wall.png");           // plywood walls
    sf::Texture crateTexture; crateTexture.loadFromFile("training_crate.png");       // crates for cover
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Plywood walls
    sf::RectangleShape wall1(sf::Vector2f(300, 200));
    wall1.setTexture(&wallTexture);
    wall1.setPosition(100, 100);

    sf::RectangleShape wall2(sf::Vector2f(300, 200));
    wall2.setTexture(&wallTexture);
    wall2.setPosition(800, 100);

    // Training crates
    sf::RectangleShape crate(sf::Vector2f(100, 80));
    crate.setTexture(&crateTexture);
    crate.setPosition(550, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(wall1);
        window.draw(wall2);
        window.draw(crate);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Haciendans Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("haciendans_ground.png");   // tiled courtyard
    sf::Texture villaTexture; villaTexture.loadFromFile("haciendans_villa.png");      // luxury villa
    sf::Texture wineCellarTexture; wineCellarTexture.loadFromFile("wine_cellar.png"); // wine cellar
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Villa building
    sf::RectangleShape villa(sf::Vector2f(400, 250));
    villa.setTexture(&villaTexture);
    villa.setPosition(100, 100);

    // Wine cellar
    sf::RectangleShape cellar(sf::Vector2f(200, 150));
    cellar.setTexture(&wineCellarTexture);
    cellar.setPosition(700, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(villa);
        window.draw(cellar);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Dommed Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("desert_ground.png");   // desert sand
    sf::Texture domeTexture; domeTexture.loadFromFile("radar_dommed.png");          // radar dome
    sf::Texture bunkerTexture; bunkerTexture.loadFromFile("bunker.png");          // military bunker
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Radar dome
    sf::RectangleShape dome(sf::Vector2f(300, 300));
    dome.setTexture(&domeTexture);
    dome.setPosition(450, 200);

    // Bunker
    sf::RectangleShape bunker(sf::Vector2f(250, 150));
    bunker.setTexture(&bunkerTexture);
    bunker.setPosition(150, 100);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(dome);
        window.draw(bunker);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Hardhatats Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("construction_ground.png"); // dirt/concrete
    sf::Texture scaffoldTexture; scaffoldTexture.loadFromFile("scaffold.png");       // scaffolding
    sf::Texture craneTexture; craneTexture.loadFromFile("construction_crane.png");   // crane
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Scaffolding
    sf::RectangleShape scaffold(sf::Vector2f(300, 200));
    scaffold.setTexture(&scaffoldTexture);
    scaffold.setPosition(200, 200);

    // Crane
    sf::RectangleShape crane(sf::Vector2f(250, 150));
    crane.setTexture(&craneTexture);
    crane.setPosition(700, 300);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(scaffold);
        window.draw(crane);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Vacant Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("vacant_ground.png");     // cracked concrete
    sf::Texture officeTexture; officeTexture.loadFromFile("office_room.png");       // office rooms
    sf::Texture crateTexture; crateTexture.loadFromFile("storage_crate.png");       // crates/cover
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Office rooms
    sf::RectangleShape office(sf::Vector2f(300, 200));
    office.setTexture(&officeTexture);
    office.setPosition(100, 100);

    // Storage crates
    sf::RectangleShape crate(sf::Vector2f(150, 100));
    crate.setTexture(&crateTexture);
    crate.setPosition(600, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(office);
        window.draw(crate);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>

void runMapName() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Jungle Map - gameXpro+");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("jungle_ground.png");     // tropical forest floor
    sf::Texture treeTexture; treeTexture.loadFromFile("jungle_tree.png");           // tall jungle trees
    sf::Texture rockTexture; rockTexture.loadFromFile("jungle_rock.png");           // rocks for cover
    sf::Texture spawnTexture; spawnTexture.loadFromFile("spawn_glow.png");

    // Ground
    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    // Trees
    sf::RectangleShape tree1(sf::Vector2f(100, 250));
    tree1.setTexture(&treeTexture);
    tree1.setPosition(200, 150);

    sf::RectangleShape tree2(sf::Vector2f(100, 250));
    tree2.setTexture(&treeTexture);
    tree2.setPosition(900, 150);

    // Rocks
    sf::RectangleShape rock(sf::Vector2f(120, 80));
    rock.setTexture(&rockTexture);
    rock.setPosition(550, 400);

    // Spawn zones
    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(1040, 620);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(ground);
        window.draw(tree1);
        window.draw(tree2);
        window.draw(rock);
        window.draw(spawnA);
        window.draw(spawnB);
        window.display();
    }

    return 0;
}
// Winter props (snow piles)
sf::RectangleShape snowPile(sf::Vector2f(60, 40));
snowPile.setTexture(&propTexture);
snowPile.setPosition(600, 500); // example position

// In the render loop
window.draw(snowPile);

// ------------------ Main Program ------------------
int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "gameXpro+");

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Menu options
    sf::Text option1("1. Crashes Map", font, 30); option1.setPosition(100, 100);
    sf::Text option2("2. Crossfiring Map", font, 30); option2.setPosition(100, 150);
    sf::Text option3("3. Firing Ranges Map", font, 30); option3.setPosition(100, 200);
    sf::Text option4("4. Hijacked Map", font, 30); option4.setPosition(100, 250);
    sf::Text option5("5. Nuketown Map", font, 30); option5.setPosition(100, 300);
    sf::Text option6("6. Killonhousees Map", font, 30); option6.setPosition(100, 350);
    sf::Text option7("7. Standoff Map", font, 30); option7.setPosition(100, 400);
    sf::Text option8("8. Standoff Halloween Map", font, 30); option8.setPosition(100, 450);
    sf::Text option9("9. Shipments Map", font, 30); option9.setPosition(100, 500);
    sf::Text option10("0. Nuketown Russia Map", font, 30); option10.setPosition(100, 550);

    // Extra maps with letter keys
    sf::Text option11("Q. Raidex Map", font, 30); option11.setPosition(500, 100);
    sf::Text option12("W. Scrapyard Map", font, 30); option12.setPosition(500, 150);
    sf::Text option13("E. Cages Map", font, 30); option13.setPosition(500, 200);
    sf::Text option14("R. Rust Map", font, 30); option14.setPosition(500, 250);
    sf::Text option15("T. Saloon Map", font, 30); option15.setPosition(500, 300);
    sf::Text option16("Y. Tunisia Map", font, 30); option16.setPosition(500, 350);
    sf::Text option17("U. Gulag Showers Map", font, 30); option17.setPosition(500, 400);
    sf::Text option18("I. Highrise Map", font, 30); option18.setPosition(500, 450);
    sf::Text option19("O. Terminal Map", font, 30); option19.setPosition(500, 500);
    sf::Text option20("P. Pine Map", font, 30); option20.setPosition(500, 550);
    sf::Text option21("A. King Map", font, 30); option21.setPosition(900, 100);
    sf::Text option22("S. Hackney Yard Map", font, 30); option22.setPosition(900, 150);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) currentState = CRASHES;
                if (event.key.code == sf::Keyboard::Num2) currentState = CROSSFIRING;
                if (event.key.code == sf::Keyboard::Num3) currentState = FIRING_RANGES;
                if (event.key.code == sf::Keyboard::Num4) currentState = HIJACKED;
                if (event.key.code == sf::Keyboard::Num5) currentState = NUKETOWN;
                if (event.key.code == sf::Keyboard::Num6) currentState = KILLONHOUSE;
                if (event.key.code == sf::Keyboard::Num7) currentState = STANDOFF;
                if (event.key.code == sf::Keyboard::Num8) currentState = STANDOFF_HALLOWEEN;
                if (event.key.code == sf::Keyboard::Num9) currentState = SHIPMENTS;
                if (event.key.code == sf::Keyboard::Num0) currentState = NUKETOWN_RUSSIA;

                if (event.key.code == sf::Keyboard::Q) currentState = RAIDEX;
                if (event.key.code == sf::Keyboard::W) currentState = SCRAPYARD;
                if (event.key.code == sf::Keyboard::E) currentState = CAGES;
                if (event.key.code == sf::Keyboard::R) currentState = RUST;
                if (event.key.code == sf::Keyboard::T) currentState = SALOON;
                if (event.key.code == sf::Keyboard::Y) currentState = TUNISIA;
                if (event.key.code == sf::Keyboard::U) currentState = GULAG_SHOWERS;
                if (event.key.code == sf::Keyboard::I) currentState = HIGHRISE;
                if (event.key.code == sf::Keyboard::O) currentState = TERMINAL;
                if (event.key.code == sf::Keyboard::P) currentState = PINE;
                if (event.key.code == sf::Keyboard::A) currentState = KING;
                if (event.key.code == sf::Keyboard::S) currentState = HACKNEY_YARD;

                if (event.key.code == sf::Keyboard::Escape) currentState = MENU;
            }
        }

        window.clear();

        if (currentState == MENU) {
            window.draw(option1); window.draw(option2); window.draw(option3);
            window.draw(option4); window.draw(option5); window.draw(option6);
            window.draw(option7); window.draw(option8); window.draw(option9);
            window.draw(option10); window.draw(option11); window.draw(option12);
            window.draw(option13); window.draw(option14); window.draw(option15);
            window.draw(option16); window.draw(option17); window.draw(option18);
            window.draw(option19); window.draw(option20); window.draw(option21);
            window.draw(option22);
        }
        else if (currentState == CRASHES) runCrashes(window);
        else if (currentState == CROSSFIRING) runCrossfiring(window);
        else if (currentState == FIRING_RANGES) runFiringRanges(window);
        else if (currentState == HIJACKED) runHijacked(window);
        else if (currentState == NUKETOWN) runNuketown(window);
        else if (currentState == KILLONHOUSE) runKillonhouse(window);
        else if (currentState == STANDOFF) runStandoff(window);
        else if (currentState == STANDOFF_HALLOWEEN) runStandoffHalloween(window);
        else if (currentState == SHIPMENTS) runShipments(window);
        else if (currentState == NUKETOWN_RUSSIA) runNuketownRussia(window);
        else if (currentState == RAIDEX) runRaidex(window);
        else if (currentState == SCRAPYARD) runScrapyard(window);
        else if (currentState == CAGES) runCages(window);
        else if (currentState == RUST) runRust(window);
        else if (currentState == SALOON) runSaloon(window);
        else if (currentState == TUNISIA) runTunisia(window);
        else if (currentState == GULAG_SHOWERS) runGulagShowers(window);
        else if (currentState == HIGHRISE) runHighrise(window);
        else if (currentState == TERMINAL) runTerminal(window);
        else if (currentState == PINE) runPine(window);
        else if (currentState == KING) runKing(window);
        else if (currentState == HACKNEY_YARD) runHackneyYard(window);

        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
// ------------------ Game States ------------------
enum GameState {
    MENU,
    CRASHES,
    CROSSFIRING,
    FIRING_RANGES,
    HIJACKED,
    KILLONHOUSE,
    NUKETOWN,
    RAIDEX,
    STANDOFF,
    STANDOFF_HALLOWEEN,
    SHIPMENTS,
    SHIPMENTS_1944,
    SHOOTING_HOUSE,
    SUMMITS,
    COLD_RAIDEX,
    SCRAPYARD,
    CAGES,
    MELTDOWN,
    RUST,
    SALOON,
    TUNISIA,
    GULAG_SHOWERS,
    HIGHRISE,
    TERMINAL,
    PINE,
    KING,
    HACKNEY_YARD,
    NUKETOWN_RUSSIA
};

GameState currentState = MENU;

// ------------------ Map Functions ------------------
void runHijacked(sf::RenderWindow& window) {
    sf::Texture deckTexture, cabinTexture, poolTexture, spawnTexture;
    deckTexture.loadFromFile("deck.png");
    cabinTexture.loadFromFile("cabin.png");
    poolTexture.loadFromFile("pool.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape deck(sf::Vector2f(1200, 800));
    deck.setTexture(&deckTexture);

    sf::RectangleShape cabin1(sf::Vector2f(200, 150));
    cabin1.setTexture(&cabinTexture);
    cabin1.setPosition(100, 100);

    sf::RectangleShape cabin2(sf::Vector2f(200, 150));
    cabin2.setTexture(&cabinTexture);
    cabin2.setPosition(900, 100);

    sf::RectangleShape pool(sf::Vector2f(300, 200));
    pool.setTexture(&poolTexture);
    pool.setPosition(450, 300);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(deck);
    window.draw(cabin1);
    window.draw(cabin2);
    window.draw(pool);
    window.draw(spawnA);
}

void runShipments(sf::RenderWindow& window) {
    sf::Texture groundTexture, containerTexture, crateTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    containerTexture.loadFromFile("container.png");
    crateTexture.loadFromFile("crate.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1000, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape container1(sf::Vector2f(200, 100));
    container1.setTexture(&containerTexture);
    container1.setPosition(100, 200);

    sf::RectangleShape crate1(sf::Vector2f(80, 80));
    crate1.setTexture(&crateTexture);
    crate1.setPosition(450, 300);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 220);

    window.draw(ground);
    window.draw(container1);
    window.draw(crate1);
    window.draw(spawnA);
}

void runNuketown(sf::RenderWindow& window) {
    sf::Texture groundTexture, houseTexture, vehicleTexture, fenceTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    houseTexture.loadFromFile("house.png");
    vehicleTexture.loadFromFile("vehicle.png");
    fenceTexture.loadFromFile("fence.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape house1(sf::Vector2f(250, 200));
    house1.setTexture(&houseTexture);
    house1.setPosition(100, 100);

    sf::RectangleShape bus(sf::Vector2f(200, 80));
    bus.setTexture(&vehicleTexture);
    bus.setPosition(500, 300);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(house1);
    window.draw(bus);
    window.draw(spawnA);
}

void runFiringRanges(sf::RenderWindow& window) {
    sf::Texture groundTexture, wallTexture, targetTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    wallTexture.loadFromFile("wall.png");
    targetTexture.loadFromFile("target.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape lane1(sf::Vector2f(1000, 80));
    lane1.setTexture(&wallTexture);
    lane1.setPosition(100, 200);

    sf::RectangleShape lane2(sf::Vector2f(1000, 80));
    lane2.setTexture(&wallTexture);
    lane2.setPosition(100, 400);

    sf::CircleShape target1(30);
    target1.setTexture(&targetTexture);
    target1.setPosition(500, 220);

    sf::CircleShape target2(30);
    target2.setTexture(&targetTexture);
    target2.setPosition(800, 420);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 220);

    sf::CircleShape spawnB(40);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(120, 420);

    window.draw(ground);
    window.draw(lane1);
    window.draw(lane2);
    window.draw(target1);
    window.draw(target2);
    window.draw(spawnA);
    window.draw(spawnB);
}

void runCrossfiring(sf::RenderWindow& window) {
    sf::Texture roadTexture, buildingTexture, spawnTexture;
    roadTexture.loadFromFile("road.png");
    buildingTexture.loadFromFile("building.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape road(sf::Vector2f(800, 100));
    road.setTexture(&roadTexture);
    road.setPosition(100, 300);

    sf::RectangleShape building1(sf::Vector2f(150, 200));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape building2(sf::Vector2f(150, 200));
    building2.setTexture(&buildingTexture);
    building2.setPosition(750, 100);

    sf::RectangleShape building3(sf::Vector2f(150, 200));
    building3.setTexture(&buildingTexture);
    building3.setPosition(100, 450);

    sf::RectangleShape building4(sf::Vector2f(150, 200));
    building4.setTexture(&buildingTexture);
    building4.setPosition(750, 450);

    sf::CircleShape spawnA(30);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 320);

    sf::CircleShape spawnB(30);
    spawnB.setTexture(&spawnTexture);
    spawnB.setPosition(820, 320);

    window.draw(road);
    window.draw(building1);
    window.draw(building2);
    window.draw(building3);
    window.draw(building4);
    window.draw(spawnA);
    window.draw(spawnB);
}

void runCrashes(sf::RenderWindow& window) {
    sf::RectangleShape building(sf::Vector2f(100, 100));
    building.setFillColor(sf::Color::Blue);
    building.setPosition(200, 200);

    sf::CircleShape crashSite(40);
    crashSite.setFillColor(sf::Color::Red);
    crashSite.setPosition(400, 300);

    window.draw(building);
    window.draw(crashSite);
}

void runKillonhouse(sf::RenderWindow& window) {
    sf::Texture groundTexture, wallTexture, crateTexture, barrelTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    wallTexture.loadFromFile("wall.png");
    crateTexture.loadFromFile("crate.png");
    barrelTexture.loadFromFile("barrel.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape wallLeft(sf::Vector2f(100, 800));
    wallLeft.setTexture(&wallTexture);
    wallLeft.setPosition(0, 0);

    sf::RectangleShape wallRight(sf::Vector2f(100, 800));
    wallRight.setTexture(&wallTexture);
    wallRight.setPosition(1100, 0);

    sf::RectangleShape crate1(sf::Vector2f(100, 100));
    crate1.setTexture(&crateTexture);
    crate1.setPosition(500, 300);

    sf::CircleShape barrel1(40);
    barrel1.setTexture(&barrelTexture);
    barrel1.setPosition(700, 350);

    sf::CircleShape spawnA(50);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 350);

    window.draw(ground);
    window.draw(wallLeft);
    window.draw(wallRight);
    window.draw(crate1);
    window.draw(barrel1);
    window.draw(spawnA);
}

void runStandoff(sf::RenderWindow& window) {
    sf::Texture groundTexture, buildingTexture, plazaTexture, vehicleTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    buildingTexture.loadFromFile("building.png");
    plazaTexture.loadFromFile("plaza.png");
    vehicleTexture.loadFromFile("vehicle.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape plaza(sf::Vector2f(300, 200));
    plaza.setTexture(&plazaTexture);
    plaza.setPosition(450, 300);

    sf::RectangleShape building1(sf::Vector2f(200, 150));
    building1.setTexture(&buildingTexture);
    building1.setPosition(100, 100);

    sf::RectangleShape car(sf::Vector2f(100, 50));
    car.setTexture(&vehicleTexture);
    car.setPosition(550, 250);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(plaza);
    window.draw(building1);
    window.draw(car);
    window.draw(spawnA);
}

void runStandoffHalloween(sf::RenderWindow& window) {
    sf::Texture groundTexture, hauntedBuildingTexture, plazaTexture, vehicleTexture, pumpkinTexture, spawnTexture;
    groundTexture.loadFromFile("halloween_ground.png");
    hauntedBuildingTexture.loadFromFile("haunted_building.png");
    plazaTexture.loadFromFile("plaza_halloween.png");
    vehicleTexture.loadFromFile("rusty_vehicle.png");
    pumpkinTexture.loadFromFile("pumpkin.png");
    spawnTexture.loadFromFile("spawn_glow.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape plaza(sf::Vector2f(300, 200));
    plaza.setTexture(&plazaTexture);
    plaza.setPosition(450, 300);

    sf::RectangleShape haunted(sf::Vector2f(200, 150));
    haunted.setTexture(&hauntedBuildingTexture);
    haunted.setPosition(100, 100);

    sf::RectangleShape car(sf::Vector2f(100, 50));
    car.setTexture(&vehicleTexture);
    car.setPosition(550, 250);

    sf::CircleShape pumpkin1(30);
    pumpkin1.setTexture(&pumpkinTexture);
    pumpkin1.setPosition(480, 320);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(plaza);
    window.draw(haunted);
    window.draw(car);
    window.draw(pumpkin1);
    window.draw(spawnA);
}

void runNuketownRussia(sf::RenderWindow& window) {
    sf::Texture groundTexture, houseTexture, vehicleTexture, fenceTexture, spawnTexture, snowTexture;
    groundTexture.loadFromFile("snow_ground.png");
    houseTexture.loadFromFile("russian_house.png");
    vehicleTexture.loadFromFile("snow_vehicle.png");
    fenceTexture.loadFromFile("snow_fence.png");
    spawnTexture.loadFromFile("spawn_glow.png");
    snowTexture.loadFromFile("snow_pile.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape house1(sf::Vector2f(250, 200));
    house1.setTexture(&houseTexture);
    house1.setPosition(100, 100);

    sf::RectangleShape bus(sf::Vector2f(200, 80));
    bus.setTexture(&vehicleTexture);
    bus.setPosition(500, 300);

    sf::RectangleShape fence1(sf::Vector2f(250, 50));
    fence1.setTexture(&fenceTexture);
    fence1.setPosition(100, 320);

    sf::RectangleShape snowPile(sf::Vector2f(60, 40));
    snowPile.setTexture(&snowTexture);
    snowPile.setPosition(600, 500);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(house1);
    window.draw(bus);
    window.draw(fence1);
    window.draw(snowPile);
    window.draw(spawnA);
}

void runRaidex(sf::RenderWindow& window) {
    sf::Texture groundTexture, buildingTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    buildingTexture.loadFromFile("building.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape building(sf::Vector2f(300, 200));
    building.setTexture(&buildingTexture);
    building.setPosition(450, 300);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(building);
    window.draw(spawnA);
}

void runScrapyard(sf::RenderWindow& window) {
    sf::Texture groundTexture, planeTexture, crateTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    planeTexture.loadFromFile("plane.png");
    crateTexture.loadFromFile("crate.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape plane(sf::Vector2f(400, 150));
    plane.setTexture(&planeTexture);
    plane.setPosition(400, 300);

    sf::RectangleShape crate(sf::Vector2f(100, 100));
    crate.setTexture(&crateTexture);
    crate.setPosition(600, 500);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(plane);
    window.draw(crate);
    window.draw(spawnA);
}

void runCages(sf::RenderWindow& window) {
    sf::Texture groundTexture, cageTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    cageTexture.loadFromFile("cage.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(800, 600));
    ground.setTexture(&groundTexture);

    sf::RectangleShape cage1(sf::Vector2f(200, 200));
    cage1.setTexture(&cageTexture);
    cage1.setPosition(100, 100);

    sf::RectangleShape cage2(sf::Vector2f(200, 200));
    cage2.setTexture(&cageTexture);
    cage2.setPosition(500, 300);

    sf::CircleShape spawnA(30);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(cage1);
    window.draw(cage2);
    window.draw(spawnA);
}

void runRust(sf::RenderWindow& window) {
    sf::Texture groundTexture, towerTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    towerTexture.loadFromFile("tower.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1000, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape tower(sf::Vector2f(200, 400));
    tower.setTexture(&towerTexture);
    tower.setPosition(400, 200);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(tower);
    window.draw(spawnA);
}

void runTerminalization(sf::RenderWindow& window) {
    sf::Texture groundTexture, planeTexture, gateTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    planeTexture.loadFromFile("plane.png");
    gateTexture.loadFromFile("gate.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape plane(sf::Vector2f(400, 150));
    plane.setTexture(&planeTexture);
    plane.setPosition(400, 300);

    sf::RectangleShape gate(sf::Vector2f(200, 200));
    gate.setTexture(&gateTexture);
    gate.setPosition(800, 200);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(plane);
    window.draw(gate);
    window.draw(spawnA);
}

void runHighrise(sf::RenderWindow& window) {
    sf::Texture groundTexture, buildingTexture, craneTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    buildingTexture.loadFromFile("building.png");
    craneTexture.loadFromFile("crane.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape building(sf::Vector2f(300, 400));
    building.setTexture(&buildingTexture);
    building.setPosition(400, 200);

    sf::RectangleShape crane(sf::Vector2f(200, 100));
    crane.setTexture(&craneTexture);
    crane.setPosition(700, 100);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(building);
    window.draw(crane);
    window.draw(spawnA);
}

void runSaloon(sf::RenderWindow& window) {
    sf::Texture groundTexture, saloonTexture, barTexture, spawnTexture;
    groundTexture.loadFromFile("ground.png");
    saloonTexture.loadFromFile("saloon.png");
    barTexture.loadFromFile("bar.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1000, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape saloon(sf::Vector2f(300, 200));
    saloon.setTexture(&saloonTexture);
    saloon.setPosition(350, 300);

    sf::RectangleShape bar(sf::Vector2f(150, 100));
    bar.setTexture(&barTexture);
    bar.setPosition(500, 500);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(saloon);
    window.draw(bar);
    window.draw(spawnA);
}

void runTunisia(sf::RenderWindow& window) {
    sf::Texture groundTexture, buildingTexture, archTexture, spawnTexture;
    groundTexture.loadFromFile("sand_ground.png");
    buildingTexture.loadFromFile("tunisia_building.png");
    archTexture.loadFromFile("arch.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape building(sf::Vector2f(250, 200));
    building.setTexture(&buildingTexture);
    building.setPosition(100, 100);

    sf::RectangleShape arch(sf::Vector2f(200, 150));
    arch.setTexture(&archTexture);
    arch.setPosition(600, 300);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(building);
    window.draw(arch);
    window.draw(spawnA);
}

void runGulagShowers(sf::RenderWindow& window) {
    sf::Texture groundTexture, wallTexture, showerTexture, spawnTexture;
    groundTexture.loadFromFile("concrete.png");
    wallTexture.loadFromFile("wall.png");
    showerTexture.loadFromFile("shower.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(800, 600));
    ground.setTexture(&groundTexture);

    sf::RectangleShape wall(sf::Vector2f(200, 400));
    wall.setTexture(&wallTexture);
    wall.setPosition(300, 100);

    sf::RectangleShape shower(sf::Vector2f(100, 150));
    shower.setTexture(&showerTexture);
    shower.setPosition(500, 300);

    sf::CircleShape spawnA(30);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(wall);
    window.draw(shower);
    window.draw(spawnA);
}

void runPine(sf::RenderWindow& window) {
    sf::Texture groundTexture, treeTexture, spawnTexture;
    groundTexture.loadFromFile("forest_ground.png");
    treeTexture.loadFromFile("pine_tree.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1000, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape tree(sf::Vector2f(100, 200));
    tree.setTexture(&treeTexture);
    tree.setPosition(400, 300);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(tree);
    window.draw(spawnA);
}

void runKing(sf::RenderWindow& window) {
    sf::Texture groundTexture, throneTexture, spawnTexture;
    groundTexture.loadFromFile("stone_ground.png");
    throneTexture.loadFromFile("throne.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(800, 600));
    ground.setTexture(&groundTexture);

    sf::RectangleShape throne(sf::Vector2f(150, 150));
    throne.setTexture(&throneTexture);
    throne.setPosition(400, 300);

    sf::CircleShape spawnA(30);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(throne);
    window.draw(spawnA);
}

void runHackneyYard(sf::RenderWindow& window) {
    sf::Texture groundTexture, warehouseTexture, containerTexture, spawnTexture;
    groundTexture.loadFromFile("yard_ground.png");
    warehouseTexture.loadFromFile("warehouse.png");
    containerTexture.loadFromFile("container.png");
    spawnTexture.loadFromFile("spawn.png");

    sf::RectangleShape ground(sf::Vector2f(1200, 800));
    ground.setTexture(&groundTexture);

    sf::RectangleShape warehouse(sf::Vector2f(300, 200));
    warehouse.setTexture(&warehouseTexture);
    warehouse.setPosition(100, 100);

    sf::RectangleShape container(sf::Vector2f(200, 100));
    container.setTexture(&containerTexture);
    container.setPosition(600, 400);

    sf::CircleShape spawnA(40);
    spawnA.setTexture(&spawnTexture);
    spawnA.setPosition(120, 120);

    window.draw(ground);
    window.draw(warehouse);
    window.draw(container);
    window.draw(spawnA);
}

// ------------------ Main Program ------------------
int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "gameXpro+");

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Example menu option
    sf::Text option1("1. Crashes Map", font, 30);
    option1.setPosition(100, 100);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) currentState = CRASHES;
                if (event.key.code == sf::Keyboard::Num2) currentState = CROSSFIRING;
                if (event.key.code == sf::Keyboard::Num3) currentState = FIRING_RANGES;
                if (event.key.code == sf::Keyboard::Num4) currentState = HIJACKED;
                if (event.key.code == sf::Keyboard::Num5) currentState = NUKETOWN;
                if (event.key.code == sf::Keyboard::Escape) currentState = MENU;
            }
        }

        window.clear();

        if (currentState == MENU) {
            window.draw(option1);
            // draw other options...
        }
        else if (currentState == CRASHES) runCrashes(window);
        else if (currentState == CROSSFIRING) runCrossfiring(window);
        else if (currentState == FIRING_RANGES) runFiringRanges(window);
        else if (currentState == HIJACKED) runHijacked(window);
        else if (currentState == NUKETOWN) runNuketown(window);

        window.display();
    }

    return 0;
}
class GameplayAnalyzer {
public:
    struct Feedback {
        std::vector<std::string> pros;
        std::vector<std::string> cons;
        std::unordered_map<std::string, std::string> improvementTips;
    };

    Feedback Analyze(const PlayerInfo& player) {
        Feedback fb;

        // Example pros
        if (player.stats.kills > player.stats.deaths) {
            fb.pros.push_back("Strong kill-to-death ratio.");
        }

        if (player.stats.assists > 5) {
            fb.pros.push_back("Good team support with assists.");
        }

        // Example cons + improvement tips
        if (player.stats.deaths > player.stats.kills) {
            fb.cons.push_back("High death count compared to kills.");
            fb.improvementTips["High death count"] = 
                "Work on positioning — avoid open areas and use cover more effectively.";
        }

        if (player.stats.kills < 3) {
            fb.cons.push_back("Low offensive impact.");
            fb.improvementTips["Low offensive impact"] = 
                "Practice aim drills and focus on engaging when you have an advantage.";
        }

        return fb;
    }
};
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

// Match lifecycle states
enum class MatchState {
    WAITING_FOR_TEAMS,
    MAP_VETO_PHASE,
    PAUSED_TECHNICAL,
    IN_PROGRESS,
    MATCH_COMPLETE
};

// Team structure
struct Team {
    uint32_t teamId;
    std::string teamName;
    std::vector<uint32_t> playerIds;
    uint32_t seriesScore = 0; // Wins in Best-of-3 or Best-of-5
};

// Map pool entry
struct MapPoolItem {
    std::string mapName;
    std::string gameMode; // e.g., "Hardpoint", "Search & Destroy", "Control"
    bool isBanned = false;
    bool isSelected = false;
};

// Tournament Match Engine
class TournamentMatchEngine {
private:
    std::string matchId_;
    Team teamA_;
    Team teamB_;
    MatchState currentState_ = MatchState::WAITING_FOR_TEAMS;
    
    uint32_t bestOfSeries_ = 3; // Best-of-3 (First to 2 wins)
    uint32_t currentMapIndex_ = 0;
    
    std::vector<MapPoolItem> mapPool_;
    std::vector<MapPoolItem> selectedMaps_;

public:
    TournamentMatchEngine(std::string matchId, Team a, Team b, uint32_t bestOf = 3)
        : matchId_(matchId), teamA_(a), teamB_(b), bestOfSeries_(bestOf) {
        
        // Load official CDL map pool
        mapPool_ = {
            {"Rio", "Hardpoint", false, false},
            {"Invasion", "Search & Destroy", false, false},
            {"Highrise", "Control", false, false},
            {"Karachi", "Hardpoint", false, false},
            {"Sub Base", "Search & Destroy", false, false}
        };

        std::cout << "[TOURNAMENT] Match Created: " << teamA_.teamName 
                  << " vs " << teamB_.teamName 
                  << " (Best of " << bestOfSeries_ << ")" << std::endl;
        
        currentState_ = MatchState::MAP_VETO_PHASE;
    }

    // --- PHASE 1: MAP VETO & PICK DRAFT ---
    void ExecuteVeto(uint32_t teamId, const std::string& mapName) {
        if (currentState_ != MatchState::MAP_VETO_PHASE) return;

        for (auto& item : mapPool_) {
            if (item.mapName == mapName && !item.isBanned && !item.isSelected) {
                item.isBanned = true;
                std::cout << "[VETO PHASE] Team " << teamId 
                          << " BANNED map: " << mapName 
                          << " (" << item.gameMode << ")" << std::endl;
                break;
            }
        }

        CheckVetoCompletion();
    }

    void CheckVetoCompletion() {
        std::vector<MapPoolItem> remaining;
        for (const auto& item : mapPool_) {
            if (!item.isBanned) remaining.push_back(item);
        }

        // Once vetoes filter down to required maps for the series
        uint32_t mapsNeeded = bestOfSeries_;
        if (remaining.size() == mapsNeeded) {
            selectedMaps_ = remaining;
            currentState_ = MatchState::IN_PROGRESS;
            
            std::cout << "\n================ MAP POOL CONFIRMED ================" << std::endl;
            for (size_t i = 0; i < selectedMaps_.size(); ++i) {
                std::cout << " Map " << (i + 1) << ": " << selectedMaps_[i].mapName 
                          << " [" << selectedMaps_[i].gameMode << "]" << std::endl;
            }
            std::cout << "====================================================\n" << std::endl;

            StartNextMap();
        }
    }

    // --- PHASE 2: MATCH PROGRESSION & WIN CONDITIONS ---
    void StartNextMap() {
        if (currentState_ != MatchState::IN_PROGRESS) return;

        uint32_t winsNeeded = (bestOfSeries_ / 2) + 1;
        if (teamA_.seriesScore == winsNeeded || teamB_.seriesScore == winsNeeded) {
            EndMatchSeries();
            return;
        }

        MapPoolItem currentMap = selectedMaps_[currentMapIndex_];
        std::cout << "[MATCH RUNNER] Starting Map " << (currentMapIndex_ + 1) << ": " 
                  << currentMap.mapName << " (" << currentMap.gameMode << ")..." << std::endl;
    }

    // Called when dedicated server reports map completion
    void RecordMapResult(uint32_t winningTeamId) {
        if (currentState_ != MatchState::IN_PROGRESS) return;

        if (winningTeamId == teamA_.teamId) {
            teamA_.seriesScore++;
            std::cout << "[MAP COMPLETED] " << teamA_.teamName 
                      << " WON Map " << (currentMapIndex_ + 1) << "!" << std::endl;
        } else if (winningTeamId == teamB_.teamId) {
            teamB_.seriesScore++;
            std::cout << "[MAP COMPLETED] " << teamB_.teamName 
                      << " WON Map " << (currentMapIndex_ + 1) << "!" << std::endl;
        }

        std::cout << "[SERIES SCORE] " << teamA_.teamName << ": " << teamA_.seriesScore 
                  << " | " << teamB_.teamName << ": " << teamB_.seriesScore << std::endl;

        currentMapIndex_++;
        StartNextMap();
    }

    // Technical Pause Engine
    void TriggerTechnicalPause(uint32_t requestingPlayerId, const std::string& reason) {
        if (currentState_ == MatchState::IN_PROGRESS) {
            currentState_ = MatchState::PAUSED_TECHNICAL;
            std::cout << "\n[TECHNICAL PAUSE] Match paused by Player " 
                      << requestingPlayerId << ". Reason: " << reason << std::endl;
        }
    }

    void ResumeMatch() {
        if (currentState_ == MatchState::PAUSED_TECHNICAL) {
            currentState_ = MatchState::IN_PROGRESS;
            std::cout << "[TECHNICAL PAUSE] Match Resumed!\n" << std::endl;
        }
    }

private:
    void EndMatchSeries() {
        currentState_ = MatchState::MATCH_COMPLETE;
        Team winner = (teamA_.seriesScore > teamB_.seriesScore) ? teamA_ : teamB_;
        
        std::cout << "\n====================================================" << std::endl;
        std::cout << "   SERIES COMPLETE! WINNER: " << winner.teamName << std::endl;
        std::cout << "   FINAL SCORE: " << teamA_.seriesScore 
                  << " - " << teamB_.seriesScore << std::endl;
        std::cout << "====================================================\n" << std::endl;
    }
};

// Example usage
int main() {
    Team teamA{1, "Team Alpha", {101, 102, 103}};
    Team teamB{2, "Team Bravo", {201, 202, 203}};

    TournamentMatchEngine engine("Match_001", teamA, teamB, 3);

    // Simulate veto phase
    engine.ExecuteVeto(teamA.teamId, "Rio");
    engine.ExecuteVeto(teamB.teamId, "Invasion");

    // Simulate match results
    engine.RecordMapResult(teamA.teamId); // Alpha wins Map 1
    engine.RecordMapResult(teamB.teamId); // Bravo wins Map 2
    engine.RecordMapResult(teamA.teamId); // Alpha wins Map 3 -> Series Win

    return 0;
}
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

struct PlayerStats {
    uint32_t id;
    uint32_t kills = 0;
    uint32_t deaths = 0;
    float accuracy = 0.0f;       // shots hit / shots fired
    float avgTimeAlive = 0.0f;   // seconds
    float objectiveCaptures = 0.0f;
    float damageTaken = 0.0f;
};

class AICoach {
private:
    std::unordered_map<uint32_t, std::vector<std::string>> feedbackHistory_;

public:
    void Analyze(const PlayerStats& stats) {
        std::vector<std::string> feedback;

        // Accuracy analysis
        if (stats.accuracy < 0.25f) {
            feedback.push_back("Work on your aim — try burst firing or aim training drills.");
        } else if (stats.accuracy > 0.5f) {
            feedback.push_back("Great accuracy! Keep maintaining steady aim under pressure.");
        }

        // Deaths vs kills
        if (stats.deaths > stats.kills) {
            feedback.push_back("Focus on positioning — use cover and avoid open areas.");
        }

        // Survival analysis
        if (stats.avgTimeAlive < 30.0f) {
            feedback.push_back("You’re dying too quickly. Play more defensively and wait for teammates.");
        }

        // Objective play
        if (stats.objectiveCaptures == 0 && stats.kills > 5) {
            feedback.push_back("Strong slaying, but try contributing to objectives for team success.");
        }

        // Damage taken
        if (stats.damageTaken > 1000.0f) {
            feedback.push_back("You’re absorbing too much damage — improve awareness and avoid risky fights.");
        }

        // Adaptive feedback (avoid repeating old tips)
        for (const auto& tip : feedback) {
            if (std::find(feedbackHistory_[stats.id].begin(), feedbackHistory_[stats.id].end(), tip) == feedbackHistory_[stats.id].end()) {
                std::cout << "[AI COACH] Player " << stats.id << ": " << tip << std::endl;
                feedbackHistory_[stats.id].push_back(tip);
            }
        }
    }
};
int main() {
    AICoach coach;
    PlayerStats stats{101, 8, 12, 0.22f, 25.0f, 0, 1200.0f};

    coach.Analyze(stats);

    return 0;
}
struct Team {
    uint32_t teamId;
    std::vector<std::shared_ptr<Player>> members;
};

struct MatchRoom {
    std::string roomId;
    std::unordered_map<uint32_t, std::shared_ptr<Player>> players;
    std::vector<Team> teams;
};

class GameServer {
private:
    std::queue<uint32_t> matchmaking_queue_;
    uint32_t next_team_id_ = 1;

    // ...

    void process_join_queue() {
        uint32_t new_id = next_player_id_++;
        auto new_player = std::make_shared<Player>();
        new_player->id = new_id;
        new_player->endpoint = remote_endpoint_;

        connected_players_[new_id] = new_player;
        matchmaking_queue_.push(new_id);

        std::cout << "[MATCHMAKER] Player " << new_id << " joined queue. Queue size: " 
                  << matchmaking_queue_.size() << std::endl;

        // Example: 5v5 team-based matchmaking
        if (matchmaking_queue_.size() >= 10) {
            std::vector<std::shared_ptr<Player>> teamA;
            std::vector<std::shared_ptr<Player>> teamB;

            for (int i = 0; i < 5; ++i) {
                uint32_t pid = matchmaking_queue_.front(); matchmaking_queue_.pop();
                teamA.push_back(connected_players_[pid]);
            }
            for (int i = 0; i < 5; ++i) {
                uint32_t pid = matchmaking_queue_.front(); matchmaking_queue_.pop();
                teamB.push_back(connected_players_[pid]);
            }

            Team tA{next_team_id_++, teamA};
            Team tB{next_team_id_++, teamB};

            std::string room_id = "Match_" + std::to_string(tA.teamId) + "_" + std::to_string(tB.teamId);
            auto room = std::make_shared<MatchRoom>();
            room->roomId = room_id;
            room->teams = {tA, tB};

            for (auto& p : teamA) {
                room->players[p->id] = p;
                player_to_room_map_[p->id] = room_id;
            }
            for (auto& p : teamB) {
                room->players[p->id] = p;
                player_to_room_map_[p->id] = room_id;
            }

            active_rooms_[room_id] = room;

            std::cout << "[MATCHMAKER] Created team-based match room: " << room_id << std::endl;
        }
    }
};
struct Team {
    uint32_t teamId;
    std::vector<std::shared_ptr<Player>> members;
};

struct MatchRoom {
    std::string roomId;
    std::unordered_map<uint32_t, std::shared_ptr<Player>> players;
    std::vector<Team> teams;
};

void process_team_queue(size_t teamSize = 5) {
    if (matchmaking_queue_.size() >= teamSize * 2) {
        std::vector<std::shared_ptr<Player>> teamA;
        std::vector<std::shared_ptr<Player>> teamB;

        for (size_t i = 0; i < teamSize; ++i) {
            uint32_t pid = matchmaking_queue_.front(); matchmaking_queue_.pop();
            teamA.push_back(connected_players_[pid]);
        }
        for (size_t i = 0; i < teamSize; ++i) {
            uint32_t pid = matchmaking_queue_.front(); matchmaking_queue_.pop();
            teamB.push_back(connected_players_[pid]);
        }

        Team tA{1, teamA};
        Team tB{2, teamB};

        std::string room_id = "Match_" + std::to_string(tA.teamId) + "_" + std::to_string(tB.teamId);
        auto room = std::make_shared<MatchRoom>();
        room->roomId = room_id;
        room->teams = {tA, tB};

        for (auto& p : teamA) { room->players[p->id] = p; player_to_room_map_[p->id] = room_id; }
        for (auto& p : teamB) { room->players[p->id] = p; player_to_room_map_[p->id] = room_id; }

        active_rooms_[room_id] = room;
        std::cout << "[MATCHMAKER] Created 5v5 match room: " << room_id << std::endl;
    }
}
#include <sqlite3.h>

class Database {
private:
    sqlite3* db;
public:
    Database(const std::string& filename) {
        if (sqlite3_open(filename.c_str(), &db)) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << "[DB] Connected to " << filename << std::endl;
        }
        initTables();
    }

    ~Database() { sqlite3_close(db); }

    void initTables() {
        const char* playerTable =
            "CREATE TABLE IF NOT EXISTS players ("
            "id INTEGER PRIMARY KEY, "
            "username TEXT, "
            "kills INTEGER, "
            "deaths INTEGER, "
            "matches_played INTEGER);";

        const char* matchTable =
            "CREATE TABLE IF NOT EXISTS matches ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "roomId TEXT, "
            "winnerTeam INTEGER, "
            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

        sqlite3_exec(db, playerTable, nullptr, nullptr, nullptr);
        sqlite3_exec(db, matchTable, nullptr, nullptr, nullptr);
    }

    void recordPlayerStats(uint32_t id, const std::string& username, int kills, int deaths) {
        std::string sql = "INSERT OR REPLACE INTO players (id, username, kills, deaths, matches_played) "
                          "VALUES (" + std::to_string(id) + ", '" + username + "', " +
                          std::to_string(kills) + ", " + std::to_string(deaths) + ", 1);";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }

    void recordMatchResult(const std::string& roomId, int winnerTeam) {
        std::string sql = "INSERT INTO matches (roomId, winnerTeam) VALUES ('" + roomId + "', " + std::to_string(winnerTeam) + ");";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
};
Database db("game_stats.db");

// Example usage after match ends
db.recordPlayerStats(player->id, player->username, player->kills, player->deaths);
db.recordMatchResult(room->roomId, winningTeamId);

#include <iostream>
#include <unordered_map>
#include <string>

class CurrencySystem {
private:
    std::unordered_map<std::string, long long> playerBalances;

public:
    // Add normal Xbucks
    void addXbucks(const std::string& playerId, long long amount) {
        playerBalances[playerId] += amount;
        std::cout << "[ECONOMY] Added " << amount << " Xbucks to " << playerId << ". Total: " 
                  << playerBalances[playerId] << std::endl;
    }

    // Add Xbuckpro+ (worth 2,000,000 Xbucks)
    void addXbuckPro(const std::string& playerId, int count) {
        long long conversionRate = 2000000; // 1 Xbuckpro+ = 2,000,000 Xbucks
        long long totalValue = count * conversionRate;
        playerBalances[playerId] += totalValue;
        std::cout << "[ECONOMY] Added " << count << " Xbuckpro+ (" << totalValue 
                  << " Xbucks equivalent) to " << playerId << ". Total: " 
                  << playerBalances[playerId] << std::endl;
    }

    // Check balance
    long long getBalance(const std::string& playerId) const {
        auto it = playerBalances.find(playerId);
        return (it != playerBalances.end()) ? it->second : 0;
    }
};

int main() {
    CurrencySystem economy;

    economy.addXbucks("Player101", 5000);
    economy.addXbuckPro("Player101", 1); // Adds 2,000,000 Xbucks equivalent

    std::cout << "\n[WALLET] Player101 total balance: " 
              << economy.getBalance("Player101") << " Xbucks" << std::endl;

    return 0;
}
#include <iostream>
#include <unordered_map>
#include <string>

struct Wallet {
    long long xbucks = 0;
    long long xbuckPro = 0; // Premium currency
};

class CurrencySystem {
private:
    std::unordered_map<std::string, Wallet> playerWallets;
    const long long conversionRate = 2000000; // 1 Xbuckpro+ = 2,000,000 Xbucks

public:
    // Add normal Xbucks
    void addXbucks(const std::string& playerId, long long amount) {
        playerWallets[playerId].xbucks += amount;
        std::cout << "[ECONOMY] Added " << amount << " Xbucks to " << playerId
                  << ". Total Xbucks: " << playerWallets[playerId].xbucks << std::endl;
    }

    // Add Xbuckpro+
    void addXbuckPro(const std::string& playerId, long long count) {
        playerWallets[playerId].xbuckPro += count;
        std::cout << "[ECONOMY] Added " << count << " Xbuckpro+ to " << playerId
                  << ". Total Xbuckpro+: " << playerWallets[playerId].xbuckPro << std::endl;
    }

    // Convert Xbuckpro+ to Xbucks
    void convertProToXbucks(const std::string& playerId, long long count) {
        if (playerWallets[playerId].xbuckPro < count) {
            std::cout << "[ERROR] Not enough Xbuckpro+ to convert!" << std::endl;
            return;
        }
        playerWallets[playerId].xbuckPro -= count;
        long long convertedValue = count * conversionRate;
        playerWallets[playerId].xbucks += convertedValue;

        std::cout << "[CONVERSION] Converted " << count << " Xbuckpro+ → "
                  << convertedValue << " Xbucks for " << playerId << std::endl;
    }

    // Display wallet summary
    void showWallet(const std::string& playerId) {
        Wallet w = playerWallets[playerId];
        std::cout << "\n[WALLET SUMMARY] " << playerId << std::endl;
        std::cout << "  Xbucks: " << w.xbucks << std::endl;
        std::cout << "  Xbuckpro+: " << w.xbuckPro << std::endl;
        std::cout << "  Total Value (in Xbucks): " << (w.xbucks + w.xbuckPro * conversionRate) << std::endl;
    }
};

int main() {
    CurrencySystem economy;

    economy.addXbucks("Player101", 5000);
    economy.addXbuckPro("Player101", 2); // Adds 2 Xbuckpro+ (worth 4,000,000 Xbucks)
    economy.convertProToXbucks("Player101", 1); // Convert 1 Xbuckpro+ to 2,000,000 Xbucks
    economy.showWallet("Player101");

    return 0;
}
struct CurrencyWallet {
    long long xbucks = 0;
    long long xbuckPro = 0; // Premium currency
};
struct Player {
    uint32_t id;
    std::string username;
    udp::endpoint endpoint;
    Vector3 position{0.0f, 0.0f, 0.0f};
    float rotationY = 0.0f;
    float health = 100.0f;

    CurrencyWallet wallet; // NEW
};
class CurrencySystem {
private:
    const long long conversionRate = 2000000; // 1 Xbuckpro+ = 2,000,000 Xbucks

public:
    void addXbucks(Player& player, long long amount) {
        player.wallet.xbucks += amount;
        std::cout << "[ECONOMY] Player " << player.id 
                  << " gained " << amount << " Xbucks. Total: " 
                  << player.wallet.xbucks << std::endl;
    }

    void addXbuckPro(Player& player, long long count) {
        player.wallet.xbuckPro += count;
        std::cout << "[ECONOMY] Player " << player.id 
                  << " gained " << count << " Xbuckpro+. Total: " 
                  << player.wallet.xbuckPro << std::endl;
    }

    void convertProToXbucks(Player& player, long long count) {
        if (player.wallet.xbuckPro < count) {
            std::cout << "[ERROR] Not enough Xbuckpro+!" << std::endl;
            return;
        }
        player.wallet.xbuckPro -= count;
        long long converted = count * conversionRate;
        player.wallet.xbucks += converted;

        std::cout << "[CONVERSION] Player " << player.id 
                  << " converted " << count << " Xbuckpro+ → " 
                  << converted << " Xbucks." << std::endl;
    }

    void showWallet(const Player& player) {
        std::cout << "[WALLET] Player " << player.id 
                  << " | Xbucks: " << player.wallet.xbucks 
                  << " | Xbuckpro+: " << player.wallet.xbuckPro 
                  << " | Total Value (in Xbucks): " 
                  << (player.wallet.xbucks + player.wallet.xbuckPro * conversionRate) 
                  << std::endl;
    }
};
int main() {
    Player p1{101, "Tanmay"};
    CurrencySystem economy;

    economy.addXbucks(p1, 5000);
    economy.addXbuckPro(p1, 2); // Adds 2 Xbuckpro+ (worth 4,000,000 Xbucks)
    economy.convertProToXbucks(p1, 1); // Convert 1 → 2,000,000 Xbucks
    economy.showWallet(p1);

    return 0;
}
enum class PacketType : uint8_t {
    JOIN_QUEUE = 1,
    MATCH_FOUND = 2,
    PLAYER_MOVEMENT = 3,
    PLAYER_SHOOT = 4,
    DAMAGE_EVENT = 5,
    CURRENCY_UPDATE = 6   // NEW
};
#pragma pack(push, 1)
struct CurrencyPacket {
    Header header;
    long long xbucks;
    long long xbuckPro;
};
#pragma pack(pop)
#include <iostream>
#include <unordered_map>
#include <string>

// Define all possible player actions
enum class Action {
    FIRE,
    RELOAD,
    ZOOM,
    WEAPON_CHANGE,
    JUMP,
    CROUCH
};

// Control button properties
struct ControlButton {
    Action action;
    float posX;     // Screen X position
    float posY;     // Screen Y position
    float size;     // Scale factor
    float opacity;  // Transparency (0.0 - 1.0)
};

// Layout containing all buttons
struct ControlLayout {
    std::unordered_map<Action, ControlButton> buttons;

    // Initialize with default COD-style positions
    void LoadDefaultLayout() {
        buttons[Action::FIRE]         = {Action::FIRE, 0.85f, 0.75f, 1.0f, 1.0f};
        buttons[Action::RELOAD]       = {Action::RELOAD, 0.75f, 0.85f, 0.8f, 1.0f};
        buttons[Action::ZOOM]         = {Action::ZOOM, 0.15f, 0.75f, 1.0f, 1.0f};
        buttons[Action::WEAPON_CHANGE]= {Action::WEAPON_CHANGE, 0.25f, 0.85f, 0.8f, 1.0f};
        buttons[Action::JUMP]         = {Action::JUMP, 0.50f, 0.90f, 1.0f, 1.0f};
        buttons[Action::CROUCH]       = {Action::CROUCH, 0.60f, 0.90f, 1.0f, 1.0f};
    }

    // Update button position (dragging)
    void UpdateButtonPosition(Action action, float newX, float newY) {
        buttons[action].posX = newX;
        buttons[action].posY = newY;
    }

    // Update button size (resizing)
    void UpdateButtonSize(Action action, float newSize) {
        buttons[action].size = newSize;
    }

    // Update button opacity
    void UpdateButtonOpacity(Action action, float newOpacity) {
        buttons[action].opacity = newOpacity;
    }

    // Print layout (for debugging)
    void PrintLayout() {
        for (auto& [action, btn] : buttons) {
            std::cout << "Action " << static_cast<int>(action)
                      << " -> Pos(" << btn.posX << "," << btn.posY << ")"
                      << " Size=" << btn.size
                      << " Opacity=" << btn.opacity << std::endl;
        }
    }
};

// Example Settings Menu Simulation
int main() {
    ControlLayout layout;
    layout.LoadDefaultLayout();

    std::cout << "=== Default COD-style Layout ===" << std::endl;
    layout.PrintLayout();

    // Simulate player customizing layout
    std::cout << "\n=== Player Customization ===" << std::endl;
    layout.UpdateButtonPosition(Action::FIRE, 0.90f, 0.70f);
    layout.UpdateButtonSize(Action::ZOOM, 1.2f);
    layout.UpdateButtonOpacity(Action::RELOAD, 0.5f);

    layout.PrintLayout();

    // In production: Save layout to file or database here
    return 0;
}
