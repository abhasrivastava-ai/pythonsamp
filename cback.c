struct Vector3D {
    float x, y, z;
};

struct Target {
    Vector3D pos;
    float radius;
    float hp;
    bool active;
};

struct GameMods {
    float fov;
    float sensitivity;
    float fireRateRPM;
    bool infiniteAmmo;
    bool lowGravity;
    bool speedHack;
    bool godMode;
    bool oneHitKill;
};

class GameEngineBackend {
public:
    Vector3D playerPos = {0.0f, 2.0f, 10.0f};
    Vector3D playerVel = {0.0f, 0.0f, 0.0f};
    bool isGrounded = true;
    
    float health = 100.0f;
    int score = 0;
    int ammo = 30;
    int maxAmmo = 120;
    int currentWeapon = 0; // 0: AR, 1: Shotgun, 2: Sniper
    
    GameMods mods = {75.0f, 1.0f, 600.0f, false, false, false, false, false};
    std::vector<Target> targets;

    GameEngineBackend() {
        for (int i = 0; i < 10; ++i) {
            Target t;
            t.pos = {(float)(rand() % 80 - 40), 1.5f, (float)(-10 - rand() % 50)};
            t.radius = 1.2f;
            t.hp = 100.0f;
            t.active = true;
            targets.push_back(t);
        }
    }

    void updatePhysics(float dt, bool moveFwd, bool moveBwd, bool moveLeft, bool moveRight, bool jump, float yaw) {
        float speed = mods.speedHack ? 30.0f : 12.0f;
        float gravity = mods.lowGravity ? 9.8f : 32.0f;

        float radYaw = yaw * (3.14159f / 180.0f);
        float moveX = 0.0f;
        float moveZ = 0.0f;

        if (moveFwd)   { moveX += sinf(radYaw); moveZ -= cosf(radYaw); }
        if (moveBwd)   { moveX -= sinf(radYaw); moveZ += cosf(radYaw); }
        if (moveLeft)  { moveX -= cosf(radYaw); moveZ -= sinf(radYaw); }
        if (moveRight) { moveX += cosf(radYaw); moveZ += sinf(radYaw); }

        playerPos.x += moveX * speed * dt;
        playerPos.z += moveZ * speed * dt;

        if (!isGrounded) {
            playerVel.y -= gravity * dt;
        }

        if (jump && isGrounded) {
            playerVel.y = mods.lowGravity ? 16.0f : 10.0f;
            isGrounded = false;
        }

        playerPos.y += playerVel.y * dt;

        if (playerPos.y <= 2.0f) {
            playerPos.y = 2.0f;
            playerVel.y = 0.0f;
            isGrounded = true;
        }
    }

    bool checkRaySphereHit(Vector3D rayOrigin, Vector3D rayDir, Vector3D sphereCenter, float sphereRadius, float& hitDist) {
        Vector3D oc = {rayOrigin.x - sphereCenter.x, rayOrigin.y - sphereCenter.y, rayOrigin.z - sphereCenter.z};
        float a = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z;
        float b = 2.0f * (oc.x * rayDir.x + oc.y * rayDir.y + oc.z * rayDir.z);
        float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - (sphereRadius * sphereRadius);
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f) return false;
        float t1 = (-b - sqrtf(discriminant)) / (2.0f * a);
        if (t1 > 0.0f) {
            hitDist = t1;
            return true;
        }
        return false;
    }

    bool shoot(Vector3D rayDir, Vector3D& outHitPos) {
        if (!mods.infiniteAmmo) {
            if (ammo <= 0) return false;
            ammo--;
        }

        float minHitDist = 1e9f;
        int hitIndex = -1;

        for (size_t i = 0; i < targets.size(); ++i) {
            if (targets[i].active) {
                float dist = 0.0f;
                if (checkRaySphereHit(playerPos, rayDir, targets[i].pos, targets[i].radius, dist)) {
                    if (dist < minHitDist) {
                        minHitDist = dist;
                        hitIndex = (int)i;
                    }
                }
            }
        }

        if (hitIndex != -1) {
            auto& target = targets[hitIndex];
            outHitPos = {playerPos.x + rayDir.x * minHitDist, playerPos.y + rayDir.y * minHitDist, playerPos.z + rayDir.z * minHitDist};
            
            float damage = mods.oneHitKill ? 100.0f : (currentWeapon == 2 ? 100.0f : (currentWeapon == 1 ? 50.0f : 25.0f));
            target.hp -= damage;

            if (target.hp <= 0.0f) {
                target.active = false;
                score += 100;
                target.pos = {(float)(rand() % 80 - 40), 1.5f, (float)(-10 - rand() % 50)};
                target.hp = 100.0f;
                target.active = true;
            }
            return true;
        }
        return false;
    }
};

extern "C" {
    GameEngineBackend* Engine_CreateAdvanced() { return new GameEngineBackend(); }
    void Engine_DestroyAdvanced(GameEngineBackend* ptr) { delete ptr; }

    void Engine_UpdateAdvanced(GameEngineBackend* ptr, float dt, bool fwd, bool bwd, bool left, bool right, bool jump, float yaw) {
        if (ptr) ptr->updatePhysics(dt, fwd, bwd, left, right, jump, yaw);
    }

    bool Engine_ShootAdvanced(GameEngineBackend* ptr, float dirX, float dirY, float dirZ, float* hitX, float* hitY, float* hitZ) {
        if (!ptr) return false;
        Vector3D hitPos = {0.0f, 0.0f, 0.0f};
        bool hit = ptr->shoot({dirX, dirY, dirZ}, hitPos);
        if (hit && hitX && hitY && hitZ) {
            *hitX = hitPos.x; *hitY = hitPos.y; *hitZ = hitPos.z;
        }
        return hit;
    }

    void Engine_SwitchWeapon(GameEngineBackend* ptr, int weaponId) {
        if (ptr && weaponId >= 0 && weaponId <= 2) ptr->currentWeapon = weaponId;
    }

    void Engine_GetPlayerStats(GameEngineBackend* ptr, float* hp, int* ammo, int* score, int* weapon) {
        if (!ptr) return;
        if (hp) *hp = ptr->health;
        if (ammo) *ammo = ptr->ammo;
        if (score) *score = ptr->score;
        if (weapon) *weapon = ptr->currentWeapon;
    }

    void Engine_SetAdvancedMod(GameEngineBackend* ptr, int modId, bool state) {
        if (!ptr) return;
        if (modId == 1) ptr->mods.infiniteAmmo = state;
        if (modId == 2) ptr->mods.lowGravity = state;
        if (modId == 3) ptr->mods.speedHack = state;
        if (modId == 4) ptr->mods.godMode = state;
        if (modId == 5) ptr->mods.oneHitKill = state;
    }

    int Engine_GetEnemyCount(GameEngineBackend* ptr) {
        return ptr ? (int)ptr->targets.size() : 0;
    }

    void Engine_GetEnemyData(GameEngineBackend* ptr, int index, float* x, float* y, float* z, float* hp) {
        if (!ptr || index < 0 || index >= (int)ptr->targets.size()) return;
        if (x) *x = ptr->targets[index].pos.x;
        if (y) *y = ptr->targets[index].pos.y;
        if (z) *z = ptr->targets[index].pos.z;
        if (hp) *hp = ptr->targets[index].hp;
    }
}