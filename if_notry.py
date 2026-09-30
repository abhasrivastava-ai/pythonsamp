struct Vector3D {
    float x, y, z;
};

struct Target {
    Vector3D pos;
    float radius;
    bool active;
};

struct GameMods {
    float fov;
    float sensitivity;
    float fireRateRPM;
    bool infiniteAmmo;
    bool lowGravity;
    bool speedHack;
};

class GameEngineBackend {
public:
    Vector3D playerPos = {0.0f, 2.0f, 10.0f};
    Vector3D playerVel = {0.0f, 0.0f, 0.0f};
    bool isGrounded = true;
    
    int score = 0;
    int ammo = 30;
    int maxAmmo = 120;
    
    GameMods mods = {75.0f, 1.0f, 600.0f, false, false, false};
    std::vector<Target> targets;

    GameEngineBackend() {
        // Spawn 10 target dummies
        for (int i = 0; i < 10; ++i) {
            Target t;
            t.pos = {(float)(rand() % 80 - 40), 1.5f, (float)(-10 - rand() % 50)};
            t.radius = 1.2f;
            t.active = true;
            targets.push_back(t);
        }
    }

    void updatePhysics(float dt, bool moveFwd, bool moveBwd, bool moveLeft, bool moveRight, bool jump, float yaw) {
        float speed = mods.speedHack ? 30.0f : 12.0f;
        float gravity = mods.lowGravity ? 9.8f : 32.0f;

        // Apply movement vector relative to player yaw rotation
        float radYaw = yaw * (3.14159f / 180.0f);
        float moveX = 0.0f;
        float moveZ = 0.0f;

        if (moveFwd)   { moveX += sinf(radYaw); moveZ -= cosf(radYaw); }
        if (moveBwd)   { moveX -= sinf(radYaw); moveZ += cosf(radYaw); }
        if (moveLeft)  { moveX -= cosf(radYaw); moveZ -= sinf(radYaw); }
        if (moveRight) { moveX += cosf(radYaw); moveZ += sinf(radYaw); }

        playerPos.x += moveX * speed * dt;
        playerPos.z += moveZ * speed * dt;

        // Gravity physics
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

    bool checkRaySphereHit(Vector3D rayOrigin, Vector3D rayDir, Vector3D sphereCenter, float sphereRadius) {
        Vector3D oc = {rayOrigin.x - sphereCenter.x, rayOrigin.y - sphereCenter.y, rayOrigin.z - sphereCenter.z};
        float b = 2.0f * (oc.x * rayDir.x + oc.y * rayDir.y + oc.z * rayDir.z);
        float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - (sphereRadius * sphereRadius);
        float discriminant = b * b - 4.0f * c;
        return (discriminant > 0.0f);
    }

    bool shoot(Vector3D rayDir) {
        if (!mods.infiniteAmmo) {
            if (ammo <= 0) return false;
            ammo--;
        }

        // Fast C++ Raycast hit test against targets
        for (auto& target : targets) {
            if (target.active) {
                if (checkRaySphereHit(playerPos, rayDir, target.pos, target.radius)) {
                    target.active = false;
                    score += 100;

                    // Respawn target position
                    target.pos = {(float)(rand() % 80 - 40), 1.5f, (float)(-10 - rand() % 50)};
                    target.active = true;
                    return true;
                }
            }
        }
        return false;
    }
};

// Expose C ABI methods for Python ctypes interface
extern "C" {
    GameEngineBackend* Engine_Create() { return new GameEngineBackend(); }
    void Engine_Destroy(GameEngineBackend* ptr) { delete ptr; }

    void Engine_Update(GameEngineBackend* ptr, float dt, bool fwd, bool bwd, bool left, bool right, bool jump, float yaw) {
        ptr->updatePhysics(dt, fwd, bwd, left, right, jump, yaw);
    }

    bool Engine_Shoot(GameEngineBackend* ptr, float dirX, float dirY, float dirZ) {
        return ptr->shoot({dirX, dirY, dirZ});
    }

    void Engine_GetPlayerPos(GameEngineBackend* ptr, float* x, float* y, float* z) {
        *x = ptr->playerPos.x; *y = ptr->playerPos.y; *z = ptr->playerPos.z;
    }

    int Engine_GetScore(GameEngineBackend* ptr) { return ptr->score; }
    int Engine_GetAmmo(GameEngineBackend* ptr) { return ptr->ammo; }
    
    void Engine_SetMod(GameEngineBackend* ptr, int modId, bool state) {
        if (modId == 1) ptr->mods.infiniteAmmo = state;
        if (modId == 2) ptr->mods.lowGravity = state;
        if (modId == 3) ptr->mods.speedHack = state;
    }

    int Engine_GetTargetCount(GameEngineBackend* ptr) { return (int)ptr->targets.size(); }
    void Engine_GetTargetPos(GameEngineBackend* ptr, int index, float* x, float* y, float* z) {
        *x = ptr->targets[index].pos.x;
        *y = ptr->targets[index].pos.y;
        *z = ptr->targets[index].pos.z;
    }
}
2. Compile the C++ shared library:
Windows (PowerShell / Command Prompt):

Bash
g++ -shared -o engine.dll engine.cpp -O3
Linux:

Bash
g++ -shared -fPIC -o engine.so engine.cpp -O3
macOS:

Bash
g++ -shared -fPIC -o engine.dylib engine.cpp -O3
Step 2: Python Frontend & GUI Client (main.py)
This Python application uses Panda3D (a high-performance 3D engine) to manage rendering, camera, input, and UI overlay, while binding directly to engine.dll using Python's ctypes library.

Install requirement:
Bash
pip install panda3d
Save as main.py:
Python
import os
import sys
import ctypes
import math
from direct.showbase.ShowBase import ShowBase
from panda3d.core import WindowProperties, Vec3, CardMaker, NodePath
from direct.gui.OnscreenText import OnscreenText
from direct.gui.DirectGui import DirectFrame, DirectButton, DirectCheckButton

# Load Compiled C++ Library
lib_path = "./engine.dll" if os.name == 'nt' else "./engine.so"
cpp_engine = ctypes.CDLL(os.path.abspath(lib_path))

# Bind C++ Signatures
cpp_engine.Engine_Create.restype = ctypes.c_void_p
cpp_engine.Engine_Update.argtypes = [ctypes.c_void_p, ctypes.c_float, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.c_float]
cpp_engine.Engine_Shoot.argtypes = [ctypes.c_void_p, ctypes.c_float, ctypes.c_float, ctypes.c_float]
cpp_engine.Engine_Shoot.restype = ctypes.c_bool
cpp_engine.Engine_GetPlayerPos.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]
cpp_engine.Engine_GetScore.argtypes = [ctypes.c_void_p]
cpp_engine.Engine_GetScore.restype = ctypes.c_int
cpp_engine.Engine_GetAmmo.argtypes = [ctypes.c_void_p]
cpp_engine.Engine_GetAmmo.restype = ctypes.c_int
cpp_engine.Engine_SetMod.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_bool]
cpp_engine.Engine_GetTargetCount.argtypes = [ctypes.c_void_p]
cpp_engine.Engine_GetTargetCount.restype = ctypes.c_int
cpp_engine.Engine_GetTargetPos.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]


