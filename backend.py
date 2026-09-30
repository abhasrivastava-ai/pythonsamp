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

    bool checkRaySphereHit(Vector3D rayOrigin, Vector3D rayDir, Vector3D sphereCenter, float sphereRadius) {
        Vector3D oc = {rayOrigin.x - sphereCenter.x, rayOrigin.y - sphereCenter.y, rayOrigin.z - sphereCenter.z};
        float a = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z;
        float b = 2.0f * (oc.x * rayDir.x + oc.y * rayDir.y + oc.z * rayDir.z);
        float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - (sphereRadius * sphereRadius);
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f) return false;
        float t1 = (-b - sqrtf(discriminant)) / (2.0f * a);
        return t1 > 0.0f;
    }

    bool shoot(Vector3D rayDir) {
        if (!mods.infiniteAmmo) {
            if (ammo <= 0) return false;
            ammo--;
        }

        for (auto& target : targets) {
            if (target.active) {
                if (checkRaySphereHit(playerPos, rayDir, target.pos, target.radius)) {
                    target.active = false;
                    score += 100;
                    target.pos = {(float)(rand() % 80 - 40), 1.5f, (float)(-10 - rand() % 50)};
                    target.active = true;
                    return true;
                }
            }
        }
        return false;
    }
};

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