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