class TheCavalaryShootingMinders(ShowBase):
    def __init__(self):
        super().__init__()
        
        # Instantiate C++ Engine Backend
        self.cpp_ptr = cpp_engine.Engine_Create()

        # Window Config
        props = WindowProperties()
        props.setTitle("THE CAVALARY SHOOTING MINDERS (Python Frontend + C++ Backend)")
        props.setCursorHidden(True)
        props.setMouseMode(WindowProperties.M_relative)
        self.win.requestProperties(props)

        # Input keys state
        self.key_state = {"w": False, "s": False, "a": False, "d": False, "space": False}
        self.accept("w", self.set_key, ["w", True])
        self.accept("w-up", self.set_key, ["w", False])
        self.accept("s", self.set_key, ["s", True])
        self.accept("s-up", self.set_key, ["s", False])
        self.accept("a", self.set_key, ["a", True])
        self.accept("a-up", self.set_key, ["a", False])
        self.accept("d", self.set_key, ["d", True])
        self.accept("d-up", self.set_key, ["d", False])
        self.accept("space", self.set_key, ["space", True])
        self.accept("space-up", self.set_key, ["space", False])
        self.accept("mouse1", self.on_shoot)
        self.accept("m", self.toggle_mods_menu)

        # Setup 3D Scene Target Models
        self.target_nodes = []
        self.setup_environment()

        # HUD Text
        self.hud_title = OnscreenText(text="THE CAVALARY SHOOTING MINDERS", pos=(0, 0.9), scale=0.07, fg=(1, 0.2, 0.2, 1))
        self.hud_stats = OnscreenText(text="AMMO: 30 | SCORE: 0", pos=(-0.8, -0.9), scale=0.06, fg=(0, 1, 0.8, 1))
        self.hud_help = OnscreenText(text="[WASD] Move | [MOUSE] Aim | [M] Advanced Mods Menu", pos=(0, -0.95), scale=0.04, fg=(1, 1, 1, 0.8))

        # Crosshair
        self.crosshair = OnscreenText(text="+", pos=(0, 0), scale=0.08, fg=(0, 1, 0, 1))

        # Settings & Mods Menu UI (Python DirectGUI)
        self.menu_open = False
        self.menu_frame = DirectFrame(frameColor=(0.1, 0.1, 0.15, 0.9), frameSize=(-0.6, 0.6, -0.6, 0.6), pos=(0, 0, 0))
        self.menu_frame.hide()

        OnscreenText(text="ADVANCED MODS MENU", parent=self.menu_frame, pos=(0, 0.45), scale=0.06, fg=(0, 1, 0.8, 1))
        
        self.chk_ammo = DirectCheckButton(text="Infinite Ammo", parent=self.menu_frame, scale=0.05, pos=(-0.3, 0, 0.2), command=lambda s: cpp_engine.Engine_SetMod(self.cpp_ptr, 1, s))
        self.chk_grav = DirectCheckButton(text="Low Gravity", parent=self.menu_frame, scale=0.05, pos=(-0.3, 0, 0.05), command=lambda s: cpp_engine.Engine_SetMod(self.cpp_ptr, 2, s))
        self.chk_speed = DirectCheckButton(text="Speed Hack", parent=self.menu_frame, scale=0.05, pos=(-0.3, 0, -0.1), command=lambda s: cpp_engine.Engine_SetMod(self.cpp_ptr, 3, s))
        
        DirectButton(text="CLOSE [M]", parent=self.menu_frame, scale=0.05, pos=(0, 0, -0.4), command=self.toggle_mods_menu)

        # Game Frame Loop Update
        self.taskMgr.add(self.update, "UpdateTask")

    def set_key(self, key, value):
        self.key_state[key] = value

    def setup_environment(self):
        # Create Target Objects in Python based on C++ initial positions
        count = cpp_engine.Engine_GetTargetCount(self.cpp_ptr)
        for i in range(count):
            x, y, z = ctypes.c_float(), ctypes.c_float(), ctypes.c_float()
            cpp_engine.Engine_GetTargetPos(self.cpp_ptr, i, ctypes.byref(x), ctypes.byref(y), ctypes.byref(z))
            
            # Load basic target mesh box
            node = self.loader.loadModel("models/box")
            node.reparentTo(self.render)
            node.setPos(x.value, z.value, y.value) # Note Panda3D Z-up coordinates
            node.setScale(1.2)
            node.setColor(1, 0.2, 0.4, 1)
            self.target_nodes.append(node)

    def toggle_mods_menu(self):
        self.menu_open = not self.menu_open
        props = WindowProperties()
        if self.menu_open:
            self.menu_frame.show()
            props.setCursorHidden(False)
            props.setMouseMode(WindowProperties.M_absolute)
        else:
            self.menu_frame.hide()
            props.setCursorHidden(True)
            props.setMouseMode(WindowProperties.M_relative)
        self.win.requestProperties(props)

    def on_shoot(self):
        if self.menu_open:
            return
        
        # Calculate viewing direction ray
        cam_mat = self.camera.getMat()
        forward = cam_mat.getRow3(2)

        # Send bullet vector directly to C++ Backend
        hit = cpp_engine.Engine_Shoot(self.cpp_ptr, forward.x, forward.y, forward.z)
        if hit:
            print("[C++ Backend] Hit Target Detected!")

    def update(self, task):
        dt = globalClock.getDt()

        if not self.menu_open and self.mouseWatcherNode.hasMouse():
            # Update Mouse Look
            md = self.win.getPointer(0)
            delta_x = md.getX()
            delta_y = md.getY()

            self.camera.setH(self.camera.getH() - delta_x * 0.1)
            self.camera.setP(self.camera.getP() - delta_y * 0.1)

        # Call Native C++ Physics Engine Update
        cpp_engine.Engine_Update(
            self.cpp_ptr, dt,
            self.key_state["w"], self.key_state["s"],
            self.key_state["a"], self.key_state["d"],
            self.key_state["space"],
            self.camera.getH()
        )

        # Get Updated Player Position from C++ Engine
        px, py, pz = ctypes.c_float(), ctypes.c_float(), ctypes.c_float()
        cpp_engine.Engine_GetPlayerPos(self.cpp_ptr, ctypes.byref(px), ctypes.byref(py), ctypes.byref(pz))
        self.camera.setPos(px.value, pz.value, py.value)

        # Sync Target positions modified by C++ logic
        count = cpp_engine.Engine_GetTargetCount(self.cpp_ptr)
        for i in range(count):
            tx, ty, tz = ctypes.c_float(), ctypes.c_float(), ctypes.c_float()
            cpp_engine.Engine_GetTargetPos(self.cpp_ptr, i, ctypes.byref(tx), ctypes.byref(ty), ctypes.byref(tz))
            self.target_nodes[i].setPos(tx.value, tz.value, ty.value)

        # Update HUD Info
        ammo = cpp_engine.Engine_GetAmmo(self.cpp_ptr)
        score = cpp_engine.Engine_GetScore(self.cpp_ptr)
        self.hud_stats.setText(f"AMMO: {ammo} | SCORE: {score}")

        return task.cont


