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
