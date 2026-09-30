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