if __name__ == "__main__":
    game = TheCavalaryShootingMinders()
    game.run()
    struct Vector3D { float x, y, z; };

enum WeaponType { ASSAULT_RIFLE = 0, SHOTGUN = 1, SNIPER = 2 };

struct WeaponData {
    WeaponType type;
    int ammo;
    int maxAmmo;
    float fireRateRPM;
    float damage;
    float recoil;
    float spread;
};

struct EnemyAI {
    Vector3D pos;
    float health;
    float maxHealth;
    bool active;
    float speed;
};

struct Particle {
    Vector3D pos;
    Vector3D vel;
    float life; // Remaining lifetime in seconds
    bool active;
};

struct AdvancedMods {
    float fov = 75.0f;
    float sensitivity = 1.0f;
    bool infiniteAmmo = false;
    bool lowGravity = false;
    bool speedHack = false;
    bool godMode = false;
    bool oneHitKill = false;
    bool explosiveBullets = false;
};

class AdvancedEngineBackend {
public:
    Vector3D playerPos = {0.0f, 2.0f, 10.0f};
    Vector3D playerVel = {0.0f, 0.0f, 0.0f};
    float playerHealth = 100.0f;
    bool isGrounded = true;
    int score = 0;

    AdvancedMods mods;
    
    // Inventory System
    std::vector<WeaponData> inventory;
    int currentWeaponIdx = 0;

    // AI Threat System
    std::vector<EnemyAI> enemies;

    // Fast C++ Particle System Memory Pool (Muzzle Flashes / Impact Sparks)
    static const int MAX_PARTICLES = 500;
    Particle particlePool[MAX_PARTICLES];

    AdvancedEngineBackend() {
        // Initialize Inventory
        inventory.push_back({ASSAULT_RIFLE, 30, 150, 700.0f, 25.0f, 0.05f, 0.02f});
        inventory.push_back({SHOTGUN, 8, 40, 90.0f, 120.0f, 0.3f, 0.15f});
        inventory.push_back({SNIPER, 5, 25, 50.0f, 200.0f, 0.5f, 0.001f});

        // Spawn Enemies
        for (int i = 0; i < 8; ++i) {
            EnemyAI enemy;
            enemy.pos = {(float)(rand() % 60 - 30), 1.5f, (float)(-20 - rand() % 40)};
            enemy.health = 100.0f;
            enemy.maxHealth = 100.0f;
            enemy.active = true;
            enemy.speed = 3.5f + (rand() % 30) / 10.0f;
            enemies.push_back(enemy);
        }

        // Init Particle Pool
        for (int i = 0; i < MAX_PARTICLES; ++i) particlePool[i].active = false;
    }

    void spawnImpactParticles(Vector3D pos) {
        for (int i = 0; i < 15; ++i) {
            for (int p = 0; p < MAX_PARTICLES; ++p) {
                if (!particlePool[p].active) {
                    particlePool[p].pos = pos;
                    particlePool[p].vel = {
                        (float)(rand() % 100 - 50) / 10.0f,
                        (float)(rand() % 100) / 10.0f,
                        (float)(rand() % 100 - 50) / 10.0f
                    };
                    particlePool[p].life = 0.5f;
                    particlePool[p].active = true;
                    break;
                }
            }
        }
    }

    void update(float dt, bool fwd, bool bwd, bool left, bool right, bool jump, float yaw) {
        // Player Physics
        float speed = mods.speedHack ? 32.0f : 12.0f;
        float gravity = mods.lowGravity ? 9.8f : 32.0f;

        float radYaw = yaw * (3.14159f / 180.0f);
        float moveX = 0.0f, moveZ = 0.0f;

        if (fwd)   { moveX += sinf(radYaw); moveZ -= cosf(radYaw); }
        if (bwd)   { moveX -= sinf(radYaw); moveZ += cosf(radYaw); }
        if (left)  { moveX -= cosf(radYaw); moveZ -= sinf(radYaw); }
        if (right) { moveX += cosf(radYaw); moveZ += sinf(radYaw); }

        playerPos.x += moveX * speed * dt;
        playerPos.z += moveZ * speed * dt;

        if (!isGrounded) playerVel.y -= gravity * dt;
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

        // Update AI Pathfinding toward Player
        for (auto& enemy : enemies) {
            if (!enemy.active) continue;

            float dx = playerPos.x - enemy.pos.x;
            float dz = playerPos.z - enemy.pos.z;
            float dist = sqrtf(dx * dx + dz * dz);

            if (dist > 1.5f) {
                enemy.pos.x += (dx / dist) * enemy.speed * dt;
                enemy.pos.z += (dz / dist) * enemy.speed * dt;
            } else {
                // Enemy attacks Player
                if (!mods.godMode) {
                    playerHealth -= 15.0f * dt;
                    if (playerHealth < 0) playerHealth = 0;
                }
            }
        }

        // Particle System Simulation Step
        for (int i = 0; i < MAX_PARTICLES; ++i) {
            if (particlePool[i].active) {
                particlePool[i].pos.x += particlePool[i].vel.x * dt;
                particlePool[i].pos.y += particlePool[i].vel.y * dt;
                particlePool[i].pos.z += particlePool[i].vel.z * dt;
                particlePool[i].life -= dt;
                if (particlePool[i].life <= 0) particlePool[i].active = false;
            }
        }
    }

    bool shoot(Vector3D rayDir, Vector3D& hitPosOut) {
        WeaponData& curWpn = inventory[currentWeaponIdx];

        if (!mods.infiniteAmmo) {
            if (curWpn.ammo <= 0) return false;
            curWpn.ammo--;
        }

        float damage = mods.oneHitKill ? 9999.0f : curWpn.damage;

        // Raycasting against Enemies
        for (auto& enemy : enemies) {
            if (!enemy.active) continue;

            // Simple Bounding Sphere check
            Vector3D oc = {playerPos.x - enemy.pos.x, playerPos.y - enemy.pos.y, playerPos.z - enemy.pos.z};
            float b = 2.0f * (oc.x * rayDir.x + oc.y * rayDir.y + oc.z * rayDir.z);
            float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - 2.25f; // Radius 1.5
            if (b * b - 4.0f * c > 0) {
                enemy.health -= damage;
                hitPosOut = enemy.pos;
                spawnImpactParticles(enemy.pos);

                if (enemy.health <= 0) {
                    enemy.active = false;
                    score += 250;
                    // Respawn elsewhere
                    enemy.pos = {(float)(rand() % 80 - 40), 1.5f, (float)(-20 - rand() % 40)};
                    enemy.health = enemy.maxHealth;
                    enemy.active = true;
                }
                return true;
            }
        }
        return false;
    }
};

// C-ABI Exports
extern "C" {
    AdvancedEngineBackend* Engine_CreateAdvanced() { return new AdvancedEngineBackend(); }
    void Engine_UpdateAdvanced(AdvancedEngineBackend* ptr, float dt, bool f, bool b, bool l, bool r, bool j, float yaw) {
        ptr->update(dt, f, b, l, r, j, yaw);
    }
    bool Engine_ShootAdvanced(AdvancedEngineBackend* ptr, float dx, float dy, float dz, float* hx, float* hy, float* hz) {
        Vector3D hitPos;
        bool hit = ptr->shoot({dx, dy, dz}, hitPos);
        *hx = hitPos.x; *hy = hitPos.y; *hz = hitPos.z;
        return hit;
    }
    void Engine_SwitchWeapon(AdvancedEngineBackend* ptr, int idx) {
        if (idx >= 0 && idx < (int)ptr->inventory.size()) ptr->currentWeaponIdx = idx;
    }
    void Engine_GetPlayerStats(AdvancedEngineBackend* ptr, float* hp, int* ammo, int* score, int* wpnType) {
        *hp = ptr->playerHealth;
        *ammo = ptr->inventory[ptr->currentWeaponIdx].ammo;
        *score = ptr->score;
        *wpnType = (int)ptr->inventory[ptr->currentWeaponIdx].type;
    }
    void Engine_SetAdvancedMod(AdvancedEngineBackend* ptr, int modId, bool val) {
        if (modId == 1) ptr->mods.infiniteAmmo = val;
        if (modId == 2) ptr->mods.lowGravity = val;
        if (modId == 3) ptr->mods.speedHack = val;
        if (modId == 4) ptr->mods.godMode = val;
        if (modId == 5) ptr->mods.oneHitKill = val;
    }
    int Engine_GetEnemyCount(AdvancedEngineBackend* ptr) { return (int)ptr->enemies.size(); }
    void Engine_GetEnemyData(AdvancedEngineBackend* ptr, int idx, float* x, float* y, float* z, float* hp) {
        *x = ptr->enemies[idx].pos.x;
        *y = ptr->enemies[idx].pos.y;
        *z = ptr->enemies[idx].pos.z;
        *hp = ptr->enemies[idx].health;
    }
}
Part 2: Advanced Python Client (main_advanced.py)
Run python main_advanced.py after building engine.dll / engine.so:

Python
import os
import sys
import ctypes
import math
from direct.showbase.ShowBase import ShowBase
from panda3d.core import WindowProperties, Vec3, NodePath
from direct.gui.OnscreenText import OnscreenText
from direct.gui.DirectGui import DirectFrame, DirectButton, DirectCheckButton

# Load Shared C++ Library
lib_path = "./engine.dll" if os.name == 'nt' else "./engine.so"
cpp = ctypes.CDLL(os.path.abspath(lib_path))

# Bind Signatures
cpp.Engine_CreateAdvanced.restype = ctypes.c_void_p
cpp.Engine_UpdateAdvanced.argtypes = [ctypes.c_void_p, ctypes.c_float, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.c_bool, ctypes.c_float]
cpp.Engine_ShootAdvanced.argtypes = [ctypes.c_void_p, ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]
cpp.Engine_ShootAdvanced.restype = ctypes.c_bool
cpp.Engine_SwitchWeapon.argtypes = [ctypes.c_void_p, ctypes.c_int]
cpp.Engine_GetPlayerStats.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
cpp.Engine_SetAdvancedMod.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_bool]
cpp.Engine_GetEnemyCount.argtypes = [ctypes.c_void_p]
cpp.Engine_GetEnemyCount.restype = ctypes.c_int
cpp.Engine_GetEnemyData.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float)]


class TheCavalaryShootingMindersAdvanced(ShowBase):
    def __init__(self):
        super().__init__()
        self.cpp_ptr = cpp.Engine_CreateAdvanced()

        # Window & Cursor Locking
        props = WindowProperties()
        props.setTitle("THE CAVALARY SHOOTING MINDERS - Advanced Edition")
        props.setCursorHidden(True)
        props.setMouseMode(WindowProperties.M_relative)
        self.win.requestProperties(props)

        # Keys
        self.keys = {"w": False, "s": False, "a": False, "d": False, "space": False, "ads": False}
        for k in ["w", "s", "a", "d", "space"]:
            self.accept(k, self.set_k, [k, True])
            self.accept(f"{k}-up", self.set_k, [k, False])

        # Mouse Weapon Aim Down Sights (ADS)
        self.accept("mouse3", self.set_k, ["ads", True])
        self.accept("mouse3-up", self.set_k, ["ads", False])
        
        # Weapon Switching Hotkeys
        self.accept("1", lambda: cpp.Engine_SwitchWeapon(self.cpp_ptr, 0))
        self.accept("2", lambda: cpp.Engine_SwitchWeapon(self.cpp_ptr, 1))
        self.accept("3", lambda: cpp.Engine_SwitchWeapon(self.cpp_ptr, 2))

        self.accept("mouse1", self.on_fire)
        self.accept("m", self.toggle_menu)

        # Nodes
        self.enemy_nodes = []
        self.setup_scene()

        # HUD
        self.hud_hp = OnscreenText(text="HEALTH: 100", pos=(-0.8, -0.85), scale=0.06, fg=(1, 0.2, 0.2, 1))
        self.hud_info = OnscreenText(text="AMMO: 30 | SCORE: 0 | WEAPON: AR", pos=(-0.8, -0.93), scale=0.05, fg=(0, 1, 0.8, 1))
        self.crosshair = OnscreenText(text="+", pos=(0, 0), scale=0.07, fg=(0, 1, 0, 1))

        # Advanced Mods Panel
        self.menu_open = False
        self.menu = DirectFrame(frameColor=(0.05, 0.05, 0.1, 0.95), frameSize=(-0.7, 0.7, -0.7, 0.7))
        self.menu.hide()

        OnscreenText(text="ADVANCED MODS & CHEATS", parent=self.menu, pos=(0, 0.55), scale=0.06, fg=(1, 0.3, 0.3, 1))
        DirectCheckButton(text="God Mode", parent=self.menu, scale=0.045, pos=(-0.4, 0, 0.3), command=lambda s: cpp.Engine_SetAdvancedMod(self.cpp_ptr, 4, s))
        DirectCheckButton(text="One Hit Kill", parent=self.menu, scale=0.045, pos=(-0.4, 0, 0.15), command=lambda s: cpp.Engine_SetAdvancedMod(self.cpp_ptr, 5, s))
        DirectCheckButton(text="Infinite Ammo", parent=self.menu, scale=0.045, pos=(-0.4, 0, 0.0), command=lambda s: cpp.Engine_SetAdvancedMod(self.cpp_ptr, 1, s))
        DirectCheckButton(text="Super Speed", parent=self.menu, scale=0.045, pos=(-0.4, 0, -0.15), command=lambda s: cpp.Engine_SetAdvancedMod(self.cpp_ptr, 3, s))
        DirectCheckButton(text="Low Gravity", parent=self.menu, scale=0.045, pos=(-0.4, 0, -0.3), command=lambda s: cpp.Engine_SetAdvancedMod(self.cpp_ptr, 2, s))

        self.taskMgr.add(self.update_loop, "UpdateLoop")

    def set_k(self, k, v): self.keys[k] = v

    def setup_scene(self):
        count = cpp.Engine_GetEnemyCount(self.cpp_ptr)
        for i in range(count):
            node = self.loader.loadModel("models/box")
            node.reparentTo(self.render)
            node.setScale(1.2, 1.2, 2.0)
            node.setColor(0.9, 0.1, 0.2, 1)
            self.enemy_nodes.append(node)

    def toggle_menu(self):
        self.menu_open = not self.menu_open
        props = WindowProperties()
        props.setCursorHidden(not self.menu_open)
        props.setMouseMode(WindowProperties.M_absolute if self.menu_open else WindowProperties.M_relative)
        self.win.requestProperties(props)
        if self.menu_open: self.menu.show()
        else: self.menu.hide()

    def on_fire(self):
        if self.menu_open: return
        fwd = self.camera.getMat().getRow3(2)
        hx, hy, hz = ctypes.c_float(), ctypes.c_float(), ctypes.c_float()
        
        hit = cpp.Engine_ShootAdvanced(self.cpp_ptr, fwd.x, fwd.y, fwd.z, ctypes.byref(hx), ctypes.byref(hy), ctypes.byref(hz))
        if hit:
            print(f"[C++ Engine] Hit target at position: ({hx.value:.1f}, {hy.value:.1f}, {hz.value:.1f})")

    def update_loop(self, task):
        dt = globalClock.getDt()

        if not self.menu_open and self.mouseWatcherNode.hasMouse():
            md = self.win.getPointer(0)
            self.camera.setH(self.camera.getH() - md.getX() * 0.1)
            self.camera.setP(self.camera.getP() - md.getY() * 0.1)

        # Dynamic FOV for Aim Down Sights (ADS)
        target_fov = 40.0 if self.keys["ads"] else 75.0
        self.camLens.setFov(target_fov)

        # Call C++ Physics and AI Update step
        cpp.Engine_UpdateAdvanced(
            self.cpp_ptr, dt,
            self.keys["w"], self.keys["s"], self.keys["a"], self.keys["d"],
            self.keys["space"], self.camera.getH()
        )

        # Sync Enemies from C++ Threat Engine
        count = cpp.Engine_GetEnemyCount(self.cpp_ptr)
        for i in range(count):
            ex, ey, ez, hp = ctypes.c_float(), ctypes.c_float(), ctypes.c_float(), ctypes.c_float()
            cpp.Engine_GetEnemyData(self.cpp_ptr, i, ctypes.byref(ex), ctypes.byref(ey), ctypes.byref(ez), ctypes.byref(hp))
            self.enemy_nodes[i].setPos(ex.value, ez.value, ey.value)

        # Read Stats from C++ Core
        hp, ammo, score, wpn = ctypes.c_float(), ctypes.c_int(), ctypes.c_int(), ctypes.c_int()
        cpp.Engine_GetPlayerStats(self.cpp_ptr, ctypes.byref(hp), ctypes.byref(ammo), ctypes.byref(score), ctypes.byref(wpn))

        wpn_names = ["ASSAULT RIFLE", "SHOTGUN", "SNIPER"]
        self.hud_hp.setText(f"HEALTH: {int(hp.value)}")
        self.hud_info.setText(f"AMMO: {ammo.value} | SCORE: {score.value} | WEAPON: {wpn_names[wpn.value]}")

        return task.cont

if __name__ == "__main__":
    app = TheCavalaryShootingMindersAdvanced()
    app.run()
    import sys
import math
import pygame

# Initialize Pygame
pygame.init()

# Display settings
SCREEN_WIDTH = 640
SCREEN_HEIGHT = 480
HALF_HEIGHT = SCREEN_HEIGHT // 2
FPS = 60

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Complete 3D Raycaster FPS in Python")
clock = pygame.time.Clock()

# Map Configuration (1 = Wall, 0 = Empty space)
MAP_SIZE = 16
GRID_MAP = [
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
]

# Player State
player_x = 2.5
player_y = 2.5
player_angle = 0.0
player_hp = 100
player_score = 0
FOV = math.pi / 3  # 60 degrees

# Game State
recoil = 0
is_shooting = False
z_buffer = [0.0] * SCREEN_WIDTH

# Enemies List
enemies = [
    {"x": 8.5, "y": 3.5, "hp": 3},
    {"x": 13.5, "y": 7.5, "hp": 3},
    {"x": 4.5, "y": 13.5, "hp": 3},
    {"x": 10.5, "y": 11.5, "hp": 3},
]

# Mouse setup for FPS aim
pygame.mouse.set_visible(False)
pygame.event.set_grab(True)

font = pygame.font.SysFont("monospace", 24, bold=True)

def reset_game():
    global player_x, player_y, player_angle, player_hp, player_score, enemies
    player_x, player_y = 2.5, 2.5
    player_angle = 0.0
    player_hp = 100
    player_score = 0
    enemies = [
        {"x": 8.5, "y": 3.5, "hp": 3},
        {"x": 13.5, "y": 7.5, "hp": 3},
        {"x": 4.5, "y": 13.5, "hp": 3},
        {"x": 10.5, "y": 11.5, "hp": 3},
    ]

def shoot():
    global recoil, is_shooting, player_score
    recoil = 12
    is_shooting = True

    closest_dist = 999.0
    hit_index = -1

    for i, enemy in enumerate(enemies):
        dx = enemy["x"] - player_x
        dy = enemy["y"] - player_y
        angle_to_enemy = math.atan2(dy, dx)
        diff = angle_to_enemy - player_angle

        # Normalize angle difference between -pi and pi
        while diff < -math.pi:
            diff += math.pi * 2
        while diff > math.pi:
            diff -= math.pi * 2

        dist = math.hypot(dx, dy)
        if abs(diff) < 0.2 and dist < closest_dist:
            closest_dist = dist
            hit_index = i

    if hit_index != -1:
        enemies[hit_index]["hp"] -= 1
        if enemies[hit_index]["hp"] <= 0:
            enemies.pop(hit_index)
            player_score += 100

# --- MAIN GAME LOOP ---
running = True
game_over = False

while running:
    # 1. Event Handling
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
            if game_over and event.key == pygame.K_r:
                reset_game()
                game_over = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1 and not game_over:
                shoot()

    # Mouse Aiming
    if not game_over:
        mouse_dx, _ = pygame.mouse.get_rel()
        player_angle += mouse_dx * 0.003
        player_angle %= 2 * math.pi

    # 2. Player Movement
    if not game_over:
        keys = pygame.key.get_pressed()
        move_speed = 0.05
        sin_a = math.sin(player_angle)
        cos_a = math.cos(player_angle)

        dx, dy = 0, 0
        if keys[pygame.K_w] or keys[pygame.K_UP]:
            dx += cos_a * move_speed
            dy += sin_a * move_speed
        if keys[pygame.K_s] or keys[pygame.K_DOWN]:
            dx -= cos_a * move_speed
            dy -= sin_a * move_speed
        if keys[pygame.K_a]:
            dx += sin_a * move_speed
            dy -= cos_a * move_speed
        if keys[pygame.K_d]:
            dx -= sin_a * move_speed
            dy += cos_a * move_speed

        # Wall Collision Check
        new_x = player_x + dx
        new_y = player_y + dy
        if GRID_MAP[int(player_y) * MAP_SIZE + int(new_x)] == 0:
            player_x = new_x
        if GRID_MAP[int(new_y) * MAP_SIZE + int(player_x)] == 0:
            player_y = new_y

    # 3. Enemy AI & Combat Logic
    if not game_over:
        for enemy in enemies:
            edx = player_x - enemy["x"]
            edy = player_y - enemy["y"]
            dist = math.hypot(edx, edy)

            if dist > 0.5:
                enemy["x"] += (edx / dist) * 0.015
                enemy["y"] += (edy / dist) * 0.015
            else:
                player_hp -= 0.5
                if player_hp <= 0:
                    player_hp = 0
                    game_over = True

        if recoil > 0:
            recoil -= 1

    # 4. Rendering
    # Background (Ceiling & Floor)
    screen.fill((17, 17, 17), (0, 0, SCREEN_WIDTH, HALF_HEIGHT))
    screen.fill((34, 34, 34), (0, HALF_HEIGHT, SCREEN_WIDTH, HALF_HEIGHT))

    # Raycasting Walls
    start_angle = player_angle - FOV / 2
    for ray in range(SCREEN_WIDTH):
        ray_angle = start_angle + (ray / SCREEN_WIDTH) * FOV
        sin_r = math.sin(ray_angle)
        cos_r = math.cos(ray_angle)

        # DDA Raycasting Step
        distance = 0
        hit_wall = False
        side = 0  # 0 for vertical, 1 for horizontal

        x_check = player_x
        y_check = player_y

        while not hit_wall and distance < 20:
            distance += 0.02
            x_check = player_x + cos_r * distance
            y_check = player_y + sin_r * distance

            map_x, map_y = int(x_check), int(y_check)
            if 0 <= map_x < MAP_SIZE and 0 <= map_y < MAP_SIZE:
                if GRID_MAP[map_y * MAP_SIZE + map_x] > 0:
                    hit_wall = True
                    # Estimate wall side orientation for shading
                    side = 1 if abs(x_check - map_x) < 0.05 else 0

        # Fish-eye correction
        corrected_dist = distance * math.cos(ray_angle - player_angle)
        z_buffer[ray] = corrected_dist

        # Wall Height & Drawing
        if corrected_dist > 0:
            line_height = min(int(SCREEN_HEIGHT / corrected_dist), SCREEN_HEIGHT * 2)
            color_val = max(0, min(255, int(255 / (1 + corrected_dist * 0.2))))
            wall_color = (0, color_val // 2, color_val) if side == 1 else (0, color_val, color_val)

            top = HALF_HEIGHT - line_height // 2
            pygame.draw.line(screen, wall_color, (ray, top), (ray, top + line_height))

    # Render Enemies (Sorted back-to-front)
    sorted_enemies = sorted(
        enemies,
        key=lambda e: math.hypot(player_x - e["x"], player_y - e["y"]),
        reverse=True,
    )

    for enemy in sorted_enemies:
        ex = enemy["x"] - player_x
        ey = enemy["y"] - player_y

        # Transform relative to player camera orientation
        cos_p = math.cos(-player_angle)
        sin_p = math.sin(-player_angle)

        transform_x = ex * cos_p - ey * sin_p
        transform_y = ex * sin_p + ey * cos_p

        if transform_x > 0.2:  # In front of camera
            screen_x = int(HALF_HEIGHT + (transform_y / transform_x) * SCREEN_WIDTH)
            size = int(SCREEN_HEIGHT / transform_x)

            top = HALF_HEIGHT - size // 2
            left = screen_x - size // 2

            # Render vertical slices with z-buffering
            color = (255, 50, 50) if enemy["hp"] < 3 else (255, 0, 85)
            for stripe in range(left, left + size):
                if 0 <= stripe < SCREEN_WIDTH:
                    if transform_x < z_buffer[stripe]:
                        pygame.draw.line(screen, color, (stripe, max(0, top)), (stripe, min(SCREEN_HEIGHT, top + size)))

    # HUD & Gun Overlay
    gun_x = SCREEN_WIDTH // 2 - 20
    gun_y = SCREEN_HEIGHT - 80 + recoil
    pygame.draw.rect(screen, (68, 68, 68), (gun_x, gun_y, 40, 80))
    pygame.draw.rect(screen, (34, 34, 34), (gun_x + 15, gun_y - 20, 10, 20))

    if is_shooting and recoil > 6:  # Muzzle Flash
        pygame.draw.circle(screen, (255, 255, 0), (SCREEN_WIDTH // 2, gun_y - 20), 20)
    is_shooting = False

    # Crosshair
    pygame.draw.line(screen, (0, 255, 0), (SCREEN_WIDTH // 2 - 10, HALF_HEIGHT), (SCREEN_WIDTH // 2 + 10, HALF_HEIGHT), 2)
    pygame.draw.line(screen, (0, 255, 0), (SCREEN_WIDTH // 2, HALF_HEIGHT - 10), (SCREEN_WIDTH // 2, HALF_HEIGHT + 10), 2)

    # UI Text
    hud_text = font.render(f"HEALTH: {int(player_hp)} | SCORE: {player_score}", True, (0, 255, 0))
    screen.blit(hud_text, (20, 20))

    if game_over:
        over_text = font.render("GAME OVER - Press 'R' to Restart", True, (255, 0, 0))
        text_rect = over_text.get_rect(center=(SCREEN_WIDTH // 2, HALF_HEIGHT))
        screen.blit(over_text, text_rect)

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
sys.exit()
import sys
import math
import pygame

# Initialize Pygame and Mixer for Audio
pygame.init()
pygame.mixer.init()

# Display settings
# We render internally at 320x240 for retro feel and performance, then scale up
RENDER_WIDTH = 320
RENDER_HEIGHT = 240
HALF_HEIGHT = RENDER_HEIGHT // 2
SCREEN_WIDTH = 640
SCREEN_HEIGHT = 480
FPS = 60

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
render_surface = pygame.Surface((RENDER_WIDTH, RENDER_HEIGHT))
pygame.display.set_caption("Textured 3D Raycaster with Audio")
clock = pygame.time.Clock()

# --- ASSET LOADING & GENERATION ---

# 1. AUDIO: Try to load sound files, fail gracefully if they don't exist
try:
    sfx_shoot = pygame.mixer.Sound("shoot.wav")
    sfx_hit = pygame.mixer.Sound("hit.wav")
except FileNotFoundError:
    sfx_shoot = None
    sfx_hit = None
    print("Audio files 'shoot.wav' / 'hit.wav' not found. Playing silently. Add them to your folder to hear sound!")

# 2. TEXTURES: Generate a procedural brick texture so no external images are required
TEX_SIZE = 64
wall_texture = pygame.Surface((TEX_SIZE, TEX_SIZE))
for x in range(TEX_SIZE):
    for y in range(TEX_SIZE):
        # Checkerboard brick pattern
        if (x // 16) % 2 == (y // 16) % 2:
            wall_texture.set_at((x, y), (139, 69, 19)) # Brown
        else:
            wall_texture.set_at((x, y), (160, 82, 45)) # Sienna
# Add mortar lines
pygame.draw.rect(wall_texture, (50, 50, 50), (0, 0, TEX_SIZE, TEX_SIZE), 2)
pygame.draw.line(wall_texture, (50, 50, 50), (0, 32), (64, 32), 2)

# To use your own image, delete the generation block above and uncomment this:
# wall_texture = pygame.image.load("wall.png").convert()
# TEX_SIZE = wall_texture.get_width()

# Map Configuration
MAP_SIZE = 16
GRID_MAP = [
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,
    1,0,1,0,1,0,1,1,1,1,0,1,0,1,0,1,
    1,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1,
    1,0,1,1,1,1,0,1,0,1,1,1,0,1,0,1,
    1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,
    1,1,1,0,0,1,0,1,1,1,0,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,
    1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,1,
    1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,
    1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,
    1,1,1,0,1,0,1,1,0,1,0,1,1,1,1,1,
    1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
    1,0,1,1,1,0,1,0,1,1,1,1,1,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
]

# Player State
player_x, player_y = 2.5, 2.5
player_dir_x, player_dir_y = -1.0, 0.0
player_plane_x, player_plane_y = 0.0, 0.66  # Controls FOV
player_hp, player_score = 100, 0

# Game State
recoil, is_shooting, game_over = 0, False, False
z_buffer = [0.0] * RENDER_WIDTH

enemies = [
    {"x": 8.5, "y": 3.5, "hp": 3},
    {"x": 13.5, "y": 7.5, "hp": 3},
    {"x": 4.5, "y": 13.5, "hp": 3},
    {"x": 10.5, "y": 11.5, "hp": 3}
]

pygame.mouse.set_visible(False)
pygame.event.set_grab(True)
font = pygame.font.SysFont("monospace", 24, bold=True)

def shoot():
    global recoil, is_shooting, player_score
    recoil = 12
    is_shooting = True
    
    if sfx_shoot:
        sfx_shoot.play()

    # Hitscan
    closest_dist = 999.0
    hit_index = -1
    for i, enemy in enumerate(enemies):
        dx, dy = enemy["x"] - player_x, enemy["y"] - player_y
        angle_to_enemy = math.atan2(dy, dx)
        player_angle = math.atan2(player_dir_y, player_dir_x)
        diff = (angle_to_enemy - player_angle + math.pi) % (2 * math.pi) - math.pi

        dist = math.hypot(dx, dy)
        if abs(diff) < 0.2 and dist < closest_dist:
            closest_dist = dist
            hit_index = i

    if hit_index != -1:
        if sfx_hit:
            sfx_hit.play()
        enemies[hit_index]["hp"] -= 1
        if enemies[hit_index]["hp"] <= 0:
            enemies.pop(hit_index)
            player_score += 100

running = True
while running:
    # 1. INPUT HANDLING
    for event in pygame.event.get():
        if event.type == pygame.QUIT or (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1 and not game_over:
            shoot()

    if not game_over:
        # Mouse Aim
        mouse_dx, _ = pygame.mouse.get_rel()
        rot_speed = -mouse_dx * 0.003
        old_dir_x = player_dir_x
        player_dir_x = player_dir_x * math.cos(rot_speed) - player_dir_y * math.sin(rot_speed)
        player_dir_y = old_dir_x * math.sin(rot_speed) + player_dir_y * math.cos(rot_speed)
        old_plane_x = player_plane_x
        player_plane_x = player_plane_x * math.cos(rot_speed) - player_plane_y * math.sin(rot_speed)
        player_plane_y = old_plane_x * math.sin(rot_speed) + player_plane_y * math.cos(rot_speed)

        # Movement
        keys = pygame.key.get_pressed()
        move_speed = 0.06
        
        if keys[pygame.K_w]:
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x + player_dir_x * move_speed)] == 0:
                player_x += player_dir_x * move_speed
            if GRID_MAP[int(player_y + player_dir_y * move_speed) * MAP_SIZE + int(player_x)] == 0:
                player_y += player_dir_y * move_speed
        if keys[pygame.K_s]:
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x - player_dir_x * move_speed)] == 0:
                player_x -= player_dir_x * move_speed
            if GRID_MAP[int(player_y - player_dir_y * move_speed) * MAP_SIZE + int(player_x)] == 0:
                player_y -= player_dir_y * move_speed
        if keys[pygame.K_a]:
            # Strafe left
            strafe_x = player_dir_y
            strafe_y = -player_dir_x
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x + strafe_x * move_speed)] == 0: player_x += strafe_x * move_speed
            if GRID_MAP[int(player_y + strafe_y * move_speed) * MAP_SIZE + int(player_x)] == 0: player_y += strafe_y * move_speed
        if keys[pygame.K_d]:
            # Strafe right
            strafe_x = -player_dir_y
            strafe_y = player_dir_x
            if GRID_MAP[int(player_y) * MAP_SIZE + int(player_x + strafe_x * move_speed)] == 0: player_x += strafe_x * move_speed
            if GRID_MAP[int(player_y + strafe_y * move_speed) * MAP_SIZE + int(player_x)] == 0: player_y += strafe_y * move_speed

        # Enemy Logic
        for enemy in enemies:
            edx, edy = player_x - enemy["x"], player_y - enemy["y"]
            dist = math.hypot(edx, edy)
            if dist > 0.5:
                enemy["x"] += (edx / dist) * 0.015
                enemy["y"] += (edy / dist) * 0.015
            else:
                player_hp -= 0.5
                if player_hp <= 0: game_over = True

        if recoil > 0: recoil -= 1

    # 2. RENDERING (To low-res internal surface)
    render_surface.fill((17, 17, 17), (0, 0, RENDER_WIDTH, HALF_HEIGHT)) # Ceiling
    render_surface.fill((34, 34, 34), (0, HALF_HEIGHT, RENDER_WIDTH, HALF_HEIGHT)) # Floor

    # True DDA Raycaster
    for x in range(RENDER_WIDTH):
        camera_x = 2 * x / RENDER_WIDTH - 1
        ray_dir_x = player_dir_x + player_plane_x * camera_x
        ray_dir_y = player_dir_y + player_plane_y * camera_x

        map_x, map_y = int(player_x), int(player_y)
        delta_dist_x = abs(1 / ray_dir_x) if ray_dir_x != 0 else 1e30
        delta_dist_y = abs(1 / ray_dir_y) if ray_dir_y != 0 else 1e30

        if ray_dir_x < 0:
            step_x, side_dist_x = -1, (player_x - map_x) * delta_dist_x
        else:
            step_x, side_dist_x = 1, (map_x + 1.0 - player_x) * delta_dist_x

        if ray_dir_y < 0:
            step_y, side_dist_y = -1, (player_y - map_y) * delta_dist_y
        else:
            step_y, side_dist_y = 1, (map_y + 1.0 - player_y) * delta_dist_y

        hit, side = 0, 0
        while hit == 0:
            if side_dist_x < side_dist_y:
                side_dist_x += delta_dist_x
                map_x += step_x
                side = 0
            else:
                side_dist_y += delta_dist_y
                map_y += step_y
                side = 1
            if GRID_MAP[map_y * MAP_SIZE + map_x] > 0: hit = 1

        if side == 0: perp_wall_dist = side_dist_x - delta_dist_x
        else: perp_wall_dist = side_dist_y - delta_dist_y

        z_buffer[x] = perp_wall_dist
        line_height = int(RENDER_HEIGHT / (perp_wall_dist + 0.0001))
        draw_start = max(0, -line_height // 2 + HALF_HEIGHT)
        draw_end = min(RENDER_HEIGHT, line_height // 2 + HALF_HEIGHT)

        # Calculate exact Texture X coordinate
        if side == 0: wall_x = player_y + perp_wall_dist * ray_dir_y
        else: wall_x = player_x + perp_wall_dist * ray_dir_x
        wall_x -= math.floor(wall_x)

        tex_x = int(wall_x * TEX_SIZE)
        if (side == 0 and ray_dir_x > 0) or (side == 1 and ray_dir_y < 0):
            tex_x = TEX_SIZE - tex_x - 1

        # Extract texture column, scale to line_height, and blit to screen
        if draw_end > draw_start:
            tex_col = wall_texture.subsurface((tex_x, 0, 1, TEX_SIZE))
            
            # Simple darkness shading based on side
            if side == 1: 
                darken = pygame.Surface((1, TEX_SIZE))
                darken.set_alpha(100)
                darken.fill((0,0,0))
                tex_col = tex_col.copy()
                tex_col.blit(darken, (0,0))

            scaled_col = pygame.transform.scale(tex_col, (1, draw_end - draw_start))
            render_surface.blit(scaled_col, (x, draw_start))

    # Render Enemies (Sprites)
    sorted_enemies = sorted(enemies, key=lambda e: (player_x - e["x"])**2 + (player_y - e["y"])**2, reverse=True)
    for enemy in sorted_enemies:
        ex, ey = enemy["x"] - player_x, enemy["y"] - player_y
        inv_det = 1.0 / (player_plane_x * player_dir_y - player_dir_x * player_plane_y)
        transform_x = inv_det * (player_dir_y * ex - player_dir_x * ey)
        transform_y = inv_det * (-player_plane_y * ex + player_plane_x * ey)

        if transform_y > 0:
            sprite_screen_x = int((RENDER_WIDTH / 2) * (1 + transform_x / transform_y))
            sprite_height = abs(int(RENDER_HEIGHT / transform_y))
            draw_start_y = max(0, -sprite_height // 2 + HALF_HEIGHT)
            draw_end_y = min(RENDER_HEIGHT - 1, sprite_height // 2 + HALF_HEIGHT)
            
            draw_start_x = max(0, -sprite_height // 2 + sprite_screen_x)
            draw_end_x = min(RENDER_WIDTH - 1, sprite_height // 2 + sprite_screen_x)

            color = (255, 50, 50) if enemy["hp"] < 3 else (255, 0, 85)
            for stripe in range(draw_start_x, draw_end_x):
                if 0 <= stripe < RENDER_WIDTH and transform_y < z_buffer[stripe]:
                    pygame.draw.line(render_surface, color, (stripe, draw_start_y), (stripe, draw_end_y))

    # 3. SCALE TO SCREEN & DRAW UI
    pygame.transform.scale(render_surface, (SCREEN_WIDTH, SCREEN_HEIGHT), screen)

    # UI Overlay (Drawn at full resolution)
    gun_x = SCREEN_WIDTH // 2 - 20
    gun_y = SCREEN_HEIGHT - 80 + recoil
    pygame.draw.rect(screen, (68, 68, 68), (gun_x, gun_y, 40, 80))
    pygame.draw.rect(screen, (34, 34, 34), (gun_x + 15, gun_y - 20, 10, 20))

    if is_shooting and recoil > 6:
        pygame.draw.circle(screen, (255, 255, 0), (SCREEN_WIDTH // 2, gun_y - 20), 20)
    is_shooting = False

    # Crosshair
    pygame.draw.line(screen, (0, 255, 0), (SCREEN_WIDTH // 2 - 10, SCREEN_HEIGHT//2), (SCREEN_WIDTH // 2 + 10, SCREEN_HEIGHT//2), 2)
    pygame.draw.line(screen, (0, 255, 0), (SCREEN_WIDTH // 2, SCREEN_HEIGHT//2 - 10), (SCREEN_WIDTH // 2, SCREEN_HEIGHT//2 + 10), 2)

    hud_text = font.render(f"HEALTH: {int(player_hp)} | SCORE: {player_score}", True, (0, 255, 0))
    screen.blit(hud_text, (20, 20))

    if game_over:
        over_text = font.render("GAME OVER", True, (255, 0, 0))
        screen.blit(over_text, over_text.get_rect(center=(SCREEN_WIDTH // 2, SCREEN_HEIGHT // 2)))

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
sys.exit()