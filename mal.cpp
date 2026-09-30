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
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <iomanip>
#include <algorithm>

// -----------------------------------------------------------------------------
// DATA MODELS
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string slot;
    float weight;         // in kg
    float damageModifier; // percentage change
    float accuracyModifier;
};

struct Weapon {
    std::string name;
    std::string caliber;
    float baseDamage;
    float fireRate;    // RPM
    float accuracy;    // 0 - 100
    float range;       // Meters
    float baseWeight;  // kg
    std::vector<Attachment> availableAttachments;
};

// -----------------------------------------------------------------------------
// LOADOUT ENGINE CLASS
// -----------------------------------------------------------------------------
class BlackClawXLoadoutEngine {
private:
    std::vector<Weapon> assaultRifles;
    int selectedWeaponIndex = 0;
    float maxLoadoutWeight = 35.0f; // Capacity in kg
    std::vector<Attachment> equippedAttachments;

public:
    BlackClawXLoadoutEngine() {
        // Initialize Assault Rifle Database
        assaultRifles = {
            {"Claw-AR 762", "7.62x39mm", 42.0f, 600.0f, 75.0f, 65.0f, 4.2f,
                {{"Suppressor X", "Muzzle", 0.5f, 0.0f, +2.0f},
                 {"Extended Mag", "Ammo", 1.2f, 0.0f, -1.0f},
                 {"Red Dot Sight", "Optic", 0.2f, 0.0f, +3.0f}}},
            
            {"Viper-556", "5.56x45mm NATO", 33.0f, 850.0f, 88.0f, 50.0f, 3.6f,
                {{"Angled Grip", "Underbarrel", 0.4f, 0.0f, +4.0f},
                 {"Heavy Barrel", "Barrel", 1.5f, +5.0f, +2.0f}}},
            
            {"Apex Valkyrie", "6.8mm SPC", 38.0f, 720.0f, 82.0f, 75.0f, 4.0f,
                {{"Thermal Scope", "Optic", 0.8f, 0.0f, +5.0f},
                 {"Match Grade Ammo", "Ammo", 0.3f, +3.0f, +2.0f}}},
            
            {"Phantom-X", ".300 Blackout", 45.0f, 550.0f, 70.0f, 40.0f, 3.2f,
                {{"Integrated Suppressor", "Barrel", 0.9f, +2.0f, +1.0f}}}
        };
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                        BLACKclawX :: LOADOUT HUB                        \n";
        std::cout << "========================================================================\n";
        std::cout << " OPERATOR: BLACKCLAW-01 | ACTIVE CATEGORY: ASSAULT RIFLES\n";
        
        float currentWeight = GetCurrentWeight();
        std::cout << " LOADOUT WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1) 
                  << currentWeight << " / " << maxLoadoutWeight << " KG]\n";
        
        // Draw ASCII Weight Bar
        int barWidth = 30;
        int filled = static_cast<int>((currentWeight / maxLoadoutWeight) * barWidth);
        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "]\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    float GetCurrentWeight() const {
        float weight = assaultRifles[selectedWeaponIndex].baseWeight;
        for (const auto& att : equippedAttachments) {
            weight += att.weight;
        }
        return weight;
    }

    void DisplayAssaultCategory() {
        const Weapon& currentGun = assaultRifles[selectedWeaponIndex];

        std::cout << "\n[ ASSAULT RIFLE SELECTION ]\n";
        for (size_t i = 0; i < assaultRifles.size(); ++i) {
            if (static_cast<int>(i) == selectedWeaponIndex) {
                std::cout << "  > [" << (i + 1) << "] " << assaultRifles[i].name << " (EQUIPPED)\n";
            } else {
                std::cout << "    [" << (i + 1) << "] " << assaultRifles[i].name << "\n";
            }
        }

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " WEAPON INSPECTOR: " << currentGun.name << " (" << currentGun.caliber << ")\n";
        std::cout << "------------------------------------------------------------------------\n";

        // Calculate modified stats
        float totalDamage = currentGun.baseDamage;
        float totalAccuracy = currentGun.accuracy;
        for (const auto& att : equippedAttachments) {
            totalDamage += att.damageModifier;
            totalAccuracy += att.accuracyModifier;
        }

        PrintStatBar("DAMAGE  ", totalDamage, 100.0f);
        PrintStatBar("FIRE RATE", currentGun.fireRate, 1000.0f);
        PrintStatBar("ACCURACY", totalAccuracy, 100.0f);
        PrintStatBar("RANGE   ", currentGun.range, 100.0f);

        std::cout << "\n[ AVAILABLE ATTACHMENTS ]\n";
        for (size_t i = 0; i < currentGun.availableAttachments.size(); ++i) {
            const auto& att = currentGun.availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] " 
                      << std::left << std::setw(20) << att.name 
                      << " Slot: " << std::setw(12) << att.slot 
                      << " Weight: +" << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void SelectWeapon(int index) {
        if (index >= 0 && index < static_cast<int>(assaultRifles.size())) {
            selectedWeaponIndex = index;
            equippedAttachments.clear(); // Reset attachments on swap
        }
    }

    void ToggleAttachment(int index) {
        const Weapon& currentGun = assaultRifles[selectedWeaponIndex];
        if (index >= 0 && index < static_cast<int>(currentGun.availableAttachments.size())) {
            const auto& targetAtt = currentGun.availableAttachments[index];

            auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

            if (it != equippedAttachments.end()) {
                equippedAttachments.erase(it);
            } else {
                if (GetCurrentWeight() + targetAtt.weight <= maxLoadoutWeight) {
                    equippedAttachments.push_back(targetAtt);
                } else {
                    std::cout << "\n>> ERROR: Weight limit exceeded! Remove an attachment first.\n";
                }
            }
        }
    }
};

// -----------------------------------------------------------------------------
// SINGLE MAIN ENTRY POINT
// -----------------------------------------------------------------------------
int main() {
    BlackClawXLoadoutEngine engine;
    bool isRunning = true;

    while (isRunning) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        engine.DisplayHeader();
        engine.DisplayAssaultCategory();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-4] Select Assault Rifle | [A] Toggle Attachments | [Q] Quit\n";
        std::cout << " Enter Choice: ";

        char choice;
        std::cin >> choice;

        if (choice >= '1' && choice <= '4') {
            engine.SelectWeapon((choice - '1'));
        } else if (choice == 'a' || choice == 'A') {
            std::cout << " Enter Attachment Number to Toggle: ";
            int attNum;
            if (std::cin >> attNum) {
                engine.ToggleAttachment(attNum - 1);
            }
        } else if (choice == 'q' || choice == 'Q') {
            isRunning = false;
        }
    }

    std::cout << "\nExiting BLACKclawX Loadout Space...\n";
    return 0;
}

#ifndef BLACKCLAWX_ARSENAL_HPP
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace BlackclawX {

// =============================================================================
// WEAPON CATEGORIES & DATA STRUCTURES
// =============================================================================
enum class WeaponCategory {
    AssaultRifle,
    SMG,
    SniperRifle,
    LMG,
    MarksmanRifle,
    Shotgun,
    Pistol
};

inline std::string CategoryToString(WeaponCategory cat) {
    switch (cat) {
        case WeaponCategory::AssaultRifle:  return "Assault Rifle";
        case WeaponCategory::SMG:           return "Submachine Gun";
        case WeaponCategory::SniperRifle:   return "Sniper Rifle";
        case WeaponCategory::LMG:           return "Light Machine Gun";
        case WeaponCategory::MarksmanRifle: return "Marksman Rifle";
        case WeaponCategory::Shotgun:       return "Shotgun";
        case WeaponCategory::Pistol:        return "Pistol/Secondary";
    }
    return "Unknown";
}

struct Attachment {
    std::string name;
    std::string slot; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel, Optic
    float weight;         
    float damageMod;      
    float accuracyMod;    
    float rangeMod;       
    float mobilityMod;    
    float controlMod;     
};

struct WeaponBaseStats {
    float damage;
    float fireRate;
    float accuracy;
    float range;
    float mobility;
    float control;
    float baseWeight;
};

struct WeaponData {
    std::string id;
    std::string displayName;
    std::string caliber;
    WeaponCategory category;
    WeaponBaseStats baseStats;
};

// =============================================================================
// GENERIC WEAPON ENGINE CLASS
// =============================================================================
class WeaponEngine {
private:
    WeaponData data;
    size_t maxAttachmentSlots = 5;
    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    explicit WeaponEngine(const WeaponData& weaponData) : data(weaponData) {
        InitializeDefaultAttachments();
    }

    const WeaponData& GetData() const { return data; }
    
    WeaponBaseStats GetCalculatedStats() const {
        WeaponBaseStats currentStats = data.baseStats;
        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.baseWeight += att.weight;
        }
        return currentStats;
    }

    bool EquipAttachment(const Attachment& att) {
        if (equippedAttachments.size() >= maxAttachmentSlots) return false;
        
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& a) { return a.slot == att.slot; }),
            equippedAttachments.end()
        );

        equippedAttachments.push_back(att);
        return true;
    }

    void UnequipSlot(const std::string& slot) {
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&slot](const Attachment& a) { return a.slot == slot; }),
            equippedAttachments.end()
        );
    }

    void ClearAttachments() { equippedAttachments.clear(); }
    const std::vector<Attachment>& GetEquippedAttachments() const { return equippedAttachments; }
    const std::vector<Attachment>& GetAvailableAttachments() const { return availableAttachments; }

private:
    void InitializeDefaultAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor", "Muzzle", 0.5f, 0.0f, -2.0f, 12.0f, -4.0f, 0.0f},
            {"OWC Laser - Tactical", "Laser", 0.1f, 0.0f, 4.0f, 0.0f, 5.0f, 0.0f},
            {"Extended Mag A", "Ammunition", 0.6f, 0.0f, 0.0f, 0.0f, -3.0f, 0.0f},
            {"Granulated Grip Tape", "Rear Grip", 0.1f, 0.0f, 5.0f, 0.0f, 0.0f, 2.0f},
            {"Merc Foregrip", "Underbarrel", 0.5f, 0.0f, 3.0f, 0.0f, -3.0f, 6.0f}
        };
    }
};

// =============================================================================
// BLACKCLAWX MASTER ARSENAL REGISTRY
// =============================================================================
class MasterArsenalRegistry {
private:
    std::unordered_map<std::string, WeaponData> registry;

public:
    MasterArsenalRegistry() {
        RegisterAllWeapons();
    }

    bool HasWeapon(const std::string& id) const {
        return registry.find(id) != registry.end();
    }

    std::unique_ptr<WeaponEngine> CreateWeapon(const std::string& id) const {
        auto it = registry.find(id);
        if (it != registry.end()) {
            return std::make_unique<WeaponEngine>(it->second);
        }
        return nullptr;
    }

    size_t TotalWeaponsCount() const { return registry.size(); }

    std::vector<std::string> GetWeaponsByCategory(WeaponCategory category) const {
        std::vector<std::string> ids;
        for (const auto& [id, data] : registry) {
            if (data.category == category) {
                ids.push_back(id);
            }
        }
        return ids;
    }

private:
    void Add(const std::string& id, const std::string& name, const std::string& cal, WeaponCategory cat, float dmg, float rpm, float acc, float rng, float mob, float ctl, float wt) {
        registry[id] = { id, name, cal, cat, { dmg, rpm, acc, rng, mob, ctl, wt } };
    }

    void RegisterAllWeapons() {
        // 1. ASSAULT RIFLES
        Add("ak47", "AK-47", "7.62x39mm", WeaponCategory::AssaultRifle, 38, 55, 67, 58, 60, 44, 3.8f);
        Add("ak117", "AK117", "5.45x39mm", WeaponCategory::AssaultRifle, 32, 77, 50, 45, 67, 46, 3.2f);
        Add("m4", "M4", "5.56x45mm", WeaponCategory::AssaultRifle, 28, 68, 58, 58, 69, 54, 3.1f);
        Add("m16", "M16", "5.56x45mm", WeaponCategory::AssaultRifle, 40, 45, 81, 62, 60, 68, 3.6f);
        Add("drh", "DR-H (FN SCAR-H)", "7.62x51mm", WeaponCategory::AssaultRifle, 47, 57, 59, 54, 63, 57, 3.6f);
        Add("kilo141", "Kilo 141", "5.56x45mm", WeaponCategory::AssaultRifle, 29, 68, 57, 54, 65, 55, 3.2f);
        Add("m13", "M13", "5.56x45mm", WeaponCategory::AssaultRifle, 24, 88, 56, 53, 72, 43, 3.0f);
        Add("grau556", "Grau 5.56", "5.56x45mm", WeaponCategory::AssaultRifle, 28, 73, 65, 55, 66, 60, 3.1f);
        Add("krig6", "Krig 6", "5.56x45mm", WeaponCategory::AssaultRifle, 30, 65, 62, 56, 67, 58, 3.3f);
        Add("oden", "Oden", "12.7x55mm", WeaponCategory::AssaultRifle, 48, 41, 56, 64, 51, 32, 4.2f);
        Add("type25", "Type 25", "5.8x42mm", WeaponCategory::AssaultRifle, 28, 79, 44, 48, 72, 43, 3.2f);
        Add("type19", "Type 19", "5.8x42mm Custom", WeaponCategory::AssaultRifle, 28, 75, 54, 48, 68, 52, 3.3f);
        Add("asval", "AS VAL", "9x39mm", WeaponCategory::AssaultRifle, 31, 80, 41, 45, 75, 38, 2.5f);
        Add("ffar1", "FFAR 1", "5.56x45mm", WeaponCategory::AssaultRifle, 26, 91, 44, 46, 74, 39, 3.0f);
        Add("bal27", "BAL-27", "5.56x45mm Custom", WeaponCategory::AssaultRifle, 27, 72, 51, 50, 70, 50, 3.1f);
        Add("bp50", "BP50", "5.56x45mm", WeaponCategory::AssaultRifle, 23, 85, 52, 47, 76, 48, 3.0f);
        Add("cr56", "CR-56 AMAX", "7.62x39mm", WeaponCategory::AssaultRifle, 33, 64, 53, 56, 64, 51, 3.5f);
        Add("lk24", "LK24", "5.56x45mm", WeaponCategory::AssaultRifle, 26, 63, 61, 58, 65, 64, 3.3f);
        Add("icr1", "ICR-1", "5.56x45mm", WeaponCategory::AssaultRifle, 30, 60, 69, 52, 64, 78, 3.1f);
        Add("asm10", "ASM10", "5.45x39mm", WeaponCategory::AssaultRifle, 34, 52, 51, 56, 57, 52, 4.0f);
        Add("bk57", "BK57", "7.62x39mm", WeaponCategory::AssaultRifle, 27, 66, 57, 50, 66, 59, 3.4f);
        Add("kn44", "KN-44", "7.62x39mm", WeaponCategory::AssaultRifle, 26, 63, 55, 51, 64, 58, 3.4f);
        Add("peacekeeper", "Peacekeeper MK2", "5.56x45mm", WeaponCategory::AssaultRifle, 27, 72, 58, 50, 75, 54, 2.9f);
        Add("hbra3", "HBRa3", "5.56x45mm", WeaponCategory::AssaultRifle, 26, 67, 52, 46, 68, 56, 3.2f);
        Add("hvk30", "HVK-30", "5.56x45mm High-Pressure", WeaponCategory::AssaultRifle, 35, 71, 55, 50, 66, 58, 3.25f);
        Add("em2", "EM2", "7.07x62mm", WeaponCategory::AssaultRifle, 36, 53, 61, 59, 59, 57, 3.7f);
        Add("maddox", "Maddox", "5.56x45mm", WeaponCategory::AssaultRifle, 28, 74, 49, 46, 73, 48, 3.1f);
        Add("groza", "Groza", "7.62x39mm", WeaponCategory::AssaultRifle, 29, 75, 48, 47, 71, 45, 3.2f);
        Add("lag53", "LAG 53", "7.62x51mm", WeaponCategory::AssaultRifle, 37, 50, 63, 61, 58, 59, 3.8f);
        Add("ram7", "RAM-7", "5.56x45mm", WeaponCategory::AssaultRifle, 28, 86, 49, 48, 71, 42, 3.0f);
        Add("xm4", "XM4", "5.56x45mm", WeaponCategory::AssaultRifle, 28, 72, 56, 54, 68, 53, 3.1f);

        // 2. SUBMACHINE GUNS
        Add("qq9", "QQ9 (MP5)", "9x19mm", WeaponCategory::SMG, 25, 83, 40, 40, 81, 42, 2.5f);
        Add("cbr4", "CBR4 (P90)", "5.7x28mm", WeaponCategory::SMG, 24, 80, 43, 48, 83, 50, 2.6f);
        Add("fennec", "Fennec (Vector)", ".45 ACP", WeaponCategory::SMG, 23, 111, 23, 32, 88, 20, 2.1f);
        Add("switchblade", "Switchblade X9", "9x19mm", WeaponCategory::SMG, 24, 86, 42, 42, 86, 47, 2.3f);
        Add("uss9", "USS 9 (Uzi)", "9x19mm", WeaponCategory::SMG, 26, 75, 45, 38, 85, 52, 2.7f);
        Add("cx9", "CX9", "9x19mm", WeaponCategory::SMG, 22, 103, 38, 36, 82, 39, 2.4f);
        Add("pp19", "PP19 Bizon", "9x18mm Makarov", WeaponCategory::SMG, 26, 65, 61, 52, 73, 63, 2.7f);
        Add("rus79u", "RUS-79U", "5.45x39mm", WeaponCategory::SMG, 28, 77, 51, 44, 79, 50, 2.7f);
        Add("msmc", "MSMC", "5.56x30mm MINSAS", WeaponCategory::SMG, 31, 92, 37, 33, 86, 26, 2.3f);
        Add("pdw57", "PDW-57", "5.7x28mm", WeaponCategory::SMG, 27, 75, 49, 43, 75, 50, 2.8f);
        Add("mx9", "MX9", "9x19mm", WeaponCategory::SMG, 30, 80, 45, 41, 82, 40, 2.4f);
        Add("mac10", "MAC-10", ".45 ACP", WeaponCategory::SMG, 24, 120, 28, 28, 92, 22, 1.8f);
        Add("lapa", "LAPA", "9x19mm", WeaponCategory::SMG, 28, 70, 52, 47, 80, 56, 2.5f);
        Add("ots9", "OTs 9", "9x18mm", WeaponCategory::SMG, 27, 86, 41, 38, 87, 43, 2.2f);
        Add("razorback", "Razorback", "5.7x28mm", WeaponCategory::SMG, 30, 62, 57, 49, 74, 57, 2.8f);
        Add("chicom", "Chicom", "9x19mm Burst", WeaponCategory::SMG, 31, 65, 63, 43, 80, 61, 2.5f);
        Add("hg40", "HG 40", "9x19mm", WeaponCategory::SMG, 28, 61, 62, 48, 76, 60, 2.9f);
        Add("gks", "GKS", "9x19mm", WeaponCategory::SMG, 28, 60, 62, 50, 72, 59, 2.8f);
        Add("pharo", "Pharo", "9x19mm Burst", WeaponCategory::SMG, 34, 68, 49, 42, 83, 38, 2.4f);
        Add("agr556", "AGR 556", "5.56x45mm", WeaponCategory::SMG, 27, 75, 52, 47, 77, 51, 2.9f);
        Add("vmp", "VMP", "9x19mm", WeaponCategory::SMG, 26, 90, 42, 41, 84, 38, 2.3f);
        Add("fss_hurricane", "FSS Hurricane", "5.7x28mm", WeaponCategory::SMG, 25, 82, 55, 46, 81, 54, 2.6f);

        // 3. SNIPER RIFLES
        Add("dlq33", "DL Q33", ".408 CheyTac", WeaponCategory::SniperRifle, 90, 28, 62, 95, 36, 32, 6.5f);
        Add("locus", "Locus", ".50 BMG", WeaponCategory::SniperRifle, 84, 35, 59, 90, 46, 35, 5.8f);
        Add("lw3_tundra", "LW3-Tundra", ".338 Lapua", WeaponCategory::SniperRifle, 91, 31, 61, 94, 40, 34, 6.1f);
        Add("koshka", "Koshka", ".338 Lapua", WeaponCategory::SniperRifle, 86, 33, 60, 92, 43, 36, 5.9f);
        Add("arctic50", "Arctic .50", ".50 BMG", WeaponCategory::SniperRifle, 85, 31, 52, 90, 33, 25, 7.2f);
        Add("rytec", "Rytec AMR", "25x59mm Payload", WeaponCategory::SniperRifle, 80, 29, 48, 92, 28, 20, 8.1f);
        Add("hdr", "HDR", "12.7x108mm", WeaponCategory::SniperRifle, 95, 24, 68, 98, 26, 28, 8.5f);
        Add("xpr50", "XPR-50", ".50 BMG Auto", WeaponCategory::SniperRifle, 69, 42, 50, 84, 40, 37, 6.8f);
        Add("m21ebr", "M21 EBR", "7.62x51mm", WeaponCategory::SniperRifle, 60, 45, 56, 80, 44, 40, 5.2f);
        Add("outlaw", "Outlaw", ".338 Lapua", WeaponCategory::SniperRifle, 83, 42, 52, 85, 54, 30, 4.8f);
        Add("na45", "NA-45", "Explosive Primer", WeaponCategory::SniperRifle, 30, 45, 40, 70, 41, 28, 6.0f);
        Add("zrg20mm", "ZRG 20mm", "20x110mm", WeaponCategory::SniperRifle, 98, 22, 70, 99, 22, 18, 9.5f);
        Add("svd", "SVD (Dragunov)", "7.62x54mmR", WeaponCategory::SniperRifle, 72, 40, 55, 85, 41, 36, 5.5f);

        // 4. LIGHT MACHINE GUNS
        Add("mg42", "MG42", "7.92x57mm Mauser", WeaponCategory::LMG, 30, 120, 42, 60, 31, 28, 11.5f);
        Add("holger26", "Holger 26", "5.56x45mm", WeaponCategory::LMG, 31, 72, 53, 62, 54, 53, 5.2f);
        Add("rpd", "RPD", "7.62x39mm", WeaponCategory::LMG, 31, 66, 61, 65, 38, 58, 7.5f);
        Add("chopper", "Chopper", "5.56x45mm", WeaponCategory::LMG, 31, 75, 43, 52, 45, 52, 6.8f);
        Add("ul736", "UL736", "5.56x45mm", WeaponCategory::LMG, 32, 62, 58, 70, 43, 61, 6.2f);
        Add("s36", "S36", "5.56x45mm", WeaponCategory::LMG, 30, 75, 45, 55, 39, 40, 7.0f);
        Add("m4lmg", "M4LMG", "5.56x45mm", WeaponCategory::LMG, 32, 63, 57, 64, 44, 59, 6.0f);
        Add("hades", "Hades", "5.56x45mm", WeaponCategory::LMG, 29, 71, 54, 58, 56, 54, 5.5f);
        Add("pkm", "PKM", "7.62x54mmR", WeaponCategory::LMG, 34, 63, 63, 72, 29, 52, 8.8f);
        Add("dingo", "Dingo", "5.56x45mm", WeaponCategory::LMG, 28, 74, 52, 56, 52, 51, 5.4f);
        Add("bruen_mk9", "Bruen MK9", "5.56x45mm", WeaponCategory::LMG, 31, 75, 56, 63, 41, 55, 7.2f);

        // 5. MARKSMAN RIFLES
        Add("sks", "SKS", "7.62x39mm Semi", WeaponCategory::MarksmanRifle, 60, 43, 69, 62, 53, 57, 3.8f);
        Add("kilo_bolt", "Kilo Bolt-Action", "7.92x57mm", WeaponCategory::MarksmanRifle, 85, 28, 59, 86, 48, 38, 4.1f);
        Add("kar98k", "Kar98k", "7.92x57mm", WeaponCategory::MarksmanRifle, 84, 29, 60, 85, 49, 39, 4.0f);
        Add("spr208", "SP-R 208", ".300 Win Mag", WeaponCategory::MarksmanRifle, 82, 32, 62, 84, 50, 41, 3.9f);
        Add("mk2", "MK2", ".45-70 Gov", WeaponCategory::MarksmanRifle, 80, 36, 56, 78, 56, 43, 3.5f);
        Add("type63", "Type 63", "7.62x39mm", WeaponCategory::MarksmanRifle, 58, 46, 67, 63, 54, 58, 3.7f);
        Add("so14", "SO-14", "7.62x51mm", WeaponCategory::MarksmanRifle, 54, 52, 64, 65, 50, 52, 4.2f);

        // 6. SHOTGUNS
        Add("krm262", "KRM-262", "12 Gauge Pump", WeaponCategory::Shotgun, 98, 28, 51, 39, 73, 41, 3.4f);
        Add("by15", "BY15", "12 Gauge Pump", WeaponCategory::Shotgun, 95, 29, 48, 42, 71, 42, 3.5f);
        Add("r90", "R9-0", "12 Gauge Double-Barrel", WeaponCategory::Shotgun, 88, 42, 49, 38, 68, 44, 4.1f);
        Add("hs0405", "HS0405", "10 Gauge Lever", WeaponCategory::Shotgun, 100, 24, 45, 37, 69, 35, 3.8f);
        Add("striker", "Striker", "12 Gauge Auto", WeaponCategory::Shotgun, 70, 38, 38, 30, 65, 46, 4.3f);
        Add("echo", "Echo", "12 Gauge Semi", WeaponCategory::Shotgun, 72, 35, 40, 32, 63, 40, 4.5f);
        Add("jak12", "JAK-12", "12 Gauge Mag-Fed", WeaponCategory::Shotgun, 68, 42, 39, 31, 59, 38, 5.1f);
        Add("argus", "Argus", "12 Gauge Precision", WeaponCategory::Shotgun, 92, 27, 65, 48, 70, 45, 3.6f);
        Add("hs2126", "HS2126", "12 Gauge Burst", WeaponCategory::Shotgun, 76, 45, 33, 28, 74, 30, 3.3f);

        // 7. SECONDARY WEAPONS (PISTOLS)
        Add("mw11", "MW11 (1911)", ".45 ACP", WeaponCategory::Pistol, 41, 61, 58, 33, 90, 60, 1.1f);
        Add("j358", "J358 (.357)", ".357 Magnum", WeaponCategory::Pistol, 72, 37, 65, 45, 87, 32, 1.4f);
        Add("50gs", ".50 GS (Deagle)", ".50 AE", WeaponCategory::Pistol, 76, 33, 62, 50, 85, 25, 2.0f);
        Add("renetti", "Renetti", "9x19mm Burst", WeaponCategory::Pistol, 38, 67, 56, 35, 89, 52, 1.2f);
        Add("lcar9", "L-CAR 9", "9x19mm Auto", WeaponCategory::Pistol, 31, 82, 46, 30, 92, 41, 1.1f);
        Add("shorty", "Shorty", "12 Gauge Break", WeaponCategory::Pistol, 88, 28, 35, 22, 88, 20, 1.8f);
        Add("dobvra", "Dobvra (Makarov)", "9x18mm Auto", WeaponCategory::Pistol, 30, 85, 44, 28, 91, 38, 1.2f);
    }
};

} // namespace BlackclawX

#endif // BLACKCLAWX_ARSENAL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Optic, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // in kg
    float damageMod;      // Damage bonus
    float accuracyMod;    // Accuracy bonus
    float rangeMod;       // Range bonus
    float controlMod;     // Control/Recoil bonus
    float mobilityMod;    // Mobility/ADS Speed bonus
};

// -----------------------------------------------------------------------------
// M4 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class M4GunsmithEngine {
private:
    // M4 Base Attributes (Authentic COD Mobile Base Stats)
    std::string weaponName = "M4 - Tactical Carbine";
    std::string caliber = "5.56x45mm NATO";
    
    float baseDamage = 39.0f;
    float baseFireRate = 68.0f;
    float baseAccuracy = 59.0f;
    float baseRange = 58.0f;
    float baseControl = 54.0f;
    float baseMobility = 69.0f;
    float baseWeight = 3.4f; // kg

    float maxWeightCapacity = 7.5f; // Max custom attachment capacity

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    M4GunsmithEngine() {
        // Initialize M4 Specific Attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor", "Muzzle", 0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
            {"OWC Light Suppressor",   "Muzzle", 0.2f, +0.0f, +0.0f, -5.0f,  +0.0f, +2.0f},
            
            // Barrel
            {"MIP Light Barrel",       "Barrel", 0.5f, +0.0f, -3.0f, -6.0f,  -2.0f, +8.0f},
            {"OWC Marksman Barrel",   "Barrel", 1.2f, +2.0f, +7.0f, +15.0f, +6.0f, -8.0f},
            
            // Optic
            {"Classic Red Dot Sight",  "Optic",  0.2f, +0.0f, +3.0f, +0.0f,  +0.0f, -1.0f},
            {"4x Tactical Scope",      "Optic",  0.6f, +0.0f, +6.0f, +8.0f,  -2.0f, -5.0f},
            
            // Stock
            {"No Stock",               "Stock", -0.8f, +0.0f, -6.0f, +0.0f,  -8.0f, +12.0f},
            {"MIP Strike Stock",       "Stock",  0.7f, +0.0f, +5.0f, +0.0f,  +6.0f, -4.0f},
            
            // Laser
            {"OWC Laser - Tactical",   "Laser",  0.1f, +0.0f, +4.0f, +0.0f,  +0.0f, +5.0f},
            
            // Underbarrel
            {"Merc Foregrip",          "Underbarrel", 0.5f, +0.0f, +2.0f, +0.0f, +8.0f, -3.0f},
            {"Strike Foregrip",        "Underbarrel", 0.4f, +0.0f, +4.0f, +0.0f, +3.0f, -2.0f},
            
            // Ammunition
            {"40 Round Extended Mag",  "Magazine",    0.8f, +0.0f, +0.0f, +0.0f, +0.0f, -3.0f},
            {"50 Round Extended Mag",  "Magazine",    1.4f, +0.0f, +0.0f, +0.0f, -2.0f, -6.0f},
            
            // Rear Grip
            {"Stippled Grip Tape",     "Rear Grip",   0.1f, +0.0f, -4.0f, +0.0f, -2.0f, +7.0f},
            {"Rubberized Grip Tape",   "Rear Grip",   0.2f, +0.0f, +0.0f, +0.0f, +9.0f, -2.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                   BLACKclawX :: M4 GUNSMITH HUB                        \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";
        
        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1) 
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        // Draw ASCII Weight Capacity Bar
        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        int filled = static_cast<int>(fillRatio * barWidth);
        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED ATTACHMENTS: (" << equippedAttachments.size() << " / 5)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayM4StatsAndAttachments() const {
        // Calculate Dynamic Modifiers
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME M4 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE  ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY", acc, 100.0f);
        PrintStatBar("RANGE   ", rng, 100.0f);
        PrintStatBar("CONTROL ", ctrl, 100.0f);
        PrintStatBar("MOBILITY", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE ATTACHMENTS FOR M4:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] " 
                      << std::left << std::setw(24) << att.name 
                      << " Slot: " << std::setw(12) << att.category 
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        // Check if already equipped
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it); // Unequip
        } else {
            // Validate Max 5 slots restriction
            if (equippedAttachments.size() >= 5) {
                std::cout << "\n>> ERROR: Max 5 attachments allowed on M4 Gunsmith!\n";
                return;
            }

            // Check duplicate category slot
            auto catIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (catIt != equippedAttachments.end()) {
                std::cout << "\n>> ERROR: Slot [" << targetAtt.category << "] is already occupied by " << catIt->name << "!\n";
                return;
            }

            equippedAttachments.push_back(targetAtt); // Equip
        }
    }

    void LoadPreset(int presetType) {
        equippedAttachments.clear();
        if (presetType == 1) { // Aggressive Close Range
            ToggleAttachment(2);  // MIP Light Barrel
            ToggleAttachment(6);  // No Stock
            ToggleAttachment(8);  // OWC Laser - Tactical
            ToggleAttachment(11); // 40 Round Ext Mag
            ToggleAttachment(13); // Stippled Grip Tape
        } else if (presetType == 2) { // Long Range BR / Stealth
            ToggleAttachment(0);  // Monolithic Suppressor
            ToggleAttachment(3);  // OWC Marksman Barrel
            ToggleAttachment(5);  // 4x Scope
            ToggleAttachment(7);  // MIP Strike Stock
            ToggleAttachment(10); // Strike Foregrip
        }
    }
};

// -----------------------------------------------------------------------------
// SINGLE MAIN ENTRY POINT
// -----------------------------------------------------------------------------
int main() {
    M4GunsmithEngine m4Gunsmith;
    bool isRunning = true;

    while (isRunning) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        m4Gunsmith.DisplayHeader();
        m4Gunsmith.DisplayM4StatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-15] Toggle Attachment | [P1] Load Aggressive Preset | [P2] Load Long-Range Preset | [Q] Quit\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            isRunning = false;
        } else if (input == "p1" || input == "P1") {
            m4Gunsmith.LoadPreset(1);
        } else if (input == "p2" || input == "P2") {
            m4Gunsmith.LoadPreset(2);
        } else {
            try {
                int index = std::stoi(input) - 1;
                m4Gunsmith.ToggleAttachment(index);
            } catch (...) {
                // Invalid input handling
            }
        }
    }

    std::cout << "\nClosing BLACKclawX M4 Gunsmith...\n";
    return 0;
}

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// TYPE 25 DATA STRUCTURES
// -----------------------------------------------------------------------------
struct Type25Attachment {
    std::string name;
    std::string category; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel
    float weight;         // in kg
    float damageMod;      // Flat damage change (e.g., Stopping Power)
    float accuracyMod;    // Bullet spread / ADS accuracy
    float rangeMod;       // Effective range (meters)
    float mobilityMod;    // ADS speed & strafe speed
    float controlMod;     // Recoil mitigation
};

struct Type25Stats {
    float damage;
    float fireRate;    // 857 RPM (Fastest-in-class AR)
    float accuracy;
    float range;
    float mobility;
    float control;
    float totalWeight;
};

// -----------------------------------------------------------------------------
// TYPE 25 GUNSMITH ENGINE CLASS (Modular Header / Engine Component)
// -----------------------------------------------------------------------------
class Type25GunsmithEngine {
private:
    // Core Weapon Identity
    const std::string weaponName = "Type 25 - Bullpup AR";
    const std::string caliber = "5.8x42mm DB P87";
    
    // Authentic Base Stats
    const float baseDamage = 25.0f;
    const float baseFireRate = 85.7f; // High RPM
    const float baseAccuracy = 37.0f;
    const float baseRange = 35.0f;
    const float baseMobility = 85.0f; // High close-quarters handling
    const float baseControl = 36.0f;  // Higher vertical recoil offset
    const float baseWeight = 3.2f;    // kg

    const float maxWeightCapacity = 6.5f; // Max capacity in kg
    const size_t maxAttachmentSlots = 5;

    std::vector<Type25Attachment> availableAttachments;
    std::vector<Type25Attachment> equippedAttachments;

public:
    Type25GunsmithEngine() {
        // Initialize Type 25 Specific Attachments & Modifiers
        availableAttachments = {
            // Barrel
            {"MIP Light Barrel (Short)", "Barrel", 0.3f, +0.0f, -4.0f, -6.0f, +10.0f, -3.0f},
            {"OWC Light Marksman",       "Barrel", 0.9f, +0.0f, +6.0f, +12.0f, -6.0f,  +5.0f},
            
            // Stock
            {"RTC Steady Stock",         "Stock",  0.6f, +0.0f, +8.0f, +0.0f,  -5.0f,  +9.0f},
            {"YKM Combat Stock",         "Stock",  0.2f, +0.0f, -3.0f, +0.0f,  +8.0f,  -4.0f},
            
            // Laser
            {"OWC Laser - Tactical",     "Laser",  0.1f, +0.0f, +5.0f, +0.0f,  +4.0f,  +0.0f},
            
            // Ammunition
            {"Stopping Power Reload",    "Ammunition", 0.7f, +8.0f, -2.0f, +5.0f, -3.0f,  -7.0f}, // High TTK Meta Mag
            {"42 Round Fast Reload",     "Ammunition", 0.9f, +0.0f, +0.0f, +0.0f, -4.0f,  +0.0f},
            {"38 Round Extended Mag",    "Ammunition", 0.5f, +0.0f, +0.0f, +0.0f, -2.0f,  +0.0f},
            
            // Rear Grip
            {"Granulated Grip Tape",     "Rear Grip",  0.1f, +0.0f, +7.0f, +0.0f, +0.0f,  +2.0f},
            {"Rubberized Grip Tape",     "Rear Grip",  0.2f, +0.0f, -2.0f, +0.0f, -1.0f,  +8.0f},
            
            // Muzzle
            {"Monolithic Suppressor",    "Muzzle",     0.5f, +0.0f, -2.0f, +10.0f, -4.0f,  +0.0f},
            
            // Underbarrel
            {"Strike Foregrip",          "Underbarrel", 0.4f, +0.0f, +4.0f, +0.0f, -2.0f,  +4.0f}
        };
    }

    // -------------------------------------------------------------------------
    // GETTERS & STAT CALCULATORS
    // -------------------------------------------------------------------------
    std::string GetWeaponName() const { return weaponName; }
    std::string GetCaliber() const { return caliber; }

    Type25Stats GetCalculatedStats() const {
        Type25Stats currentStats;
        currentStats.damage = baseDamage;
        currentStats.fireRate = baseFireRate;
        currentStats.accuracy = baseAccuracy;
        currentStats.range = baseRange;
        currentStats.mobility = baseMobility;
        currentStats.control = baseControl;
        currentStats.totalWeight = baseWeight;

        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.totalWeight += att.weight;
        }

        return currentStats;
    }

    const std::vector<Type25Attachment>& GetAvailableAttachments() const {
        return availableAttachments;
    }

    const std::vector<Type25Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    size_t GetEquippedCount() const { return equippedAttachments.size(); }
    size_t GetMaxSlots() const { return maxAttachmentSlots; }

    // -------------------------------------------------------------------------
    // ATTACHMENT MUTATORS
    // -------------------------------------------------------------------------
    bool EquipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(availableAttachments.begin(), availableAttachments.end(),
            [&attachmentName](const Type25Attachment& att) { return att.name == attachmentName; });

        if (it == availableAttachments.end()) return false; // Not found

        // Check if already equipped
        auto eqIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const Type25Attachment& att) { return att.name == attachmentName; });

        if (eqIt != equippedAttachments.end()) return false; // Already equipped

        // Check limits
        if (equippedAttachments.size() >= maxAttachmentSlots) return false;

        // Check category conflict (replace if same category)
        for (auto catIt = equippedAttachments.begin(); catIt != equippedAttachments.end(); ++catIt) {
            if (catIt->category == it->category) {
                equippedAttachments.erase(catIt);
                break;
            }
        }

        equippedAttachments.push_back(*it);
        return true;
    }

    bool UnequipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const Type25Attachment& att) { return att.name == attachmentName; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
            return true;
        }
        return false;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    // -------------------------------------------------------------------------
    // PRESET BUILD LOADERS
    // -------------------------------------------------------------------------
    void LoadStoppingPowerMetaBuild() {
        ClearAttachments();
        EquipAttachmentByName("RTC Steady Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("Stopping Power Reload");
        EquipAttachmentByName("Granulated Grip Tape");
        EquipAttachmentByName("Strike Foregrip");
    }

    void LoadFastADSRushBuild() {
        ClearAttachments();
        EquipAttachmentByName("MIP Light Barrel (Short)");
        EquipAttachmentByName("YKM Combat Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("42 Round Fast Reload");
        EquipAttachmentByName("Rubberized Grip Tape");
    }
};

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// TYPE 19 DATA STRUCTURES
// -----------------------------------------------------------------------------
struct Type19Attachment {
    std::string name;
    std::string category; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel, Optic
    float weight;         // in kg
    float damageMod;      // Flat damage modifier
    float accuracyMod;    // Bullet spread & stability
    float rangeMod;       // Effective damage range (meters)
    float mobilityMod;    // ADS speed, strafe speed, movement speed
    float controlMod;     // Vertical/Horizontal recoil reduction
};

struct Type19Stats {
    float damage;
    float fireRate;    // 750 RPM (Versatile, modular assault rifle)
    float accuracy;
    float range;
    float mobility;
    float control;
    float totalWeight;
};

// -----------------------------------------------------------------------------
// TYPE 19 GUNSMITH ENGINE CLASS (Modular Header Component)
// -----------------------------------------------------------------------------
class Type19GunsmithEngine {
private:
    // Core Weapon Identity
    const std::string weaponName = "Type 19 - Modular AR";
    const std::string caliber = "5.8x42mm Modern NATO/Custom";

    // Authentic Base Stats
    const float baseDamage = 28.0f;
    const float baseFireRate = 75.0f;
    const float baseAccuracy = 54.0f;
    const float baseRange = 48.0f;
    const float baseMobility = 68.0f;
    const float baseControl = 52.0f;
    const float baseWeight = 3.3f; // kg

    const float maxWeightCapacity = 7.0f; // Capacity limit in kg
    const size_t maxAttachmentSlots = 5;

    std::vector<Type19Attachment> availableAttachments;
    std::vector<Type19Attachment> equippedAttachments;

public:
    Type19GunsmithEngine() {
        // Initialize Type 19 Modular Attachments
        availableAttachments = {
            // Barrel
            {"Type 19 Rapid Barrel",     "Barrel", 0.4f, +0.0f, -3.0f, -8.0f, +9.0f, -4.0f},
            {"Type 19 Marksman Barrel",  "Barrel", 1.1f, +3.0f, +6.0f, +14.0f, -7.0f, +6.0f},
            {"Type 19 Sniper Barrel",    "Barrel", 1.4f, +5.0f, +8.0f, +20.0f, -12.0f, +8.0f},

            // Stock
            {"Lightweight Skeleton Stock","Stock",  0.3f, +0.0f, -2.0f, +0.0f,  +7.0f, -3.0f},
            {"Tactical Precision Stock",  "Stock",  0.7f, +0.0f, +5.0f, +0.0f,  -4.0f, +8.0f},

            // Laser
            {"Tactical 5mW Laser",       "Laser",  0.1f, +0.0f, +4.0f, +0.0f,  +5.0f, +0.0f},

            // Ammunition
            {"Type 19 High-Caliber Mag", "Ammunition", 0.6f, +6.0f, +0.0f, +5.0f, -3.0f, -4.0f},
            {"45 Round Polymer Mag",     "Ammunition", 0.8f, +0.0f, +0.0f, +0.0f, -4.0f, +0.0f},
            {"60 Round Drum Mag",        "Ammunition", 1.5f, +0.0f, +0.0f, +0.0f, -8.0f, -3.0f},

            // Rear Grip
            {"Stippled Polymer Grip",    "Rear Grip",  0.1f, +0.0f, -3.0f, +0.0f, +6.0f, -2.0f},
            {"Rubberized Control Grip",  "Rear Grip",  0.2f, +0.0f, +2.0f, +0.0f, -1.0f, +7.0f},

            // Muzzle
            {"Monolithic Suppressor",    "Muzzle",     0.5f, +0.0f, -2.0f, +12.0f, -4.0f, +0.0f},
            {"Tactical Compensator",     "Muzzle",     0.4f, +0.0f, +3.0f, +0.0f,  -2.0f, +8.0f},

            // Underbarrel
            {"Angled Foregrip",          "Underbarrel", 0.3f, +0.0f, +4.0f, +0.0f, +2.0f, +3.0f},
            {"Operator Heavy Foregrip",  "Underbarrel", 0.6f, +0.0f, +2.0f, +0.0f, -4.0f, +9.0f}
        };
    }

    // -------------------------------------------------------------------------
    // GETTERS & STAT CALCULATORS
    // -------------------------------------------------------------------------
    std::string GetWeaponName() const { return weaponName; }
    std::string GetCaliber() const { return caliber; }

    Type19Stats GetCalculatedStats() const {
        Type19Stats currentStats;
        currentStats.damage = baseDamage;
        currentStats.fireRate = baseFireRate;
        currentStats.accuracy = baseAccuracy;
        currentStats.range = baseRange;
        currentStats.mobility = baseMobility;
        currentStats.control = baseControl;
        currentStats.totalWeight = baseWeight;

        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.totalWeight += att.weight;
        }

        return currentStats;
    }

    const std::vector<Type19Attachment>& GetAvailableAttachments() const {
        return availableAttachments;
    }

    const std::vector<Type19Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    size_t GetEquippedCount() const { return equippedAttachments.size(); }
    size_t GetMaxSlots() const { return maxAttachmentSlots; }

    // -------------------------------------------------------------------------
    // ATTACHMENT MUTATORS
    // -------------------------------------------------------------------------
    bool EquipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(availableAttachments.begin(), availableAttachments.end(),
            [&attachmentName](const Type19Attachment& att) { return att.name == attachmentName; });

        if (it == availableAttachments.end()) return false;

        auto eqIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const Type19Attachment& att) { return att.name == attachmentName; });

        if (eqIt != equippedAttachments.end()) return false;

        if (equippedAttachments.size() >= maxAttachmentSlots) return false;

        // Replace if slot category is occupied
        for (auto catIt = equippedAttachments.begin(); catIt != equippedAttachments.end(); ++catIt) {
            if (catIt->category == it->category) {
                equippedAttachments.erase(catIt);
                break;
            }
        }

        equippedAttachments.push_back(*it);
        return true;
    }

    bool UnequipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const Type19Attachment& att) { return att.name == attachmentName; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
            return true;
        }
        return false;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    // -------------------------------------------------------------------------
    // PRESET BUILD LOADERS
    // -------------------------------------------------------------------------
    void LoadVersatileMetaBuild() {
        ClearAttachments();
        EquipAttachmentByName("Type 19 Marksman Barrel");
        EquipAttachmentByName("Monolithic Suppressor");
        EquipAttachmentByName("Tactical 5mW Laser");
        EquipAttachmentByName("Type 19 High-Caliber Mag");
        EquipAttachmentByName("Angled Foregrip");
    }

    void LoadSniperSupportBuild() {
        ClearAttachments();
        EquipAttachmentByName("Type 19 Sniper Barrel");
        EquipAttachmentByName("Tactical Precision Stock");
        EquipAttachmentByName("Type 19 High-Caliber Mag");
        EquipAttachmentByName("Operator Heavy Foregrip");
        EquipAttachmentByName("Rubberized Control Grip");
    }
};

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// ICR-1 DATA STRUCTURES
// -----------------------------------------------------------------------------
struct ICR1Attachment {
    std::string name;
    std::string category; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel, Optic
    float weight;         // in kg
    float damageMod;      // Damage bonus
    float accuracyMod;    // Accuracy & bullet spread reduction
    float rangeMod;       // Effective damage range (meters)
    float mobilityMod;    // ADS speed & sprint speed
    float controlMod;     // Recoil mitigation
};

struct ICR1Stats {
    float damage;
    float fireRate;    // 600 RPM (Laser-accurate, steady fire rate)
    float accuracy;
    float range;
    float mobility;
    float control;
    float totalWeight;
};

// -----------------------------------------------------------------------------
// ICR-1 GUNSMITH ENGINE CLASS (Modular Header Component)
// -----------------------------------------------------------------------------
class ICR1GunsmithEngine {
private:
    // Core Weapon Identity
    const std::string weaponName = "ICR-1 - Precision AR";
    const std::string caliber = "5.56x45mm Lightweight";

    // Authentic Base Stats (Known for near-zero base recoil & high accuracy)
    const float baseDamage = 30.0f;
    const float baseFireRate = 60.0f;
    const float baseAccuracy = 69.0f; // High base precision
    const float baseRange = 52.0f;
    const float baseMobility = 64.0f;
    const float baseControl = 78.0f;  // Top-tier base stability
    const float baseWeight = 3.1f;    // kg

    const float maxWeightCapacity = 6.8f; // Capacity limit in kg
    const size_t maxAttachmentSlots = 5;

    std::vector<ICR1Attachment> availableAttachments;
    std::vector<ICR1Attachment> equippedAttachments;

public:
    ICR1GunsmithEngine() {
        // Initialize ICR-1 Attachments
        availableAttachments = {
            // Barrel
            {"YKM Integral Suppressor Light", "Barrel", 0.4f, +0.0f, +2.0f, -4.0f, +7.0f, +0.0f},
            {"OWC Ranger Barrel",             "Barrel", 1.2f, +2.0f, +8.0f, +15.0f, -8.0f, +6.0f},
            {"MIP Light Barrel (Short)",      "Barrel", 0.3f, +0.0f, -4.0f, -6.0f, +10.0f, -3.0f},

            // Stock
            {"YKM Combat Stock",              "Stock",  0.2f, +0.0f, -3.0f, +0.0f,  +8.0f, -4.0f},
            {"RTC Steady Stock",              "Stock",  0.6f, +0.0f, +7.0f, +0.0f,  -4.0f, +8.0f},
            {"No Stock",                      "Stock", -0.7f, +0.0f, -6.0f, +0.0f, +12.0f, -9.0f},

            // Laser
            {"OWC Laser - Tactical",          "Laser",  0.1f, +0.0f, +5.0f, +0.0f,  +4.0f, +0.0f},

            // Ammunition
            {"50 Round Fast Reload",          "Ammunition", 0.9f, +0.0f, +0.0f, +0.0f, -4.0f, +0.0f},
            {"40 Round Fast Reload",          "Ammunition", 0.5f, +0.0f, +0.0f, +0.0f, -2.0f, +0.0f},

            // Rear Grip
            {"Granulated Grip Tape",          "Rear Grip",  0.1f, +0.0f, +6.0f, +0.0f, +0.0f, +2.0f},
            {"Stippled Grip Tape",            "Rear Grip",  0.1f, +0.0f, -3.0f, +0.0f, +6.0f, -2.0f},

            // Muzzle
            {"Monolithic Suppressor",         "Muzzle",     0.5f, +0.0f, -2.0f, +12.0f, -4.0f, +0.0f},
            {"Tactical Suppressor",           "Muzzle",     0.3f, +0.0f, +0.0f, +0.0f,  -2.0f, +0.0f},

            // Underbarrel
            {"Strike Foregrip",               "Underbarrel", 0.4f, +0.0f, +4.0f, +0.0f, -2.0f, +4.0f},
            {"Tactical Foregrip A",          "Underbarrel", 0.3f, +0.0f, +5.0f, +0.0f, +0.0f, +1.0f}
        };
    }
    // -------------------------------------------------------------------------
    // GETTERS & STAT CALCULATORS
    // -------------------------------------------------------------------------
    std::string GetWeaponName() const { return weaponName; }
    std::string GetCaliber() const { return caliber; }

    ICR1Stats GetCalculatedStats() const {
        ICR1Stats currentStats;
        currentStats.damage = baseDamage;
        currentStats.fireRate = baseFireRate;
        currentStats.accuracy = baseAccuracy;
        currentStats.range = baseRange;
        currentStats.mobility = baseMobility;
        currentStats.control = baseControl;
        currentStats.totalWeight = baseWeight;

        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.totalWeight += att.weight;
        }

        return currentStats;
    }

    const std::vector<ICR1Attachment>& GetAvailableAttachments() const {
        return availableAttachments;
    }

    const std::vector<ICR1Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    size_t GetEquippedCount() const { return equippedAttachments.size(); }
    size_t GetMaxSlots() const { return maxAttachmentSlots; }

    // -------------------------------------------------------------------------
    // ATTACHMENT MUTATORS
    // -------------------------------------------------------------------------
    bool EquipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(availableAttachments.begin(), availableAttachments.end(),
            [&attachmentName](const ICR1Attachment& att) { return att.name == attachmentName; });

        if (it == availableAttachments.end()) return false;

        auto eqIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const ICR1Attachment& att) { return att.name == attachmentName; });

        if (eqIt != equippedAttachments.end()) return false;

        if (equippedAttachments.size() >= maxAttachmentSlots) return false;

        // Replace if slot category is occupied
        for (auto catIt = equippedAttachments.begin(); catIt != equippedAttachments.end(); ++catIt) {
            if (catIt->category == it->category) {
                equippedAttachments.erase(catIt);
                break;
            }
        }

        equippedAttachments.push_back(*it);
        return true;
    }

    bool UnequipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const ICR1Attachment& att) { return att.name == attachmentName; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
            return true;
        }
        return false;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    // -------------------------------------------------------------------------
    // PRESET BUILD LOADERS
    // -------------------------------------------------------------------------
    void LoadZeroRecoilBeamBuild() {
        ClearAttachments();
        EquipAttachmentByName("OWC Ranger Barrel");
        EquipAttachmentByName("RTC Steady Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("Granulated Grip Tape");
        EquipAttachmentByName("50 Round Fast Reload");
    }

    void LoadAggressiveMobilityBuild() {
        ClearAttachments();
        EquipAttachmentByName("YKM Integral Suppressor Light");
        EquipAttachmentByName("No Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("40 Round Fast Reload");
        EquipAttachmentByName("Stippled Grip Tape");
    }
};

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// HVK-30 DATA STRUCTURES
// -----------------------------------------------------------------------------
struct HVK30Attachment {
    std::string name;
    std::string category; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel, Optic
    float weight;         // in kg
    float damageMod;      // Flat damage bonus
    float accuracyMod;    // Bullet spread & stability
    float rangeMod;       // Effective damage range (meters)
    float mobilityMod;    // ADS speed, strafe speed, movement speed
    float controlMod;     // Recoil mitigation
};

struct HVK30Stats {
    float damage;
    float fireRate;    // 710 RPM (Rapid mid-range AR)
    float accuracy;
    float range;
    float mobility;
    float control;
    float totalWeight;
};

// -----------------------------------------------------------------------------
// HVK-30 GUNSMITH ENGINE CLASS (Modular Header Component)
// -----------------------------------------------------------------------------
class HVK30GunsmithEngine {
private:
    // Core Weapon Identity
    const std::string weaponName = "HVK-30 - Rapid AR";
    const std::string caliber = "5.56x45mm NATO / High-Pressure";

    // Authentic Base Stats
    const float baseDamage = 35.0f;
    const float baseFireRate = 71.0f;
    const float baseAccuracy = 55.0f;
    const float baseRange = 50.0f;
    const float baseMobility = 66.0f;
    const float baseControl = 58.0f;
    const float baseWeight = 3.25f; // kg

    const float maxWeightCapacity = 6.8f; // Capacity limit in kg
    const size_t maxAttachmentSlots = 5;

    std::vector<HVK30Attachment> availableAttachments;
    std::vector<HVK30Attachment> equippedAttachments;

public:
    HVK30GunsmithEngine() {
        // Initialize HVK-30 Attachments
        availableAttachments = {
            // Barrel
            {"OWC Marksman Barrel",          "Barrel", 1.1f, +2.0f, +6.0f, +12.0f, -6.0f, +5.0f},
            {"MIP Light Barrel (Short)",      "Barrel", 0.3f, +0.0f, -4.0f, -6.0f, +9.0f, -3.0f},
            {"YKM Integrated Suppressor",    "Barrel", 0.5f, +0.0f, +1.0f, -3.0f, +6.0f, +0.0f},

            // Stock
            {"MIP Strike Stock",             "Stock",  0.6f, +0.0f, +5.0f, +0.0f,  -3.0f, +7.0f},
            {"YKM Combat Stock",             "Stock",  0.2f, +0.0f, -3.0f, +0.0f,  +7.0f, -4.0f},
            {"No Stock",                     "Stock", -0.7f, +0.0f, -6.0f, +0.0f, +11.0f, -8.0f},

            // Laser
            {"OWC Laser - Tactical",         "Laser",  0.1f, +0.0f, +4.0f, +0.0f,  +4.0f, +0.0f},

            // Ammunition (Large Caliber Ammo turns HVK-30 into a top-tier TTK meta)
            {"Large Caliber Ammo",           "Ammunition", 0.6f, +10.0f, -2.0f, +5.0f, -2.0f, -4.0f},
            {"44 Round Extended Mag",        "Ammunition", 0.8f, +0.0f, +0.0f, +0.0f, -4.0f, +0.0f},
            {"Fast Reload Mag",              "Ammunition", 0.3f, +0.0f, +0.0f, +0.0f, -1.0f, +0.0f},

            // Rear Grip
            {"Granulated Grip Tape",         "Rear Grip",  0.1f, +0.0f, +6.0f, +0.0f, +0.0f, +2.0f},
            {"Stippled Grip Tape",           "Rear Grip",  0.1f, +0.0f, -3.0f, +0.0f, +6.0f, -2.0f},

            // Muzzle
            {"Monolithic Suppressor",        "Muzzle",     0.5f, +0.0f, -2.0f, +10.0f, -4.0f, +0.0f},
            {"Tactical Suppressor",          "Muzzle",     0.3f, +0.0f, +0.0f, +0.0f,  -2.0f, +0.0f},

            // Underbarrel
            {"Strike Foregrip",              "Underbarrel", 0.4f, +0.0f, +4.0f, +0.0f, -2.0f, +4.0f},
            {"Ranger Foregrip",              "Underbarrel", 0.7f, +0.0f, +3.0f, +0.0f, -5.0f, +8.0f}
        };
    }

    // -------------------------------------------------------------------------
    // GETTERS & STAT CALCULATORS
    // -------------------------------------------------------------------------
    std::string GetWeaponName() const { return weaponName; }
    std::string GetCaliber() const { return caliber; }

    HVK30Stats GetCalculatedStats() const {
        HVK30Stats currentStats;
        currentStats.damage = baseDamage;
        currentStats.fireRate = baseFireRate;
        currentStats.accuracy = baseAccuracy;
        currentStats.range = baseRange;
        currentStats.mobility = baseMobility;
        currentStats.control = baseControl;
        currentStats.totalWeight = baseWeight;

        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.totalWeight += att.weight;
        }

        return currentStats;
    }

    const std::vector<HVK30Attachment>& GetAvailableAttachments() const {
        return availableAttachments;
    }

    const std::vector<HVK30Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    size_t GetEquippedCount() const { return equippedAttachments.size(); }
    size_t GetMaxSlots() const { return maxAttachmentSlots; }

    // -------------------------------------------------------------------------
    // ATTACHMENT MUTATORS
    // -------------------------------------------------------------------------
    bool EquipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(availableAttachments.begin(), availableAttachments.end(),
            [&attachmentName](const HVK30Attachment& att) { return att.name == attachmentName; });

        if (it == availableAttachments.end()) return false;

        auto eqIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const HVK30Attachment& att) { return att.name == attachmentName; });

        if (eqIt != equippedAttachments.end()) return false;

        if (equippedAttachments.size() >= maxAttachmentSlots) return false;

        // Replace if slot category is occupied
        for (auto catIt = equippedAttachments.begin(); catIt != equippedAttachments.end(); ++catIt) {
            if (catIt->category == it->category) {
                equippedAttachments.erase(catIt);
                break;
            }
        }

        equippedAttachments.push_back(*it);
        return true;
    }

    bool UnequipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const HVK30Attachment& att) { return att.name == attachmentName; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
            return true;
        }
        return false;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    // -------------------------------------------------------------------------
    // PRESET BUILD LOADERS
    // -------------------------------------------------------------------------
    void LoadLargeCaliberMetaBuild() {
        ClearAttachments();
        EquipAttachmentByName("OWC Marksman Barrel");
        EquipAttachmentByName("MIP Strike Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("Large Caliber Ammo");
        EquipAttachmentByName("Granulated Grip Tape");
    }

    void LoadAggressiveCQBBuild() {
        ClearAttachments();
        EquipAttachmentByName("MIP Light Barrel (Short)");
        EquipAttachmentByName("No Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("Large Caliber Ammo");
        EquipAttachmentByName("Stippled Grip Tape");
    }
};

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// AK117 DATA STRUCTURES
// -----------------------------------------------------------------------------
struct AK117Attachment {
    std::string name;
    std::string category; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel, Optic
    float weight;         // in kg
    float damageMod;      // Flat damage bonus
    float accuracyMod;    // Bullet spread & stability
    float rangeMod;       // Effective damage range (meters)
    float mobilityMod;    // ADS speed, strafe speed, movement speed
    float controlMod;     // Recoil mitigation
};

struct AK117Stats {
    float damage;
    float fireRate;    // 770 RPM (Rapid-fire aggressive AR)
    float accuracy;
    float range;
    float mobility;
    float control;
    float totalWeight;
};

// -----------------------------------------------------------------------------
// AK117 GUNSMITH ENGINE CLASS (Modular Header Component)
// -----------------------------------------------------------------------------
class AK117GunsmithEngine {
private:
    // Core Weapon Identity
    const std::string weaponName = "AK117 - High RPM AR";
    const std::string caliber = "5.45x39mm High-Velocity";

    // Authentic Base Stats
    const float baseDamage = 32.0f;
    const float baseFireRate = 77.0f;
    const float baseAccuracy = 50.0f;
    const float baseRange = 45.0f;
    const float baseMobility = 67.0f;
    const float baseControl = 46.0f; // High horizontal recoil kick requires control attachments
    const float baseWeight = 3.2f;  // kg

    const float maxWeightCapacity = 6.9f; // Capacity limit in kg
    const size_t maxAttachmentSlots = 5;

    std::vector<AK117Attachment> availableAttachments;
    std::vector<AK117Attachment> equippedAttachments;

public:
    AK117GunsmithEngine() {
        // Initialize AK117 Attachments
        availableAttachments = {
            // Barrel
            {"OWC Marksman Barrel",           "Barrel", 1.2f, +2.0f, +7.0f, +15.0f, -8.0f, +6.0f},
            {"MIP Extended Light Barrel",     "Barrel", 0.6f, +0.0f, +2.0f, +8.0f,  -3.0f, +1.0f},
            {"RTC Light Short Barrel",        "Barrel", 0.3f, +0.0f, -4.0f, -6.0f,  +9.0f, -3.0f},

            // Stock
            {"No Stock",                      "Stock", -0.8f, +0.0f, -6.0f, +0.0f, +12.0f, -8.0f},
            {"YKM Combat Stock",              "Stock",  0.2f, +0.0f, -3.0f, +0.0f,  +7.0f, -3.0f},
            {"RTC Steady Stock",              "Stock",  0.7f, +0.0f, +6.0f, +0.0f,  -4.0f, +8.0f},

            // Laser
            {"OWC Laser - Tactical",          "Laser",  0.1f, +0.0f, +4.0f, +0.0f,  +4.0f, +0.0f},

            // Ammunition
            {"48 Round Extended Mag",         "Ammunition", 0.7f, +0.0f, +0.0f, +0.0f, -3.0f, +0.0f},
            {"40 Round Extended Mag",         "Ammunition", 0.4f, +0.0f, +0.0f, +0.0f, -1.0f, +0.0f},
            {"50 Round Reload Mag",           "Ammunition", 0.9f, +0.0f, +0.0f, +0.0f, -5.0f, -1.0f},

            // Rear Grip
            {"Granulated Grip Tape",          "Rear Grip",  0.1f, +0.0f, +6.0f, +0.0f, +0.0f, +2.0f},
            {"Rubberized Grip Tape",          "Rear Grip",  0.2f, +0.0f, -2.0f, +0.0f, -1.0f, +8.0f},
            {"Stippled Grip Tape",            "Rear Grip",  0.1f, +0.0f, -3.0f, +0.0f, +5.0f, -2.0f},

            // Muzzle
            {"Monolithic Suppressor",         "Muzzle",     0.5f, +0.0f, -2.0f, +12.0f, -4.0f, +0.0f},
            {"OWC Light Compensator",         "Muzzle",     0.4f, +0.0f, +3.0f, +0.0f,  -2.0f, +7.0f},

            // Underbarrel
            {"Strike Foregrip",               "Underbarrel", 0.4f, +0.0f, +4.0f, +0.0f, -2.0f, +3.0f},
            {"Operator Foregrip",             "Underbarrel", 0.6f, +0.0f, +1.0f, +0.0f, -4.0f, +8.0f}
        };
    }

    // -------------------------------------------------------------------------
    // GETTERS & STAT CALCULATORS
    // -------------------------------------------------------------------------
    std::string GetWeaponName() const { return weaponName; }
    std::string GetCaliber() const { return caliber; }

    AK117Stats GetCalculatedStats() const {
        AK117Stats currentStats;
        currentStats.damage = baseDamage;
        currentStats.fireRate = baseFireRate;
        currentStats.accuracy = baseAccuracy;
        currentStats.range = baseRange;
        currentStats.mobility = baseMobility;
        currentStats.control = baseControl;
        currentStats.totalWeight = baseWeight;

        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.totalWeight += att.weight;
        }

        return currentStats;
    }

    const std::vector<AK117Attachment>& GetAvailableAttachments() const {
        return availableAttachments;
    }

    const std::vector<AK117Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    size_t GetEquippedCount() const { return equippedAttachments.size(); }
    size_t GetMaxSlots() const { return maxAttachmentSlots; }

    // -------------------------------------------------------------------------
    // ATTACHMENT MUTATORS
    // -------------------------------------------------------------------------
    bool EquipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(availableAttachments.begin(), availableAttachments.end(),
            [&attachmentName](const AK117Attachment& att) { return att.name == attachmentName; });

        if (it == availableAttachments.end()) return false;

        auto eqIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const AK117Attachment& att) { return att.name == attachmentName; });

        if (eqIt != equippedAttachments.end()) return false;

        if (equippedAttachments.size() >= maxAttachmentSlots) return false;

        // Replace if slot category is occupied
        for (auto catIt = equippedAttachments.begin(); catIt != equippedAttachments.end(); ++catIt) {
            if (catIt->category == it->category) {
                equippedAttachments.erase(catIt);
                break;
            }
        }

        equippedAttachments.push_back(*it);
        return true;
    }

    bool UnequipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const AK117Attachment& att) { return att.name == attachmentName; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
            return true;
        }
        return false;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    // -------------------------------------------------------------------------
    // PRESET BUILD LOADERS
    // -------------------------------------------------------------------------
    void LoadBattleRoyaleMetaBuild() {
        ClearAttachments();
        EquipAttachmentByName("OWC Marksman Barrel");
        EquipAttachmentByName("Monolithic Suppressor");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("48 Round Extended Mag");
        EquipAttachmentByName("Granulated Grip Tape");
    }

    void LoadAggressiveMultiplayerBuild() {
        ClearAttachments();
        EquipAttachmentByName("RTC Light Short Barrel");
        EquipAttachmentByName("No Stock");
        EquipAttachmentByName("OWC Laser - Tactical");
        EquipAttachmentByName("40 Round Extended Mag");
        EquipAttachmentByName("Strike Foregrip");
    }
};

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// -----------------------------------------------------------------------------
// GRAU 5.56 DATA STRUCTURES
// -----------------------------------------------------------------------------
struct Grau556Attachment {
    std::string name;
    std::string category; // Barrel, Stock, Laser, Ammunition, Rear Grip, Muzzle, Underbarrel, Optic
    float weight;         // in kg
    float damageMod;      // Flat damage bonus
    float accuracyMod;    // Bullet spread & stability
    float rangeMod;       // Effective damage range (meters)
    float mobilityMod;    // ADS speed, strafe speed, movement speed
    float controlMod;     // Recoil mitigation
};

struct Grau556Stats {
    float damage;
    float fireRate;    // 730 RPM (Reliable, easy-to-control fire rate)
    float accuracy;
    float range;
    float mobility;
    float control;
    float totalWeight;
};

// -----------------------------------------------------------------------------
// GRAU 5.56 GUNSMITH ENGINE CLASS (Modular Header Component)
// -----------------------------------------------------------------------------
class Grau556GunsmithEngine {
private:
    // Core Weapon Identity
    const std::string weaponName = "Grau 5.56 - Lightweight AR";
    const std::string caliber = "5.56x45mm NATO";

    // Authentic Base Stats
    const float baseDamage = 28.0f;
    const float baseFireRate = 73.0f;
    const float baseAccuracy = 65.0f;
    const float baseRange = 55.0f; // Excellent base range
    const float baseMobility = 66.0f;
    const float baseControl = 60.0f;
    const float baseWeight = 3.1f;  // kg

    const float maxWeightCapacity = 7.0f; // Capacity limit in kg
    const size_t maxAttachmentSlots = 5;

    std::vector<Grau556Attachment> availableAttachments;
    std::vector<Grau556Attachment> equippedAttachments;

public:
    Grau556GunsmithEngine() {
        // Initialize Grau 5.56 Attachments
        availableAttachments = {
            // Barrel (Grau's defining feature - changes the iron sights)
            {"Tempus 26.4\" Archangel",       "Barrel", 1.4f, +0.0f, +8.0f, +20.0f, -9.0f, +7.0f},
            {"FSS 20.8\" Nexus",              "Barrel", 1.1f, +0.0f, +5.0f, +15.0f, -6.0f, +5.0f},
            {"ZLR Drifter A-08",              "Barrel", 0.4f, +0.0f, -4.0f, -8.0f, +12.0f, -3.0f},

            // Stock
            {"No Stock",                      "Stock", -0.7f, +0.0f, -5.0f, +0.0f, +11.0f, -7.0f},
            {"XRK StrikeLite III",            "Stock",  0.2f, +0.0f, -2.0f, +0.0f,  +6.0f, -2.0f},
            {"FSS Blackjack",                 "Stock",  0.6f, +0.0f, +6.0f, +0.0f,  -4.0f, +7.0f},

            // Laser
            {"Tac Laser",                     "Laser",  0.1f, +0.0f, +4.0f, +0.0f,  +6.0f, +0.0f},

            // Ammunition
            {"50 Round Mags",                 "Ammunition", 0.6f, +0.0f, +0.0f, +0.0f, -3.0f, +0.0f},
            {"60 Round Mags",                 "Ammunition", 0.9f, +0.0f, +0.0f, +0.0f, -6.0f, -1.0f},

            // Rear Grip
            {"XRK Void II",                   "Rear Grip",  0.1f, +0.0f, -3.0f, +0.0f, +7.0f, -2.0f},
            {"Cronen Sniper Elite",           "Rear Grip",  0.2f, +0.0f, +5.0f, +0.0f, -2.0f, +4.0f},
            {"FTAC Rubber",                   "Rear Grip",  0.2f, +0.0f, +0.0f, +0.0f, -1.0f, +5.0f},

            // Muzzle
            {"Monolithic Suppressor",         "Muzzle",     0.5f, +0.0f, -2.0f, +12.0f, -5.0f, +0.0f},
            {"Tactical Suppressor",           "Muzzle",     0.3f, +0.0f, +0.0f, +0.0f,  -2.0f, +0.0f},

            // Underbarrel
            {"Commando Foregrip",             "Underbarrel", 0.4f, +0.0f, +5.0f, +0.0f, -2.0f, +3.0f},
            {"Ranger Foregrip",               "Underbarrel", 0.6f, +0.0f, +3.0f, +0.0f, -5.0f, +7.0f}
        };
    }

    // -------------------------------------------------------------------------
    // GETTERS & STAT CALCULATORS
    // -------------------------------------------------------------------------
    std::string GetWeaponName() const { return weaponName; }
    std::string GetCaliber() const { return caliber; }

    Grau556Stats GetCalculatedStats() const {
        Grau556Stats currentStats;
        currentStats.damage = baseDamage;
        currentStats.fireRate = baseFireRate;
        currentStats.accuracy = baseAccuracy;
        currentStats.range = baseRange;
        currentStats.mobility = baseMobility;
        currentStats.control = baseControl;
        currentStats.totalWeight = baseWeight;

        for (const auto& att : equippedAttachments) {
            currentStats.damage += att.damageMod;
            currentStats.accuracy += att.accuracyMod;
            currentStats.range += att.rangeMod;
            currentStats.mobility += att.mobilityMod;
            currentStats.control += att.controlMod;
            currentStats.totalWeight += att.weight;
        }

        return currentStats;
    }

    const std::vector<Grau556Attachment>& GetAvailableAttachments() const {
        return availableAttachments;
    }

    const std::vector<Grau556Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    size_t GetEquippedCount() const { return equippedAttachments.size(); }
    size_t GetMaxSlots() const { return maxAttachmentSlots; }

    // -------------------------------------------------------------------------
    // ATTACHMENT MUTATORS
    // -------------------------------------------------------------------------
    bool EquipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(availableAttachments.begin(), availableAttachments.end(),
            [&attachmentName](const Grau556Attachment& att) { return att.name == attachmentName; });

        if (it == availableAttachments.end()) return false;

        auto eqIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const Grau556Attachment& att) { return att.name == attachmentName; });

        if (eqIt != equippedAttachments.end()) return false;

        if (equippedAttachments.size() >= maxAttachmentSlots) return false;

        // Replace if slot category is occupied
        for (auto catIt = equippedAttachments.begin(); catIt != equippedAttachments.end(); ++catIt) {
            if (catIt->category == it->category) {
                equippedAttachments.erase(catIt);
                break;
            }
        }

        equippedAttachments.push_back(*it);
        return true;
    }

    bool UnequipAttachmentByName(const std::string& attachmentName) {
        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&attachmentName](const Grau556Attachment& att) { return att.name == attachmentName; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
            return true;
        }
        return false;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    // -------------------------------------------------------------------------
    // PRESET BUILD LOADERS
    // -------------------------------------------------------------------------
    void LoadWarzoneMetaBuild() {
        // The classic, extremely popular long-range beam build
        ClearAttachments();
        EquipAttachmentByName("Monolithic Suppressor");
        EquipAttachmentByName("Tempus 26.4\" Archangel");
        EquipAttachmentByName("Tac Laser");
        EquipAttachmentByName("Commando Foregrip");
        EquipAttachmentByName("60 Round Mags");
    }

    void LoadAggressiveCQBBuild() {
        // High mobility build for close quarters
        ClearAttachments();
        EquipAttachmentByName("ZLR Drifter A-08");
        EquipAttachmentByName("No Stock");
        EquipAttachmentByName("Tac Laser");
        EquipAttachmentByName("50 Round Mags");
        EquipAttachmentByName("XRK Void II");
    }
};

#ifndef KRIG6_WEAPON_DESIGN_HPP
#define KRIG6_WEAPON_DESIGN_HPP

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace BlackclawX {

// -----------------------------------------------------------------------------
// DATA STRUCTURES
// -----------------------------------------------------------------------------
struct Krig6Stats {
    float damage          = 30.0f; // Base damage per shot
    float fireRate        = 65.0f; // Rounds per minute representation
    float accuracy        = 62.0f; // Base hit accuracy
    float range           = 56.0f; // Effective range in meters
    float mobility        = 67.0f; // Movement/ADS speed factor
    float control         = 58.0f; // Recoil stability
    float weight          = 3.3f;  // Mass in kg
    float headshotMultiplier = 1.4f;
    int magazineCapacity   = 30;
};

struct Krig6Attachment {
    std::string name;
    std::string slot; // Muzzle, Barrel, Optic, Underbarrel, Ammunition, Rear Grip, Stock
    float damageMod   = 0.0f;
    float accuracyMod = 0.0f;
    float rangeMod    = 0.0f;
    float mobilityMod = 0.0f;
    float controlMod  = 0.0f;
    int extraCapacity = 0;
};

// -----------------------------------------------------------------------------
// KRIG 6 CLASS DESIGN
// -----------------------------------------------------------------------------
class Krig6 {
private:
    std::string weaponId = "krig6";
    std::string displayName = "Krig 6";
    std::string caliber = "5.56x45mm NATO";
    Krig6Stats baseStats;
    std::vector<Krig6Attachment> equippedAttachments;
    static constexpr size_t MAX_ATTACHMENTS = 5;

public:
    Krig6() = default;

    // Getters
    std::string GetID() const { return weaponId; }
    std::string GetName() const { return displayName; }
    std::string GetCaliber() const { return caliber; }
    
    // Calculates total dynamic stats based on equipped attachments
    Krig6Stats GetFinalStats() const {
        Krig6Stats calculated = baseStats;
        for (const auto& att : equippedAttachments) {
            calculated.damage += att.damageMod;
            calculated.accuracy += att.accuracyMod;
            calculated.range += att.rangeMod;
            calculated.mobility += att.mobilityMod;
            calculated.control += att.controlMod;
            calculated.magazineCapacity += att.extraCapacity;
        }
        return calculated;
    }

    // Attachment System Mechanics
    bool Attach(const Krig6Attachment& attachment) {
        if (equippedAttachments.size() >= MAX_ATTACHMENTS) {
            return false;
        }

        // Remove existing attachment in the same slot if present
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&attachment](const Krig6Attachment& a) { return a.slot == attachment.slot; }),
            equippedAttachments.end()
        );

        equippedAttachments.push_back(attachment);
        return true;
    }

    void DetachSlot(const std::string& slotName) {
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&slotName](const Krig6Attachment& a) { return a.slot == slotName; }),
            equippedAttachments.end()
        );
    }

    void ResetToStock() {
        equippedAttachments.clear();
    }

    const std::vector<Krig6Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    // Damage Drop-off Calculator over distance
    float CalculateDamageAtDistance(float distanceMeters) const {
        Krig6Stats current = GetFinalStats();
        float effectiveRange = current.range;

        if (distanceMeters <= effectiveRange) {
            return current.damage; // Full damage within effective range
        } else if (distanceMeters <= effectiveRange * 1.5f) {
            return current.damage * 0.85f; // 15% drop-off
        } else {
            return current.damage * 0.65f; // 35% drop-off at long range
        }
    }
};

// -----------------------------------------------------------------------------
// PRESET BLUEPRINT FACTORY FOR KRIG 6
// -----------------------------------------------------------------------------
class Krig6BlueprintFactory {
public:
    // Builds a long-range meta loadout for Warzone/Battle Royale
    static Krig6 CreateMetaLongRange() {
        Krig6 rifle;
        rifle.Attach({"Agency Suppressor", "Muzzle", 0.0f, -2.0f, 10.0f, -3.0f, 4.0f, 0});
        rifle.Attach({"15.0\" CMV Mil-Spec", "Barrel", 0.0f, 5.0f, 12.0f, -4.0f, 0.0f, 0});
        rifle.Attach({"Field Agent Grip", "Underbarrel", 0.0f, 4.0f, 0.0f, -2.0f, 8.0f, 0});
        rifle.Attach({"45 Rnd Magazine", "Ammunition", 0.0f, 0.0f, 0.0f, -3.0f, 0.0f, 15});
        rifle.Attach({"Axial Arms 3x", "Optic", 0.0f, 3.0f, 2.0f, -2.0f, 0.0f, 0});
        return rifle;
    }

    // Builds an aggressive high-mobility Multiplayer loadout
    static Krig6 CreateAgroMultiplayer() {
        Krig6 rifle;
        rifle.Attach({"Infantry Compensator", "Muzzle", 0.0f, 2.0f, 0.0f, -1.0f, 6.0f, 0});
        rifle.Attach({"Airborne Elastic Wrap", "Rear Grip", 0.0f, 3.0f, 0.0f, 7.0f, 0.0f, 0});
        rifle.Attach({"Raider Stock", "Stock", 0.0f, -2.0f, 0.0f, 8.0f, -3.0f, 0});
        rifle.Attach({"Fast Mag", "Ammunition", 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 10});
        return rifle;
    }
};

} // namespace BlackclawX

#endif // KRIG6_WEAPON_DESIGN_HPP

#ifndef LK24_WEAPON_DESIGN_HPP
#define LK24_WEAPON_DESIGN_HPP

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace BlackclawX {

// -----------------------------------------------------------------------------
// DATA STRUCTURES
// -----------------------------------------------------------------------------
struct LK24Stats {
    float damage          = 26.0f; // Base damage per hit
    float fireRate        = 63.0f; // RPM representation (~630 RPM)
    float accuracy        = 61.0f; // High base bullet spread accuracy
    float range           = 58.0f; // Effective range in meters
    float mobility        = 65.0f; // ADS/Movement speed factor
    float control         = 64.0f; // High base recoil control (very predictable vertical pull)
    float weight          = 3.3f;  // Weight in kg
    float headshotMultiplier = 1.3f;
    int magazineCapacity   = 30;
};

struct LK24Attachment {
    std::string name;
    std::string slot; // Muzzle, Barrel, Optic, Stock, Laser, Underbarrel, Ammunition, Rear Grip
    float damageMod   = 0.0f;
    float accuracyMod = 0.0f;
    float rangeMod    = 0.0f;
    float mobilityMod = 0.0f;
    float controlMod  = 0.0f;
    int extraCapacity = 0;
};

// -----------------------------------------------------------------------------
// LK24 CLASS DESIGN
// -----------------------------------------------------------------------------
class LK24 {
private:
    std::string weaponId = "lk24";
    std::string displayName = "LK24";
    std::string caliber = "5.56x45mm NATO";
    LK24Stats baseStats;
    std::vector<LK24Attachment> equippedAttachments;
    static constexpr size_t MAX_ATTACHMENTS = 5;

public:
    LK24() = default;

    // Getters
    std::string GetID() const { return weaponId; }
    std::string GetName() const { return displayName; }
    std::string GetCaliber() const { return caliber; }

    // Calculates current stats based on equipped attachments
    LK24Stats GetFinalStats() const {
        LK24Stats calculated = baseStats;
        for (const auto& att : equippedAttachments) {
            calculated.damage += att.damageMod;
            calculated.accuracy += att.accuracyMod;
            calculated.range += att.rangeMod;
            calculated.mobility += att.mobilityMod;
            calculated.control += att.controlMod;
            calculated.magazineCapacity += att.extraCapacity;
        }
        return calculated;
    }

    // Attachment Management
    bool Attach(const LK24Attachment& attachment) {
        if (equippedAttachments.size() >= MAX_ATTACHMENTS) {
            return false;
        }

        // Replace any existing attachment in the same slot
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&attachment](const LK24Attachment& a) { return a.slot == attachment.slot; }),
            equippedAttachments.end()
        );

        equippedAttachments.push_back(attachment);
        return true;
    }

    void DetachSlot(const std::string& slotName) {
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&slotName](const LK24Attachment& a) { return a.slot == slotName; }),
            equippedAttachments.end()
        );
    }

    void ResetToStock() {
        equippedAttachments.clear();
    }

    const std::vector<LK24Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    // Damage drop-off profile over distance
    float CalculateDamageAtDistance(float distanceMeters) const {
        LK24Stats current = GetFinalStats();
        float baseRange = current.range;

        if (distanceMeters <= baseRange * 0.35f) {
            return current.damage;          // 26 Damage (0 - ~20m)
        } else if (distanceMeters <= baseRange * 0.70f) {
            return current.damage * 0.92f;   // 24 Damage (~20m - ~40m)
        } else if (distanceMeters <= baseRange * 1.20f) {
            return current.damage * 0.73f;   // 19 Damage (~40m - ~70m)
        } else {
            return current.damage * 0.57f;   // 15 Damage (Extreme range)
        }
    }
};

// -----------------------------------------------------------------------------
// PRESET BLUEPRINT FACTORY FOR LK24
// -----------------------------------------------------------------------------
class LK24BlueprintFactory {
public:
    // Builds the popular laser-beam / zero-recoil long-range loadout
    static LK24 CreateZeroRecoilMeta() {
        LK24 rifle;
        rifle.Attach({"Monolithic Suppressor", "Muzzle", 0.0f, -2.0f, 12.0f, -4.0f, 0.0f, 0});
        rifle.Attach({"OWC Marksman", "Barrel", 0.0f, 6.0f, 15.0f, -5.0f, 7.0f, 0});
        rifle.Attach({"No Stock", "Stock", 0.0f, -4.0f, 0.0f, 10.0f, -6.0f, 0});
        rifle.Attach({"OWC Laser - Tactical", "Laser", 0.0f, 4.0f, 0.0f, 5.0f, 0.0f, 0});
        rifle.Attach({"Granulated Grip Tape", "Rear Grip", 0.0f, 5.0f, 0.0f, 0.0f, 2.0f, 0});
        return rifle;
    }

    // Builds a fast-ADS close-to-medium range aggressive loadout
    static LK24 CreateAggressiveRespawn() {
        LK24 rifle;
        rifle.Attach({"Tactical Suppressor", "Muzzle", 0.0f, 0.0f, 5.0f, -2.0f, 0.0f, 0});
        rifle.Attach({"MIP Light Barrel (Short)", "Barrel", 0.0f, -3.0f, -5.0f, 8.0f, -2.0f, 0});
        rifle.Attach({"No Stock", "Stock", 0.0f, -4.0f, 0.0f, 10.0f, -6.0f, 0});
        rifle.Attach({"OWC Laser - Tactical", "Laser", 0.0f, 4.0f, 0.0f, 5.0f, 0.0f, 0});
        rifle.Attach({"40 Round Extended Mag", "Ammunition", 0.0f, 0.0f, 0.0f, -2.0f, 0.0f, 10});
        return rifle;
    }
};

} // namespace BlackclawX

#endif // LK24_WEAPON_DESIGN_HPP

#ifndef PACEMAKER_WEAPON_DESIGN_HPP
#define PACEMAKER_WEAPON_DESIGN_HPP

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace BlackclawX {

// -----------------------------------------------------------------------------
// DATA STRUCTURES
// -----------------------------------------------------------------------------
struct PacemakerStats {
    float damagePerPellet = 15.0f; // Base damage per pellet
    int pelletCount       = 8;     // Total pellets fired per shot
    float fireRate        = 32.0f; // Lever-action / pump cycle speed
    float accuracy        = 48.0f; // Tightness of pellet spread pattern
    float range           = 35.0f; // Effective range in meters
    float mobility        = 72.0f; // ADS / sprint speed factor
    float control         = 45.0f; // Vertical/horizontal Kick control
    float weight          = 3.1f;  // Weight in kg
    float headshotMultiplier = 1.25f;
    int magazineCapacity   = 6;    // Shell capacity
};

struct PacemakerAttachment {
    std::string name;
    std::string slot; // Muzzle/Choke, Barrel, Stock, Laser, Ammunition, Rear Grip
    float damageMod   = 0.0f;
    float accuracyMod = 0.0f; // Tighter spread modifier
    float rangeMod    = 0.0f;
    float mobilityMod = 0.0f;
    float controlMod  = 0.0f;
    int extraCapacity = 0;
};

// -----------------------------------------------------------------------------
// PACEMAKER CLASS DESIGN
// -----------------------------------------------------------------------------
class Pacemaker {
private:
    std::string weaponId = "pacemaker";
    std::string displayName = "Pacemaker";
    std::string caliber = "12 Gauge";
    PacemakerStats baseStats;
    std::vector<PacemakerAttachment> equippedAttachments;
    static constexpr size_t MAX_ATTACHMENTS = 5;

public:
    Pacemaker() = default;

    // Getters
    std::string GetID() const { return weaponId; }
    std::string GetName() const { return displayName; }
    std::string GetCaliber() const { return caliber; }

    // Calculates dynamic stats including pellet tightness and total potential damage
    PacemakerStats GetFinalStats() const {
        PacemakerStats calculated = baseStats;
        for (const auto& att : equippedAttachments) {
            calculated.damagePerPellet += att.damageMod;
            calculated.accuracy += att.accuracyMod;
            calculated.range += att.rangeMod;
            calculated.mobility += att.mobilityMod;
            calculated.control += att.controlMod;
            calculated.magazineCapacity += att.extraCapacity;
        }
        return calculated;
    }

    float GetMaxDamagePotential() const {
        PacemakerStats stats = GetFinalStats();
        return stats.damagePerPellet * stats.pelletCount;
    }

    // Attachment Management
    bool Attach(const PacemakerAttachment& attachment) {
        if (equippedAttachments.size() >= MAX_ATTACHMENTS) {
            return false;
        }

        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&attachment](const PacemakerAttachment& a) { return a.slot == attachment.slot; }),
            equippedAttachments.end()
        );

        equippedAttachments.push_back(attachment);
        return true;
    }

    void DetachSlot(const std::string& slotName) {
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&slotName](const PacemakerAttachment& a) { return a.slot == slotName; }),
            equippedAttachments.end()
        );
    }

    void ResetToStock() {
        equippedAttachments.clear();
    }

    const std::vector<PacemakerAttachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    // Calculates damage per landing pellet based on distance drop-off
    float CalculatePelletDamageAtDistance(float distanceMeters) const {
        PacemakerStats current = GetFinalStats();
        float effectiveRange = current.range;

        if (distanceMeters <= effectiveRange * 0.3f) {
            return current.damagePerPellet;                 // Full pellet damage (0 - ~10m)
        } else if (distanceMeters <= effectiveRange * 0.6f) {
            return current.damagePerPellet * 0.75f;         // Mid-range pellet decay
        } else if (distanceMeters <= effectiveRange) {
            return current.damagePerPellet * 0.45f;         // Far edge of spread drop-off
        } else {
            return 0.0f;                                    // Complete pellet dissipation
        }
    }

    // Returns effective cone spread angle in degrees (higher accuracy = tighter choke angle)
    float GetPelletSpreadAngleDegrees() const {
        PacemakerStats current = GetFinalStats();
        float baseCone = 8.5f; // Base spread degrees
        float angleReduction = (current.accuracy / 100.0f) * 4.0f;
        return std::max(1.5f, baseCone - angleReduction);
    }
};

// -----------------------------------------------------------------------------
// PRESET BLUEPRINT FACTORY FOR PACEMAKER
// -----------------------------------------------------------------------------
class PacemakerBlueprintFactory {
public:
    // Builds a choke-heavy, long-range tight spread setup
    static Pacemaker CreateTightChokeSniper() {
        Pacemaker shotgun;
        shotgun.Attach({"Marauder Suppressor", "Muzzle", 1.0f, 8.0f, 10.0f, -4.0f, 0.0f, 0});
        shotgun.Attach({"Extended Heavy Barrel", "Barrel", 0.0f, 6.0f, 12.0f, -5.0f, 4.0f, 0});
        shotgun.Attach({"OWC Laser - Tactical", "Laser", 0.0f, 5.0f, 0.0f, 4.0f, 0.0f, 0});
        shotgun.Attach({"Choke", "Muzzle", 0.0f, 10.0f, 5.0f, -2.0f, 0.0f, 0});
        shotgun.Attach({"500gr Slug Ammunition", "Ammunition", 60.0f, 12.0f, 15.0f, -6.0f, -8.0f, -2});
        return shotgun;
    }

    // Builds a fast-movement hip-fire close quarters rusher build
    static Pacemaker CreateHipfireRusher() {
        Pacemaker shotgun;
        shotgun.Attach({"MIP Laser 5mW", "Laser", 0.0f, 8.0f, 0.0f, 6.0f, 0.0f, 0});
        shotgun.Attach({"No Stock", "Stock", 0.0f, -5.0f, 0.0f, 12.0f, -8.0f, 0});
        shotgun.Attach({"Speed Reload Tube", "Ammunition", 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 2});
        shotgun.Attach({"Stippled Grip Tape", "Rear Grip", 0.0f, -3.0f, 0.0f, 7.0f, -2.0f, 0});
        return shotgun;
    }
};

} // namespace BlackclawX

#endif // PACEMAKER_WEAPON_DESIGN_HPP

#ifndef DRH_WEAPON_DESIGN_HPP
#define DRH_WEAPON_DESIGN_HPP

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace BlackclawX {

// -----------------------------------------------------------------------------
// DATA STRUCTURES
// -----------------------------------------------------------------------------
struct DRHStats {
    float damage          = 32.0f; // Base damage (32-28-21 range tiers)
    float fireRate        = 57.0f; // Slower heavy-hitter RPM (~570 RPM)
    float accuracy        = 54.0f; // Base bullet spread accuracy
    float range           = 54.0f; // Effective range in meters
    float mobility        = 63.0f; // ADS / movement speed factor
    float control         = 57.0f; // Vertical recoil intensity
    float weight          = 3.6f;  // Heavy caliber frame weight in kg
    float chestMultiplier = 1.0f;  // Can be boosted via OTM magazine
    float headshotMultiplier = 1.3f;
    int magazineCapacity   = 20;    // Standard 20-round magazine
};

struct DRHAttachment {
    std::string name;
    std::string slot; // Muzzle, Barrel, Optic, Stock, Laser, Underbarrel, Ammunition, Rear Grip
    float damageMod        = 0.0f;
    float accuracyMod      = 0.0f;
    float rangeMod         = 0.0f;
    float mobilityMod      = 0.0f;
    float controlMod       = 0.0f;
    float chestMultiMod    = 0.0f; // Specific for OTM Mags
    int extraCapacity      = 0;
};

// -----------------------------------------------------------------------------
// DR-H CLASS DESIGN
// -----------------------------------------------------------------------------
class DRH {
private:
    std::string weaponId = "dr_h";
    std::string displayName = "DR-H";
    std::string caliber = "7.62x51mm NATO";
    DRHStats baseStats;
    std::vector<DRHAttachment> equippedAttachments;
    static constexpr size_t MAX_ATTACHMENTS = 5;

public:
    DRH() = default;

    // Getters
    std::string GetID() const { return weaponId; }
    std::string GetName() const { return displayName; }
    std::string GetCaliber() const { return caliber; }

    // Calculates dynamic stats including chest multipliers (e.g., OTM Mag)
    DRHStats GetFinalStats() const {
        DRHStats calculated = baseStats;
        for (const auto& att : equippedAttachments) {
            calculated.damage += att.damageMod;
            calculated.accuracy += att.accuracyMod;
            calculated.range += att.rangeMod;
            calculated.mobility += att.mobilityMod;
            calculated.control += att.controlMod;
            calculated.chestMultiplier += att.chestMultiMod;
            calculated.magazineCapacity += att.extraCapacity;
        }
        return calculated;
    }

    // Attachment Management
    bool Attach(const DRHAttachment& attachment) {
        if (equippedAttachments.size() >= MAX_ATTACHMENTS) {
            return false;
        }

        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&attachment](const DRHAttachment& a) { return a.slot == attachment.slot; }),
            equippedAttachments.end()
        );

        equippedAttachments.push_back(attachment);
        return true;
    }

    void DetachSlot(const std::string& slotName) {
        equippedAttachments.erase(
            std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&slotName](const DRHAttachment& a) { return a.slot == slotName; }),
            equippedAttachments.end()
        );
    }

    void ResetToStock() {
        equippedAttachments.clear();
    }

    const std::vector<DRHAttachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }

    // Calculates damage per shot considering location and drop-off
    float CalculateDamageAtDistance(float distanceMeters, bool isChestShot = false, bool isHeadshot = false) const {
        DRHStats current = GetFinalStats();
        float baseRange = current.range;
        float baseDamage = current.damage;

        // Determine base damage tier by distance
        if (distanceMeters > baseRange * 0.80f) {
            baseDamage *= 0.65f; // Drop to long-range tier (~21 damage)
        } else if (distanceMeters > baseRange * 0.40f) {
            baseDamage *= 0.87f; // Drop to mid-range tier (~28 damage)
        }

        // Apply hit zone multipliers
        if (isHeadshot) {
            return baseDamage * current.headshotMultiplier;
        } else if (isChestShot) {
            return baseDamage * current.chestMultiplier;
        }

        return baseDamage;
    }
};

// -----------------------------------------------------------------------------
// PRESET BLUEPRINT FACTORY FOR DR-H
// -----------------------------------------------------------------------------
class DRHBlueprintFactory {
public:
    // Builds the 3-shot kill meta loadout featuring 25 Round OTM Mag
    static DRH Create3ShotOTMMeta() {
        DRH rifle;
        rifle.Attach({"Monolithic Suppressor", "Muzzle", 0.0f, -2.0f, 12.0f, -4.0f, 0.0f, 0, 0});
        rifle.Attach({"OWC Ranger", "Barrel", 0.0f, 6.0f, 15.0f, -5.0f, 8.0f, 0, 0});
        rifle.Attach({"No Stock", "Stock", 0.0f, -4.0f, 0.0f, 10.0f, -7.0f, 0, 0});
        rifle.Attach({"OWC Laser - Tactical", "Laser", 0.0f, 4.0f, 0.0f, 5.0f, 0.0f, 0, 0});
        rifle.Attach({"25 Round OTM Mag", "Ammunition", 0.0f, 0.0f, 0.0f, -2.0f, 0.0f, 0.2f, 5}); // Grants chest multiplier boost
        return rifle;
    }

    // Builds a fast-ADS close-quarter aggressive build
    static DRH CreateAgroRespawn() {
        DRH rifle;
        rifle.Attach({"MIP Light Barrel (Short)", "Barrel", 0.0f, -3.0f, -6.0f, 8.0f, -3.0f, 0, 0});
        rifle.Attach({"No Stock", "Stock", 0.0f, -4.0f, 0.0f, 10.0f, -6.0f, 0, 0});
        rifle.Attach({"OWC Laser - Tactical", "Laser", 0.0f, 4.0f, 0.0f, 5.0f, 0.0f, 0, 0});
        rifle.Attach({"30 Round OTM Mag", "Ammunition", 0.0f, 0.0f, 0.0f, -4.0f, 0.0f, 0.2f, 10});
        rifle.Attach({"Granulated Grip Tape", "Rear Grip", 0.0f, 5.0f, 0.0f, 0.0f, 2.0f, 0, 0});
        return rifle;
    }
};

} // namespace BlackclawX

#endif // DRH_WEAPON_DESIGN_HPP

#ifndef BLACKCLAWX_BAL27_WEAPON_HPP
#define BLACKCLAWX_BAL27_WEAPON_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>

namespace BlackclawX {

enum class BAL27AttachmentSlot {
    Muzzle,
    Barrel,
    Optic,
    Stock,
    Underbarrel,
    RearGrip,
    Magazine,
    Ammunition
};

struct BAL27Stats {
    float baseDamage = 28.0f;
    float initialRPM = 660.0f;       // Rate of fire for the first 4 shots
    float acceleratedRPM = 857.0f;   // Rate of fire after shot threshold
    int rampUpThreshold = 4;         // Shot index where fire rate accelerates
    
    float accuracy = 64.0f;
    float rangeMeters = 24.0f;
    float mobility = 66.0f;
    float control = 58.0f;
    
    float headshotMultiplier = 1.35f;
    float chestMultiplier = 1.10f;
    float legMultiplier = 0.90f;
    
    int magazineCapacity = 30;
    float adsTimeMs = 240.0f;
    float sprintToFireMs = 150.0f;
    float reloadTimeSec = 2.15f;
};

struct BAL27Attachment {
    std::string id;
    std::string name;
    BAL27AttachmentSlot slot;
    
    float damageMod = 0.0f;
    float initialRpmMod = 0.0f;
    float acceleratedRpmMod = 0.0f;
    float accuracyMod = 0.0f;
    float rangeMod = 0.0f;          // Percentage modifier (+0.20 = +20%)
    float mobilityMod = 0.0f;
    float controlMod = 0.0f;
    float adsTimeModMs = 0.0f;      // Negative reduces ADS time
    int magCapacityDelta = 0;
};

class BAL27 {
private:
    std::string weaponId;
    std::string displayName;
    std::string caliber;
    BAL27Stats baseStats;
    std::unordered_map<BAL27AttachmentSlot, BAL27Attachment> equippedAttachments;

public:
    BAL27(std::string id = "bal27_std", std::string name = "BAL-27")
        : weaponId(std::move(id)), displayName(std::move(name)), caliber("5.7x28mm") {}

    const std::string& GetWeaponID() const { return weaponId; }
    const std::string& GetDisplayName() const { return displayName; }
    const std::string& GetCaliber() const { return caliber; }

    bool Attach(const BAL27Attachment& attachment) {
        if (equippedAttachments.size() >= 5 && 
            equippedAttachments.find(attachment.slot) == equippedAttachments.end()) {
            return false; // Reached maximum 5-attachment gunsmith limit
        }
        equippedAttachments[attachment.slot] = attachment;
        return true;
    }

    bool DetachSlot(BAL27AttachmentSlot slot) {
        return equippedAttachments.erase(slot) > 0;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    BAL27Stats GetFinalStats() const {
        BAL27Stats finalStats = baseStats;
        float totalRangePercent = 0.0f;

        for (const auto& [slot, att] : equippedAttachments) {
            finalStats.baseDamage += att.damageMod;
            finalStats.initialRPM += att.initialRpmMod;
            finalStats.acceleratedRPM += att.acceleratedRpmMod;
            finalStats.accuracy += att.accuracyMod;
            totalRangePercent += att.rangeMod;
            finalStats.mobility += att.mobilityMod;
            finalStats.control += att.controlMod;
            finalStats.adsTimeMs += att.adsTimeModMs;
            finalStats.magazineCapacity += att.magCapacityDelta;
        }

        finalStats.rangeMeters *= (1.0f + totalRangePercent);
        finalStats.adsTimeMs = std::max(120.0f, finalStats.adsTimeMs);
        return finalStats;
    }

    // Calculates dynamic RPM based on continuous fire burst length
    float GetCurrentRPM(int continuousShotsFired) const {
        BAL27Stats current = GetFinalStats();
        if (continuousShotsFired < current.rampUpThreshold) {
            return current.initialRPM;
        }
        return current.acceleratedRPM;
    }

    // Calculates damage dropoff based on distance and hit location
    float CalculateDamage(float distanceMeters, bool isHead, bool isChest) const {
        BAL27Stats stats = GetFinalStats();
        float effectiveDamage = stats.baseDamage;

        // Damage dropoff curves
        if (distanceMeters > stats.rangeMeters && distanceMeters <= stats.rangeMeters * 1.8f) {
            effectiveDamage *= 0.82f;
        } else if (distanceMeters > stats.rangeMeters * 1.8f) {
            effectiveDamage *= 0.65f;
        }

        if (isHead) {
            effectiveDamage *= stats.headshotMultiplier;
        } else if (isChest) {
            effectiveDamage *= stats.chestMultiplier;
        } else {
            effectiveDamage *= stats.legMultiplier;
        }

        return effectiveDamage;
    }

    const std::unordered_map<BAL27AttachmentSlot, BAL27Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }
};

class BAL27BlueprintFactory {
public:
    static BAL27 CreateObsidianSteedMeta() {
        BAL27 rifle("bal27_obsidian_steed", "BAL-27 - Obsidian Steed");

        rifle.Attach({"crown_h3_barrel", "Crown-H3 Barrel", BAL27AttachmentSlot::Barrel, 0.0f, 0.0f, 0.0f, 5.0f, 0.20f, -2.0f, 8.0f, 15.0f, 0});
        rifle.Attach({"quartermaster_supp", "Quartermaster Suppressor", BAL27AttachmentSlot::Muzzle, 0.0f, 0.0f, 0.0f, 4.0f, 0.10f, 0.0f, 6.0f, 5.0f, 0});
        rifle.Attach({"sandbar_heavy_stock", "Sandbar Heavy Stock", BAL27AttachmentSlot::Stock, 0.0f, 0.0f, 0.0f, 6.0f, 0.0f, -3.0f, 10.0f, 8.0f, 0});
        rifle.Attach({"60rnd_mag", "60 Round Mag", BAL27AttachmentSlot::Magazine, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -5.0f, 0.0f, 18.0f, 30});
        rifle.Attach({"highgrain_57", "5.7x28mm High Grain", BAL27AttachmentSlot::Ammunition, 2.0f, 0.0f, 0.0f, 2.0f, 0.15f, 0.0f, -4.0f, 0.0f, 0});

        return rifle;
    }

    static BAL27 CreateAggressiveRampUp() {
        BAL27 rifle("bal27_cqc_ramp", "BAL-27 - CQC Accelerator");

        rifle.Attach({"noctkill_barrel", "Noctkill Long Barrel", BAL27AttachmentSlot::Barrel, 0.0f, 0.0f, 30.0f, 3.0f, 0.15f, -1.0f, 4.0f, 10.0f, 0});
        rifle.Attach({"ardent_tac_stock", "Ardent Tac Stock", BAL27AttachmentSlot::Stock, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, -2.0f, -25.0f, 0});
        rifle.Attach({"razorhawk_laser", "SL Razorhawk Laser", BAL27AttachmentSlot::Underbarrel, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 6.0f, 2.0f, -18.0f, 0});
        rifle.Attach({"channel_m_grip", "Channel-M Grip", BAL27AttachmentSlot::RearGrip, 0.0f, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 7.0f, 0.0f, 0});

        return rifle;
    }
};

} // namespace BlackclawX

#endif // BLACKCLAWX_BAL27_WEAPON_HPP

#ifndef BLACKCLAWX_MAN_O_WAR_WEAPON_HPP
#define BLACKCLAWX_MAN_O_WAR_WEAPON_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>

namespace BlackclawX {

enum class ManOWarAttachmentSlot {
    Muzzle,
    Barrel,
    Optic,
    Stock,
    Underbarrel,
    RearGrip,
    Magazine,
    Ammunition
};

struct ManOWarStats {
    float baseDamage = 37.0f;        // High base damage (3-shot kill potential)
    float fireRateRPM = 500.0f;      // Slow, heavy cyclic fire rate (~500 RPM)
    
    float accuracy = 69.0f;          // High base bullet spread accuracy
    float rangeMeters = 30.0f;       // Strong base 3-shot kill distance
    float mobility = 58.0f;          // Slower ADS & movement profile
    float control = 53.0f;           // Heavy vertical kick requirement
    
    float headshotMultiplier = 1.40f;
    float chestMultiplier = 1.10f;
    float legMultiplier = 0.90f;
    
    int magazineCapacity = 25;       // Standard 25-round heavy mag
    float adsTimeMs = 300.0f;        // Base ADS speed (sluggish)
    float sprintToFireMs = 190.0f;
    float reloadTimeSec = 2.40f;
};

struct ManOWarAttachment {
    std::string id;
    std::string name;
    ManOWarAttachmentSlot slot;
    
    float damageMod = 0.0f;
    float fireRateRpmMod = 0.0f;
    float accuracyMod = 0.0f;
    float rangeMod = 0.0f;           // Percentage modifier (+0.15 = +15%)
    float mobilityMod = 0.0f;
    float controlMod = 0.0f;
    float adsTimeModMs = 0.0f;       // Negative values reduce ADS time
    int magCapacityDelta = 0;
};

class ManOWar {
private:
    std::string weaponId;
    std::string displayName;
    std::string caliber;
    ManOWarStats baseStats;
    std::unordered_map<ManOWarAttachmentSlot, ManOWarAttachment> equippedAttachments;

public:
    ManOWar(std::string id = "mow_std", std::string name = "Man-O-War")
        : weaponId(std::move(id)), displayName(std::move(name)), caliber("7.62x51mm NATO") {}

    const std::string& GetWeaponID() const { return weaponId; }
    const std::string& GetDisplayName() const { return displayName; }
    const std::string& GetCaliber() const { return caliber; }

    bool Attach(const ManOWarAttachment& attachment) {
        if (equippedAttachments.size() >= 5 && 
            equippedAttachments.find(attachment.slot) == equippedAttachments.end()) {
            return false; // Reached maximum 5-attachment gunsmith cap
        }
        equippedAttachments[attachment.slot] = attachment;
        return true;
    }

    bool DetachSlot(ManOWarAttachmentSlot slot) {
        return equippedAttachments.erase(slot) > 0;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    ManOWarStats GetFinalStats() const {
        ManOWarStats finalStats = baseStats;
        float totalRangePercent = 0.0f;

        for (const auto& [slot, att] : equippedAttachments) {
            finalStats.baseDamage += att.damageMod;
            finalStats.fireRateRPM += att.fireRateRpmMod;
            finalStats.accuracy += att.accuracyMod;
            totalRangePercent += att.rangeMod;
            finalStats.mobility += att.mobilityMod;
            finalStats.control += att.controlMod;
            finalStats.adsTimeMs += att.adsTimeModMs;
            finalStats.magazineCapacity += att.magCapacityDelta;
        }

        finalStats.rangeMeters *= (1.0f + totalRangePercent);
        finalStats.adsTimeMs = std::max(160.0f, finalStats.adsTimeMs);
        return finalStats;
    }

    // Calculates damage dropoff based on distance and hit zones
    float CalculateDamage(float distanceMeters, bool isHead, bool isChest) const {
        ManOWarStats stats = GetFinalStats();
        float effectiveDamage = stats.baseDamage;

        // Damage dropoff curves (37 -> 30 -> 24)
        if (distanceMeters > stats.rangeMeters && distanceMeters <= stats.rangeMeters * 1.6f) {
            effectiveDamage *= 0.81f; // ~30 damage
        } else if (distanceMeters > stats.rangeMeters * 1.6f) {
            effectiveDamage *= 0.65f; // ~24 damage
        }

        if (isHead) {
            effectiveDamage *= stats.headshotMultiplier;
        } else if (isChest) {
            effectiveDamage *= stats.chestMultiplier;
        } else {
            effectiveDamage *= stats.legMultiplier;
        }

        return effectiveDamage;
    }

    // Calculates theoretical Time-To-Kill in milliseconds for 100 HP target
    float CalculateChestTTK(float distanceMeters) const {
        ManOWarStats stats = GetFinalStats();
        float damagePerHit = CalculateDamage(distanceMeters, false, true);
        int shotsToKill = static_cast<int>(std::ceil(100.0f / damagePerHit));

        if (shotsToKill <= 1) return 0.0f;
        
        // Time between shots in ms = (60 / RPM) * 1000
        float timeBetweenShotsMs = (60.0f / stats.fireRateRPM) * 1000.0f;
        return (shotsToKill - 1) * timeBetweenShotsMs;
    }

    const std::unordered_map<ManOWarAttachmentSlot, ManOWarAttachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }
};

class ManOWarBlueprintFactory {
public:
    // Builds the 3-shot extended range meta build
    static ManOWar CreateLaserBeamMeta() {
        ManOWar rifle("mow_beam_meta", "Man-O-War - Cardinal");

        rifle.Attach({"mono_suppressor", "Monolithic Suppressor", ManOWarAttachmentSlot::Muzzle, 0.0f, 0.0f, -2.0f, 0.25f, -4.0f, -3.0f, 15.0f, 0});
        rifle.Attach({"owc_ranger_barrel", "OWC Ranger Barrel", ManOWarAttachmentSlot::Barrel, 0.0f, 0.0f, 8.0f, 0.20f, -5.0f, 10.0f, 22.0f, 0});
        rifle.Attach({"rtc_steady_stock", "RTC Steady Stock", ManOWarAttachmentSlot::Stock, 0.0f, 0.0f, 6.0f, 0.0f, -6.0f, 12.0f, 10.0f, 0});
        rifle.Attach({"owc_laser_tac", "OWC Laser - Tactical", ManOWarAttachmentSlot::Underbarrel, 0.0f, 0.0f, 5.0f, 0.0f, 4.0f, 0.0f, -15.0f, 0});
        rifle.Attach({"granulated_grip", "Granulated Grip Tape", ManOWarAttachmentSlot::RearGrip, 0.0f, 0.0f, 7.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0});

        return rifle;
    }

    // Builds a fast-mobility configuration compensating for heavy ADS penalty
    static ManOWar CreateAggressiveMobility() {
        ManOWar rifle("mow_aggressive", "Man-O-War - Pack Leader");

        rifle.Attach({"short_barrel", "MIP Light Barrel (Short)", ManOWarAttachmentSlot::Barrel, 0.0f, 0.0f, -4.0f, -0.10f, 10.0f, -6.0f, -25.0f, 0});
        rifle.Attach({"ykm_combat_stock", "YKM Combat Stock", ManOWarAttachmentSlot::Stock, 0.0f, 0.0f, -3.0f, 0.0f, 8.0f, -5.0f, -20.0f, 0});
        rifle.Attach({"owc_laser_tac", "OWC Laser - Tactical", ManOWarAttachmentSlot::Underbarrel, 0.0f, 0.0f, 5.0f, 0.0f, 4.0f, 0.0f, -15.0f, 0});
        rifle.Attach({"35rnd_mag", "35 Round Large Mag", ManOWarAttachmentSlot::Magazine, 0.0f, 0.0f, 0.0f, 0.0f, -3.0f, -2.0f, 10.0f, 10});
        rifle.Attach({"stippled_grip", "Stippled Grip Tape", ManOWarAttachmentSlot::RearGrip, 0.0f, 0.0f, -5.0f, 0.0f, 6.0f, -2.0f, -12.0f, 0});

        return rifle;
    }
};

} // namespace BlackclawX

#endif // BLACKCLAWX_MAN_O_WAR_WEAPON_HPP

#ifndef BLACKCLAWX_BP50_WEAPON_HPP
#define BLACKCLAWX_BP50_WEAPON_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>

namespace BlackclawX {

enum class BP50AttachmentSlot {
    Muzzle,
    Barrel,
    Optic,
    Stock,
    Underbarrel,
    RearGrip,
    Magazine,
    Ammunition
};

struct BP50Stats {
    float baseDamage = 24.0f;        // 5-shot kill base (high RPM compensation)
    float fireRateRPM = 900.0f;      // Ultra-fast cyclic fire rate
    
    float accuracy = 61.0f;          // Moderate base spread
    float rangeMeters = 18.0f;       // Compact bullpup effective range limit
    float mobility = 74.0f;          // High base movement/strafe profile
    float control = 65.0f;           // Smooth, highly controllable recoil vector
    
    float headshotMultiplier = 1.30f;
    float chestMultiplier = 1.10f;
    float legMultiplier = 0.90f;
    
    int magazineCapacity = 30;       // Standard 30-round mag
    float adsTimeMs = 210.0f;        // Snappy base ADS time
    float sprintToFireMs = 130.0f;
    float reloadTimeSec = 1.95f;     // Quick bullpup magazine swap
};

struct BP50Attachment {
    std::string id;
    std::string name;
    BP50AttachmentSlot slot;
    
    float damageMod = 0.0f;
    float fireRateRpmMod = 0.0f;
    float accuracyMod = 0.0f;
    float rangeMod = 0.0f;           // Percentage modifier (+0.15 = +15%)
    float mobilityMod = 0.0f;
    float controlMod = 0.0f;
    float adsTimeModMs = 0.0f;       // Negative values reduce ADS time
    int magCapacityDelta = 0;
};

class BP50 {
private:
    std::string weaponId;
    std::string displayName;
    std::string caliber;
    BP50Stats baseStats;
    std::unordered_map<BP50AttachmentSlot, BP50Attachment> equippedAttachments;

public:
    BP50(std::string id = "bp50_std", std::string name = "BP50")
        : weaponId(std::move(id)), displayName(std::move(name)), caliber("5.56x45mm NATO") {}

    const std::string& GetWeaponID() const { return weaponId; }
    const std::string& GetDisplayName() const { return displayName; }
    const std::string& GetCaliber() const { return caliber; }

    bool Attach(const BP50Attachment& attachment) {
        if (equippedAttachments.size() >= 5 && 
            equippedAttachments.find(attachment.slot) == equippedAttachments.end()) {
            return false; // Gunsmith 5-attachment limit reached
        }
        equippedAttachments[attachment.slot] = attachment;
        return true;
    }

    bool DetachSlot(BP50AttachmentSlot slot) {
        return equippedAttachments.erase(slot) > 0;
    }

    void ClearAttachments() {
        equippedAttachments.clear();
    }

    BP50Stats GetFinalStats() const {
        BP50Stats finalStats = baseStats;
        float totalRangePercent = 0.0f;

        for (const auto& [slot, att] : equippedAttachments) {
            finalStats.baseDamage += att.damageMod;
            finalStats.fireRateRPM += att.fireRateRpmMod;
            finalStats.accuracy += att.accuracyMod;
            totalRangePercent += att.rangeMod;
            finalStats.mobility += att.mobilityMod;
            finalStats.control += att.controlMod;
            finalStats.adsTimeMs += att.adsTimeModMs;
            finalStats.magazineCapacity += att.magCapacityDelta;
        }

        finalStats.rangeMeters *= (1.0f + totalRangePercent);
        finalStats.adsTimeMs = std::max(100.0f, finalStats.adsTimeMs);
        return finalStats;
    }

    // Calculates damage dropoff based on distance and hit zones
    float CalculateDamage(float distanceMeters, bool isHead, bool isChest) const {
        BP50Stats stats = GetFinalStats();
        float effectiveDamage = stats.baseDamage;

        // Damage dropoff tiers (24 -> 20 -> 17)
        if (distanceMeters > stats.rangeMeters && distanceMeters <= stats.rangeMeters * 1.5f) {
            effectiveDamage *= 0.83f;
        } else if (distanceMeters > stats.rangeMeters * 1.5f) {
            effectiveDamage *= 0.70f;
        }

        if (isHead) {
            effectiveDamage *= stats.headshotMultiplier;
        } else if (isChest) {
            effectiveDamage *= stats.chestMultiplier;
        } else {
            effectiveDamage *= stats.legMultiplier;
        }

        return effectiveDamage;
    }

    // Calculates theoretical Time-To-Kill in milliseconds for 100 HP target
    float CalculateChestTTK(float distanceMeters) const {
        BP50Stats stats = GetFinalStats();
        float damagePerHit = CalculateDamage(distanceMeters, false, true);
        int shotsToKill = static_cast<int>(std::ceil(100.0f / damagePerHit));

        if (shotsToKill <= 1) return 0.0f;
        
        float timeBetweenShotsMs = (60.0f / stats.fireRateRPM) * 1000.0f;
        return (shotsToKill - 1) * timeBetweenShotsMs;
    }

    const std::unordered_map<BP50AttachmentSlot, BP50Attachment>& GetEquippedAttachments() const {
        return equippedAttachments;
    }
};

class BP50BlueprintFactory {
public:
    // Builds maximum RPM and extended capacity build for close/mid range dominance
    static BP50 CreateLaserRpmMeta() {
        BP50 rifle("bp50_laser_meta", "BP50 - Shredder");

        rifle.Attach({"lore_4_barrel", "Lore-4 Heavy Barrel", BP50AttachmentSlot::Barrel, 0.0f, 0.0f, 6.0f, 0.25f, -3.0f, 8.0f, 18.0f, 0});
        rifle.Attach({"vt7_suppressor", "VT-7 Spitfire Suppressor", BP50AttachmentSlot::Muzzle, 0.0f, 0.0f, 3.0f, 0.10f, 0.0f, 4.0f, 8.0f, 0});
        rifle.Attach({"moat_light_stock", "Moat-Heavy Stock", BP50AttachmentSlot::Stock, 0.0f, 0.0f, 5.0f, 0.0f, -4.0f, 10.0f, 10.0f, 0});
        rifle.Attach({"50rnd_drum", "50 Round Drum Mag", BP50AttachmentSlot::Magazine, 0.0f, 0.0f, 0.0f, 0.0f, -6.0f, 0.0f, 22.0f, 20});
        rifle.Attach({"trident_grip", "Trident-9 Rear Grip", BP50AttachmentSlot::RearGrip, 0.0f, 0.0f, 4.0f, 0.0f, 2.0f, 5.0f, -12.0f, 0});

        return rifle;
    }

    // High mobility SMG-hybrid build
    static BP50 CreateSMGInfiltrator() {
        BP50 rifle("bp50_smg_hybrid", "BP50 - Velocity");

        rifle.Attach({"short_stub_barrel", "Short Stub Barrel", BP50AttachmentSlot::Barrel, 0.0f, 0.0f, -3.0f, -0.12f, 12.0f, -5.0f, -30.0f, 0});
        rifle.Attach({"no_stock_mod", "Duster Stock Removal", BP50AttachmentSlot::Stock, 0.0f, 0.0f, -8.0f, 0.0f, 15.0f, -12.0f, -35.0f, 0});
        rifle.Attach({"tac_laser", "Laser Sight - Tactical", BP50AttachmentSlot::Underbarrel, 0.0f, 0.0f, 4.0f, 0.0f, 5.0f, 0.0f, -15.0f, 0});
        rifle.Attach({"40rnd_mag", "40 Round Mag", BP50AttachmentSlot::Magazine, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f, 0.0f, 8.0f, 10});

        return rifle;
    }
};

} // namespace BlackclawX

#endif // BLACKCLAWX_BP50_WEAPON_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace WeaponSystem {

    // Weapon Operating States
    enum class WeaponState {
        Idle,
        Firing,
        Reloading,
        Empty,
        Jammed
    };

    // Firing Modes supported by RAM-7
    enum class FireMode {
        SemiAuto,
        FullAuto
    };

    // Attachment Types
    enum class AttachmentSlot {
        Muzzle,
        Barrel,
        Optic,
        Underbarrel,
        Magazine
    };

    struct Attachment {
        std::string name;
        AttachmentSlot slot;
        float recoilModifier = 1.0f;     // Multiplier for recoil (e.g., 0.85 = 15% reduction)
        float adsSpeedModifier = 1.0f;    // Multiplier for Aim-Down-Sights speed
        int extraCapacity = 0;           // Additional magazine capacity
    };

    struct Stats {
        float fireRateRPM = 857.0f;       // High rate of fire characteristic of RAM-7
        float baseDamage = 28.0f;
        float effectiveRangeMeters = 35.0f;
        float reloadTimeSeconds = 2.3f;
        float verticalRecoil = 4.5f;      // Recoil kick value
        float horizontalRecoil = 2.1f;
    };

    class RAM7 {
    private:
        // Core Attributes
        std::string weaponName;
        std::string caliber;
        
        // Ammunition Mechanics
        int currentAmmo;
        int maxMagazineCapacity;
        int reserveAmmo;

        // Weapon States
        WeaponState currentState;
        FireMode currentFireMode;
        Stats baseStats;

        // Attachments
        std::vector<Attachment> equippedAttachments;

    public:
        // Constructor
        RAM7(int initialReserveAmmo = 150)
            : weaponName("RAM-7"),
              caliber("5.56x45mm NATO"),
              maxMagazineCapacity(30),
              reserveAmmo(initialReserveAmmo),
              currentState(WeaponState::Idle),
              currentFireMode(FireMode::FullAuto) {
            
            currentAmmo = maxMagazineCapacity;
        }

        // --- Core Combat Actions ---

        bool PullTrigger() {
            if (currentState == WeaponState::Reloading) {
                std::cout << weaponName << " is currently reloading.\n";
                return false;
            }

            if (currentAmmo <= 0) {
                currentState = WeaponState::Empty;
                std::cout << "*Click* - " << weaponName << " is empty!\n";
                return false;
            }

            currentState = WeaponState::Firing;
            currentAmmo--;

            float currentVertRecoil = GetEffectiveVerticalRecoil();
            float currentHorizRecoil = GetEffectiveHorizontalRecoil();

            std::cout << "BANG! [" << weaponName << "] Ammo remaining: " 
                      << currentAmmo << "/" << GetMaxCapacity() 
                      << " | Recoil Kick (V/H): (" << currentVertRecoil 
                      << " / " << currentHorizRecoil << ")\n";

            if (currentFireMode == FireMode::SemiAuto || currentAmmo == 0) {
                currentState = (currentAmmo == 0) ? WeaponState::Empty : WeaponState::Idle;
            }

            return true;
        }

        void ReleaseTrigger() {
            if (currentState == WeaponState::Firing) {
                currentState = (currentAmmo == 0) ? WeaponState::Empty : WeaponState::Idle;
            }
        }

        void Reload() {
            if (currentAmmo == GetMaxCapacity()) {
                std::cout << weaponName << " magazine is already full.\n";
                return;
            }

            if (reserveAmmo <= 0) {
                std::cout << "No reserve ammunition remaining for " << weaponName << "!\n";
                return;
            }

            currentState = WeaponState::Reloading;
            std::cout << "Reloading " << weaponName << "...\n";

            int neededAmmo = GetMaxCapacity() - currentAmmo;
            int ammoToLoad = std::min(neededAmmo, reserveAmmo);

            currentAmmo += ammoToLoad;
            reserveAmmo -= ammoToLoad;

            currentState = WeaponState::Idle;
            std::cout << "Reload complete. Ammo: " << currentAmmo << "/" << GetMaxCapacity() 
                      << " | Reserve: " << reserveAmmo << "\n";
        }

        void ToggleFireMode() {
            if (currentFireMode == FireMode::FullAuto) {
                currentFireMode = FireMode::SemiAuto;
                std::cout << weaponName << " switched to Semi-Automatic mode.\n";
            } else {
                currentFireMode = FireMode::FullAuto;
                std::cout << weaponName << " switched to Full-Automatic mode.\n";
            }
        }

        // --- Attachment System ---

        bool EquipAttachment(const Attachment& newAttachment) {
            // Remove existing attachment in the same slot if present
            UnequipAttachment(newAttachment.slot);

            equippedAttachments.push_back(newAttachment);
            std::cout << "Equipped " << newAttachment.name << " to " << weaponName << ".\n";
            return true;
        }

        bool UnequipAttachment(AttachmentSlot slot) {
            auto it = std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
                [slot](const Attachment& att) { return att.slot == slot; });

            if (it != equippedAttachments.end()) {
                equippedAttachments.erase(it, equippedAttachments.end());
                std::cout << "Removed attachment from designated slot.\n";
                return true;
            }
            return false;
        }

        // --- Stat Modifiers & Getters ---

        int GetMaxCapacity() const {
            int extra = 0;
            for (const auto& att : equippedAttachments) {
                extra += att.extraCapacity;
            }
            return maxMagazineCapacity + extra;
        }

        float GetEffectiveVerticalRecoil() const {
            float modifier = 1.0f;
            for (const auto& att : equippedAttachments) {
                modifier *= att.recoilModifier;
            }
            return baseStats.verticalRecoil * modifier;
        }

        float GetEffectiveHorizontalRecoil() const {
            float modifier = 1.0f;
            for (const auto& att : equippedAttachments) {
                modifier *= att.recoilModifier;
            }
            return baseStats.horizontalRecoil * modifier;
        }

        // Getters for Inspection
        std::string GetName() const { return weaponName; }
        int GetCurrentAmmo() const { return currentAmmo; }
        int GetReserveAmmo() const { return reserveAmmo; }
        WeaponState GetState() const { return currentState; }
        FireMode GetFireMode() const { return currentFireMode; }
    };

} // namespace WeaponSystem

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace TacticalWeapons {

    enum class FireMode {
        SemiAuto,
        ThreeRoundBurst,
        FullAuto
    };

    enum class AttachmentSlot {
        Muzzle,
        Barrel,
        Optic,
        Stock,
        Underbarrel,
        Magazine
    };

    struct Attachment {
        std::string name;
        AttachmentSlot slot;
        float recoilControlMultiplier = 1.0f; // e.g., 0.90 = 10% recoil reduction
        float damageRangeMultiplier = 1.0f;   // e.g., 1.15 = 15% range boost
        float mobilityMultiplier = 1.0f;      // Movement/ADS speed scaling
        int capacityBonus = 0;
    };

    struct Ballistics {
        float muzzleVelocityFPS = 2900.0f;   // 5.56x45mm NATO standard out of 14.5" barrel
        float baseDamage = 30.0f;
        float effectiveRangeMeters = 48.0f;
        float rateOfFireRPM = 800.0f;
    };

    class XM4 {
    private:
        // Identify & Spec
        std::string weaponName;
        std::string platform;
        
        // Ammo & Capacity
        int currentAmmo;
        int baseCapacity;
        int reserveAmmo;

        // Mechanics & Heating
        FireMode activeFireMode;
        Ballistics baseSpecs;
        float barrelTemperatureCelsius; // Heat builds with fire and affects accuracy
        float verticalRecoilBase;
        float horizontalRecoilBase;

        // Attachment System
        std::vector<Attachment> attachments;

    public:
        // Constructor
        XM4(int initialReserveAmmo = 210)
            : weaponName("XM4"),
              platform("AR-15 / CAR-15 Platform"),
              baseCapacity(30),
              reserveAmmo(initialReserveAmmo),
              activeFireMode(FireMode::FullAuto),
              barrelTemperatureCelsius(20.0f), // Ambient room temperature
              verticalRecoilBase(3.8f),
              horizontalRecoilBase(1.8f) {
            
            currentAmmo = GetMaxCapacity();
        }

        // --- Firing Mechanisms ---

        bool FireSingleRound() {
            if (currentAmmo <= 0) {
                std::cout << "[" << weaponName << "] *Click* - Weapon Empty!\n";
                return false;
            }

            currentAmmo--;
            barrelTemperatureCelsius += 4.5f; // Heat generated per round

            float currentVertRecoil = ComputeVerticalRecoil();
            float currentHorizRecoil = ComputeHorizontalRecoil();
            float effectiveVelocity = baseSpecs.muzzleVelocityFPS * GetRangeMultiplier();

            std::cout << "FIRE! [" << weaponName << "] Ammo: " << currentAmmo << "/" 
                      << GetMaxCapacity() << " | Velocity: " << effectiveVelocity << " FPS"
                      << " | Recoil Kick: (" << currentVertRecoil << "V, " << currentHorizRecoil << "H)"
                      << " | Temp: " << std::round(barrelTemperatureCelsius) << "C\n";

            return true;
        }

        int FireBurst() {
            int roundsFired = 0;
            std::cout << "--- Initializing 3-Round Burst ---\n";
            for (int i = 0; i < 3; ++i) {
                if (FireSingleRound()) {
                    roundsFired++;
                } else {
                    break;
                }
            }
            return roundsFired;
        }

        void CoolBarrel(float timeElapsedSeconds) {
            // Natural cooling system
            float coolingRate = 5.0f; // degrees per second
            barrelTemperatureCelsius = std::max(20.0f, barrelTemperatureCelsius - (coolingRate * timeElapsedSeconds));
        }

        // --- Reloading & Fire Mode Control ---

        void Reload() {
            int maxCap = GetMaxCapacity();
            if (currentAmmo >= maxCap) {
                std::cout << "[" << weaponName << "] Magazine already full.\n";
                return;
            }

            if (reserveAmmo <= 0) {
                std::cout << "[" << weaponName << "] No reserve ammo left to reload!\n";
                return;
            }

            int roundsNeeded = maxCap - currentAmmo;
            int roundsToLoad = std::min(roundsNeeded, reserveAmmo);

            currentAmmo += roundsToLoad;
            reserveAmmo -= roundsToLoad;

            std::cout << "[" << weaponName << "] Reload complete. Current Mag: " 
                      << currentAmmo << "/" << maxCap << " | Reserve: " << reserveAmmo << "\n";
        }

        void CycleFireMode() {
            switch (activeFireMode) {
                case FireMode::FullAuto:
                    activeFireMode = FireMode::SemiAuto;
                    std::cout << "[" << weaponName << "] Switched to Semi-Automatic.\n";
                    break;
                case FireMode::SemiAuto:
                    activeFireMode = FireMode::ThreeRoundBurst;
                    std::cout << "[" << weaponName << "] Switched to 3-Round Burst.\n";
                    break;
                case FireMode::ThreeRoundBurst:
                    activeFireMode = FireMode::FullAuto;
                    std::cout << "[" << weaponName << "] Switched to Full-Automatic.\n";
                    break;
            }
        }

        // --- Modular Attachment API ---

        void AttachMod(const Attachment& mod) {
            DetachModBySlot(mod.slot); // Swap out if slot is occupied
            attachments.push_back(mod);
            std::cout << "[" << weaponName << "] Attached: " << mod.name << "\n";
        }

        void DetachModBySlot(AttachmentSlot slot) {
            auto it = std::remove_if(attachments.begin(), attachments.end(),
                [slot](const Attachment& a) { return a.slot == slot; });
            
            if (it != attachments.end()) {
                attachments.erase(it, attachments.end());
                std::cout << "[" << weaponName << "] Detached attachment from slot.\n";
            }
        }

        // --- Dynamic Calculated Stats ---

        int GetMaxCapacity() const {
            int bonus = 0;
            for (const auto& mod : attachments) {
                bonus += mod.capacityBonus;
            }
            return baseCapacity + bonus;
        }

        float ComputeVerticalRecoil() const {
            float modMult = 1.0f;
            for (const auto& mod : attachments) {
                modMult *= mod.recoilControlMultiplier;
            }
            // Overheating increases spread/recoil inaccuracy
            float heatPenalty = (barrelTemperatureCelsius > 100.0f) ? 1.25f : 1.0f;
            return verticalRecoilBase * modMult * heatPenalty;
        }

        float ComputeHorizontalRecoil() const {
            float modMult = 1.0f;
            for (const auto& mod : attachments) {
                modMult *= mod.recoilControlMultiplier;
            }
            return horizontalRecoilBase * modMult;
        }

        float GetRangeMultiplier() const {
            float mult = 1.0f;
            for (const auto& mod : attachments) {
                mult *= mod.damageRangeMultiplier;
            }
            return mult;
        }

        // Getters
        std::string GetName() const { return weaponName; }
        int GetCurrentAmmo() const { return currentAmmo; }
        int GetReserveAmmo() const { return reserveAmmo; }
        FireMode GetFireMode() const { return activeFireMode; }
        float GetTemperature() const { return barrelTemperatureCelsius; }
    };

} // namespace TacticalWeapons

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace BlackclawX {

enum class GrozaVariant {
    Groza1_762x39,     // Standard 7.62x39mm carbine variant
    Groza4_9x39        // Subsonic 9x39mm suppressed variant
};

struct GrozaAttachment {
    std::string name;
    std::string category; // Muzzle, Optics, Magazine, Underbarrel, Stock, Grip
    float weight;         // in kg
    float damageMod;      
    float accuracyMod;    
    float rangeMod;       
    float controlMod;     
    float mobilityMod;    
};

struct GrozaBaseStats {
    float damage;
    float fireRate;
    float accuracy;
    float range;
    float control;
    float mobility;
    float weight;
};

class GrozaEngine {
private:
    std::string weaponName;
    GrozaVariant variant;
    std::string caliber;
    
    GrozaBaseStats baseStats;
    float maxWeightCapacity;

    std::vector<GrozaAttachment> availableAttachments;
    std::vector<GrozaAttachment> equippedAttachments;

public:
    explicit GrozaEngine(GrozaVariant weaponVariant = GrozaVariant::Groza1_762x39);
    ~GrozaEngine() = default;

    // Core Mechanics
    bool ToggleAttachment(size_t index);
    bool EquipAttachment(const std::string& attachmentName);
    bool UnequipAttachment(const std::string& attachmentName);
    void ClearAttachments();

    // Stats & Data Accessors
    float GetTotalWeight() const;
    GrozaBaseStats CalculateEffectiveStats() const;
    
    // UI / Output Generation
    void DisplayHeader() const;
    void DisplayStatsAndAttachments() const;

private:
    void InitializeAttachments();
    void PrintStatBar(const std::string& label, float value, float maxValue) const;
};

} // namespace BlackclawX

#include "GrozaEngine.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace BlackclawX {

GrozaEngine::GrozaEngine(GrozaVariant weaponVariant)
    : variant(weaponVariant), maxWeightCapacity(6.5f) {
    
    if (variant == GrozaVariant::Groza1_762x39) {
        weaponName = "OTs-14-4A Groza-1";
        caliber = "7.62x39mm";
        baseStats = { 42.0f, 65.0f, 55.0f, 52.0f, 48.0f, 74.0f, 2.7f };
    } else {
        weaponName = "OTs-14-4A Groza-4";
        caliber = "9x39mm Subsonic";
        baseStats = { 46.0f, 60.0f, 62.0f, 44.0f, 58.0f, 70.0f, 3.1f };
    }

    InitializeAttachments();
}

void GrozaEngine::InitializeAttachments() {
    availableAttachments = {
        // Muzzles & Suppressors
        {"Groza Tactical Suppressor",  "Muzzle",      0.5f, +0.0f, +2.0f, +8.0f, +3.0f, -4.0f},
        {"PBS-1 Suppressor",          "Muzzle",      0.6f, +0.0f, +1.0f, +10.0f, +4.0f, -5.0f},
        {"Compensator 7.62",          "Muzzle",      0.3f, +0.0f, +4.0f, +0.0f, +9.0f, -2.0f},

        // Optics (Mounted on Integrated Carrying Handle Rail)
        {"PO 4x24 Scope",              "Optic",       0.6f, +0.0f, +8.0f, +12.0f, -4.0f, -6.0f},
        {"OKP-7 Red Dot",              "Optic",       0.2f, +0.0f, +3.0f, +0.0f, +0.0f, -1.0f},
        {"Kobra Reflex Sight",         "Optic",       0.3f, +0.0f, +4.0f, +0.0f, +1.0f, -2.0f},

        // Underbarrel Modules (Integrated directly into the front frame)
        {"GP-30 40mm Grenade Launcher","Underbarrel", 1.5f, +0.0f, -5.0f, +0.0f, +14.0f,-12.0f},
        {"Vertical Bullpup Foregrip",  "Underbarrel", 0.4f, +0.0f, +5.0f, +0.0f, +8.0f, -3.0f},
        {"Tactical Laser Sight",       "Underbarrel", 0.1f, +0.0f, +6.0f, +0.0f, +2.0f, +4.0f},

        // Magazines
        {"40-Round Extended Mag",      "Magazine",    0.7f, +0.0f, +0.0f, +0.0f, -2.0f, -4.0f},
        {"75-Round Drum Magazine",     "Magazine",    1.8f, +0.0f, -6.0f, +0.0f, -5.0f,-14.0f},

        // Stocks & Recoil Pads
        {"Rubber Buttpad",             "Stock",       0.2f, +0.0f, +2.0f, +0.0f, +6.0f, -1.0f},
        {"Heavy Duty Recoil Pad",      "Stock",       0.4f, +0.0f, +4.0f, +0.0f, +10.0f, -3.0f}
    };
}

float GrozaEngine::GetTotalWeight() const {
    float total = baseStats.weight;
    for (const auto& att : equippedAttachments) {
        total += att.weight;
    }
    return total;
}

GrozaBaseStats GrozaEngine::CalculateEffectiveStats() const {
    GrozaBaseStats stats = baseStats;

    for (const auto& att : equippedAttachments) {
        stats.damage   += att.damageMod;
        stats.accuracy += att.accuracyMod;
        stats.range    += att.rangeMod;
        stats.control  += att.controlMod;
        stats.mobility += att.mobilityMod;
    }

    return stats;
}

bool GrozaEngine::ToggleAttachment(size_t index) {
    if (index >= availableAttachments.size()) return false;

    const auto& target = availableAttachments[index];
    auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
        [&target](const GrozaAttachment& att) { return att.name == target.name; });

    if (it != equippedAttachments.end()) {
        equippedAttachments.erase(it);
        return true;
    }

    if (equippedAttachments.size() >= 5) return false;
    if (GetTotalWeight() + target.weight > (baseStats.weight + maxWeightCapacity)) return false;

    // Swap attachment if category is already occupied
    equippedAttachments.erase(
        std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&target](const GrozaAttachment& a) { return a.category == target.category; }),
        equippedAttachments.end()
    );

    equippedAttachments.push_back(target);
    return true;
}

bool GrozaEngine::EquipAttachment(const std::string& attachmentName) {
    for (size_t i = 0; i < availableAttachments.size(); ++i) {
        if (availableAttachments[i].name == attachmentName) {
            return ToggleAttachment(i);
        }
    }
    return false;
}

bool GrozaEngine::UnequipAttachment(const std::string& attachmentName) {
    auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
        [&attachmentName](const GrozaAttachment& att) { return att.name == attachmentName; });

    if (it != equippedAttachments.end()) {
        equippedAttachments.erase(it);
        return true;
    }
    return false;
}

void GrozaEngine::ClearAttachments() {
    equippedAttachments.clear();
}

void GrozaEngine::DisplayHeader() const {
    std::cout << "========================================================================\n";
    std::cout << "               BLACKclawX :: GROZA (OTs-14) GUNSMITH SYSTEM            \n";
    std::cout << "========================================================================\n";
    std::cout << " MODEL: " << weaponName << " | CALIBER: " << caliber << "\n";
    
    float currentWeight = GetTotalWeight();
    std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1) 
              << currentWeight << " / " << (baseStats.weight + maxWeightCapacity) << " KG]\n";

    int barWidth = 30;
    float fillRatio = (currentWeight - baseStats.weight) / maxWeightCapacity;
    int filled = static_cast<int>(fillRatio * barWidth);
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) std::cout << "#";
        else std::cout << "-";
    }
    std::cout << "] EQUIPPED ATTACHMENTS: (" << equippedAttachments.size() << " / 5)\n";
    std::cout << "------------------------------------------------------------------------\n";
}

void GrozaEngine::DisplayStatsAndAttachments() const {
    GrozaBaseStats currentStats = CalculateEffectiveStats();

    std::cout << "\n[ REAL-TIME GROZA PERFORMANCE STATS ]\n";
    PrintStatBar("DAMAGE  ", currentStats.damage, 100.0f);
    PrintStatBar("FIRE RATE", currentStats.fireRate, 100.0f);
    PrintStatBar("ACCURACY", currentStats.accuracy, 100.0f);
    PrintStatBar("RANGE   ", currentStats.range, 100.0f);
    PrintStatBar("CONTROL ", currentStats.control, 100.0f);
    PrintStatBar("MOBILITY", currentStats.mobility, 100.0f);

    std::cout << "\n------------------------------------------------------------------------\n";
    std::cout << " AVAILABLE ATTACHMENTS FOR GROZA:\n";
    std::cout << "------------------------------------------------------------------------\n";

    for (size_t i = 0; i < availableAttachments.size(); ++i) {
        const auto& att = availableAttachments[i];
        bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
            [&att](const GrozaAttachment& e) { return e.name == att.name; });

        std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] " 
                  << std::left << std::setw(28) << att.name 
                  << " Slot: " << std::setw(12) << att.category 
                  << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
    }
    std::cout << "------------------------------------------------------------------------\n";
}

void GrozaEngine::PrintStatBar(const std::string& label, float value, float maxValue) const {
    int barLength = 20;
    int filled = static_cast<int>((value / maxValue) * barLength);
    if (filled > barLength) filled = barLength;
    if (filled < 0) filled = 0;

    std::cout << " " << label << " [";
    for (int i = 0; i < barLength; ++i) {
        if (i < filled) std::cout << "=";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(value) << "\n";
}

} // namespace BlackclawX

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace BlackclawX {

struct HBRa3Attachment {
    std::string name;
    std::string category; // Muzzle, Optics, Stock, Laser, Underbarrel, Rear Grip, Ammunition
    float weight;         // in kg
    float damageMod;      
    float accuracyMod;    
    float rangeMod;       
    float controlMod;     
    float mobilityMod;    
};

struct HBRa3BaseStats {
    float damage;
    float baseFireRate;      // Standard RPM after first 3 shots
    float initialBurstRPM;   // Accelerated RPM for the first 3 shots
    float accuracy;
    float range;
    float control;
    float mobility;
    float weight;
};

class HBRa3Engine {
private:
    std::string weaponName;
    std::string caliber;
    
    HBRa3BaseStats baseStats;
    float maxWeightCapacity;

    std::vector<HBRa3Attachment> availableAttachments;
    std::vector<HBRa3Attachment> equippedAttachments;

public:
    HBRa3Engine();
    ~HBRa3Engine() = default;

    // Loadout Controls
    bool ToggleAttachment(size_t index);
    bool EquipAttachment(const std::string& attachmentName);
    bool UnequipAttachment(const std::string& attachmentName);
    void ClearAttachments();

    // Stat & Property Queries
    float GetTotalWeight() const;
    HBRa3BaseStats CalculateEffectiveStats() const;
    
    // Engine Rendering
    void DisplayHeader() const;
    void DisplayStatsAndAttachments() const;

private:
    void InitializeAttachments();
    void PrintStatBar(const std::string& label, float value, float maxValue) const;
};

} // namespace BlackclawX

#include "HBRa3Engine.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace BlackclawX {

HBRa3Engine::HBRa3Engine()
    : weaponName("HBRa3 - Insanity"),
      caliber("5.56x45mm NATO"),
      maxWeightCapacity(7.0f) {
    
    // Standard base configuration: 625 RPM base fire rate with 750 RPM initial 3-round burst
    baseStats = { 38.0f, 625.0f, 750.0f, 60.0f, 56.0f, 52.0f, 68.0f, 3.2f };
    InitializeAttachments();
}

void HBRa3Engine::InitializeAttachments() {
    availableAttachments = {
        // Muzzles
        {"Monolithic Suppressor",   "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
        {"Tactical Suppressor",     "Muzzle",      0.2f, +0.0f, +0.0f,  +5.0f, +0.0f, -2.0f},
        {"RTC Light Muzzle Brake",  "Muzzle",      0.3f, +0.0f, +4.0f,  +0.0f, +7.0f, -2.0f},

        // Optics
        {"Red Dot Sight",           "Optic",       0.2f, +0.0f, +3.0f,  +0.0f, +0.0f, -1.0f},
        {"Holographic Sight",       "Optic",       0.3f, +0.0f, +4.0f,  +0.0f, +1.0f, -2.0f},
        {"3x Tactical Scope",       "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},

        // Stocks
        {"No Stock",                "Stock",      -0.7f, +0.0f, -5.0f,  +0.0f, -7.0f,+10.0f},
        {"RTC Steady Stock",        "Stock",       0.6f, +0.0f, +5.0f,  +0.0f, +6.0f, -3.0f},

        // Lasers
        {"OWC Laser - Tactical",    "Laser",       0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, +4.0f},

        // Underbarrel
        {"Ranger Foregrip",         "Underbarrel", 0.5f, +0.0f, +6.0f,  +0.0f, +9.0f, -4.0f},
        {"Strike Foregrip",         "Underbarrel", 0.3f, +0.0f, +3.0f,  +0.0f, +2.0f, -1.0f},

        // Magazines
        {"44 Round Extended Mag",   "Magazine",    0.8f, +0.0f, +0.0f,  +0.0f, +0.0f, -4.0f},
        {"Fast Reload Mag",         "Magazine",    0.3f, +0.0f, +0.0f,  +0.0f, +0.0f, +2.0f},

        // Rear Grips
        {"Granulated Grip Tape",    "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
        {"Stippled Grip Tape",      "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f}
    };
}

float HBRa3Engine::GetTotalWeight() const {
    float total = baseStats.weight;
    for (const auto& att : equippedAttachments) {
        total += att.weight;
    }
    return total;
}

HBRa3BaseStats HBRa3Engine::CalculateEffectiveStats() const {
    HBRa3BaseStats stats = baseStats;

    for (const auto& att : equippedAttachments) {
        stats.damage   += att.damageMod;
        stats.accuracy += att.accuracyMod;
        stats.range    += att.rangeMod;
        stats.control  += att.controlMod;
        stats.mobility += att.mobilityMod;
    }

    return stats;
}

bool HBRa3Engine::ToggleAttachment(size_t index) {
    if (index >= availableAttachments.size()) return false;

    const auto& target = availableAttachments[index];
    auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
        [&target](const HBRa3Attachment& att) { return att.name == target.name; });

    if (it != equippedAttachments.end()) {
        equippedAttachments.erase(it);
        return true;
    }

    if (equippedAttachments.size() >= 5) return false;
    if (GetTotalWeight() + target.weight > (baseStats.weight + maxWeightCapacity)) return false;

    // Auto-swap attachment if category slot is occupied
    equippedAttachments.erase(
        std::remove_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&target](const HBRa3Attachment& a) { return a.category == target.category; }),
        equippedAttachments.end()
    );

    equippedAttachments.push_back(target);
    return true;
}

bool HBRa3Engine::EquipAttachment(const std::string& attachmentName) {
    for (size_t i = 0; i < availableAttachments.size(); ++i) {
        if (availableAttachments[i].name == attachmentName) {
            return ToggleAttachment(i);
        }
    }
    return false;
}

bool HBRa3Engine::UnequipAttachment(const std::string& attachmentName) {
    auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
        [&attachmentName](const HBRa3Attachment& att) { return att.name == attachmentName; });

    if (it != equippedAttachments.end()) {
        equippedAttachments.erase(it);
        return true;
    }
    return false;
}

void HBRa3Engine::ClearAttachments() {
    equippedAttachments.clear();
}

void HBRa3Engine::DisplayHeader() const {
    std::cout << "========================================================================\n";
    std::cout << "               BLACKclawX :: HBRa3 GUNSMITH SYSTEM                      \n";
    std::cout << "========================================================================\n";
    std::cout << " MODEL: " << weaponName << " | CALIBER: " << caliber << "\n";
    std::cout << " SPECIAL PASSIVE: High Fire-Rate for First 3 Rounds (" 
              << static_cast<int>(baseStats.initialBurstRPM) << " RPM Burst -> " 
              << static_cast<int>(baseStats.baseFireRate) << " RPM Sustained)\n";
    
    float currentWeight = GetTotalWeight();
    std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1) 
              << currentWeight << " / " << (baseStats.weight + maxWeightCapacity) << " KG]\n";

    int barWidth = 30;
    float fillRatio = (currentWeight - baseStats.weight) / maxWeightCapacity;
    int filled = static_cast<int>(fillRatio * barWidth);
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) std::cout << "#";
        else std::cout << "-";
    }
    std::cout << "] EQUIPPED ATTACHMENTS: (" << equippedAttachments.size() << " / 5)\n";
    std::cout << "------------------------------------------------------------------------\n";
}

void HBRa3Engine::DisplayStatsAndAttachments() const {
    HBRa3BaseStats currentStats = CalculateEffectiveStats();

    std::cout << "\n[ REAL-TIME HBRa3 PERFORMANCE STATS ]\n";
    PrintStatBar("DAMAGE    ", currentStats.damage, 100.0f);
    PrintStatBar("ACCURACY  ", currentStats.accuracy, 100.0f);
    PrintStatBar("RANGE     ", currentStats.range, 100.0f);
    PrintStatBar("CONTROL   ", currentStats.control, 100.0f);
    PrintStatBar("MOBILITY  ", currentStats.mobility, 100.0f);

    std::cout << "\n------------------------------------------------------------------------\n";
    std::cout << " AVAILABLE ATTACHMENTS FOR HBRa3:\n";
    std::cout << "------------------------------------------------------------------------\n";

    for (size_t i = 0; i < availableAttachments.size(); ++i) {
        const auto& att = availableAttachments[i];
        bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
            [&att](const HBRa3Attachment& e) { return e.name == att.name; });

        std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] " 
                  << std::left << std::setw(28) << att.name 
                  << " Slot: " << std::setw(12) << att.category 
                  << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
    }
    std::cout << "------------------------------------------------------------------------\n";
}

void HBRa3Engine::PrintStatBar(const std::string& label, float value, float maxValue) const {
    int barLength = 20;
    int filled = static_cast<int>((value / maxValue) * barLength);
    if (filled > barLength) filled = barLength;
    if (filled < 0) filled = 0;

    std::cout << " " << label << " [";
    for (int i = 0; i < barLength; ++i) {
        if (i < filled) std::cout << "=";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(value) << "\n";
}

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class LAG53Engine : public BaseWeaponEngine {
private:
    float headshotMultiplier;
    bool highCaliberEquipped;

public:
    LAG53Engine()
        : BaseWeaponEngine("LAG 53", ".308 Winchester / 7.62x51mm NATO", 
                           {45.0f, 420.0f, 64.0f, 62.0f, 58.0f, 60.0f, 4.1f}, 8.0f),
          headshotMultiplier(1.6f),
          highCaliberEquipped(false) {
        
        InitializeAttachments();
    }

    // Passive description specific to LAG 53
    std::string GetSpecialPassiveText() const override {
        std::string text = "High Single-Shot Stopping Power (Headshot Multiplier: " + 
                           std::to_string(headshotMultiplier).substr(0, 4) + "x)";
        if (highCaliberEquipped) {
            text += " [HIGH-CALIBER AMMO ACTIVE]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        highCaliberEquipped = false;
        headshotMultiplier = 1.6f;

        for (const auto& att : equippedAttachments) {
            if (att.name == ".308 High-Caliber Mags") {
                highCaliberEquipped = true;
                headshotMultiplier = 2.0f; // Boosts critical multiplier
            }
        }
    }

    // Extended Damage Profile calculation for LAG 53
    float CalculateHeadshotDamage() const {
        WeaponStats stats = CalculateEffectiveStats();
        return stats.damage * headshotMultiplier;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Tactical Suppressor",       "Muzzle",      0.3f, +0.0f, +0.0f,  +6.0f, +0.0f, -2.0f},
            {"Agency Suppressor",         "Muzzle",      0.5f, +0.0f, -2.0f, +14.0f, +0.0f, -5.0f},
            {"Infantry Compensator",      "Muzzle",      0.4f, +0.0f, +5.0f,  +0.0f, +8.0f, -3.0f},

            // Barrels
            {"XRK Marksman Barrel",       "Barrel",      1.1f, +3.0f, +8.0f, +18.0f, +5.0f, -9.0f},
            {"16.5\" Heavy Barrel",       "Barrel",      0.8f, +0.0f, +5.0f, +10.0f, +3.0f, -5.0f},

            // Optics
            {"4.0x Flip Hybrid",          "Optic",       0.6f, +0.0f, +7.0f, +10.0f, -3.0f, -5.0f},
            {"Reflex Sight",              "Optic",       0.2f, +0.0f, +2.0f,  +0.0f, +0.0f, -1.0f},

            // Stocks
            {"Heavy Padded Stock",        "Stock",       0.7f, +0.0f, +6.0f,  +0.0f, +7.0f, -4.0f},
            {"Ultralight Stock",          "Stock",      -0.5f, +0.0f, -4.0f,  +0.0f, -5.0f, +8.0f},

            // Underbarrels
            {"Operator Foregrip",         "Underbarrel", 0.6f, +0.0f, +4.0f,  +0.0f, +9.0f, -4.0f},
            {"Bipod",                     "Underbarrel", 0.9f, +0.0f, +9.0f,  +0.0f,+15.0f,-10.0f},

            // Magazines / Ammunition
            {".308 High-Caliber Mags",    "Magazine",    1.2f, +8.0f, -3.0f,  +5.0f, -6.0f, -6.0f},
            {"30 Round Extended Mag",     "Magazine",    0.9f, +0.0f, +0.0f,  +0.0f, -2.0f, -4.0f},

            // Rear Grips
            {"Rubberized Grip Tape",      "Rear Grip",   0.1f, +0.0f, +2.0f,  +0.0f, +5.0f, -1.0f},
            {"Granulated Grip Tape",      "Rear Grip",   0.1f, +0.0f, +5.0f,  +0.0f, +0.0f, -1.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class M13Engine : public BaseWeaponEngine {
private:
    bool blackoutAmmoEquipped;
    float baseBulletVelocity; // meters per second

public:
    M13Engine()
        : BaseWeaponEngine("M13 - Short Stroke Pistol Rifle", "5.56x45mm NATO", 
                           {36.0f, 900.0f, 62.0f, 56.0f, 65.0f, 72.0f, 3.0f}, 7.0f),
          blackoutAmmoEquipped(false),
          baseBulletVelocity(910.0f) {
        
        InitializeAttachments();
    }

    // Passive description specific to M13
    std::string GetSpecialPassiveText() const override {
        std::string text = "High Fire-Rate Piston System (900 RPM Base) | Velocity: " + 
                           std::to_string(static_cast<int>(baseBulletVelocity)) + " m/s";
        if (blackoutAmmoEquipped) {
            text += " [.300 BLACKOUT MAGS ACTIVE - No Skull Markers / Suppressed Tracks]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        blackoutAmmoEquipped = false;
        baseBulletVelocity = 910.0f; // Default 5.56 NATO velocity

        for (const auto& att : equippedAttachments) {
            if (att.name == ".300 Blackout 30-Round Mags") {
                blackoutAmmoEquipped = true;
                caliber = ".300 AAC Blackout";
                baseBulletVelocity = 680.0f; // Subsonic/Specialized velocity adjustment
            } else if (att.name == "Tempus Marksman Barrel") {
                baseBulletVelocity += 220.0f;
            }
        }

        if (!blackoutAmmoEquipped) {
            caliber = "5.56x45mm NATO";
        }
    }

    // Velocity Calculator
    float GetEffectiveBulletVelocity() const {
        return baseBulletVelocity;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Monolithic Suppressor",     "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
            {"Tactical Suppressor",       "Muzzle",      0.2f, +0.0f, +0.0f,  +5.0f, +0.0f, -2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f, +0.0f, +4.0f,  +0.0f, +7.0f, -2.0f},

            // Barrels
            {"Tempus Marksman Barrel",    "Barrel",      1.1f, +2.0f, +8.0f, +16.0f, +6.0f, -8.0f},
            {"Tempus Mini Barrel",        "Barrel",     -0.6f, +0.0f, -5.0f,  -8.0f, -4.0f, +10.0f},
            {"Tempus Cyclone (Integrated)","Barrel",     0.5f, +0.0f, +3.0f,  +8.0f, +2.0f, -3.0f},

            // Optics
            {"GI Mini Reflex",            "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +0.0f},
            {"VLK 3.0x Optic",            "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},

            // Stocks
            {"M13 Skeleton Stock",        "Stock",      -0.4f, +0.0f, -3.0f,  +0.0f, -4.0f, +8.0f},
            {"Forge TAC Stalker",         "Stock",       0.5f, +0.0f, +5.0f,  +0.0f, +5.0f, -3.0f},

            // Underbarrel
            {"Commando Foregrip",         "Underbarrel", 0.4f, +0.0f, +5.0f,  +0.0f, +4.0f, -2.0f},
            {"Merc Foregrip",             "Underbarrel", 0.5f, +0.0f, +2.0f,  +0.0f, +8.0f, -3.0f},

            // Magazines / Ammunition
            {".300 Blackout 30-Round Mags","Magazine",   0.4f, +5.0f, +0.0f,  +6.0f, -2.0f, -2.0f},
            {"60 Round Mag",              "Magazine",    1.5f, +0.0f, +0.0f,  +0.0f, -3.0f, -8.0f},
            {"50 Round Mag",              "Magazine",    1.1f, +0.0f, +0.0f,  +0.0f, -1.0f, -5.0f},

            // Rear Grips
            {"Granulated Grip Tape",      "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class M16Engine : public BaseWeaponEngine {
private:
    bool burstDelayReduced;
    float burstDelayMs;       // Time in ms between 3-round bursts
    float burstRPM;            // Cyclic rate during the 3-round burst

public:
    M16Engine()
        : BaseWeaponEngine("M16A4 - Burst Assault Rifle", "5.56x45mm NATO", 
                           {40.0f, 480.0f, 68.0f, 64.0f, 62.0f, 65.0f, 3.6f}, 7.5f),
          burstDelayReduced(false),
          burstDelayMs(220.0f),
          burstRPM(900.0f) {
        
        InitializeAttachments();
    }

    // Passive description specific to M16
    std::string GetSpecialPassiveText() const override {
        std::string text = "3-Round Burst Mechanism (" + 
                           std::to_string(static_cast<int>(burstRPM)) + " Cyclic RPM) | Burst Delay: " + 
                           std::to_string(static_cast<int>(burstDelayMs)) + "ms";
        if (burstDelayReduced) {
            text += " [RAPID FIRE TRIGGER EQUIPPED]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        burstDelayReduced = false;
        burstDelayMs = 220.0f;

        for (const auto& att : equippedAttachments) {
            if (att.name == "Rapid Fire Perk") {
                burstDelayReduced = true;
                burstDelayMs = 150.0f; // Reduces inter-burst delay
            }
        }
    }

    // Burst Dynamics Query
    float GetEffectiveBurstDelay() const {
        return burstDelayMs;
    }

    float GetCyclicBurstRPM() const {
        return burstRPM;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Monolithic Suppressor",      "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
            {"Tactical Suppressor",        "Muzzle",      0.2f, +0.0f, +0.0f,  +5.0f, +0.0f, -2.0f},
            {"Muzzle Brake",               "Muzzle",      0.3f, +0.0f, +4.0f,  +0.0f, +6.0f, -2.0f},

            // Barrels
            {"20.0\" Grenadier Barrel",    "Barrel",      1.3f, +3.0f, +9.0f, +18.0f, +7.0f, -9.0f},
            {"16.3\" Factory Barrel",      "Barrel",      0.6f, +0.0f, +3.0f,  +6.0f, +2.0f, -3.0f},

            // Optics
            {"4.0x Flip Hybrid",           "Optic",       0.6f, +0.0f, +7.0f, +10.0f, -3.0f, -5.0f},
            {"Classic Carry Handle Sight", "Optic",       0.1f, +0.0f, +1.0f,  +0.0f, +0.0f, +1.0f},
            {"Integral 3.0x Scope",        "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},

            // Stocks
            {"M16 Stock",                  "Stock",       0.5f, +0.0f, +5.0f,  +0.0f, +5.0f, -3.0f},
            {"No Stock",                   "Stock",      -0.7f, +0.0f, -6.0f,  +0.0f, -8.0f,+11.0f},

            // Underbarrels
            {"M203 40mm Launcher",         "Underbarrel", 1.6f, +0.0f, -4.0f,  +0.0f,+12.0f,-11.0f},
            {"Operator Foregrip",          "Underbarrel", 0.6f, +0.0f, +4.0f,  +0.0f, +8.0f, -4.0f},
            {"Commando Foregrip",          "Underbarrel", 0.4f, +0.0f, +5.0f,  +0.0f, +3.0f, -2.0f},

            // Magazines / Perks
            {"48 Round Extended Mag",      "Magazine",    1.0f, +0.0f, +0.0f,  +0.0f, -2.0f, -5.0f},
            {"Rapid Fire Perk",            "Perk",        0.0f, +0.0f, +0.0f,  +0.0f, -3.0f, +0.0f},
            {"Slight of Hand",             "Perk",        0.0f, +0.0f, +0.0f,  +0.0f, +0.0f, +3.0f},

            // Rear Grips
            {"Granulated Grip Tape",       "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
            {"Stippled Grip Tape",         "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class Lachmann556Engine : public BaseWeaponEngine {
private:
    bool heavyBarrelEquipped;
    float rollerDelayStabilityBonus;

public:
    Lachmann556Engine()
        : BaseWeaponEngine("Lachmann-556", "5.56x45mm NATO", 
                           {38.0f, 800.0f, 63.0f, 58.0f, 66.0f, 70.0f, 3.3f}, 7.2f),
          heavyBarrelEquipped(false),
          rollerDelayStabilityBonus(15.0f) {
        
        InitializeAttachments();
    }

    // Passive description specific to Lachmann-556
    std::string GetSpecialPassiveText() const override {
        std::string text = "Roller-Delayed Blowback System (+15% Recoil Stabilization Base)";
        if (heavyBarrelEquipped) {
            text += " [15.9\" LACHMANN RAPP BARREL - Max Range & Velocity]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        heavyBarrelEquipped = false;

        for (const auto& att : equippedAttachments) {
            if (att.name == "15.9\" Lachmann RAPP Barrel") {
                heavyBarrelEquipped = true;
            }
        }
    }

    // System-specific stability calculation
    float GetEffectiveStabilityScore() const {
        WeaponStats stats = CalculateEffectiveStats();
        return stats.control + rollerDelayStabilityBonus;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Echoless-80",                "Muzzle",      0.4f, +0.0f, -2.0f, +10.0f, +2.0f, -4.0f},
            {"Harbinger D20",              "Muzzle",      0.6f, +1.0f, -3.0f, +14.0f, +4.0f, -6.0f},
            {"Sakin Tread-40",             "Muzzle",      0.3f, +0.0f, +5.0f,  +0.0f, +7.0f, -2.0f},

            // Barrels
            {"15.9\" Lachmann RAPP Barrel", "Barrel",      1.1f, +2.0f, +8.0f, +16.0f, +5.0f, -8.0f},
            {"12\" Lachmann S-9",          "Barrel",     -0.5f, +0.0f, -4.0f,  -6.0f, -3.0f, +9.0f},
            {"Lachmann NOVA",              "Barrel",      0.7f, +0.0f, +4.0f,  +8.0f, +3.0f, -4.0f},

            // Optics
            {"Cronen Mini Pro",            "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +0.0f},
            {"SZ Holotherm",               "Optic",       0.3f, +0.0f, +4.0f,  +0.0f, +0.0f, -2.0f},
            {"Aim OP-V4",                  "Optic",       0.2f, +0.0f, +3.0f,  +2.0f, +1.0f, -1.0f},

            // Stocks
            {"Lachmann S9 Factory",        "Stock",       0.4f, +0.0f, +4.0f,  +0.0f, +5.0f, -3.0f},
            {"FT Mobile Stock",            "Stock",      -0.4f, +0.0f, -3.0f,  +0.0f, -4.0f, +7.0f},
            {"LM S-38 Stock Factory",      "Stock",       0.6f, +0.0f, +6.0f,  +0.0f, +8.0f, -5.0f},

            // Underbarrel
            {"FTAC Ripper 56",             "Underbarrel", 0.5f, +0.0f, +6.0f,  +0.0f, +8.0f, -4.0f},
            {"VX Pineapple",               "Underbarrel", 0.4f, +0.0f, +4.0f,  +0.0f, +6.0f, -3.0f},
            {"Lockgrip BF120",             "Underbarrel", 0.3f, +0.0f, +3.0f,  +0.0f, +5.0f, -2.0f},

            // Magazines
            {"40 Round Mag",               "Magazine",    0.6f, +0.0f, +0.0f,  +0.0f, -1.0f, -3.0f},
            {"60 Round Drum",              "Magazine",    1.6f, +0.0f, -4.0f,  +0.0f, -4.0f, -9.0f},

            // Rear Grips
            {"LKM-75 Grip",                "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +1.0f, -1.0f},
            {"Lachmann TCG-10",            "Rear Grip",   0.1f, +0.0f, -2.0f,  +0.0f, +4.0f, -1.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class MaddoxEngine : public BaseWeaponEngine {
private:
    bool echoFireEquipped;
    bool fastMagIIEquipped;
    float echoFireRPM;

public:
    MaddoxEngine()
        : BaseWeaponEngine("Maddox RFB", "5.56x45mm NATO", 
                           {36.0f, 720.0f, 62.0f, 52.0f, 58.0f, 76.0f, 3.1f}, 6.8f),
          echoFireEquipped(false),
          fastMagIIEquipped(false),
          echoFireRPM(860.0f) {
        
        InitializeAttachments();
    }

    // Passive description specific to Maddox RFB
    std::string GetSpecialPassiveText() const override {
        std::string text = "Bullpup High-Agility Profile";
        if (echoFireEquipped) {
            text += " [OPERATOR MOD: ECHO FIRE ACTIVE - Rapid 2-Round Cyclic Burst Mode (" + 
                    std::to_string(static_cast<int>(echoFireRPM)) + " RPM)]";
        }
        if (fastMagIIEquipped) {
            text += " [FAST MAG II - Ultra-Instant Magazine Ejection]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        echoFireEquipped = false;
        fastMagIIEquipped = false;

        for (const auto& att : equippedAttachments) {
            if (att.name == "Operator Mod: Echo Fire") {
                echoFireEquipped = true;
            } else if (att.name == "Fast Mag II") {
                fastMagIIEquipped = true;
            }
        }
    }

    // Custom query for firing profile
    float GetEffectiveRPM() const {
        if (echoFireEquipped) {
            return echoFireRPM;
        }
        return CalculateEffectiveStats().fireRate;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Suppressor",                 "Muzzle",      0.3f, +0.0f, -2.0f, +8.0f,  +0.0f, -3.0f},
            {"Muzzle Brake",               "Muzzle",      0.3f, +0.0f, +4.0f, +0.0f,  +6.0f, -2.0f},

            // Optics
            {"Elo Sight",                  "Optic",       0.1f, +0.0f, +3.0f, +0.0f,  +0.0f, +2.0f},
            {"Reflex Sight",               "Optic",       0.2f, +0.0f, +2.0f, +0.0f,  +0.0f, -1.0f},
            {"Trojan Optic",               "Optic",       0.4f, +0.0f, +4.0f, +4.0f,  -1.0f, -3.0f},

            // Barrel / Receiver Options
            {"Echo Fire Mod",              "Operator Mod",0.8f, +4.0f, -6.0f, +0.0f,  -5.0f, -4.0f},
            {"Laser Sight I",              "Laser",       0.1f, +0.0f, +5.0f, +0.0f,  +0.0f, +4.0f},
            {"Laser Sight II",             "Laser",       0.2f, +0.0f, +9.0f, +0.0f,  +0.0f, +7.0f},

            // Stocks
            {"Stock I",                    "Stock",      -0.4f, +0.0f, -2.0f, +0.0f,  -3.0f, +8.0f},
            {"Stock II",                   "Stock",      -0.7f, +0.0f, -5.0f, +0.0f,  -6.0f, +14.0f},

            // Underbarrels
            {"Quickdraw I",                "Underbarrel", 0.2f, +0.0f, +0.0f, +0.0f,  +0.0f, +8.0f},
            {"Quickdraw II",               "Underbarrel", 0.3f, +0.0f, +0.0f, +0.0f,  +0.0f, +15.0f},

            // Magazines
            {"Fast Mag I",                 "Magazine",    0.3f, +0.0f, +0.0f, +0.0f,  +0.0f, +3.0f},
            {"Fast Mag II",                "Magazine",    0.5f, +0.0f, +0.0f, +0.0f,  +0.0f, +7.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class ASM10Engine : public BaseWeaponEngine {
private:
    float hyperBurstRPM;
    float sustainedRPM;
    bool extendedBarrelEquipped;
    bool electricReloadEquipped;

public:
    ASM10Engine()
        : BaseWeaponEngine("ASM10 - Hyper-Burst Rifle", "5.45x39mm", 
                           {48.0f, 500.0f, 66.0f, 62.0f, 60.0f, 55.0f, 3.8f}, 7.8f),
          hyperBurstRPM(1800.0f),
          sustainedRPM(500.0f),
          extendedBarrelEquipped(false),
          electricReloadEquipped(false) {
        
        InitializeAttachments();
    }

    // Passive description specific to ASM10
    std::string GetSpecialPassiveText() const override {
        std::string text = "2-Round Hyper-Burst Passive (" + 
                           std::to_string(static_cast<int>(hyperBurstRPM)) + " RPM First 2 Rounds -> " + 
                           std::to_string(static_cast<int>(sustainedRPM)) + " RPM Sustained)";
        if (extendedBarrelEquipped) {
            text += " [OWC MARKSMAN - Extended 3-Shot Elimination Range]";
        }
        if (electricReloadEquipped) {
            text += " [SLIGHT OF HAND EQUIPPED]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        extendedBarrelEquipped = false;
        electricReloadEquipped = false;

        for (const auto& att : equippedAttachments) {
            if (att.name == "OWC Marksman Barrel") {
                extendedBarrelEquipped = true;
            } else if (att.name == "Slight of Hand") {
                electricReloadEquipped = true;
            }
        }
    }

    // Specialized query for initial burst rate vs sustained rate
    float GetHyperBurstRPM() const {
        return hyperBurstRPM;
    }

    float GetSustainedRPM() const {
        return CalculateEffectiveStats().fireRate;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"RTC Light Muzzle Brake",     "Muzzle",      0.3f, +0.0f, +4.0f, +0.0f,  +7.0f, -2.0f},
            {"Monolithic Suppressor",      "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
            {"Tactical Suppressor",        "Muzzle",      0.2f, +0.0f, +0.0f, +5.0f,  +0.0f, -2.0f},

            // Barrels
            {"OWC Marksman Barrel",        "Barrel",      1.2f, +3.0f, +8.0f, +18.0f, +6.0f, -9.0f},
            {"MIP Ranger Barrel",          "Barrel",      0.8f, +0.0f, +5.0f, +10.0f, +4.0f, -5.0f},
            {"MIP Light Barrel (Short)",   "Barrel",     -0.5f, +0.0f, -4.0f, -8.0f,  -3.0f, +8.0f},

            // Optics
            {"Classic Red Dot Sight",      "Optic",       0.2f, +0.0f, +3.0f, +0.0f,  +0.0f, -1.0f},
            {"3x Tactical Scope",          "Optic",       0.5f, +0.0f, +6.0f, +8.0f,  -2.0f, -4.0f},
            {"Holographic Sight 1",        "Optic",       0.3f, +0.0f, +4.0f, +0.0f,  +1.0f, -2.0f},

            // Stocks
            {"No Stock",                   "Stock",      -0.8f, +0.0f, -6.0f, +0.0f,  -8.0f, +12.0f},
            {"RTC Steady Stock",           "Stock",       0.6f, +0.0f, +5.0f, +0.0f,  +6.0f, -3.0f},
            {"MIP Strike Stock",           "Stock",       0.4f, +0.0f, +3.0f, +0.0f,  +4.0f, -2.0f},

            // Underbarrels
            {"Strike Foregrip",            "Underbarrel", 0.3f, +0.0f, +3.0f, +0.0f,  +2.0f, -1.0f},
            {"Ranger Foregrip",            "Underbarrel", 0.5f, +0.0f, +6.0f, +0.0f,  +9.0f, -4.0f},
            {"Operator Foregrip",          "Underbarrel", 0.6f, +0.0f, +4.0f, +0.0f,  +8.0f, -4.0f},

            // Magazines / Perks
            {"40 Round Extended Mag",      "Magazine",    0.8f, +0.0f, +0.0f, +0.0f,  -1.0f, -4.0f},
            {"45 Round Extended Mag",      "Magazine",    1.2f, +0.0f, +0.0f, +0.0f,  -3.0f, -6.0f},
            {"Slight of Hand",             "Perk",        0.0f, +0.0f, +0.0f, +0.0f,  +0.0f, +3.0f},

            // Rear Grips
            {"Granulated Grip Tape",       "Rear Grip",   0.1f, +0.0f, +4.0f, +0.0f,  +0.0f, -1.0f},
            {"Stippled Grip Tape",         "Rear Grip",   0.1f, +0.0f, -3.0f, +0.0f,  -2.0f, +6.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class CR56AMAXEngine : public BaseWeaponEngine {
private:
    bool m67ConversionEquipped;
    bool isSemiAuto;
    float headshotMultiplier;

public:
    CR56AMAXEngine()
        : BaseWeaponEngine("CR-56 AMAX", "7.62x39mm", 
                           {42.0f, 640.0f, 63.0f, 62.0f, 58.0f, 66.0f, 3.6f}, 7.6f),
          m67ConversionEquipped(false),
          isSemiAuto(false),
          headshotMultiplier(1.5f) {
        
        InitializeAttachments();
    }

    // Passive description specific to CR-56 AMAX
    std::string GetSpecialPassiveText() const override {
        std::string text = "7.62 High Impact Profile (Strong Upper Torso / Headshot Multipliers)";
        if (m67ConversionEquipped) {
            text += " [M67 10-RND MAGS EQUIPPED - Semi-Auto Conversion | 2.1x Headshot Multiplier]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        m67ConversionEquipped = false;
        isSemiAuto = false;
        headshotMultiplier = 1.5f;

        for (const auto& att : equippedAttachments) {
            if (att.name == "M67 10-Round Mags") {
                m67ConversionEquipped = true;
                isSemiAuto = true;
                headshotMultiplier = 2.1f;
            }
        }
    }

    // Custom query for firing mode & multiplier
    bool IsSemiAutomatic() const {
        return isSemiAuto;
    }

    float GetHeadshotMultiplier() const {
        return headshotMultiplier;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Monolithic Suppressor",      "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
            {"Tactical Suppressor",        "Muzzle",      0.2f, +0.0f, +0.0f,  +5.0f, +0.0f, -2.0f},
            {"XRK CR-56 Muzzle Brake",     "Muzzle",      0.3f, +0.0f, +5.0f,  +0.0f, +6.0f, -2.0f},

            // Barrels
            {"XRK Zodiac S440",            "Barrel",      1.2f, +3.0f, +8.0f, +18.0f, +6.0f, -9.0f},
            {"XRK Squall 11.8\" Short",    "Barrel",     -0.6f, +0.0f, -5.0f,  -8.0f, -4.0f, +10.0f},
            {"FSS 8.3\" Intruder",         "Barrel",     -0.8f, +0.0f, -7.0f, -12.0f, -6.0f, +14.0f},

            // Optics
            {"VLK 3.0x Optic",             "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},
            {"GI Mini Reflex",             "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +0.0f},
            {"Corp Combat Holo Sight",     "Optic",       0.3f, +0.0f, +4.0f,  +0.0f, +0.0f, -2.0f},

            // Stocks
            {"XRK Gatekeeper",             "Optic",       0.5f, +0.0f, +5.0f,  +0.0f, +5.0f, -3.0f},
            {"CR-56 AMAX Exo",             "Stock",      -0.4f, +0.0f, -3.0f,  +0.0f, -4.0f, +8.0f},
            {"No Stock",                   "Stock",      -0.8f, +0.0f, -6.0f,  +0.0f, -8.0f,+12.0f},

            // Underbarrel
            {"Commando Foregrip",          "Underbarrel", 0.4f, +0.0f, +5.0f,  +0.0f, +4.0f, -2.0f},
            {"Merc Foregrip",              "Underbarrel", 0.5f, +0.0f, +2.0f,  +0.0f, +8.0f, -3.0f},
            {"Ranger Foregrip",            "Underbarrel", 0.6f, +0.0f, +6.0f,  +0.0f, +9.0f, -4.0f},

            // Magazines
            {"45 Round Mags",              "Magazine",    0.8f, +0.0f, +0.0f,  +0.0f, -2.0f, -4.0f},
            {"M67 10-Round Mags",          "Magazine",   -0.3f, +8.0f, +8.0f, +14.0f, +4.0f, +2.0f},

            // Rear Grips
            {"Granulated Grip Tape",       "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
            {"XRK CR-56 Stippled Wrap",    "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class FFAR1Engine : public BaseWeaponEngine {
private:
    bool fastMagEquipped;
    bool heavyBarrelEquipped;
    float baseReloadTimeSec;

public:
    FFAR1Engine()
        : BaseWeaponEngine("FFAR 1 - Bullpup Assault Rifle", "5.56x45mm NATO", 
                           {33.0f, 912.0f, 58.0f, 51.0f, 52.0f, 78.0f, 3.2f}, 7.0f),
          fastMagEquipped(false),
          heavyBarrelEquipped(false),
          baseReloadTimeSec(2.4f) {
        
        InitializeAttachments();
    }

    // Passive description specific to FFAR 1
    std::string GetSpecialPassiveText() const override {
        std::string text = "Ultra-Fast Cycle Rate (912 RPM Base) | Bullpup Close-Quarters Handling";
        if (heavyBarrelEquipped) {
            text += " [21.2\" TASK FORCE BARREL - Enhanced Bullet Velocity & Effective Range]";
        }
        if (fastMagEquipped) {
            text += " [SALVO 50 RND FAST MAG - Fast Ejection Cycle Active]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        fastMagEquipped = false;
        heavyBarrelEquipped = false;
        baseReloadTimeSec = 2.4f;

        for (const auto& att : equippedAttachments) {
            if (att.name == "Salvo 50 Rnd Fast Mag" || att.name == "44 Rnd Fast Mag") {
                fastMagEquipped = true;
                baseReloadTimeSec = 1.7f;
            } else if (att.name == "21.2\" Task Force") {
                heavyBarrelEquipped = true;
            }
        }
    }

    // Reload Speed Query
    float GetEffectiveReloadTime() const {
        return baseReloadTimeSec;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Agency Suppressor",          "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +2.0f, -4.0f},
            {"Infiltrator Muzzle Brake",   "Muzzle",      0.2f, +0.0f, +4.0f,  +0.0f, +5.0f, -2.0f},
            {"Infantry Compensator",       "Muzzle",      0.3f, +0.0f, +6.0f,  +0.0f, +8.0f, -3.0f},

            // Barrels
            {"21.2\" Task Force",          "Barrel",      1.1f, +3.0f, +6.0f, +16.0f, +4.0f, -8.0f},
            {"19.5\" Reinforced Heavy",    "Barrel",      0.8f, +0.0f, +4.0f, +10.0f, +3.0f, -5.0f},
            {"17.9\" Takedown",            "Barrel",      0.5f, +0.0f, +2.0f,  +8.0f, +0.0f, -3.0f},

            // Optics
            {"Microflex LED",              "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +1.0f},
            {"Axial Arms 3.0x",            "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},
            {"Sillix Holoscout",           "Optic",       0.3f, +0.0f, +3.0f,  +2.0f, +0.0f, -2.0f},

            // Stocks
            {"Raider Stock",               "Stock",      -0.5f, +0.0f, -4.0f,  +0.0f, -5.0f,+10.0f},
            {"SAS Combat Stock",           "Stock",      -0.3f, +0.0f, -2.0f,  +0.0f, -3.0f, +6.0f},
            {"Tactical Stock",             "Stock",       0.2f, +0.0f, +3.0f,  +0.0f, +2.0f, -1.0f},

            // Underbarrel
            {"Field Agent Grip",           "Underbarrel", 0.5f, +0.0f, +6.0f,  +0.0f, +9.0f, -4.0f},
            {"Bruiser Grip",               "Underbarrel", 0.2f, +0.0f, +0.0f,  +0.0f, +2.0f, +4.0f},
            {"Infiltrator Grip",           "Underbarrel", 0.2f, +0.0f, +2.0f,  +0.0f, +0.0f, +3.0f},

            // Magazines
            {"Salvo 50 Rnd Fast Mag",      "Magazine",    1.1f, +0.0f, -3.0f,  +0.0f, -3.0f, -5.0f},
            {"STANAG 50 Rnd Mag",          "Magazine",    1.0f, +0.0f, +0.0f,  +0.0f, -2.0f, -6.0f},
            {"44 Rnd Fast Mag",            "Magazine",    0.6f, +0.0f, -1.0f,  +0.0f, -1.0f, -2.0f},

            // Rear Grips
            {"Serpent Wrap",               "Rear Grip",   0.1f, +0.0f, -2.0f,  +0.0f, -1.0f, +5.0f},
            {"Airborne Elastic Wrap",      "Rear Grip",   0.2f, +0.0f, +3.0f,  +0.0f, +2.0f, +3.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class ASVALEngine : public BaseWeaponEngine {
private:
    bool sppConversionEquipped;
    bool isSemiAuto;
    float armorPenetrationMultiplier;

public:
    ASVALEngine()
        : BaseWeaponEngine("AS VAL - Integrally Suppressed Rifle", "9x39mm Subsonic", 
                           {37.0f, 900.0f, 59.0f, 48.0f, 54.0f, 75.0f, 2.5f}, 6.5f),
          sppConversionEquipped(false),
          isSemiAuto(false),
          armorPenetrationMultiplier(1.0f) {
        
        InitializeAttachments();
    }

    // Passive description specific to AS VAL
    std::string GetSpecialPassiveText() const override {
        std::string text = "Integral Suppressor System (Permanent Radar Stealth & Reduced Audible Footprint)";
        if (sppConversionEquipped) {
            text += " [SPP 10-RND MAGS - DMR Semi-Auto Conversion | High Armor Penetration]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        sppConversionEquipped = false;
        isSemiAuto = false;
        armorPenetrationMultiplier = 1.0f;

        for (const auto& att : equippedAttachments) {
            if (att.name == "SPP 10-Rnd Mags") {
                sppConversionEquipped = true;
                isSemiAuto = true;
                armorPenetrationMultiplier = 1.85f;
            }
        }
    }

    // Custom query for firing mode & armor penetration profile
    bool IsSemiAutomatic() const {
        return isSemiAuto;
    }

    float GetArmorPenetrationMultiplier() const {
        return armorPenetrationMultiplier;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Barrels (Replaces standard muzzle options due to integral suppression)
            {"VLK 200mm Osa",              "Barrel",      0.6f, +2.0f, +6.0f, +12.0f, +3.0f, -5.0f},
            {"VLK 105mm Sova",             "Barrel",     -0.4f, +0.0f, -4.0f,  -6.0f, -3.0f, +8.0f},

            // Optics
            {"VLK 3.0x Optic",             "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},
            {"GI Mini Reflex",             "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +0.0f},
            {"Solozero Optics Mini Reflex","Optic",       0.2f, +0.0f, +3.0f,  +0.0f, +0.0f, -1.0f},

            // Stocks
            {"VLK Strikertac Guard",       "Stock",       0.4f, +0.0f, +4.0f,  +0.0f, +4.0f, -2.0f},
            {"FSS Scanline Pro",           "Stock",       0.5f, +0.0f, +5.0f,  +0.0f, +6.0f, -3.0f},
            {"VLK Vintage Crest",          "Stock",      -0.5f, +0.0f, -4.0f,  +0.0f, -5.0f, +9.0f},

            // Underbarrel
            {"Commando Foregrip",          "Underbarrel", 0.4f, +0.0f, +5.0f,  +0.0f, +4.0f, -2.0f},
            {"Merc Foregrip",              "Underbarrel", 0.5f, +0.0f, +2.0f,  +0.0f, +8.0f, -3.0f},
            {"Operator Foregrip",          "Underbarrel", 0.6f, +0.0f, +4.0f,  +0.0f, +7.0f, -4.0f},

            // Magazines
            {"30 Round Mags",              "Magazine",    0.5f, +0.0f, +0.0f,  +0.0f, -1.0f, -3.0f},
            {"SPP 10-Rnd Mags",            "Magazine",   -0.3f, +10.0f,+8.0f, +16.0f, +5.0f, +4.0f},

            // Rear Grips
            {"Granulated Grip Tape",       "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
            {"Stippled Grip Tape",         "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f},
            {"Rubberized Grip Tape",       "Rear Grip",   0.1f, +0.0f, +0.0f,  +0.0f, +5.0f, -1.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class OdenEngine : public BaseWeaponEngine {
private:
    bool colossusSuppressorEquipped;
    bool duplexAmmoEquipped;
    float baseTorsoMultiplier;

public:
    OdenEngine()
        : BaseWeaponEngine("Oden - Heavy Bullpup Rifle", "12.7x55mm Subsonic", 
                           {62.0f, 410.0f, 65.0f, 68.0f, 52.0f, 48.0f, 4.2f}, 8.5f),
          colossusSuppressorEquipped(false),
          duplexAmmoEquipped(false),
          baseTorsoMultiplier(1.35f) {
        
        InitializeAttachments();
    }

    // Passive description specific to Oden
    std::string GetSpecialPassiveText() const override {
        std::string text = "12.7mm High Caliber Penetration (Severe Upper Torso Knockback)";
        if (colossusSuppressorEquipped) {
            text += " [COLOSSUS SUPPRESSOR - Massive Range & Stealth Extension]";
        }
        if (duplexAmmoEquipped) {
            text += " [DUPLEX AMMO - Fires 2 Projectiles Per Cycle]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        colossusSuppressorEquipped = false;
        duplexAmmoEquipped = false;
        baseTorsoMultiplier = 1.35f;

        for (const auto& att : equippedAttachments) {
            if (att.name == "Colossus Suppressor") {
                colossusSuppressorEquipped = true;
            } else if (att.name == "12.7x55mm Duplex Ammo") {
                duplexAmmoEquipped = true;
                baseTorsoMultiplier = 1.10f; // Adjusted damage scaling per bullet
            }
        }
    }

    // Custom query for torso multiplier and projectile mechanics
    float GetTorsoDamageMultiplier() const {
        return baseTorsoMultiplier;
    }

    bool HasDuplexProjectiles() const {
        return duplexAmmoEquipped;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Colossus Suppressor",        "Muzzle",      0.9f, +2.0f, -4.0f, +22.0f, +4.0f,-12.0f},
            {"Tactical Suppressor",        "Muzzle",      0.2f, +0.0f, +0.0f,  +5.0f, +0.0f, -2.0f},
            {"Oden Muzzle Brake",          "Muzzle",      0.3f, +0.0f, +5.0f,  +0.0f, +7.0f, -2.0f},

            // Barrels
            {"810mm Oden Factory Barrel",  "Barrel",      1.4f, +4.0f, +8.0f, +20.0f, +6.0f,-10.0f},
            {"730mm Oden Factory Barrel",  "Barrel",      0.9f, +2.0f, +5.0f, +12.0f, +3.0f, -6.0f},
            {"420mm Short Barrel",         "Barrel",     -0.7f, +0.0f, -6.0f, -10.0f, -5.0f,+12.0f},

            // Optics
            {"VLK 3.0x Optic",             "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},
            {"Cronen C480 Pro Optic",      "Optic",       0.6f, +0.0f, +7.0f,  +9.0f, -2.0f, -5.0f},
            {"GI Mini Reflex",             "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +0.0f},

            // Stocks
            {"Oden Ultralight Hollow",     "Stock",      -0.5f, +0.0f, -3.0f,  +0.0f, -4.0f, +9.0f},
            {"FORGE TAC Ballast Pack",     "Stock",       0.6f, +0.0f, +6.0f,  +0.0f, +7.0f, -4.0f},
            {"FTAC XL Elite Comb",         "Stock",       0.3f, +0.0f, +3.0f,  +0.0f, +3.0f, -1.0f},

            // Underbarrel
            {"Commando Foregrip",          "Underbarrel", 0.4f, +0.0f, +5.0f,  +0.0f, +4.0f, -2.0f},
            {"Merc Foregrip",              "Underbarrel", 0.5f, +0.0f, +2.0f,  +0.0f, +8.0f, -3.0f},
            {"Ranger Foregrip",            "Underbarrel", 0.6f, +0.0f, +6.0f,  +0.0f, +9.0f, -4.0f},

            // Magazines / Ammunition
            {"25 Round Mags",              "Magazine",    0.6f, +0.0f, +0.0f,  +0.0f, -1.0f, -3.0f},
            {"30 Round Mags",              "Magazine",    1.1f, +0.0f, -2.0f,  +0.0f, -2.0f, -6.0f},
            {"12.7x55mm Duplex Ammo",      "Magazine",    0.4f, +6.0f, -5.0f,  -4.0f, -3.0f, +0.0f},

            // Rear Grips
            {"Granulated Grip Tape",       "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
            {"Stippled Grip Tape",         "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f},
            {"Rubberized Grip Tape",       "Rear Grip",   0.1f, +0.0f, +0.0f,  +0.0f, +5.0f, -1.0f}
        };
    }
};

} // namespace BlackclawX

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

class Kilo141Engine : public BaseWeaponEngine {
private:
    bool drumMagEquipped;
    bool longBarrelEquipped;
    float mobilityPenalty;

public:
    Kilo141Engine()
        : BaseWeaponEngine("Kilo 141 - Modular Rifle", "5.56x45mm NATO", 
                           {38.0f, 750.0f, 68.0f, 58.0f, 68.0f, 64.0f, 3.2f}, 7.2f),
          drumMagEquipped(false),
          longBarrelEquipped(false),
          mobilityPenalty(0.0f) {
        
        InitializeAttachments();
    }

    // Passive description specific to Kilo 141
    std::string GetSpecialPassiveText() const override {
        std::string text = "Linear Recoil Stabilization System (High First-10 Shot Accuracy)";
        if (longBarrelEquipped) {
            text += " [Singuard Arms 19.8\" Prowler - Maximum Bullet Velocity & Stabilization]";
        }
        if (drumMagEquipped) {
            text += " [100-ROUND DRUM MAG ACTIVE - Extended Fire Sustain]";
        }
        return text;
    }

    // Dynamic state listener when attachments change
    void OnAttachmentStateChanged() override {
        drumMagEquipped = false;
        longBarrelEquipped = false;
        mobilityPenalty = 0.0f;

        for (const auto& att : equippedAttachments) {
            if (att.name == "100 Round Drum Mag") {
                drumMagEquipped = true;
                mobilityPenalty += 12.0f;
            } else if (att.name == "Singuard Arms 19.8\" Prowler") {
                longBarrelEquipped = true;
                mobilityPenalty += 6.0f;
            }
        }
    }

    // Custom query for mobility impact
    float GetMobilityPenalty() const {
        return mobilityPenalty;
    }

    bool HasDrumMagazine() const {
        return drumMagEquipped;
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            // Muzzles
            {"Monolithic Suppressor",             "Muzzle",      0.4f, +0.0f, -2.0f, +12.0f, +0.0f, -4.0f},
            {"Tactical Suppressor",               "Muzzle",      0.2f, +0.0f, +0.0f,  +5.0f, +0.0f, -2.0f},
            {"Muzzle Brake",                      "Muzzle",      0.3f, +0.0f, +5.0f,  +0.0f, +6.0f, -2.0f},

            // Barrels
            {"Singuard Arms 19.8\" Prowler",      "Barrel",      1.3f, +3.0f, +8.0f, +18.0f, +7.0f, -8.0f},
            {"Singuard Arms 16.6\" SOCOM",        "Barrel",      0.7f, +1.0f, +4.0f, +10.0f, +3.0f, -4.0f},
            {"Singuard Arms Whisper (Integrated)","Barrel",      0.9f, +0.0f, +3.0f,  +8.0f, +2.0f, -3.0f},

            // Optics
            {"VLK 3.0x Optic",                    "Optic",       0.5f, +0.0f, +6.0f,  +8.0f, -2.0f, -4.0f},
            {"GI Mini Reflex",                    "Optic",       0.1f, +0.0f, +2.0f,  +0.0f, +0.0f, +0.0f},
            {"Corp Combat Holo Sight",            "Optic",       0.3f, +0.0f, +4.0f,  +0.0f, +0.0f, -2.0f},

            // Stocks
            {"Singuard Arms Sniper Pro",          "Stock",       0.6f, +0.0f, +6.0f,  +0.0f, +7.0f, -5.0f},
            {"FORGE TAC Ultralight",              "Stock",      -0.4f, +0.0f, -3.0f,  +0.0f, -3.0f, +7.0f},
            {"No Stock",                          "Stock",      -0.8f, +0.0f, -6.0f,  +0.0f, -8.0f,+12.0f},

            // Underbarrel
            {"Commando Foregrip",                 "Underbarrel", 0.4f, +0.0f, +5.0f,  +0.0f, +4.0f, -2.0f},
            {"Merc Foregrip",                     "Underbarrel", 0.5f, +0.0f, +2.0f,  +0.0f, +8.0f, -3.0f},
            {"Ranger Foregrip",                   "Underbarrel", 0.6f, +0.0f, +6.0f,  +0.0f, +9.0f, -4.0f},

            // Magazines
            {"50 Round Mags",                     "Magazine",    0.8f, +0.0f, +0.0f,  +0.0f, -1.0f, -4.0f},
            {"60 Round Mags",                     "Magazine",    1.1f, +0.0f, -1.0f,  +0.0f, -2.0f, -6.0f},
            {"100 Round Drum Mag",                "Magazine",    2.4f, +0.0f, -4.0f,  +0.0f, -5.0f,-14.0f},

            // Rear Grips
            {"Granulated Grip Tape",              "Rear Grip",   0.1f, +0.0f, +4.0f,  +0.0f, +0.0f, -1.0f},
            {"Stippled Grip Tape",                "Rear Grip",   0.1f, +0.0f, -3.0f,  +0.0f, -2.0f, +6.0f},
            {"Rubberized Grip Tape",              "Rear Grip",   0.1f, +0.0f, +0.0f,  +0.0f, +5.0f, -1.0f}
        };
    }
};

} // namespace BlackclawX

// 1. ASSAULT RIFLES
Add("ak47", "AK-47", "7.62x39mm", WeaponCategory::AssaultRifle,
    38, 55, 67, 58, 60, 44, 3.8f);

Add("ak117", "AK117", "5.45x39mm", WeaponCategory::AssaultRifle,
    32, 77, 50, 45, 67, 46, 3.2f);

Add("m4", "M4", "5.56x45mm", WeaponCategory::AssaultRifle,
    28, 68, 58, 58, 69, 54, 3.1f);

Add("m16", "M16", "5.56x45mm", WeaponCategory::AssaultRifle,
    40, 45, 81, 62, 60, 68, 3.6f);

Add("drh", "DR-H (FN SCAR-H)", "7.62x51mm", WeaponCategory::AssaultRifle,
    47, 57, 59, 54, 63, 57, 3.6f);

Add("kilo141", "Kilo 141", "5.56x45mm", WeaponCategory::AssaultRifle,
    29, 68, 57, 54, 65, 55, 3.2f);

Add("m13", "M13", "5.56x45mm", WeaponCategory::AssaultRifle,
    24, 88, 56, 53, 72, 43, 3.0f);

Add("grau556", "Grau 5.56", "5.56x45mm", WeaponCategory::AssaultRifle,
    28, 73, 65, 55, 66, 60, 3.1f);

Add("krig6", "Krig 6", "5.56x45mm", WeaponCategory::AssaultRifle,
    30, 65, 62, 56, 67, 58, 3.3f);

Add("oden", "Oden", "12.7x55mm", WeaponCategory::AssaultRifle,
    48, 41, 56, 64, 51, 32, 4.2f);

Add("type25", "Type 25", "5.8x42mm", WeaponCategory::AssaultRifle,
    28, 79, 44, 48, 72, 43, 3.2f);

Add("type19", "Type 19", "5.8x42mm Custom", WeaponCategory::AssaultRifle,
    28, 75, 54, 48, 68, 52, 3.3f);

Add("asval", "AS VAL", "9x39mm", WeaponCategory::AssaultRifle,
    31, 80, 41, 45, 75, 38, 2.5f);

Add("ffar1", "FFAR 1", "5.56x45mm", WeaponCategory::AssaultRifle,
    26, 91, 44, 46, 74, 39, 3.0f);

Add("bal27", "BAL-27", "5.56x45mm Custom", WeaponCategory::AssaultRifle,
    27, 72, 51, 50, 70, 50, 3.1f);

Add("bp50", "BP50", "5.56x45mm", WeaponCategory::AssaultRifle,
    23, 85, 52, 47, 76, 48, 3.0f);

Add("cr56", "CR-56 AMAX", "7.62x39mm", WeaponCategory::AssaultRifle,
    33, 64, 53, 56, 64, 51, 3.5f);

Add("lk24", "LK24", "5.56x45mm", WeaponCategory::AssaultRifle,
    26, 63, 61, 58, 65, 64, 3.3f);

Add("icr1", "ICR-1", "5.56x45mm", WeaponCategory::AssaultRifle,
    30, 60, 69, 52, 64, 78, 3.1f);

Add("asm10", "ASM10", "5.45x39mm", WeaponCategory::AssaultRifle,
    34, 52, 51, 56, 57, 52, 4.0f);

Add("bk57", "BK57", "7.62x39mm", WeaponCategory::AssaultRifle,
    27, 66, 57, 50, 66, 59, 3.4f);

Add("kn44", "KN-44", "7.62x39mm", WeaponCategory::AssaultRifle,
    26, 63, 55, 51, 64, 58, 3.4f);

Add("peacekeeper", "Peacekeeper MK2", "5.56x45mm", WeaponCategory::AssaultRifle,
    27, 72, 58, 50, 75, 54, 2.9f);

Add("hbra3", "HBRa3", "5.56x45mm", WeaponCategory::AssaultRifle,
    26, 67, 52, 46, 68, 56, 3.2f);

Add("hvk30", "HVK-30", "5.56x45mm High-Pressure", WeaponCategory::AssaultRifle,
    35, 71, 55, 50, 66, 58, 3.25f);

Add("em2", "EM2", "7.07x62mm", WeaponCategory::AssaultRifle,
    36, 53, 61, 59, 59, 57, 3.7f);

Add("maddox", "Maddox", "5.56x45mm", WeaponCategory::AssaultRifle,
    28, 74, 49, 46, 73, 48, 3.1f);

Add("groza", "Groza", "7.62x39mm", WeaponCategory::AssaultRifle,
    29, 75, 48, 47, 71, 45, 3.2f);

Add("lag53", "LAG 53", "7.62x51mm", WeaponCategory::AssaultRifle,
    37, 50, 63, 61, 58, 59, 3.8f);

Add("ram7", "RAM-7", "5.56x45mm", WeaponCategory::AssaultRifle,
    28, 86, 49, 48, 71, 42, 3.0f);

Add("xm4", "XM4", "5.56x45mm", WeaponCategory::AssaultRifle,
    28, 72, 56, 54, 68, 53, 3.1f);

#pragma once

#include "BaseWeaponEngine.hpp"

namespace BlackclawX {

// =============================================================================
// 1. QQ9 (H&K MP5)
// =============================================================================
class QQ9Engine : public BaseWeaponEngine {
public:
    QQ9Engine()
        : BaseWeaponEngine("QQ9 - Submachine Gun", "9x19mm Parabellum", 
                           {25.0f, 830.0f, 40.0f, 40.0f, 81.0f, 42.0f, 2.5f}, 5.0f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "10mm Auto Conversion Compatibility & High Close-Quarters Strafing Speed";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor",   "Muzzle",    0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"RTC Recon Tac Long",      "Barrel",    0.8f,  0.0f, +4.0f, +15.0f, +3.0f, -6.0f},
            {"MIP Tactical Stock",      "Stock",     0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -2.0f},
            {"10mm Auto 30-Round Reload","Magazine", 0.6f, +5.0f, -2.0f,  +8.0f, -2.0f, -3.0f},
            {"OWC Laser - Tactical",    "Laser",     0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f}
        };
    }
};

// =============================================================================
// 2. CBR4 (FN P90)
// =============================================================================
class CBR4Engine : public BaseWeaponEngine {
public:
    CBR4Engine()
        : BaseWeaponEngine("CBR4 - Personal Defense Weapon", "5.7x28mm", 
                           {24.0f, 800.0f, 43.0f, 48.0f, 83.0f, 50.0f, 2.6f}, 5.2f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "50-Round Top-Mounted Horizontal Magazine (High Armor Penetration)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Marksman",            "Barrel",    1.1f,  0.0f, +6.0f, +18.0f, +5.0f, -7.0f},
            {"YKM Light Stock",         "Stock",    -0.4f,  0.0f, -2.0f,   0.0f, -3.0f, +8.0f},
            {"Granulated Grip Tape",    "Rear Grip", 0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"RTC Laser 1mW",           "Laser",     0.1f,  0.0f, +2.0f,   0.0f,  0.0f, +2.0f}
        };
    }
};

// =============================================================================
// 3. Fennec (KRISS Vector)
// =============================================================================
class FennecEngine : public BaseWeaponEngine {
public:
    FennecEngine()
        : BaseWeaponEngine("Fennec - Super V SMG", ".45 ACP", 
                           {23.0f, 1110.0f, 23.0f, 32.0f, 88.0f, 20.0f, 2.1f}, 4.8f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Super V Recoil Mitigation System (Ultra-High Rate of Fire)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"MIP Light Flash Guard",   "Muzzle",    0.2f,  0.0f, +3.0f,   0.0f, +4.0f, -1.0f},
            {"ZLR 16\" Apex Barrel",    "Barrel",    1.2f,  0.0f, +8.0f, +20.0f, +8.0f,-10.0f},
            {"No Stock",                "Stock",    -0.8f,  0.0f, -8.0f,   0.0f,-10.0f,+14.0f},
            {"Extended Mag A",          "Magazine",  0.8f,  0.0f,  0.0f,   0.0f, -2.0f, -5.0f}
        };
    }
};

// =============================================================================
// 4. Switchblade X9
// =============================================================================
class SwitchbladeX9Engine : public BaseWeaponEngine {
public:
    SwitchbladeX9Engine()
        : BaseWeaponEngine("Switchblade X9 - Compact SMG", "9x19mm Parabellum", 
                           {24.0f, 860.0f, 42.0f, 42.0f, 86.0f, 47.0f, 2.3f}, 4.5f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Foldable Frame (Ultra-Fast Sprint-to-Fire & ADS Speed)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor",   "Muzzle",    0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"MIP Extended Light Barrel","Barrel",   0.7f,  0.0f, +3.0f, +10.0f, +2.0f, -4.0f},
            {"YKM Combat Stock",        "Stock",     0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +6.0f},
            {"Extended Mag Mag B",      "Magazine",  0.9f,  0.0f,  0.0f,   0.0f, -2.0f, -5.0f}
        };
    }
};

// =============================================================================
// 5. USS 9 (Uzi)
// =============================================================================
class USS9Engine : public BaseWeaponEngine {
public:
    USS9Engine()
        : BaseWeaponEngine("USS 9 - Micro Open-Bolt SMG", "9x19mm Parabellum", 
                           {26.0f, 750.0f, 45.0f, 38.0f, 85.0f, 52.0f, 2.7f}, 5.0f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Heavy Open-Bolt Design (High Hipfire Spread Control)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Tactical Suppressor",     "Muzzle",    0.2f,  0.0f,  0.0f,  +5.0f,  0.0f, -2.0f},
            {"Carbine Barrel",          "Barrel",    0.9f,  0.0f, +5.0f, +14.0f, +4.0f, -6.0f},
            {"Wood Stock",              "Stock",     0.6f,  0.0f, +6.0f,   0.0f, +6.0f, -5.0f},
            {"50 Round Drum",           "Magazine",  1.5f,  0.0f, -3.0f,   0.0f, -4.0f,-10.0f}
        };
    }
};

// =============================================================================
// 6. CX9 (CZ Scorpion EVO 3)
// =============================================================================
class CX9Engine : public BaseWeaponEngine {
public:
    CX9Engine()
        : BaseWeaponEngine("CX9 - Rapid Tactical SMG", "9x19mm Parabellum", 
                           {22.0f, 1030.0f, 38.0f, 36.0f, 82.0f, 39.0f, 2.4f}, 4.9f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "CX-23 Integrated Suppressed Barrel Compatibility & High Fire Rate";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"CX-23 Integrated Barrel", "Barrel",    0.8f,  0.0f, +3.0f, +10.0f, +2.0f, -3.0f},
            {"CX-FR Stock",             "Stock",    -0.5f,  0.0f, -3.0f,   0.0f, -4.0f, +9.0f},
            {"50 Round Drum Mags",      "Magazine",  1.4f,  0.0f, -2.0f,   0.0f, -3.0f, -8.0f},
            {"Commando Foregrip",       "Underbarrel",0.4f, 0.0f, +4.0f,   0.0f, +3.0f, -2.0f}
        };
    }
};

// =============================================================================
// 7. PP19 Bizon
// =============================================================================
class PP19BizonEngine : public BaseWeaponEngine {
public:
    PP19BizonEngine()
        : BaseWeaponEngine("PP19 Bizon - Helical Feed SMG", "9x18mm Makarov", 
                           {26.0f, 650.0f, 61.0f, 52.0f, 73.0f, 63.0f, 2.7f}, 5.5f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "64-Round Helical Drum Base Capacity (Low Vertical Recoil)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor",   "Muzzle",    0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Marksman Barrel",     "Barrel",    1.2f,  0.0f, +7.0f, +18.0f, +6.0f, -8.0f},
            {"YKM Light Stock",         "Stock",    -0.4f,  0.0f, -2.0f,   0.0f, -3.0f, +8.0f},
            {"Large Caliber Ammo A",    "Magazine",  0.3f, +4.0f, -1.0f,  -2.0f,  0.0f, -1.0f}
        };
    }
};

// =============================================================================
// 8. RUS-79U (AKS-74U)
// =============================================================================
class RUS79UEngine : public BaseWeaponEngine {
public:
    RUS79UEngine()
        : BaseWeaponEngine("RUS-79U - Short Carbine SMG", "5.45x39mm", 
                           {28.0f, 770.0f, 51.0f, 44.0f, 79.0f, 50.0f, 2.7f}, 5.1f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Assault Rifle Caliber Penetration in an SMG Chassis";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Marksman Barrel",     "Barrel",    1.1f,  0.0f, +6.0f, +16.0f, +5.0f, -7.0f},
            {"No Stock",                "Stock",    -0.8f,  0.0f, -6.0f,   0.0f, -8.0f,+12.0f},
            {"50 Round Extended Mag",   "Magazine",  1.2f,  0.0f,  0.0f,   0.0f, -2.0f, -6.0f},
            {"Strike Foregrip",         "Underbarrel",0.4f, 0.0f, +4.0f,   0.0f, +3.0f, -2.0f}
        };
    }
};

// =============================================================================
// 9. MSMC
// =============================================================================
class MSMCEngine : public BaseWeaponEngine {
public:
    MSMCEngine()
        : BaseWeaponEngine("MSMC - Close-Quarters Carbine", "5.56x30mm MINSAS", 
                           {31.0f, 920.0f, 37.0f, 33.0f, 86.0f, 26.0f, 2.3f}, 4.7f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "High Close-Range Damage Burst (High Vertical Recoil Peak)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Ranger Barrel",       "Barrel",    1.0f,  0.0f, +5.0f, +15.0f, +7.0f, -8.0f},
            {"YKM Combat Stock",        "Stock",     0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"30 Round Fast Reload",    "Magazine",  0.5f,  0.0f,  0.0f,   0.0f,  0.0f, +2.0f},
            {"Operator Foregrip",       "Underbarrel",0.6f, 0.0f, +2.0f,   0.0f, +8.0f, -4.0f}
        };
    }
};

// =============================================================================
// 10. PDW-57
// =============================================================================
class PDW57Engine : public BaseWeaponEngine {
public:
    PDW57Engine()
        : BaseWeaponEngine("PDW-57 - High Capacity PDW", "5.7x28mm", 
                           {27.0f, 750.0f, 49.0f, 43.0f, 75.0f, 50.0f, 2.8f}, 5.4f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Large 50-Round Magazine Capacity Base with Stable Hipfire Spread";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor",   "Muzzle",    0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Extended Light Barrel","Barrel",   0.8f,  0.0f, +4.0f, +12.0f, +3.0f, -5.0f},
            {"YKM Light Stock",         "Stock",    -0.4f,  0.0f, -2.0f,   0.0f, -3.0f, +8.0f},
            {"Granulated Grip Tape",    "Rear Grip", 0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f}
        };
    }
};

// =============================================================================
// 11. MX9
// =============================================================================
class MX9Engine : public BaseWeaponEngine {
public:
    MX9Engine()
        : BaseWeaponEngine("MX9 - Tactical SMG", "9x19mm Parabellum", 
                           {30.0f, 800.0f, 45.0f, 41.0f, 82.0f, 40.0f, 2.4f}, 4.8f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Large Caliber Ammo Conversion Compatibility (Increases Chest Multiplier)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Built-In Suppressor Barrel","Barrel",  0.6f,  0.0f, +2.0f,  +8.0f, +1.0f, -3.0f},
            {"Agile Stock",             "Stock",    -0.3f,  0.0f, -2.0f,   0.0f, -3.0f, +7.0f},
            {"Large Caliber Ammo",      "Magazine",  0.5f, +4.0f, -2.0f,   0.0f, -2.0f, -3.0f},
            {"Infiltrator Foregrip",    "Underbarrel",0.3f, 0.0f, +2.0f,   0.0f, +2.0f, +2.0f}
        };
    }
};

// =============================================================================
// 12. MAC-10
// =============================================================================
class MAC10Engine : public BaseWeaponEngine {
public:
    MAC10Engine()
        : BaseWeaponEngine("MAC-10 - Machine Pistol SMG", ".45 ACP", 
                           {24.0f, 1200.0f, 28.0f, 28.0f, 92.0f, 22.0f, 1.8f}, 4.2f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Extreme Rate of Fire (1200 RPM) for Extreme Close Range TTK";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Agency Suppressor",       "Muzzle",    0.3f,  0.0f, -1.0f, +10.0f, +2.0f, -3.0f},
            {"6.1\" Cavalry Barrel",    "Barrel",    0.6f,  0.0f, +3.0f, +10.0f, +1.0f, -4.0f},
            {"SAS Combat Stock",        "Stock",    -0.4f,  0.0f, -3.0f,   0.0f, -4.0f, +9.0f},
            {"43 Round Drum",           "Magazine",  1.0f,  0.0f, -2.0f,   0.0f, -3.0f, -7.0f}
        };
    }
};

// =============================================================================
// 13. LAPA
// =============================================================================
class LAPAEngine : public BaseWeaponEngine {
public:
    LAPAEngine()
        : BaseWeaponEngine("LAPA - Modular Compact Rifle", "9x19mm Parabellum", 
                           {28.0f, 700.0f, 52.0f, 47.0f, 80.0f, 56.0f, 2.5f}, 5.1f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "High Precision Barrel Options (Controlled Firing Velocity)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"7.9\" Rifled Barrel",     "Barrel",    0.8f, +2.0f, +4.0f, +12.0f, +3.0f, -5.0f},
            {"Conscript Stock",         "Stock",    -0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +5.0f},
            {"50 Round Extended Mag",   "Magazine",  1.1f,  0.0f, -1.0f,   0.0f, -2.0f, -6.0f},
            {"Field Agent Foregrip",    "Underbarrel",0.5f, 0.0f, +4.0f,   0.0f, +5.0f, -3.0f}
        };
    }
};

// =============================================================================
// 14. OTs 9
// =============================================================================
class OTs9Engine : public BaseWeaponEngine {
public:
    OTs9Engine()
        : BaseWeaponEngine("OTs 9 - Compact Tactical SMG", "9x18mm Makarov", 
                           {27.0f, 860.0f, 41.0f, 38.0f, 87.0f, 43.0f, 2.2f}, 4.6f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "High Mobility and Fast Reload Cycling";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"GRU Suppressor",          "Muzzle",    0.4f,  0.0f, -1.0f, +12.0f, +2.0f, -4.0f},
            {"8.1\" Task Force Barrel", "Barrel",    0.9f, +2.0f, +5.0f, +15.0f, +4.0f, -6.0f},
            {"KGB Skeletal Stock",      "Stock",    -0.5f,  0.0f, -3.0f,   0.0f, -4.0f, +10.0f},
            {"VDV 40 Round Fast Mag",   "Magazine",  0.8f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f}
        };
    }
};

// =============================================================================
// 15. Razorback
// =============================================================================
class RazorbackEngine : public BaseWeaponEngine {
public:
    RazorbackEngine()
        : BaseWeaponEngine("Razorback - Bullpup SMG", "5.7x28mm", 
                           {30.0f, 620.0f, 57.0f, 49.0f, 74.0f, 57.0f, 2.8f}, 5.3f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Rapid Fire Perk Compatibility (Mid-Range Bullpup Stability)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Rapid Fire Perk",     "Perk",      0.0f, +3.0f, -3.0f,   0.0f, -2.0f,  0.0f},
            {"OWC Marksman Barrel",     "Barrel",    1.1f,  0.0f, +6.0f, +16.0f, +5.0f, -7.0f},
            {"YKM Combat Stock",        "Stock",     0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +6.0f},
            {"Extended Mag A",          "Magazine",  0.7f,  0.0f,  0.0f,   0.0f, -1.0f, -4.0f}
        };
    }
};

// =============================================================================
// 16. Chicom
// =============================================================================
class ChicomEngine : public BaseWeaponEngine {
public:
    ChicomEngine()
        : BaseWeaponEngine("Chicom - 3-Round Burst SMG", "9x19mm Parabellum", 
                           {31.0f, 650.0f, 63.0f, 43.0f, 80.0f, 61.0f, 2.5f}, 5.0f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "3-Round Burst Mechanism with Low Recoil Reset Delay";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor",   "Muzzle",    0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Strike Suppressor",   "Muzzle",    0.3f,  0.0f, +2.0f,  +5.0f, +3.0f, -2.0f},
            {"MIP Strike Stock",        "Stock",     0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -3.0f},
            {"45 Round Extended Mag",   "Magazine",  0.9f,  0.0f,  0.0f,   0.0f, -1.0f, -5.0f}
        };
    }
};

// =============================================================================
// 17. HG 40
// =============================================================================
class HG40Engine : public BaseWeaponEngine {
public:
    HG40Engine()
        : BaseWeaponEngine("HG 40 - Classic Machine Pistol", "9x19mm Parabellum", 
                           {28.0f, 610.0f, 62.0f, 48.0f, 76.0f, 60.0f, 2.9f}, 5.2f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Low Controlled Fire Rate with High Firing Stability";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Marksman Barrel",     "Barrel",    1.0f,  0.0f, +6.0f, +15.0f, +5.0f, -7.0f},
            {"No Stock",                "Stock",    -0.8f,  0.0f, -5.0f,   0.0f, -7.0f,+11.0f},
            {"40 Round Fast Reload",    "Magazine",  0.7f,  0.0f,  0.0f,   0.0f,  0.0f, -3.0f},
            {"Strike Foregrip",         "Underbarrel",0.4f, 0.0f, +4.0f,   0.0f, +3.0f, -2.0f}
        };
    }
};

// =============================================================================
// 18. GKS
// =============================================================================
class GKSEngine : public BaseWeaponEngine {
public:
    GKSEngine()
        : BaseWeaponEngine("GKS - High Precision SMG", "9x19mm Parabellum", 
                           {28.0f, 600.0f, 62.0f, 50.0f, 72.0f, 59.0f, 2.8f}, 5.3f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Assault-Rifle-Like Accuracy Profile and Range Multipliers";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Marksman Barrel",     "Barrel",    1.1f,  0.0f, +7.0f, +18.0f, +6.0f, -8.0f},
            {"YKM Combat Stock",        "Stock",     0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +6.0f},
            {"32 Round Fast Reload",    "Magazine",  0.5f,  0.0f,  0.0f,   0.0f,  0.0f, -2.0f},
            {"Granulated Grip Tape",    "Rear Grip", 0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f}
        };
    }
};

// =============================================================================
// 19. Pharo
// =============================================================================
class PharoEngine : public BaseWeaponEngine {
public:
    PharoEngine()
        : BaseWeaponEngine("Pharo - 4-Round Burst SMG", "9x19mm Parabellum", 
                           {34.0f, 680.0f, 49.0f, 42.0f, 83.0f, 38.0f, 2.4f}, 4.8f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "4-Round High-Damage Burst Cycle (One-Burst Lethality Potential)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"OWC Ranger Barrel",       "Barrel",    1.0f,  0.0f, +5.0f, +14.0f, +6.0f, -7.0f},
            {"MIP Strike Stock",        "Stock",     0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -3.0f},
            {"36 Round Extended Mag",   "Magazine",  0.8f,  0.0f,  0.0f,   0.0f, -1.0f, -4.0f},
            {"Operator Foregrip",       "Underbarrel",0.6f, 0.0f, +2.0f,   0.0f, +8.0f, -4.0f}
        };
    }
};

// =============================================================================
// 20. AGR 556 (AUG 5.56 Conversion)
// =============================================================================
class AGR556Engine : public BaseWeaponEngine {
public:
    AGR556Engine()
        : BaseWeaponEngine("AGR 556 - Convertible SMG", "9x19mm / 5.56 NATO", 
                           {27.0f, 750.0f, 52.0f, 47.0f, 77.0f, 51.0f, 2.9f}, 5.4f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "5.56 NATO Magazine Conversion (Converts SMG to AR Damage Profile)";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"5.56 30-Round Reload",    "Magazine",  0.8f, +6.0f, -2.0f, +10.0f, -3.0f, -4.0f},
            {"5.56 60-Round Reload",    "Magazine",  1.8f, +6.0f, -4.0f, +10.0f, -5.0f,-10.0f},
            {"OWC Extended Light Barrel","Barrel",   0.8f,  0.0f, +4.0f, +12.0f, +3.0f, -5.0f},
            {"YKM Combat Stock",        "Stock",     0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +6.0f}
        };
    }
};

// =============================================================================
// 21. VMP
// =============================================================================
class VMPEngine : public BaseWeaponEngine {
public:
    VMPEngine()
        : BaseWeaponEngine("VMP - Rapid Combat SMG", "9x19mm Parabellum", 
                           {26.0f, 900.0f, 42.0f, 41.0f, 84.0f, 38.0f, 2.3f}, 4.7f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Balanced High Rate of Fire and Low Initial Recoil Kick";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"Monolithic Suppressor",   "Muzzle",    0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Ranger Barrel",       "Barrel",    1.0f,  0.0f, +5.0f, +15.0f, +6.0f, -7.0f},
            {"No Stock",                "Stock",    -0.8f,  0.0f, -6.0f,   0.0f, -8.0f,+12.0f},
            {"38 Round Extended Mag",   "Magazine",  0.8f,  0.0f,  0.0f,   0.0f, -1.0f, -4.0f}
        };
    }
};

// =============================================================================
// 22. FSS Hurricane
// =============================================================================
class FSSHurricaneEngine : public BaseWeaponEngine {
public:
    FSSHurricaneEngine()
        : BaseWeaponEngine("FSS Hurricane - AR Platform SMG", "5.7x28mm", 
                           {25.0f, 820.0f, 55.0f, 46.0f, 81.0f, 54.0f, 2.6f}, 5.1f) {
        InitializeAttachments();
    }

    std::string GetSpecialPassiveText() const override {
        return "Top-Mounted AR-57 Magazine System with High Recoil Stability";
    }

private:
    void InitializeAttachments() {
        availableAttachments = {
            {"FSS Cannonade 16\" Barrel","Barrel",   1.1f,  0.0f, +6.0f, +18.0f, +5.0f, -7.0f},
            {"Demo Quicksilver Stock",   "Stock",    -0.4f,  0.0f, -2.0f,   0.0f, -3.0f, +8.0f},
            {"Phantom Grip",            "Rear Grip", 0.1f,  0.0f, -2.0f,   0.0f, -1.0f, +5.0f},
            {"VX Pineapple Foregrip",   "Underbarrel",0.5f, 0.0f, +3.0f,   0.0f, +6.0f, -3.0f}
        };
    }
};

} // namespace BlackclawX

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip, Perk
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// RUS-79U GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class RUS79UGunsmithEngine {
private:
    std::string weaponName = "RUS-79U (AKS-74U)";
    std::string caliber = "5.45x39mm Soviet";

    // Base weapon performance attributes
    float baseDamage = 28.0f;
    float baseFireRate = 77.0f;
    float baseAccuracy = 51.0f;
    float baseRange = 44.0f;
    float baseControl = 79.0f;
    float baseMobility = 50.0f;
    float baseWeight = 2.7f; // kg

    float maxWeightCapacity = 5.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    RUS79UGunsmithEngine() {
        // Initialize signature RUS-79U / AKS-74U attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",    "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",    "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            
            // Barrel
            {"OWC Marksman Barrel",      "Barrel",      1.1f,  0.0f, +6.0f, +16.0f, +5.0f, -7.0f},
            {"MIP Short Barrel",         "Barrel",      0.4f,  0.0f, -3.0f,  -8.0f, -2.0f, +6.0f},
            
            // Stock
            {"No Stock",                 "Stock",      -0.8f,  0.0f, -6.0f,   0.0f, -8.0f,+12.0f},
            {"MIP Strike Stock",         "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -3.0f},
            {"YKM Combat Stock",         "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            
            // Laser
            {"OWC Laser - Tactical",     "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f},
            
            // Underbarrel
            {"Strike Foregrip",          "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Merc Foregrip",            "Underbarrel", 0.5f,  0.0f, +2.0f,   0.0f, +8.0f, -3.0f},
            
            // Ammunition
            {"38 Round Extended Mag",    "Magazine",    0.7f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},
            {"50 Round Extended Mag",    "Magazine",    1.2f,  0.0f,  0.0f,   0.0f, -2.0f, -6.0f},
            
            // Rear Grip
            {"Granulated Grip Tape",     "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",       "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +7.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "               BLACKclawX :: RUS-79U (AKS-74U) GUNSMITH                \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";
        
        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1) 
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);
        
        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME RUS-79U PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE RUS-79U ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] " 
                      << std::left << std::setw(25) << att.name 
                      << " Category: " << std::setw(12) << att.category 
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    RUS79UGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-14] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = RUS79UGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting RUS-79U Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// CORDITE GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class CorditeGunsmithEngine {
private:
    std::string weaponName = "Cordite";
    std::string caliber = "9mm High Capacity";

    // Base Cordite weapon performance attributes
    float baseDamage = 24.0f;
    float baseFireRate = 80.0f;
    float baseAccuracy = 46.0f;
    float baseRange = 53.0f;
    float baseControl = 61.0f;
    float baseMobility = 73.0f;
    float baseWeight = 3.1f; // kg

    float maxWeightCapacity = 5.5f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    CorditeGunsmithEngine() {
        // Initialize signature Cordite attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",    "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"RTC Light Muzzle Brake",   "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -3.0f},
            {"OWC Light Suppressor",    "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},

            // Barrel
            {"OWC Marksman Barrel",      "Barrel",      1.2f,  0.0f, +7.0f, +18.0f, +6.0f, -8.0f},
            {"MIP Extended Light Barrel","Barrel",      0.6f,  0.0f, +2.0f,  +8.0f,  0.0f, -3.0f},
            {"YKM Combat Barrel",        "Barrel",      0.3f,  0.0f, -3.0f,  -6.0f, -2.0f, +5.0f},

            // Stock
            {"YKM Combat Stock",         "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -3.0f, +6.0f},
            {"OWC Stalker Stock",        "Stock",       0.7f,  0.0f, +5.0f,   0.0f, +4.0f, -4.0f},
            {"MIP Strike Stock",         "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Laser
            {"OWC Laser - Tactical",     "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f},

            // Underbarrel
            {"Strike Foregrip",          "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Operator Foregrip",        "Underbarrel", 0.5f,  0.0f,  0.0f,   0.0f, +8.0f, -4.0f},

            // Ammunition
            {"80 Round Extended Mag",    "Magazine",    0.8f,  0.0f,  0.0f,   0.0f, -2.0f, -4.0f},
            {"90 Round Extended Mag",    "Magazine",    1.3f,  0.0f,  0.0f,   0.0f, -4.0f, -7.0f},

            // Rear Grip
            {"Granulated Grip Tape",     "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",       "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +7.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                   CORDITE SMG GUNSMITH ENGINE                         \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME CORDITE PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE CORDITE ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(26) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    CorditeGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-16] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = CorditeGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting Cordite Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// PDW-57 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class PDW57GunsmithEngine {
private:
    std::string weaponName = "PDW-57";
    std::string caliber = "5.7x28mm High-Velocity";

    // Base PDW-57 weapon performance attributes
    float baseDamage = 27.0f;
    float baseFireRate = 75.0f;
    float baseAccuracy = 49.0f;
    float baseRange = 43.0f;
    float baseControl = 50.0f;
    float baseMobility = 75.0f;
    float baseWeight = 2.8f; // kg

    float maxWeightCapacity = 5.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    PDW57GunsmithEngine() {
        // Initialize signature PDW-57 attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",    "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",    "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"MIP Flash Guard",          "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +1.0f, -1.0f},

            // Barrel
            {"OWC Marksman Barrel",      "Barrel",      1.1f,  0.0f, +6.0f, +16.0f, +5.0f, -7.0f},
            {"MIP Extended Light Barrel","Barrel",      0.5f,  0.0f, +2.0f,  +7.0f,  0.0f, -3.0f},
            {"YKM Combat Barrel",        "Barrel",      0.3f,  0.0f, -3.0f,  -5.0f, -2.0f, +5.0f},

            // Stock
            {"YKM Combat Stock",         "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -3.0f, +6.0f},
            {"OWC Ranger Stock",         "Stock",       0.8f,  0.0f, +5.0f,   0.0f, +6.0f, -5.0f},
            {"No Stock",                 "Stock",      -0.7f,  0.0f, -6.0f,   0.0f, -8.0f,+12.0f},

            // Laser
            {"OWC Laser - Tactical",     "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f},
            {"MIP Laser 5mW",            "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +4.0f},

            // Underbarrel
            {"Strike Foregrip",          "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Merc Foregrip",            "Underbarrel", 0.5f,  0.0f, +2.0f,   0.0f, +8.0f, -3.0f},

            // Ammunition
            {"Fast Reload Mag",          "Magazine",    0.2f,  0.0f,  0.0f,   0.0f,  0.0f, +3.0f},
            {"Large Extended Mag",       "Magazine",    0.9f,  0.0f,  0.0f,   0.0f, -2.0f, -5.0f},

            // Rear Grip
            {"Granulated Grip Tape",     "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",       "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +7.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                     PDW-57 SMG GUNSMITH ENGINE                         \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME PDW-57 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE PDW-57 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(26) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    PDW57GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = PDW57GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting PDW-57 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// QQ9 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class QQ9GunsmithEngine {
private:
    std::string weaponName = "QQ9 (MP5)";
    std::string caliber = "9x19mm Parabellum";

    // Base QQ9 weapon performance attributes
    float baseDamage = 25.0f;
    float baseFireRate = 83.0f;
    float baseAccuracy = 40.0f;
    float baseRange = 46.0f;
    float baseControl = 42.0f;
    float baseMobility = 81.0f;
    float baseWeight = 2.5f; // kg

    float maxWeightCapacity = 4.5f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    QQ9GunsmithEngine() {
        // Initialize signature QQ9 attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +4.0f, -2.0f},

            // Barrel
            {"RTC Recon Tac Long",        "Barrel",      1.0f,  0.0f, +5.0f, +14.0f, +4.0f, -6.0f},
            {"MIP Tactical Carbine",      "Barrel",      0.6f,  0.0f, +2.0f,  +6.0f, +2.0f, -3.0f},
            {"MIP Light Barrel (Short)",  "Barrel",      0.3f,  0.0f, -3.0f,  -6.0f, -2.0f, +5.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.6f,  0.0f, -5.0f,   0.0f, -7.0f,+10.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"MIP Strike Stock",          "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -3.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f},
            {"RTC Laser 1mW",             "Laser",       0.1f,  0.0f, +2.0f,   0.0f,  0.0f, +2.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Merc Foregrip",             "Underbarrel", 0.5f,  0.0f, +2.0f,   0.0f, +7.0f, -3.0f},

            // Ammunition
            {"10mm Auto 30-Round Reload", "Magazine",    0.4f, +5.0f,  0.0f,  +8.0f, -3.0f, -2.0f},
            {"45 Round Extended Mag",     "Magazine",    0.6f,  0.0f,  0.0f,   0.0f, -1.0f, -4.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +7.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                     QQ9 (MP5) GUNSMITH ENGINE                          \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME QQ9 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE QQ9 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    QQ9GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = QQ9GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting QQ9 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// PP-19 BIZON GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class PP19BizonGunsmithEngine {
private:
    std::string weaponName = "PP-19 Bizon";
    std::string caliber = "9x18mm Makarov";

    // Base PP-19 Bizon weapon performance attributes
    float baseDamage = 26.0f;
    float baseFireRate = 65.0f;
    float baseAccuracy = 62.0f;
    float baseRange = 52.0f;
    float baseControl = 64.0f;
    float baseMobility = 73.0f;
    float baseWeight = 2.7f; // kg

    float maxWeightCapacity = 4.8f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    PP19BizonGunsmithEngine() {
        // Initialize signature PP-19 Bizon attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"OWC Marksman Barrel",       "Barrel",      1.1f,  0.0f, +6.0f, +16.0f, +5.0f, -7.0f},
            {"MIP Extended Light Barrel", "Barrel",      0.5f,  0.0f, +2.0f,  +7.0f,  0.0f, -3.0f},
            {"YKM Integrated Suppressor", "Barrel",      0.7f,  0.0f, +1.0f,  +5.0f, +2.0f, -4.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.6f,  0.0f, -5.0f,   0.0f, -7.0f,+11.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"MIP Strike Stock",          "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -3.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +4.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Tactical Foregrip A",       "Underbarrel", 0.3f,  0.0f, +2.0f,   0.0f, +2.0f, -1.0f},

            // Ammunition
            {"Large Extended Mag",        "Magazine",    0.8f,  0.0f,  0.0f,   0.0f, -2.0f, -4.0f},
            {"Fast Reload Mag",           "Magazine",    0.2f,  0.0f,  0.0f,   0.0f,  0.0f, +3.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +7.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                   PP-19 BIZON GUNSMITH ENGINE                          \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME PP-19 BIZON PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE PP-19 BIZON ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    PP19BizonGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = PP19BizonGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting PP-19 Bizon Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip, Perk
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// FENNEC GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class FennecGunsmithEngine {
private:
    std::string weaponName = "Fennec (Vector)";
    std::string caliber = ".45 ACP High Velocity";

    // Base Fennec weapon performance attributes
    float baseDamage = 23.0f;
    float baseFireRate = 111.0f; // Ultra-high fire rate
    float baseAccuracy = 38.0f;
    float baseRange = 40.0f;
    float baseControl = 24.0f;
    float baseMobility = 88.0f;
    float baseWeight = 2.4f; // kg

    float maxWeightCapacity = 4.2f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    FennecGunsmithEngine() {
        // Initialize signature Fennec attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"MIP Flash Guard",           "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +2.0f, -1.0f},

            // Barrel
            {"ZLR 16\" Apex Barrel",      "Barrel",      1.1f,  0.0f, +6.0f, +16.0f, +5.0f, -7.0f},
            {"MIP Extended Light Barrel", "Barrel",      0.5f,  0.0f, +2.0f,  +7.0f,  0.0f, -3.0f},
            {"MIP Light Barrel (Short)",  "Barrel",      0.3f,  0.0f, -3.0f,  -6.0f, -2.0f, +6.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.6f,  0.0f, -5.0f,   0.0f, -8.0f,+12.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"RTC Steady Stock",          "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +5.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +4.0f},

            // Underbarrel
            {"Operator Foregrip",         "Underbarrel", 0.5f,  0.0f,  0.0f,   0.0f, +8.0f, -4.0f},
            {"Merc Foregrip",             "Underbarrel", 0.5f,  0.0f, +2.0f,   0.0f, +7.0f, -3.0f},

            // Ammunition
            {"Extended Mag A (33 Rnd)",   "Magazine",    0.5f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},
            {"Extended Mag B (42 Rnd)",   "Magazine",    0.8f,  0.0f,  0.0f,   0.0f, -2.0f, -5.0f},

            // Perk / Special
            {"Akimbo (Dual Wield)",       "Perk",        1.2f, +10.0f,-15.0f, -10.0f,-12.0f, +5.0f},
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                   FENNEC (VECTOR) GUNSMITH ENGINE                      \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME FENNEC PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 120.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE FENNEC ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    FennecGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = FennecGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting Fennec Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// MSMC GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class MSMCGunsmithEngine {
private:
    std::string weaponName = "MSMC";
    std::string caliber = "5.56x30mm MINSAS";

    // Base MSMC weapon performance attributes
    float baseDamage = 30.0f;
    float baseFireRate = 79.0f;
    float baseAccuracy = 42.0f;
    float baseRange = 40.0f;
    float baseControl = 37.0f;
    float baseMobility = 82.0f;
    float baseWeight = 2.8f; // kg

    float maxWeightCapacity = 4.5f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    MSMCGunsmithEngine() {
        // Initialize signature MSMC attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"OWC Marksman Barrel",       "Barrel",      1.0f,  0.0f, +5.0f, +15.0f, +4.0f, -6.0f},
            {"MIP Extended Light Barrel", "Barrel",      0.5f,  0.0f, +2.0f,  +7.0f,  0.0f, -3.0f},
            {"YKM Integrated Suppressor", "Barrel",      0.6f,  0.0f, +1.0f,  +5.0f, +2.0f, -4.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.5f,  0.0f, -5.0f,   0.0f, -7.0f,+10.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"RTC Steady Stock",          "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Operator Foregrip",         "Underbarrel", 0.5f,  0.0f,  0.0f,   0.0f, +8.0f, -4.0f},

            // Ammunition
            {"30 Round Fast Reload",      "Magazine",    0.3f,  0.0f,  0.0f,   0.0f,  0.0f, +2.0f},
            {"36 Round Extended Mag",     "Magazine",    0.6f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +6.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                      MSMC GUNSMITH ENGINE                              \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME MSMC PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE MSMC ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    MSMCGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = MSMCGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting MSMC Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// MAC-10 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class MAC10GunsmithEngine {
private:
    std::string weaponName = "MAC-10";
    std::string caliber = ".45 ACP / 9mm Parabellum";

    // Base MAC-10 weapon performance attributes
    float baseDamage = 24.0f;
    float baseFireRate = 105.0f; // Rapid fire rate
    float baseAccuracy = 36.0f;
    float baseRange = 38.0f;
    float baseControl = 28.0f;
    float baseMobility = 92.0f;  // Ultra high baseline mobility
    float baseWeight = 2.1f;    // kg

    float maxWeightCapacity = 3.8f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    MAC10GunsmithEngine() {
        // Initialize signature MAC-10 attachments
        availableAttachments = {
            // Muzzle
            {"Agency Suppressor",         "Muzzle",      0.4f,  0.0f, -2.0f, +10.0f, +3.0f, -4.0f},
            {"Sound Suppressor",          "Muzzle",      0.2f,  0.0f,  0.0f,  -4.0f,  0.0f, +2.0f},
            {"SOCOM Eliminator",          "Muzzle",      0.3f,  0.0f, +4.0f,   0.0f, +6.0f, -3.0f},

            // Barrel
            {"6.1\" Reinforced Heavy",    "Barrel",      0.8f,  0.0f, +4.0f, +12.0f, +2.0f, -5.0f},
            {"5.3\" Extended Barrel",     "Barrel",      0.4f,  0.0f, +1.0f,  +6.0f,  0.0f, -2.0f},
            {"5.9\" Task Force Barrel",   "Barrel",      0.6f, +3.0f, -3.0f,  +8.0f, -4.0f, -3.0f},

            // Stock
            {"Wire Stock",                "Stock",      -0.4f,  0.0f, -4.0f,   0.0f, -5.0f, +8.0f},
            {"SAS Combat Stock",          "Stock",       0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +7.0f},
            {"Raider Stock",              "Stock",       0.4f,  0.0f, -3.0f,   0.0f, -3.0f, +10.0f},

            // Laser
            {"Steady Aim Laser",          "Laser",       0.1f,  0.0f, +3.0f,   0.0f,  0.0f, +3.0f},
            {"SWAT 5mW Laser Sight",      "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +4.0f},

            // Underbarrel
            {"Field Agent Grip",          "Underbarrel", 0.4f,  0.0f, +3.0f,   0.0f, +7.0f, -3.0f},
            {"SFOD Speedgrip",            "Underbarrel", 0.3f,  0.0f, +1.0f,   0.0f, +2.0f, +2.0f},

            // Ammunition
            {"43 Rnd Speed Mag",          "Magazine",    0.4f,  0.0f,  0.0f,   0.0f, -1.0f, +1.0f},
            {"53 Rnd Drum Mag",           "Magazine",    0.9f,  0.0f,  0.0f,   0.0f, -3.0f, -6.0f},

            // Rear Grip
            {"Airborne Elastic Wrap",     "Rear Grip",   0.1f,  0.0f, +4.0f,   0.0f, +2.0f, -1.0f},
            {"SASR Jungle Grip",          "Rear Grip",   0.1f,  0.0f, -3.0f,   0.0f, -1.0f, +5.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                     MAC-10 GUNSMITH ENGINE                             \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME MAC-10 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 120.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE MAC-10 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    MAC10GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = MAC10GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting MAC-10 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// SWITCHBLADE X9 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class SwitchbladeX9GunsmithEngine {
private:
    std::string weaponName = "Switchblade X9";
    std::string caliber = "9x19mm Parabellum";

    // Base Switchblade X9 weapon performance attributes
    float baseDamage = 27.0f;
    float baseFireRate = 85.0f;
    float baseAccuracy = 54.0f;
    float baseRange = 44.0f;
    float baseControl = 48.0f;
    float baseMobility = 86.0f;
    float baseWeight = 2.3f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    SwitchbladeX9GunsmithEngine() {
        // Initialize signature Switchblade X9 attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"MIP Extended Light Barrel", "Barrel",      0.6f,  0.0f, +3.0f, +10.0f, +2.0f, -4.0f},
            {"OWC Marksman Barrel",       "Barrel",      1.0f,  0.0f, +5.0f, +15.0f, +4.0f, -6.0f},
            {"YKM Integrated Suppressor", "Barrel",      0.5f,  0.0f, +1.0f,  +5.0f, +2.0f, -3.0f},

            // Stock
            {"YKM Light Stock",           "Stock",      -0.3f,  0.0f, -2.0f,   0.0f, -3.0f, +7.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"RTC Steady Stock",          "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Operator Foregrip",         "Underbarrel", 0.5f,  0.0f,  0.0f,   0.0f, +7.0f, -4.0f},

            // Ammunition
            {"Fast Reload Mag",           "Magazine",    0.2f,  0.0f,  0.0f,   0.0f,  0.0f, +3.0f},
            {"Extended Mag A (39 Rnd)",   "Magazine",    0.6f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +6.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                 SWITCHBLADE X9 GUNSMITH ENGINE                         \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME SWITCHBLADE X9 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 110.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE SWITCHBLADE X9 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    SwitchbladeX9GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = SwitchbladeX9GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting Switchblade X9 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// USS 9 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class USS9GunsmithEngine {
private:
    std::string weaponName = "USS 9";
    std::string caliber = "9x19mm Parabellum";

    // Base USS 9 weapon performance attributes
    float baseDamage = 28.0f;
    float baseFireRate = 71.0f;
    float baseAccuracy = 58.0f;
    float baseRange = 46.0f;
    float baseControl = 52.0f;
    float baseMobility = 84.0f;
    float baseWeight = 2.5f; // kg

    float maxWeightCapacity = 4.2f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    USS9GunsmithEngine() {
        // Initialize signature USS 9 attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Muzzle Brake",          "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"16.2\" Carbine Barrel",     "Barrel",      1.1f,  0.0f, +6.0f, +16.0f, +4.0f, -7.0f},
            {"OWC Marksman Barrel",       "Barrel",      0.8f,  0.0f, +4.0f, +10.0f, +3.0f, -5.0f},
            {"MIP Light Barrel (Short)",  "Barrel",      0.3f,  0.0f, -3.0f,  -6.0f, -2.0f, +6.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.5f,  0.0f, -5.0f,   0.0f, -7.0f,+11.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"RTC Heavy Stock",           "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +6.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +6.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Merc Foregrip",             "Underbarrel", 0.5f,  0.0f, +2.0f,   0.0f, +7.0f, -3.0f},

            // Ammunition
            {"40 Round Extended Mag",     "Magazine",    0.5f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},
            {"50 Round Drum Mag",         "Magazine",    0.9f,  0.0f,  0.0f,   0.0f, -3.0f, -6.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -4.0f,   0.0f, -2.0f, +6.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                      USS 9 GUNSMITH ENGINE                             \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME USS 9 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE USS 9 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    USS9GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = USS9GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting USS 9 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// LAPA GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class LAPAGunsmithEngine {
private:
    std::string weaponName = "LAPA";
    std::string caliber = "9x19mm Parabellum";

    // Base LAPA weapon performance attributes
    float baseDamage = 26.0f;
    float baseFireRate = 70.0f;
    float baseAccuracy = 57.0f;
    float baseRange = 50.0f;
    float baseControl = 54.0f;
    float baseMobility = 82.0f;
    float baseWeight = 2.7f; // kg

    float maxWeightCapacity = 4.2f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    LAPAGunsmithEngine() {
        // Initialize signature LAPA attachments
        availableAttachments = {
            // Muzzle
            {"Agency Suppressor",         "Muzzle",      0.4f,  0.0f, -2.0f, +10.0f, +3.0f, -3.0f},
            {"Sound Suppressor",          "Muzzle",      0.2f,  0.0f,  0.0f,  -4.0f,  0.0f, +2.0f},
            {"Infantry Compensator",      "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +6.0f, -2.0f},

            // Barrel
            {"10.2\" Rifled Barrel",      "Barrel",      0.8f, +3.0f, +3.0f,  +8.0f,  0.0f, -4.0f},
            {"8.9\" Extended Barrel",     "Barrel",      0.6f,  0.0f, +2.0f, +12.0f, +2.0f, -3.0f},
            {"7.9\" Compact Barrel",      "Barrel",      0.3f,  0.0f, -3.0f,  -5.0f, -2.0f, +6.0f},

            // Stock
            {"Raider Stock",              "Stock",       0.2f,  0.0f, -3.0f,   0.0f, -2.0f, +8.0f},
            {"SAS Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"KGB Target Designator",     "Stock",       0.6f,  0.0f, +5.0f,   0.0f, +4.0f, -4.0f},

            // Laser
            {"Tiger Team Spotlight",      "Laser",       0.1f,  0.0f, +3.0f,   0.0f,  0.0f, +5.0f},
            {"Target Team Laser",         "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Field Agent Grip",          "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +6.0f, -3.0f},
            {"Infiltrator Grip",          "Underbarrel", 0.2f,  0.0f, +1.0f,   0.0f, +1.0f, +3.0f},

            // Ammunition
            {"45 Rnd Extended Mag",       "Magazine",    0.5f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},
            {"50 Rnd Fast Mag",           "Magazine",    0.8f,  0.0f,  0.0f,   0.0f, -2.0f, -5.0f},

            // Rear Grip
            {"Serpent Wrap",              "Rear Grip",   0.1f,  0.0f, -2.0f,   0.0f, -1.0f, +5.0f},
            {"Airborne Elastic Wrap",     "Rear Grip",   0.2f,  0.0f, +4.0f,   0.0f, +2.0f, +2.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                      LAPA GUNSMITH ENGINE                              \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME LAPA PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE LAPA ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    LAPAGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = LAPAGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting LAPA Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// OTS 9 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class OTs9GunsmithEngine {
private:
    std::string weaponName = "OTs 9";
    std::string caliber = "9x19mm Parabellum";

    // Base OTs 9 weapon performance attributes
    float baseDamage = 28.0f;
    float baseFireRate = 86.0f;
    float baseAccuracy = 51.0f;
    float baseRange = 42.0f;
    float baseControl = 46.0f;
    float baseMobility = 88.0f;
    float baseWeight = 2.2f; // kg

    float maxWeightCapacity = 3.8f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    OTs9GunsmithEngine() {
        // Initialize signature OTs 9 attachments
        availableAttachments = {
            // Muzzle
            {"GRU Suppressor",            "Muzzle",      0.4f,  0.0f, -2.0f, +10.0f, +3.0f, -3.0f},
            {"Sound Suppressor",          "Muzzle",      0.2f,  0.0f,  0.0f,  -4.0f,  0.0f, +2.0f},
            {"KGB Eliminator",            "Muzzle",      0.3f,  0.0f, +4.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"8.1\" Task Force Barrel",   "Barrel",      0.9f, +4.0f, +2.0f, +12.0f, -3.0f, -5.0f},
            {"7.1\" Rifled Barrel",       "Barrel",      0.6f, +2.0f, +2.0f,  +6.0f,  0.0f, -3.0f},
            {"6.2\" VDV Reinforced",      "Barrel",      0.5f,  0.0f, +3.0f,  +8.0f, +2.0f, -3.0f},

            // Stock
            {"KGB Skeletal Stock",        "Stock",       0.2f,  0.0f, -3.0f,   0.0f, -2.0f, +8.0f},
            {"Spetsnaz Stock",            "Stock",       0.3f,  0.0f, -1.0f,   0.0f, -2.0f, +5.0f},
            {"Tactical Stock",            "Stock",       0.4f,  0.0f, +3.0f,   0.0f, +2.0f, -2.0f},

            // Laser
            {"Tiger Team Spotlight",      "Laser",       0.1f,  0.0f, +3.0f,   0.0f,  0.0f, +5.0f},
            {"GR5 mW Laser Sight",        "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Spetsnaz Speedgrip",        "Underbarrel", 0.4f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},
            {"Patrol Grip",               "Underbarrel", 0.2f,  0.0f, +1.0f,   0.0f,  0.0f, +4.0f},

            // Ammunition
            {"VDV 40 Rnd Fast Mag",       "Magazine",    0.6f,  0.0f,  0.0f,   0.0f, -1.0f, -2.0f},
            {"Spetsnaz 40 Rnd Mag",       "Magazine",    0.5f,  0.0f,  0.0f,   0.0f, -2.0f, -4.0f},

            // Rear Grip
            {"GRU Elastic Wrap",          "Rear Grip",   0.2f,  0.0f, +3.0f,   0.0f, +1.0f, +3.0f},
            {"Serpent Wrap",              "Rear Grip",   0.1f,  0.0f, -2.0f,   0.0f, -1.0f, +5.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                      OTS 9 GUNSMITH ENGINE                             \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME OTS 9 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 110.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE OTS 9 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    OTs9GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = OTs9GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting OTs 9 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// MX9 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class MX9GunsmithEngine {
private:
    std::string weaponName = "MX9";
    std::string caliber = "9x19mm Parabellum / High Caliber";

    // Base MX9 weapon performance attributes
    float baseDamage = 30.0f;
    float baseFireRate = 80.0f;
    float baseAccuracy = 45.0f;
    float baseRange = 41.0f;
    float baseControl = 40.0f;
    float baseMobility = 95.0f;
    float baseWeight = 2.1f; // kg

    float maxWeightCapacity = 3.8f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    MX9GunsmithEngine() {
        // Initialize signature MX9 attachments
        availableAttachments = {
            // Muzzle
            {"Agency Suppressor",         "Muzzle",      0.4f,  0.0f, -2.0f, +10.0f, +3.0f, -3.0f},
            {"Infantry Compensator",      "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +6.0f, -2.0f},
            {"Sound Suppressor",          "Muzzle",      0.2f,  0.0f,  0.0f,  -4.0f,  0.0f, +2.0f},

            // Barrel
            {"9.5\" Task Force Barrel",   "Barrel",      0.9f, +5.0f, +2.0f, +12.0f, -4.0f, -5.0f},
            {"8.6\" Extended Barrel",     "Barrel",      0.6f,  0.0f, +2.0f,  +8.0f, +1.0f, -3.0f},
            {"7.5\" Compact Barrel",      "Barrel",      0.3f,  0.0f, -3.0f,  -5.0f, -2.0f, +6.0f},

            // Stock
            {"Agile Stock",               "Stock",       0.2f,  0.0f, -3.0f,   0.0f, -2.0f, +9.0f},
            {"SAS Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"Striker Stock",             "Stock",       0.4f,  0.0f, +2.0f,   0.0f, +3.0f,  0.0f},

            // Laser
            {"Aim Assist Laser",          "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"Steady Aim Laser",          "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +2.0f},

            // Underbarrel
            {"Field Agent Grip",          "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +5.0f, -3.0f},
            {"Infiltrator Grip",          "Underbarrel", 0.2f,  0.0f, +1.0f,   0.0f, +1.0f, +3.0f},

            // Ammunition
            {"Large Caliber Ammo",        "Magazine",    0.5f, +6.0f, -1.0f,  +5.0f, -2.0f, -3.0f},
            {"Fast Reload Mag",           "Magazine",    0.3f,  0.0f,  0.0f,   0.0f,  0.0f, +2.0f},

            // Rear Grip
            {"Rustle Grip Tape",          "Rear Grip",   0.1f,  0.0f, -2.0f,   0.0f, -1.0f, +5.0f},
            {"Airborne Elastic Wrap",     "Rear Grip",   0.2f,  0.0f, +3.0f,   0.0f, +2.0f, +2.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                       MX9 GUNSMITH ENGINE                              \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME MX9 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE MX9 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    MX9GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = MX9GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting MX9 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// CHICOM GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class ChicomGunsmithEngine {
private:
    std::string weaponName = "Chicom";
    std::string caliber = "9x19mm Parabellum (3-Round Burst)";

    // Base Chicom weapon performance attributes
    float baseDamage = 25.0f;
    float baseFireRate = 65.0f;
    float baseAccuracy = 62.0f;
    float baseRange = 43.0f;
    float baseControl = 57.0f;
    float baseMobility = 80.0f;
    float baseWeight = 2.4f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    ChicomGunsmithEngine() {
        // Initialize signature Chicom attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"OWC Marksman Barrel",       "Barrel",      1.0f,  0.0f, +5.0f, +15.0f, +4.0f, -6.0f},
            {"MIP Strike Barrel",         "Barrel",      0.6f,  0.0f, +2.0f,  +8.0f, +2.0f, -3.0f},
            {"YKM Integrated Suppressor", "Barrel",      0.5f,  0.0f, +1.0f,  +5.0f, +2.0f, -3.0f},

            // Stock
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"MIP Strike Stock",          "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -2.0f},
            {"OWC Ranger Stock",          "Stock",       0.7f,  0.0f, +5.0f,   0.0f, +6.0f, -5.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Operator Foregrip",         "Underbarrel", 0.5f,  0.0f,  0.0f,   0.0f, +7.0f, -4.0f},

            // Ammunition
            {"39 Round Extended Mag",     "Magazine",    0.5f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},
            {"45 Round Extended Mag",     "Magazine",    0.8f,  0.0f,  0.0f,   0.0f, -2.0f, -5.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Rubberized Grip Tape",      "Rear Grip",   0.1f,  0.0f, -2.0f,   0.0f, +5.0f, -2.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                      CHICOM GUNSMITH ENGINE                            \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME CHICOM PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE CHICOM ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
int main() {
    ChicomGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = ChicomGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting Chicom Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Perk, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// RAZORBACK GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class RazorbackGunsmithEngine {
private:
    std::string weaponName = "Razorback";
    std::string caliber = "10mm Auto (Bullpup SMG)";

    // Base Razorback weapon performance attributes
    float baseDamage = 30.0f;
    float baseFireRate = 62.0f;
    float baseAccuracy = 52.0f;
    float baseRange = 50.0f;
    float baseControl = 57.0f;
    float baseMobility = 75.0f;
    float baseWeight = 2.9f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    RazorbackGunsmithEngine() {
        // Initialize signature Razorback attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            
            // Barrel
            {"OWC Marksman Barrel",       "Barrel",      1.1f,  0.0f, +6.0f, +18.0f, +5.0f, -7.0f},
            {"MIP Extended Light Barrel", "Barrel",      0.7f,  0.0f, +3.0f, +10.0f, +2.0f, -4.0f},

            // Stock
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"MIP Strike Stock",          "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -2.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},

            // Perk
            {"Rapid Fire Perk",           "Perk",        0.0f, +5.0f, -3.0f,  -5.0f, -2.0f,  0.0f},

            // Ammunition
            {"Extended Mag A",            "Magazine",    0.5f,  0.0f,  0.0f,   0.0f, -1.0f, -3.0f},
            {"Fast Reload Mag",           "Magazine",    0.3f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                     RAZORBACK GUNSMITH ENGINE                          \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, fr = baseFireRate, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            fr += (att.category == "Perk" && att.name == "Rapid Fire Perk") ? 8.0f : 0.0f;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME RAZORBACK PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", fr, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE RAZORBACK ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    RazorbackGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-11] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = RazorbackGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting Razorback Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// HG 40 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class HG40GunsmithEngine {
private:
    std::string weaponName = "HG 40";
    std::string caliber = "9x19mm Full-Auto";

    // Base HG 40 weapon performance attributes
    float baseDamage = 26.0f;
    float baseFireRate = 60.0f;
    float baseAccuracy = 62.0f;
    float baseRange = 46.0f;
    float baseControl = 51.0f;
    float baseMobility = 78.0f;
    float baseWeight = 2.7f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    HG40GunsmithEngine() {
        // Initialize signature HG 40 attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"OWC Marksman Barrel",       "Barrel",      1.1f,  0.0f, +6.0f, +18.0f, +5.0f, -7.0f},
            {"MIP Strike Barrel",         "Barrel",      0.6f,  0.0f, +2.0f,  +8.0f, +2.0f, -3.0f},
            {"YKM Integral Suppressor",   "Barrel",      0.5f,  0.0f, +1.0f,  +5.0f, +2.0f, -3.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.4f,  0.0f, -4.0f,   0.0f, -5.0f, +8.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"RTC Steady Stock",          "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"RTC Laser 1mW",             "Laser",       0.1f,  0.0f, +2.0f,   0.0f,  0.0f, +2.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Merc Foregrip",             "Underbarrel", 0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -3.0f},

            // Ammunition
            {"40 Round Fast Reload",      "Magazine",    0.4f,  0.0f,  0.0f,   0.0f,  0.0f, -2.0f},
            {"45 Round Extended Mag",     "Magazine",    0.7f,  0.0f,  0.0f,   0.0f, -2.0f, -4.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Rubberized Grip Tape",      "Rear Grip",   0.1f,  0.0f, -2.0f,   0.0f, +5.0f, -2.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                       HG 40 GUNSMITH ENGINE                            \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME HG 40 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE HG 40 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void () {
    HG40GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = HG40GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting HG 40 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// GKS GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class GKSGunsmithEngine {
private:
    std::string weaponName = "GKS";
    std::string caliber = "9x19mm Precision";

    // Base GKS weapon performance attributes
    float baseDamage = 28.0f;
    float baseFireRate = 60.0f;
    float baseAccuracy = 62.0f;
    float baseRange = 50.0f;
    float baseControl = 59.0f;
    float baseMobility = 72.0f;
    float baseWeight = 2.8f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    GKSGunsmithEngine() {
        // Initialize signature GKS attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"OWC Ranger Barrel",         "Barrel",      1.2f,  0.0f, +7.0f, +20.0f, +6.0f, -8.0f},
            {"MIP Extended Light Barrel", "Barrel",      0.7f,  0.0f, +3.0f, +10.0f, +2.0f, -4.0f},
            {"YKM Integral Suppressor",   "Barrel",      0.5f,  0.0f, +1.0f,  +5.0f, +2.0f, -3.0f},

            // Stock
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"YKM Light Stock",           "Stock",       0.2f,  0.0f, -1.0f,   0.0f, -1.0f, +4.0f},
            {"MIP Strike Stock",          "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -2.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"RTC Laser 1mW",             "Laser",       0.1f,  0.0f, +2.0f,   0.0f,  0.0f, +2.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Operator Foregrip",         "Underbarrel", 0.5f,  0.0f,  0.0f,   0.0f, +7.0f, -4.0f},

            // Ammunition
            {"32 Round Fast Reload",      "Magazine",    0.3f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f},
            {"40 Round Extended Mag",     "Magazine",    0.6f,  0.0f,  0.0f,   0.0f, -2.0f, -4.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Rubberized Grip Tape",      "Rear Grip",   0.1f,  0.0f, -2.0f,   0.0f, +5.0f, -2.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                        GKS GUNSMITH ENGINE                             \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME GKS PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE GKS ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
void BlackclawX() {
    GKSGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = GKSGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting GKS Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// VMP GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class VMPGunsmithEngine {
private:
    std::string weaponName = "VMP";
    std::string caliber = "9x19mm High-RPM";

    // Base VMP weapon performance attributes
    float baseDamage = 25.0f;
    float baseFireRate = 90.0f;
    float baseAccuracy = 49.0f;
    float baseRange = 42.0f;
    float baseControl = 43.0f;
    float baseMobility = 84.0f;
    float baseWeight = 2.4f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    VMPGunsmithEngine() {
        // Initialize signature VMP attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"OWC Marksman Barrel",       "Barrel",      1.1f,  0.0f, +6.0f, +18.0f, +5.0f, -7.0f},
            {"MIP Light Barrel (Short)",  "Barrel",      0.3f,  0.0f, -3.0f,  -6.0f, -2.0f, +6.0f},
            {"YKM Integral Suppressor",   "Barrel",      0.5f,  0.0f, +1.0f,  +5.0f, +2.0f, -3.0f},

            // Stock
            {"No Stock",                  "Stock",      -0.4f,  0.0f, -4.0f,   0.0f, -5.0f, +8.0f},
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"RTC Steady Stock",          "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"MIP Laser 5mW",             "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +3.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Ranger Foregrip",           "Underbarrel", 0.6f,  0.0f, +5.0f,   0.0f, +6.0f, -5.0f},

            // Ammunition
            {"48 Round Extended Mag",     "Magazine",    0.6f,  0.0f,  0.0f,   0.0f, -2.0f, -4.0f},
            {"36 Round Fast Reload",      "Magazine",    0.3f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -3.0f,   0.0f, -2.0f, +5.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                        VMP GUNSMITH ENGINE                             \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME VMP PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE VMP ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
int main() {
    VMPGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = VMPGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting VMP Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Magazine, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// AGR 556 GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class AGR556GunsmithEngine {
private:
    std::string weaponName = "AGR 556";
    std::string caliber = "9x19mm Parabellum / 5.56x45mm NATO";

    // Base AGR 556 weapon performance attributes
    float baseDamage = 26.0f;
    float baseFireRate = 67.0f;
    float baseAccuracy = 55.0f;
    float baseRange = 48.0f;
    float baseControl = 57.0f;
    float baseMobility = 73.0f;
    float baseWeight = 3.2f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    AGR556GunsmithEngine() {
        // Initialize signature AGR 556 attachments
        availableAttachments = {
            // Muzzle
            {"Monolithic Suppressor",     "Muzzle",      0.4f,  0.0f, -2.0f, +12.0f,  0.0f, -4.0f},
            {"OWC Light Suppressor",     "Muzzle",      0.2f,  0.0f,  0.0f,  -5.0f,  0.0f, +2.0f},
            {"RTC Light Muzzle Brake",    "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},

            // Barrel
            {"MIP Extended Light Barrel", "Barrel",      0.8f,  0.0f, +4.0f, +12.0f, +3.0f, -5.0f},
            {"OWC Marksman Barrel",       "Barrel",      1.2f,  0.0f, +7.0f, +20.0f, +6.0f, -8.0f},
            {"YKM Integral Suppressor",   "Barrel",      0.5f,  0.0f, +1.0f,  +5.0f, +2.0f, -3.0f},

            // Stock
            {"YKM Combat Stock",          "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +5.0f},
            {"YKM Light Stock",           "Stock",       0.2f,  0.0f, -1.0f,   0.0f, -1.0f, +4.0f},
            {"RTC Steady Stock",          "Stock",       0.6f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},

            // Laser
            {"OWC Laser - Tactical",      "Laser",       0.1f,  0.0f, +4.0f,   0.0f,  0.0f, +4.0f},
            {"RTC Laser 1mW",             "Laser",       0.1f,  0.0f, +2.0f,   0.0f,  0.0f, +2.0f},

            // Underbarrel
            {"Strike Foregrip",           "Underbarrel", 0.4f,  0.0f, +4.0f,   0.0f, +3.0f, -2.0f},
            {"Merc Foregrip",             "Underbarrel", 0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -3.0f},

            // Ammunition
            {"5.56 Reload 30 Round Mag",  "Magazine",    0.5f, +6.0f, -2.0f, +10.0f, -3.0f, -3.0f},
            {"5.56 Reload 60 Round Mag",  "Magazine",    0.9f, +6.0f, -4.0f, +10.0f, -5.0f, -7.0f},
            {"30 Round Fast Reload",      "Magazine",    0.3f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f},

            // Rear Grip
            {"Granulated Grip Tape",      "Rear Grip",   0.1f,  0.0f, +5.0f,   0.0f, +1.0f, -1.0f},
            {"Stippled Grip Tape",        "Rear Grip",   0.1f,  0.0f, -3.0f,   0.0f, -2.0f, +5.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                      AGR 556 GUNSMITH ENGINE                           \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME AGR 556 PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE AGR 556 ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
int main() {
    AGR556GunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-18] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = AGR556GunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting AGR 556 Gunsmith System...\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// -----------------------------------------------------------------------------
// ATTACHMENT STRUCTURE
// -----------------------------------------------------------------------------
struct Attachment {
    std::string name;
    std::string category; // Muzzle, Barrel, Stock, Laser, Underbarrel, Optics, Rear Grip
    float weight;         // Weight in kg
    float damageMod;
    float accuracyMod;
    float rangeMod;
    float controlMod;
    float mobilityMod;
};

// -----------------------------------------------------------------------------
// FSS HURRICANE GUNSMITH ENGINE CLASS
// -----------------------------------------------------------------------------
class FSSHurricaneGunsmithEngine {
private:
    std::string weaponName = "FSS Hurricane";
    std::string caliber = "5.7x28mm Top-Feed";

    // Base FSS Hurricane weapon performance attributes
    float baseDamage = 27.0f;
    float baseFireRate = 73.0f;
    float baseAccuracy = 64.0f;
    float baseRange = 52.0f;
    float baseControl = 61.0f;
    float baseMobility = 76.0f;
    float baseWeight = 2.9f; // kg

    float maxWeightCapacity = 4.0f; // Max custom attachment weight allowance (kg)
    const size_t maxSlots = 5;

    std::vector<Attachment> availableAttachments;
    std::vector<Attachment> equippedAttachments;

public:
    FSSHurricaneGunsmithEngine() {
        // Initialize signature FSS Hurricane attachments
        availableAttachments = {
            // Muzzle
            {"XTEN Razor Comp",           "Muzzle",      0.3f,  0.0f, +3.0f,   0.0f, +5.0f, -2.0f},
            {"Echoless-80 Suppressor",     "Muzzle",      0.4f,  0.0f, -1.0f, +10.0f, +2.0f, -4.0f},
            {"FSS Castle Compensator",    "Muzzle",      0.3f,  0.0f, +2.0f,   0.0f, +6.0f, -3.0f},

            // Barrel
            {"FSS Cannonade 16\" Barrel", "Barrel",      1.1f,  0.0f, +7.0f, +18.0f, +5.0f, -7.0f},
            {"16\" OLE-V Barrel",         "Barrel",      0.8f,  0.0f, +4.0f, +12.0f, +3.0f, -4.0f},
            {"8\" Light Barrel",          "Barrel",      0.3f,  0.0f, -3.0f,  -6.0f, -2.0f, +6.0f},

            // Stock
            {"Demoncomb Collar Stock",    "Stock",       0.3f,  0.0f, -2.0f,   0.0f, -2.0f, +6.0f},
            {"Torrent Span-9 Stock",      "Stock",       0.5f,  0.0f, +3.0f,   0.0f, +4.0f, -3.0f},
            {"Demo Quicksilver Stock",    "Stock",      -0.3f,  0.0f, -4.0f,   0.0f, -4.0f, +7.0f},

            // Laser
            {"FSS OLE-V Laser",           "Laser",       0.1f,  0.0f, +5.0f,   0.0f,  0.0f, +4.0f},
            {"SZ 1mW PEQ Laser",          "Laser",       0.1f,  0.0f, +2.0f,   0.0f,  0.0f, +2.0f},

            // Underbarrel
            {"Lockgrip Precision-40",     "Underbarrel", 0.4f,  0.0f, +5.0f,   0.0f, +4.0f, -3.0f},
            {"VX Pineapple Foregrip",     "Underbarrel", 0.5f,  0.0f, +4.0f,   0.0f, +5.0f, -4.0f},
            {"Phase-3 Grip",              "Underbarrel", 0.4f,  0.0f, +3.0f,   0.0f, +3.0f, -2.0f},

            // Optics
            {"Cronen Mini Pro",           "Optics",      0.2f,  0.0f, +2.0f,   0.0f,  0.0f, -1.0f},
            {"SZ Holotherm",              "Optics",      0.4f,  0.0f, +3.0f,   0.0f,  0.0f, -2.0f},

            // Rear Grip
            {"Sakin ZX Grip",             "Rear Grip",   0.1f,  0.0f, +4.0f,   0.0f, +3.0f, -1.0f},
            {"Phantom Grip",              "Rear Grip",   0.1f,  0.0f, -3.0f,   0.0f, -2.0f, +5.0f}
        };
    }

    float GetTotalWeight() const {
        float total = baseWeight;
        for (const auto& att : equippedAttachments) {
            total += att.weight;
        }
        return total;
    }

    void DisplayHeader() const {
        std::cout << "========================================================================\n";
        std::cout << "                     FSS HURRICANE GUNSMITH ENGINE                      \n";
        std::cout << "========================================================================\n";
        std::cout << " WEAPON: " << weaponName << " | CALIBER: " << caliber << "\n";

        float currentWeight = GetTotalWeight();
        std::cout << " WEIGHT CAPACITY: [" << std::fixed << std::setprecision(1)
                  << currentWeight << " / " << (baseWeight + maxWeightCapacity) << " KG]\n";

        int barWidth = 30;
        float fillRatio = (currentWeight - baseWeight) / maxWeightCapacity;
        if (fillRatio < 0.0f) fillRatio = 0.0f;
        int filled = static_cast<int>(fillRatio * barWidth);

        std::cout << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) std::cout << "#";
            else std::cout << "-";
        }
        std::cout << "] EQUIPPED: (" << equippedAttachments.size() << " / " << maxSlots << " SLOTS)\n";
        std::cout << "------------------------------------------------------------------------\n";
    }

    void DisplayStatsAndAttachments() const {
        float dmg = baseDamage, acc = baseAccuracy, rng = baseRange;
        float ctrl = baseControl, mob = baseMobility;

        for (const auto& att : equippedAttachments) {
            dmg += att.damageMod;
            acc += att.accuracyMod;
            rng += att.rangeMod;
            ctrl += att.controlMod;
            mob += att.mobilityMod;
        }

        std::cout << "\n[ REAL-TIME FSS HURRICANE PERFORMANCE STATS ]\n";
        PrintStatBar("DAMAGE   ", dmg, 100.0f);
        PrintStatBar("FIRE RATE", baseFireRate, 100.0f);
        PrintStatBar("ACCURACY ", acc, 100.0f);
        PrintStatBar("RANGE    ", rng, 100.0f);
        PrintStatBar("CONTROL  ", ctrl, 100.0f);
        PrintStatBar("MOBILITY ", mob, 100.0f);

        std::cout << "\n------------------------------------------------------------------------\n";
        std::cout << " AVAILABLE FSS HURRICANE ATTACHMENTS:\n";
        std::cout << "------------------------------------------------------------------------\n";

        for (size_t i = 0; i < availableAttachments.size(); ++i) {
            const auto& att = availableAttachments[i];
            bool isEquipped = std::any_of(equippedAttachments.begin(), equippedAttachments.end(),
                [&att](const Attachment& e) { return e.name == att.name; });

            std::cout << "  (" << std::setw(2) << (i + 1) << ") [" << (isEquipped ? "X" : " ") << "] "
                      << std::left << std::setw(28) << att.name
                      << " Category: " << std::setw(12) << att.category
                      << " Wt: " << (att.weight >= 0 ? "+" : "") << att.weight << "kg\n";
        }
        std::cout << "------------------------------------------------------------------------\n";
    }

    void PrintStatBar(const std::string& label, float value, float maxValue) const {
        int barLength = 20;
        int filled = static_cast<int>((value / maxValue) * barLength);
        if (filled > barLength) filled = barLength;
        if (filled < 0) filled = 0;

        std::cout << " " << label << " [";
        for (int i = 0; i < barLength; ++i) {
            if (i < filled) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(value) << "\n";
    }

    void ToggleAttachment(int index) {
        if (index < 0 || index >= static_cast<int>(availableAttachments.size())) return;

        const auto& targetAtt = availableAttachments[index];

        auto it = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
            [&targetAtt](const Attachment& att) { return att.name == targetAtt.name; });

        if (it != equippedAttachments.end()) {
            equippedAttachments.erase(it);
        } else {
            if (equippedAttachments.size() >= maxSlots) {
                std::cout << "\n>> ERROR: Slot capacity full (5/5)! Unequip an attachment first.\n";
                return;
            }

            // Replace attachment in same slot if present
            auto slotIt = std::find_if(equippedAttachments.begin(), equippedAttachments.end(),
                [&targetAtt](const Attachment& att) { return att.category == targetAtt.category; });

            if (slotIt != equippedAttachments.end()) {
                equippedAttachments.erase(slotIt);
            }

            if (GetTotalWeight() + targetAtt.weight <= (baseWeight + maxWeightCapacity)) {
                equippedAttachments.push_back(targetAtt);
            } else {
                std::cout << "\n>> ERROR: Weight capacity exceeded!\n";
            }
        }
    }
};

// -----------------------------------------------------------------------------
// MAIN PROGRAM LOOP
// -----------------------------------------------------------------------------
int main() {
    FSSHurricaneGunsmithEngine gunsmith;
    bool active = true;

    while (active) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        gunsmith.DisplayHeader();
        gunsmith.DisplayStatsAndAttachments();

        std::cout << "\nCONTROLS:\n";
        std::cout << " [1-17] Toggle Attachment | [0] Clear Loadout | [Q] Exit Hub\n";
        std::cout << " Enter Choice: ";

        std::string input;
        std::cin >> input;

        if (input == "q" || input == "Q") {
            active = false;
        } else if (input == "0") {
            gunsmith = FSSHurricaneGunsmithEngine();
        } else {
            try {
                int choice = std::stoi(input);
                gunsmith.ToggleAttachment(choice - 1);
            } catch (...) {
                // Ignore non-numeric inputs
            }
        }
    }

    std::cout << "\nExiting FSS Hurricane Gunsmith System...\n";
    return 0;
}

using System.Collections;
using UnityEngine;

public class PharoWeapon : MonoBehaviour
{
    [System.Serializable]
    public class WeaponStats
    {
        [Header("General Settings")]
        public string weaponName = "Pharo SMG";
        public LayerMask hitLayers;

        [Header("Burst Mechanics")]
        public int roundsPerBurst = 4;
        public float timeBetweenShots = 0.055f; // Fast internal burst rate (~1090 RPM)
        public float burstDelay = 0.20f;       // Cooldown between bursts

        [Header("Damage Profile")]
        public float baseDamage = 34f;         // 3-shot kill close range
        public float headshotMultiplier = 1.4f;
        public float maxRange = 35f;

        [Header("Ammo & Reload")]
        public int magazineCapacity = 36;
        public float reloadTime = 1.75f;

        [Header("Procedural Recoil")]
        public Vector3 verticalRecoil = new Vector3(-1.2f, 0f, 0f);
        public Vector3 horizontalRecoil = new Vector3(0f, 0.4f, 0f);
        public float recoilSnappiness = 20f;
        public float recoilReturnSpeed = 10f;
    }

    [Header("Configuration")]
    [SerializeField] private WeaponStats stats;

    [Header("References")]
    [SerializeField] private Transform muzzlePoint;
    [SerializeField] private Camera playerCamera;
    [SerializeField] private ParticleSystem muzzleFlash;
    [SerializeField] private GameObject hitImpactPrefab;
    [SerializeField] private AudioSource audioSource;
    [SerializeField] private AudioClip fireSound;
    [SerializeField] private AudioClip reloadSound;

    // Internal State
    private int currentAmmo;
    private bool isFiring = false;
    private bool isReloading = false;

    // Recoil Tracking
    private Vector3 currentRotation;
    private Vector3 targetRotation;

    private void Start()
    {
        currentAmmo = stats.magazineCapacity;
    }

    private void Update()
    {
        // Smoothly return camera from recoil
        targetRotation = Vector3.Lerp(targetRotation, Vector3.zero, stats.recoilReturnSpeed * Time.deltaTime);
        currentRotation = Vector3.Slerp(currentRotation, targetRotation, stats.recoilSnappiness * Time.fixedDeltaTime);
        playerCamera.transform.localRotation *= Quaternion.Euler(currentRotation);

        // Player Input
        if (Input.GetButtonDown("Fire1") && !isFiring && !isReloading)
        {
            if (currentAmmo >= stats.roundsPerBurst)
            {
                StartCoroutine(FireBurstRoutine());
            }
            else if (currentAmmo > 0)
            {
                StartCoroutine(FirePartialBurstRoutine(currentAmmo));
            }
            else
            {
                StartCoroutine(ReloadRoutine());
            }
        }

        if (Input.GetKeyDown(KeyCode.R) && currentAmmo < stats.magazineCapacity && !isReloading)
        {
            StartCoroutine(ReloadRoutine());
        }
    }

    private IEnumerator FireBurstRoutine()
    {
        isFiring = true;

        for (int i = 0; i < stats.roundsPerBurst; i++)
        {
            ExecuteSingleShot();
            currentAmmo--;

            yield return new WaitForSeconds(stats.timeBetweenShots);
        }

        yield return new WaitForSeconds(stats.burstDelay);
        isFiring = false;
    }

    private IEnumerator FirePartialBurstRoutine(int remainingRounds)
    {
        isFiring = true;

        for (int i = 0; i < remainingRounds; i++)
        {
            ExecuteSingleShot();
            currentAmmo--;

            yield return new WaitForSeconds(stats.timeBetweenShots);
        }

        yield return new WaitForSeconds(stats.burstDelay);
        isFiring = false;
    }

    private void ExecuteSingleShot()
    {
        // Audio & Visual Effects
        if (muzzleFlash) muzzleFlash.Play();
        if (audioSource && fireSound) audioSource.PlayOneShot(fireSound);

        // Apply Recoil Kick
        ApplyRecoil();

        // Raycast Hit Detection
        Ray ray = playerCamera.ViewportPointToRay(new Vector3(0.5f, 0.5f, 0));
        if (Physics.Raycast(ray, out RaycastHit hit, stats.maxRange, stats.hitLayers))
        {
            float damage = stats.baseDamage;
            if (hit.collider.CompareTag("Head"))
            {
                damage *= stats.headshotMultiplier;
            }

            // Damage Interface Check
            IDamageable target = hit.collider.GetComponent<IDamageable>();
            if (target != null)
            {
                target.TakeDamage(damage);
            }

            // Spawn Impact Visual
            if (hitImpactPrefab)
            {
                GameObject impact = Instantiate(hitImpactPrefab, hit.point, Quaternion.LookRotation(hit.normal));
                Destroy(impact, 2.0f);
            }
        }
    }

    private void ApplyRecoil()
    {
        float randomHorizontal = Random.Range(-stats.horizontalRecoil.y, stats.horizontalRecoil.y);
        targetRotation += new Vector3(stats.verticalRecoil.x, randomHorizontal, 0f);
    }

    private IEnumerator ReloadRoutine()
    {
        isReloading = true;
        if (audioSource && reloadSound) audioSource.PlayOneShot(reloadSound);

        yield return new WaitForSeconds(stats.reloadTime);

        currentAmmo = stats.magazineCapacity;
        isReloading = false;
    }
}

// Damage Interface to apply damage to enemies or bots
public interface IDamageable
{
    void TakeDamage(float amount);
}

#pragma once

#include "CoreMinimal.h"
#include "LoadoutTypes.generated.h"

// Attachment Categories (Gunsmith)
UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
    Muzzle,
    Barrel,
    Optic,
    Stock,
    Underbarrel,
    Laser,
    Ammunition,
    RearGrip,
    Perk
};

// Attachment Structure
USTRUCT(BlueprintType)
struct FWeaponAttachment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
    FString AttachmentName = "Default";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
    EAttachmentType Category = EAttachmentType::Muzzle;

    // Stat Modifiers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float ADSSpeedModifier = 0.0f; // e.g., -0.05s

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DamageRangeModifier = 1.0f; // Multiplier

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float RecoilModifier = 1.0f;
};

// Customized Weapon Configuration
USTRUCT(BlueprintType)
struct FCustomWeaponConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FString WeaponName = "DL Q33";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gunsmith")
    TArray<FWeaponAttachment> EquippedAttachments;

    // Maximum 5 attachments allowed in COD Gunsmith
    static const int32 MAX_ATTACHMENTS = 5;
};

// Full Player Loadout Structure
USTRUCT(BlueprintType)
struct FPlayerLoadout
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
    FString LoadoutName = "Custom Loadout 1";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    FCustomWeaponConfig PrimarySniper;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    FString SecondaryWeapon = "MW11 Pistol";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FString LethalEquipment = "Frag Grenade";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FString TacticalEquipment = "Trophy System";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perks")
    TArray<FString> Perks = { "Lightweight", "Toughness", "Dead Silence" };
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LoadoutTypes.h"
#include "LoadoutManager.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ULoadoutManager : public UActorComponent
{
    GENERATED_BODY()

public:	
    ULoadoutManager();

protected:
    virtual void BeginPlay() override;

public:	
    // Loadout Slot Operations
    UFUNCTION(BlueprintCallable, Category = "Loadout")
    void CreateNewLoadout(FString LoadoutName, FString PrimarySniperName);

    UFUNCTION(BlueprintCallable, Category = "Loadout")
    void EquipLoadoutSlot(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Loadout")
    FPlayerLoadout GetActiveLoadout() const { return ActiveLoadout; }

    // Gunsmith Operations
    UFUNCTION(BlueprintCallable, Category = "Gunsmith")
    bool AddAttachmentToPrimary(int32 LoadoutIndex, FWeaponAttachment NewAttachment);

    UFUNCTION(BlueprintCallable, Category = "Gunsmith")
    bool RemoveAttachmentFromPrimary(int32 LoadoutIndex, EAttachmentType Category);

    // Sniper Catalog Query
    UFUNCTION(BlueprintCallable, Category = "Catalog")
    TArray<FString> GetAvailableSniperCatalog() const { return SniperRifleCatalog; }

private:
    // Available 13 COD Sniper Rifles
    TArray<FString> SniperRifleCatalog;

    // Saved Player Loadouts (up to 10 slots)
    UPROPERTY(EditAnywhere, Category = "Loadout Storage")
    TArray<FPlayerLoadout> SavedLoadouts;

    // Active Loadout in game
    UPROPERTY(VisibleAnywhere, Category = "Loadout Storage")
    FPlayerLoadout ActiveLoadout;

    int32 ActiveLoadoutIndex = 0;

    void InitializeSniperCatalog();
    void BuildDefaultLoadoutPresets();
};

#include "LoadoutManager.h"

ULoadoutManager::ULoadoutManager()
{
    PrimaryComponentTick.bCanEverTick = false;
    InitializeSniperCatalog();
}

void ULoadoutManager::BeginPlay()
{
    Super::BeginPlay();
    BuildDefaultLoadoutPresets();
    EquipLoadoutSlot(0); // Equip slot 1 by default on spawn
}

void ULoadoutManager::InitializeSniperCatalog()
{
    SniperRifleCatalog = {
        "DL Q33 (Intervention)",
        "Locus",
        "LW3-Tundra",
        "Koshka",
        "Arctic .50",
        "Rytec AMR",
        "HDR",
        "XPR-50",
        "M21 EBR",
        "Outlaw",
        "NA-45",
        "ZRG 20mm",
        "SVD (Dragunov)"
    };
}

void ULoadoutManager::BuildDefaultLoadoutPresets()
{
    SavedLoadouts.Empty();

    // Default Loadout 1: DL Q33 Quickscope Preset
    FPlayerLoadout Loadout1;
    Loadout1.LoadoutName = "Aggressive Sniper (DL Q33)";
    Loadout1.PrimarySniper.WeaponName = "DL Q33 (Intervention)";

    FWeaponAttachment OWC_Laser;
    OWC_Laser.AttachmentName = "OWC Laser - Tactical";
    OWC_Laser.Category = EAttachmentType::Laser;
    OWC_Laser.ADSSpeedModifier = -0.04f;
    Loadout1.PrimarySniper.EquippedAttachments.Add(OWC_Laser);

    FWeaponAttachment MIP_Barrel;
    MIP_Barrel.AttachmentName = "MIP Light Barrel";
    MIP_Barrel.Category = EAttachmentType::Barrel;
    MIP_Barrel.ADSSpeedModifier = -0.05f;
    Loadout1.PrimarySniper.EquippedAttachments.Add(MIP_Barrel);

    SavedLoadouts.Add(Loadout1);

    // Default Loadout 2: Locus Balanced Preset
    FPlayerLoadout Loadout2;
    Loadout2.LoadoutName = "Speed Demon (Locus)";
    Loadout2.PrimarySniper.WeaponName = "Locus";
    SavedLoadouts.Add(Loadout2);

    // Default Loadout 3: HDR Hardscope Preset
    FPlayerLoadout Loadout3;
    Loadout3.LoadoutName = "Long Range (HDR)";
    Loadout3.PrimarySniper.WeaponName = "HDR";
    SavedLoadouts.Add(Loadout3);
}

void ULoadoutManager::CreateNewLoadout(FString LoadoutName, FString PrimarySniperName)
{
    if (!SniperRifleCatalog.Contains(PrimarySniperName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Sniper Rifle Name: %s"), *PrimarySniperName);
        return;
    }

    FPlayerLoadout NewLoadout;
    NewLoadout.LoadoutName = LoadoutName;
    NewLoadout.PrimarySniper.WeaponName = PrimarySniperName;

    SavedLoadouts.Add(NewLoadout);
    UE_LOG(LogTemp, Log, TEXT("Created Loadout: %s with Sniper: %s"), *LoadoutName, *PrimarySniperName);
}

void ULoadoutManager::EquipLoadoutSlot(int32 SlotIndex)
{
    if (SavedLoadouts.IsValidIndex(SlotIndex))
    {
        ActiveLoadoutIndex = SlotIndex;
        ActiveLoadout = SavedLoadouts[SlotIndex];
        UE_LOG(LogTemp, Log, TEXT("Equipped Loadout Slot %d: %s [%s]"), 
            SlotIndex + 1, *ActiveLoadout.LoadoutName, *ActiveLoadout.PrimarySniper.WeaponName);
    }
}

bool ULoadoutManager::AddAttachmentToPrimary(int32 LoadoutIndex, FWeaponAttachment NewAttachment)
{
    if (!SavedLoadouts.IsValidIndex(LoadoutIndex)) return false;

    FCustomWeaponConfig& SniperConfig = SavedLoadouts[LoadoutIndex].PrimarySniper;

    // Check 5-attachment Gunsmith Limit
    if (SniperConfig.EquippedAttachments.Num() >= FCustomWeaponConfig::MAX_ATTACHMENTS)
    {
        UE_LOG(LogTemp, Warning, TEXT("Gunsmith Full! Cannot equip more than 5 attachments."));
        return false;
    }

    // Replace if same category exists
    RemoveAttachmentFromPrimary(LoadoutIndex, NewAttachment.Category);

    SniperConfig.EquippedAttachments.Add(NewAttachment);
    UE_LOG(LogTemp, Log, TEXT("Added %s to %s"), *NewAttachment.AttachmentName, *SniperConfig.WeaponName);
    return true;
}

bool ULoadoutManager::RemoveAttachmentFromPrimary(int32 LoadoutIndex, EAttachmentType Category)
{
    if (!SavedLoadouts.IsValidIndex(LoadoutIndex)) return false;

    FCustomWeaponConfig& SniperConfig = SavedLoadouts[LoadoutIndex].PrimarySniper;

    for (int32 i = 0; i < SniperConfig.EquippedAttachments.Num(); ++i)
    {
        if (SniperConfig.EquippedAttachments[i].Category == Category)
        {
            SniperConfig.EquippedAttachments.RemoveAt(i);
            return true;
        }
    }
    return false;
}

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the DL Q33 Sniper Rifle
class DLQ33MeshGenerator
{
public:
    DLQ33MeshGenerator() = default;

    // Generates complete mesh geometry for all DL Q33 components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildReceiverAndFrame());
        FullWeaponMesh.push_back(BuildHeavyFlutedBarrel());
        FullWeaponMesh.push_back(BuildScopeRailAndOptic());
        FullWeaponMesh.push_back(BuildBipodAssembly());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs Receiver, Magazine Well, and Rear Stock
    MeshData BuildReceiverAndFrame()
    {
        MeshData ReceiverMesh;
        ReceiverMesh.MeshSegmentName = "ReceiverAndStock";

        // Main Receiver Block
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(25.0f, 3.5f, 5.0f), ReceiverMesh);
        // Magazine Well
        AddBoxPrimitive(Vector3D(-5.0f, 0.0f, -6.0f), Vector3D(8.0f, 3.0f, 4.0f), ReceiverMesh);
        // Skeletonized Stock Frame
        AddBoxPrimitive(Vector3D(-35.0f, 0.0f, -2.0f), Vector3D(12.0f, 2.5f, 6.0f), ReceiverMesh);

        return ReceiverMesh;
    }

    // Constructs Heavy Anti-Materiel Barrel and Muzzle Brake
    MeshData BuildHeavyFlutedBarrel()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "BarrelAndMuzzle";

        // Extended Heavy Barrel
        AddBoxPrimitive(Vector3D(50.0f, 0.0f, 1.5f), Vector3D(30.0f, 1.8f, 1.8f), BarrelMesh);
        // Square Dual-Port Muzzle Brake
        AddBoxPrimitive(Vector3D(83.0f, 0.0f, 1.5f), Vector3D(5.0f, 3.2f, 3.2f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Top Mounting Bridge and Scope Assembly
    MeshData BuildScopeRailAndOptic()
    {
        MeshData ScopeMesh;
        ScopeMesh.MeshSegmentName = "ScopeAndRail";

        // Picatinny Rail
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, 6.5f), Vector3D(18.0f, 2.0f, 1.2f), ScopeMesh);
        // High-Power Optics Unit
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, 11.5f), Vector3D(14.0f, 2.5f, 2.5f), ScopeMesh);

        return ScopeMesh;
    }

    // Constructs Front Bipod Supports
    MeshData BuildBipodAssembly()
    {
        MeshData BipodMesh;
        BipodMesh.MeshSegmentName = "BipodAssembly";

        // Left Leg
        AddBoxPrimitive(Vector3D(30.0f, -3.0f, -8.0f), Vector3D(1.2f, 1.0f, 10.0f), BipodMesh);
        // Right Leg
        AddBoxPrimitive(Vector3D(30.0f, 3.0f, -8.0f), Vector3D(1.2f, 1.0f, 10.0f), BipodMesh);

        return BipodMesh;
    }
};

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the Locus Sniper Rifle
class LocusMeshGenerator
{
public:
    LocusMeshGenerator() = default;

    // Generates complete mesh geometry for all Locus components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildReceiverAndChassis());
        FullWeaponMesh.push_back(BuildPrecisionBarrelAndMuzzle());
        FullWeaponMesh.push_back(BuildTacticalScopeAndRail());
        FullWeaponMesh.push_back(BuildBoltAndStockAssembly());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs the Main Receiver Block, Handguard, and Curved Pistol Grip
    MeshData BuildReceiverAndChassis()
    {
        MeshData ChassisMesh;
        ChassisMesh.MeshSegmentName = "ReceiverAndChassis";

        // Main Sleek Receiver Body
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(22.0f, 2.5f, 4.5f), ChassisMesh);
        // Slanted Forward Handguard Frame
        AddBoxPrimitive(Vector3D(18.0f, 0.0f, -0.5f), Vector3D(12.0f, 2.0f, 3.5f), ChassisMesh);
        // Angular Detachable Magazine
        AddBoxPrimitive(Vector3D(2.0f, 0.0f, -5.5f), Vector3D(6.0f, 2.2f, 4.5f), ChassisMesh);
        // Ergonomic Pistol Grip (Angled Downward)
        AddBoxPrimitive(Vector3D(-10.0f, 0.0f, -6.0f), Vector3D(3.0f, 1.8f, 5.0f), ChassisMesh);

        return ChassisMesh;
    }

    // Constructs Long Slim Precision Barrel and Multi-Port Flash Hider
    MeshData BuildPrecisionBarrelAndMuzzle()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "PrecisionBarrel";

        // Extended Tapered Barrel
        AddBoxPrimitive(Vector3D(52.0f, 0.0f, 1.0f), Vector3D(28.0f, 1.3f, 1.3f), BarrelMesh);
        // Compact Multi-Slot Muzzle Device
        AddBoxPrimitive(Vector3D(82.0f, 0.0f, 1.0f), Vector3D(4.0f, 2.0f, 2.0f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Integrated Optics Mount, Scope Body, and Lens Bezel
    MeshData BuildTacticalScopeAndRail()
    {
        MeshData ScopeMesh;
        ScopeMesh.MeshSegmentName = "TacticalScope";

        // Low-Profile Picatinny Rail
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 5.5f), Vector3D(14.0f, 1.8f, 1.0f), ScopeMesh);
        // Main Optical Scope Tube
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 9.5f), Vector3D(13.0f, 2.0f, 2.0f), ScopeMesh);
        // Front Objective Lens Housing (Wider Bell)
        AddBoxPrimitive(Vector3D(10.0f, 0.0f, 9.5f), Vector3D(3.0f, 2.8f, 2.8f), ScopeMesh);

        return ScopeMesh;
    }

    // Constructs Rear Stock, Adjustable Cheek Riser, and Side Bolt Handle
    MeshData BuildBoltAndStockAssembly()
    {
        MeshData StockMesh;
        StockMesh.MeshSegmentName = "StockAndBolt";

        // Lightweight Skeleton Stock Body
        AddBoxPrimitive(Vector3D(-32.0f, 0.0f, -1.0f), Vector3D(12.0f, 2.0f, 4.5f), StockMesh);
        // Elevated Cheek Riser
        AddBoxPrimitive(Vector3D(-30.0f, 0.0f, 4.0f), Vector3D(7.0f, 2.2f, 1.2f), StockMesh);
        // Side-Mounted Straight Bolt Handle (Right Side)
        AddBoxPrimitive(Vector3D(-8.0f, 3.2f, 1.0f), Vector3D(1.5f, 2.5f, 1.0f), StockMesh);

        return StockMesh;
    }
};

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the Arctic .50 Sniper Rifle
class Arctic50MeshGenerator
{
public:
    Arctic50MeshGenerator() = default;

    // Generates complete mesh geometry for all Arctic .50 components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildReceiverAndEjectionPort());
        FullWeaponMesh.push_back(BuildThumbholeStockAssembly());
        FullWeaponMesh.push_back(BuildHeavyCaliberBarrelAndBrake());
        FullWeaponMesh.push_back(BuildOpticsAndThermalRail());
        FullWeaponMesh.push_back(BuildMagazineAndBipod());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs the Wide .50 Caliber Upper/Lower Receiver and Ejection Port
    MeshData BuildReceiverAndEjectionPort()
    {
        MeshData ReceiverMesh;
        ReceiverMesh.MeshSegmentName = "ReceiverAndEjectionPort";

        // Main Heavy Upper Receiver Block
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(26.0f, 3.8f, 5.5f), ReceiverMesh);
        // Lower Receiver Frame
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, -4.5f), Vector3D(20.0f, 3.2f, 2.5f), ReceiverMesh);
        // Semi-Auto Right-Side Ejection Port Recess
        AddBoxPrimitive(Vector3D(-4.0f, 2.0f, 1.0f), Vector3D(6.0f, 0.5f, 2.0f), ReceiverMesh);

        return ReceiverMesh;
    }

    // Constructs the Signature Tactical Thumbhole Stock and Recoil Pad
    MeshData BuildThumbholeStockAssembly()
    {
        MeshData StockMesh;
        StockMesh.MeshSegmentName = "ThumbholeStock";

        // Main Upper Buttstock Section
        AddBoxPrimitive(Vector3D(-34.0f, 0.0f, 0.0f), Vector3D(12.0f, 2.8f, 5.0f), StockMesh);
        // Lower Connecting Bridge (Creating the Thumbhole Cutout)
        AddBoxPrimitive(Vector3D(-28.0f, 0.0f, -6.0f), Vector3D(10.0f, 2.0f, 1.5f), StockMesh);
        // Integrated Pistol Grip (Front boundary of the thumbhole)
        AddBoxPrimitive(Vector3D(-16.0f, 0.0f, -5.5f), Vector3D(3.2f, 2.2f, 4.5f), StockMesh);
        // Thick Rubberized Heavy Recoil Pad
        AddBoxPrimitive(Vector3D(-47.0f, 0.0f, -0.5f), Vector3D(1.8f, 3.2f, 6.0f), StockMesh);

        return StockMesh;
    }

    // Constructs the Heavy Anti-Materiel Barrel and Multi-Slot Muzzle Brake
    MeshData BuildHeavyCaliberBarrelAndBrake()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "HeavyBarrelAndBrake";

        // Reinforced Quad-Rail Handguard
        AddBoxPrimitive(Vector3D(28.0f, 0.0f, 0.0f), Vector3D(10.0f, 3.0f, 4.0f), BarrelMesh);
        // Thick .50 Caliber Heavy Barrel Tube
        AddBoxPrimitive(Vector3D(56.0f, 0.0f, 0.8f), Vector3D(24.0f, 2.0f, 2.0f), BarrelMesh);
        // Massive Rectangular Dual-Chamber Muzzle Brake
        AddBoxPrimitive(Vector3D(84.0f, 0.0f, 0.8f), Vector3D(6.0f, 4.0f, 3.5f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Rail System and Magnified Scope Unit
    MeshData BuildOpticsAndThermalRail()
    {
        MeshData ScopeMesh;
        ScopeMesh.MeshSegmentName = "ScopeAndRail";

        // Top Continuous Picatinny Mounting Rail
        AddBoxPrimitive(Vector3D(2.0f, 0.0f, 6.5f), Vector3D(20.0f, 2.2f, 1.0f), ScopeMesh);
        // Dual Scope Mounting Rings
        AddBoxPrimitive(Vector3D(-6.0f, 0.0f, 8.5f), Vector3D(2.0f, 2.2f, 1.5f), ScopeMesh);
        AddBoxPrimitive(Vector3D(8.0f, 0.0f, 8.5f), Vector3D(2.0f, 2.2f, 1.5f), ScopeMesh);
        // High-Magnification Tactical Scope Body
        AddBoxPrimitive(Vector3D(1.0f, 0.0f, 11.0f), Vector3D(15.0f, 2.5f, 2.5f), ScopeMesh);

        return ScopeMesh;
    }

    // Constructs Curved Heavy Box Magazine and Folded Bipod Support
    MeshData BuildMagazineAndBipod()
    {
        MeshData BottomMesh;
        BottomMesh.MeshSegmentName = "MagazineAndBipod";

        // Curved High-Capacity .50 BMG Box Magazine
        AddBoxPrimitive(Vector3D(2.0f, 0.0f, -8.0f), Vector3D(7.0f, 2.8f, 6.0f), BottomMesh);
        // Front Underside Bipod Mount Point & Legs
        AddBoxPrimitive(Vector3D(34.0f, 0.0f, -3.0f), Vector3D(2.0f, 2.5f, 1.5f), BottomMesh);
        AddBoxPrimitive(Vector3D(34.0f, -3.5f, -9.0f), Vector3D(1.2f, 1.2f, 8.0f), BottomMesh);
        AddBoxPrimitive(Vector3D(34.0f, 3.5f, -9.0f), Vector3D(1.2f, 1.2f, 8.0f), BottomMesh);

        return BottomMesh;
    }
};

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the XPR-50 Sniper Rifle
class XPR50MeshGenerator
{
public:
    XPR50MeshGenerator() = default;

    // Generates complete mesh geometry for all XPR-50 components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildFuturisticChassisAndGrip());
        FullWeaponMesh.push_back(BuildBullpupStockAndMagazine());
        FullWeaponMesh.push_back(BuildShieldedBarrelAndMuzzle());
        FullWeaponMesh.push_back(BuildElectronicScopeAndRail());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs the Main Forward Receiver Block and Angled Pistol Grip
    MeshData BuildFuturisticChassisAndGrip()
    {
        MeshData ChassisMesh;
        ChassisMesh.MeshSegmentName = "ChassisAndGrip";

        // Angular Center Receiver Core
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(20.0f, 3.0f, 4.5f), ChassisMesh);
        // Forward Handguard Cover
        AddBoxPrimitive(Vector3D(18.0f, 0.0f, -0.5f), Vector3D(10.0f, 2.5f, 3.8f), ChassisMesh);
        // High-Tech Ergonomic Grip with Trigger Guard Bridge
        AddBoxPrimitive(Vector3D(-4.0f, 0.0f, -6.0f), Vector3D(2.8f, 1.8f, 5.0f), ChassisMesh);
        AddBoxPrimitive(Vector3D(1.0f, 0.0f, -4.5f), Vector3D(4.0f, 1.2f, 1.0f), ChassisMesh);

        return ChassisMesh;
    }

    // Constructs the Rear Bullpup Feed, Magazine, and Buttstock
    MeshData BuildBullpupStockAndMagazine()
    {
        MeshData StockMesh;
        StockMesh.MeshSegmentName = "BullpupStockAndMag";

        // Rear Bullpup Receiver Housing
        AddBoxPrimitive(Vector3D(-24.0f, 0.0f, 0.5f), Vector3D(12.0f, 2.8f, 5.0f), StockMesh);
        // Rear-Mounted Curved Magazine (Bullpup Configuration behind trigger)
        AddBoxPrimitive(Vector3D(-22.0f, 0.0f, -6.5f), Vector3D(5.5f, 2.2f, 5.5f), StockMesh);
        // Integrated Polymer Buttpad
        AddBoxPrimitive(Vector3D(-37.0f, 0.0f, 0.0f), Vector3D(1.5f, 3.0f, 6.0f), StockMesh);

        return StockMesh;
    }

    // Constructs Heat-Shielded Heavy Barrel and Multi-Chamber Muzzle Brake
    MeshData BuildShieldedBarrelAndMuzzle()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "ShieldedBarrel";

        // Ventilated Barrel Heat Shield
        AddBoxPrimitive(Vector3D(34.0f, 0.0f, 0.5f), Vector3D(12.0f, 2.2f, 2.5f), BarrelMesh);
        // Extended Semi-Auto Barrel Tube
        AddBoxPrimitive(Vector3D(58.0f, 0.0f, 0.5f), Vector3D(20.0f, 1.5f, 1.5f), BarrelMesh);
        // Angular Cylindrical Multi-Port Muzzle Compensator
        AddBoxPrimitive(Vector3D(80.0f, 0.0f, 0.5f), Vector3D(5.0f, 2.5f, 2.5f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Electronic Optic Housing and Top Mounting Rail
    MeshData BuildElectronicScopeAndRail()
    {
        MeshData ScopeMesh;
        ScopeMesh.MeshSegmentName = "ElectronicScope";

        // Top Continuous Picatinny Rail
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, 5.8f), Vector3D(18.0f, 2.0f, 1.0f), ScopeMesh);
        // Blocky High-Tech Scope Housing (XPR-50 Digital Optic)
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, 9.5f), Vector3D(12.0f, 2.6f, 2.8f), ScopeMesh);
        // Front Objective Display Bezel
        AddBoxPrimitive(Vector3D(8.0f, 0.0f, 9.5f), Vector3D(2.5f, 3.0f, 3.0f), ScopeMesh);

        return ScopeMesh;
    }
};

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the M21 EBR Sniper Rifle
class M21EBRMeshGenerator
{
public:
    M21EBRMeshGenerator() = default;

    // Generates complete mesh geometry for all M21 EBR components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildReceiverAndAction());
        FullWeaponMesh.push_back(BuildQuadRailHandguard());
        FullWeaponMesh.push_back(BuildTelescopingStockAndGrip());
        FullWeaponMesh.push_back(BuildBarrelAndFlashHider());
        FullWeaponMesh.push_back(BuildOpticAndMagazine());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs Receiver Body and M14 Rotating Bolt Action Area
    MeshData BuildReceiverAndAction()
    {
        MeshData ReceiverMesh;
        ReceiverMesh.MeshSegmentName = "ReceiverAndAction";

        // Lower Metal Chassis Base
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, -0.5f), Vector3D(18.0f, 2.6f, 3.5f), ReceiverMesh);
        // M14 Receiver Housing
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, 2.0f), Vector3D(14.0f, 2.4f, 2.2f), ReceiverMesh);
        // Right-Side Charging Handle & Operating Rod
        AddBoxPrimitive(Vector3D(3.0f, 2.0f, 1.8f), Vector3D(8.0f, 0.8f, 0.8f), ReceiverMesh);

        return ReceiverMesh;
    }

    // Constructs the Aluminum Chassis Handguard with 4 Picatinny Rails
    MeshData BuildQuadRailHandguard()
    {
        MeshData RailMesh;
        RailMesh.MeshSegmentName = "QuadRailHandguard";

        // Main Rectangular Handguard Body
        AddBoxPrimitive(Vector3D(24.0f, 0.0f, 0.5f), Vector3D(12.0f, 2.4f, 2.8f), RailMesh);
        // Top Rail Strip
        AddBoxPrimitive(Vector3D(24.0f, 0.0f, 3.5f), Vector3D(12.0f, 1.8f, 0.5f), RailMesh);
        // Bottom Rail Strip
        AddBoxPrimitive(Vector3D(24.0f, 0.0f, -2.5f), Vector3D(12.0f, 1.8f, 0.5f), RailMesh);
        // Left & Right Side Rails
        AddBoxPrimitive(Vector3D(24.0f, -2.0f, 0.5f), Vector3D(12.0f, 0.5f, 1.8f), RailMesh);
        AddBoxPrimitive(Vector3D(24.0f, 2.0f, 0.5f), Vector3D(12.0f, 0.5f, 1.8f), RailMesh);

        return RailMesh;
    }

    // Constructs Pistol Grip, Dual Metal Guide Struts, and Stock with Cheek Rest
    MeshData BuildTelescopingStockAndGrip()
    {
        MeshData StockMesh;
        StockMesh.MeshSegmentName = "TelescopingStock";

        // Vertical Pistol Grip
        AddBoxPrimitive(Vector3D(-10.0f, 0.0f, -5.5f), Vector3D(2.6f, 1.6f, 4.8f), StockMesh);
        // Twin Metal Telescoping Stock Guide Rods
        AddBoxPrimitive(Vector3D(-22.0f, -1.8f, 0.5f), Vector3D(12.0f, 0.5f, 0.5f), StockMesh);
        AddBoxPrimitive(Vector3D(-22.0f, 1.8f, 0.5f), Vector3D(12.0f, 0.5f, 0.5f), StockMesh);
        // Adjustable Polymer Buttplate
        AddBoxPrimitive(Vector3D(-34.0f, 0.0f, -0.5f), Vector3D(1.5f, 2.6f, 5.5f), StockMesh);
        // Height-Adjustable Cheek Comb Rest
        AddBoxPrimitive(Vector3D(-28.0f, 0.0f, 3.2f), Vector3D(6.0f, 2.0f, 1.2f), StockMesh);

        return StockMesh;
    }

    // Constructs Medium-Contour Match Barrel, Gas Block, and Long Cage Flash Hider
    MeshData BuildBarrelAndFlashHider()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "BarrelAndFlashHider";

        // Exposed Front Match Barrel
        AddBoxPrimitive(Vector3D(52.0f, 0.0f, 0.5f), Vector3D(16.0f, 1.2f, 1.2f), BarrelMesh);
        // Gas System Cylinder Assembly (Below Barrel)
        AddBoxPrimitive(Vector3D(42.0f, 0.0f, -1.2f), Vector3D(6.0f, 1.0f, 1.0f), BarrelMesh);
        // Slender M14 Style Slotted Cage Flash Hider
        AddBoxPrimitive(Vector3D(72.0f, 0.0f, 0.5f), Vector3D(5.0f, 1.5f, 1.5f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Scope Rail, Medium Magnification DMR Scope, and Steel Box Magazine
    MeshData BuildOpticAndMagazine()
    {
        MeshData OpticMesh;
        OpticMesh.MeshSegmentName = "OpticAndMagazine";

        // Receiver Bridge Scope Mount
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 4.5f), Vector3D(10.0f, 1.8f, 0.8f), OpticMesh);
        // DMR Tactical Optic Body
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 7.8f), Vector3D(12.0f, 2.0f, 2.0f), OpticMesh);
        // Objective Housing
        AddBoxPrimitive(Vector3D(9.0f, 0.0f, 7.8f), Vector3D(2.5f, 2.6f, 2.6f), OpticMesh);
        // Angled 20-Round 7.62x51mm Steel Magazine
        AddBoxPrimitive(Vector3D(2.0f, 0.0f, -6.0f), Vector3D(5.0f, 2.2f, 5.0f), OpticMesh);

        return OpticMesh;
    }
};

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the Outlaw Sniper Rifle
class OutlawMeshGenerator
{
public:
    OutlawMeshGenerator() = default;

    // Generates complete mesh geometry for all Outlaw components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildRevolverCylinderAndFrame());
        FullWeaponMesh.push_back(BuildLeverMechanismAndGrip());
        FullWeaponMesh.push_back(BuildSkeletonizedStock());
        FullWeaponMesh.push_back(BuildTaperedBarrelAndMuzzle());
        FullWeaponMesh.push_back(BuildOpticsAndRailSystem());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs the Main Receiver and the Iconic Revolver Cylinder Assembly
    MeshData BuildRevolverCylinderAndFrame()
    {
        MeshData ReceiverMesh;
        ReceiverMesh.MeshSegmentName = "RevolverCylinderAndFrame";

        // Outer Receiver Frame Casing
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(20.0f, 3.2f, 4.8f), ReceiverMesh);
        // Central Revolver Cylinder Block (Hexagonal Profile Housing)
        AddBoxPrimitive(Vector3D(1.0f, 0.0f, -0.2f), Vector3D(6.5f, 3.8f, 3.8f), ReceiverMesh);
        // Fluted Side Cylinder Channels
        AddBoxPrimitive(Vector3D(1.0f, -2.0f, -0.2f), Vector3D(6.0f, 0.6f, 1.2f), ReceiverMesh);
        AddBoxPrimitive(Vector3D(1.0f, 2.0f, -0.2f), Vector3D(6.0f, 0.6f, 1.2f), ReceiverMesh);

        return ReceiverMesh;
    }

    // Constructs the Under-Slung Cocking Loop and Pistol Grip
    MeshData BuildLeverMechanismAndGrip()
    {
        MeshData LeverMesh;
        LeverMesh.MeshSegmentName = "LeverMechanismAndGrip";

        // Ergonomic Tactical Pistol Grip
        AddBoxPrimitive(Vector3D(-8.0f, 0.0f, -5.5f), Vector3D(2.8f, 1.8f, 4.8f), LeverMesh);
        // Under-slung Revolver/Lever Cycling Guard Loop
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, -5.0f), Vector3D(5.0f, 1.2f, 1.8f), LeverMesh);
        // Rear Cocking Spur/Lever Extension
        AddBoxPrimitive(Vector3D(-6.0f, 0.0f, -1.8f), Vector3D(2.0f, 1.0f, 1.2f), LeverMesh);

        return LeverMesh;
    }

    // Constructs the Lightweight Skeletonized Stock
    MeshData BuildSkeletonizedStock()
    {
        MeshData StockMesh;
        StockMesh.MeshSegmentName = "SkeletonizedStock";

        // Upper Metal Receiver Connector
        AddBoxPrimitive(Vector3D(-20.0f, 0.0f, 1.0f), Vector3D(10.0f, 2.0f, 1.2f), StockMesh);
        // Lower Angled Frame Support
        AddBoxPrimitive(Vector3D(-22.0f, 0.0f, -3.0f), Vector3D(11.0f, 1.8f, 1.0f), StockMesh);
        // Buttplate and Cheek Comb Support
        AddBoxPrimitive(Vector3D(-33.0f, 0.0f, -1.0f), Vector3D(1.5f, 2.5f, 5.2f), StockMesh);
        AddBoxPrimitive(Vector3D(-25.0f, 0.0f, 2.8f), Vector3D(5.0f, 2.2f, 0.8f), StockMesh);

        return StockMesh;
    }

    // Constructs the Fast-ADS Light Tapered Barrel and Flash Suppressor
    MeshData BuildTaperedBarrelAndMuzzle()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "TaperedBarrel";

        // Forward Shroud Base
        AddBoxPrimitive(Vector3D(22.0f, 0.0f, 0.5f), Vector3D(8.0f, 2.2f, 2.2f), BarrelMesh);
        // Slim Lightweight Precision Barrel
        AddBoxPrimitive(Vector3D(52.0f, 0.0f, 0.5f), Vector3D(22.0f, 1.2f, 1.2f), BarrelMesh);
        // Compact Slotted Cylinder Flash Hider
        AddBoxPrimitive(Vector3D(76.0f, 0.0f, 0.5f), Vector3D(4.0f, 1.8f, 1.8f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Top Mount Bridge and Compact Fast-ADS Scope
    MeshData BuildOpticsAndRailSystem()
    {
        MeshData OpticMesh;
        OpticMesh.MeshSegmentName = "OpticsAndRail";

        // Top Mounting Picatinny Rail
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 5.2f), Vector3D(16.0f, 2.0f, 0.8f), OpticMesh);
        // Outlaw Compact Scope Body
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 8.2f), Vector3D(11.0f, 2.0f, 2.0f), OpticMesh);
        // Front Flared Lens Objective
        AddBoxPrimitive(Vector3D(8.0f, 0.0f, 8.2f), Vector3D(2.5f, 2.6f, 2.6f), OpticMesh);

        return OpticMesh;
    }
};

#include <vector>
#include <string>

// Basic 3D Vector Structure
struct Vector3D
{
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;

    Vector3D() = default;
    Vector3D(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

    Vector3D operator+(const Vector3D& Other) const
    {
        return Vector3D(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
};

// Simple Mesh Data Container
struct MeshData
{
    std::vector<Vector3D> Vertices;
    std::vector<unsigned int> Indices;
    std::string MeshSegmentName;
};

// Procedural Generator for the NA-45 Explosive Sniper Rifle
class NA45MeshGenerator
{
public:
    NA45MeshGenerator() = default;

    // Generates complete mesh geometry for all NA-45 components
    std::vector<MeshData> GenerateCompleteModel()
    {
        std::vector<MeshData> FullWeaponMesh;

        FullWeaponMesh.push_back(BuildTwinChamberReceiver());
        FullWeaponMesh.push_back(BuildFuturisticChassisAndGrip());
        FullWeaponMesh.push_back(BuildDualBarrelAndMuzzle());
        FullWeaponMesh.push_back(BuildElectronicTrackingScope());

        return FullWeaponMesh;
    }

private:
    // Helper to add box geometry relative to a central position
    void AddBoxPrimitive(const Vector3D& Center, const Vector3D& Extents, MeshData& OutMesh)
    {
        unsigned int StartIndex = static_cast<unsigned int>(OutMesh.Vertices.size());

        // 8 Corner Vertices
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y, -Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X, -Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D( Extents.X,  Extents.Y,  Extents.Z));
        OutMesh.Vertices.push_back(Center + Vector3D(-Extents.X,  Extents.Y,  Extents.Z));

        // 12 Triangles (6 Box Faces)
        const unsigned int BoxIndices[] = {
            0, 2, 1, 0, 3, 2, // Bottom Face
            4, 5, 6, 4, 6, 7, // Top Face
            0, 1, 5, 0, 5, 4, // Front Face
            2, 3, 7, 2, 7, 6, // Back Face
            0, 4, 7, 0, 7, 3, // Left Face
            1, 2, 6, 1, 6, 5  // Right Face
        };

        for (unsigned int Index : BoxIndices)
        {
            OutMesh.Indices.push_back(StartIndex + Index);
        }
    }

    // Constructs the Heavy Dual-Chamber Receiver Core
    MeshData BuildTwinChamberReceiver()
    {
        MeshData ReceiverMesh;
        ReceiverMesh.MeshSegmentName = "TwinChamberReceiver";

        // Main Heavy Upper Receiver Body
        AddBoxPrimitive(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(22.0f, 3.4f, 5.0f), ReceiverMesh);
        // Side Ignition Coil Channels (Left & Right)
        AddBoxPrimitive(Vector3D(2.0f, -2.2f, 0.5f), Vector3D(10.0f, 0.8f, 2.0f), ReceiverMesh);
        AddBoxPrimitive(Vector3D(2.0f, 2.2f, 0.5f), Vector3D(10.0f, 0.8f, 2.0f), ReceiverMesh);
        // Short 2-Round Magazine Well
        AddBoxPrimitive(Vector3D(-2.0f, 0.0f, -5.5f), Vector3D(5.0f, 2.5f, 4.0f), ReceiverMesh);

        return ReceiverMesh;
    }

    // Constructs lower grip frame and futuristic stock assembly
    MeshData BuildFuturisticChassisAndGrip()
    {
        MeshData ChassisMesh;
        ChassisMesh.MeshSegmentName = "ChassisAndStock";

        // Vertical Ergonomic Grip
        AddBoxPrimitive(Vector3D(-8.0f, 0.0f, -6.0f), Vector3D(3.0f, 1.8f, 5.0f), ChassisMesh);
        // Thick Polymer Heavy Stock Frame
        AddBoxPrimitive(Vector3D(-28.0f, 0.0f, -0.5f), Vector3D(12.0f, 2.8f, 5.5f), ChassisMesh);
        // Integrated Counterweight Recoil Pad
        AddBoxPrimitive(Vector3D(-41.0f, 0.0f, -0.5f), Vector3D(1.8f, 3.2f, 6.0f), ChassisMesh);

        return ChassisMesh;
    }

    // Constructs Twin-Barrel Shroud and High-Impulse Blast Compensator
    MeshData BuildDualBarrelAndMuzzle()
    {
        MeshData BarrelMesh;
        BarrelMesh.MeshSegmentName = "DualBarrelAndMuzzle";

        // Angular Outer Barrel Housing / Heat Shroud
        AddBoxPrimitive(Vector3D(28.0f, 0.0f, 0.5f), Vector3D(12.0f, 2.8f, 3.2f), BarrelMesh);
        // Forward Heavy Barrel Extension
        AddBoxPrimitive(Vector3D(52.0f, 0.0f, 0.5f), Vector3D(18.0f, 2.0f, 2.0f), BarrelMesh);
        // Thick Cylindrical Dual-Port Blast Compensator
        AddBoxPrimitive(Vector3D(74.0f, 0.0f, 0.5f), Vector3D(5.0f, 3.0f, 3.0f), BarrelMesh);

        return BarrelMesh;
    }

    // Constructs Electronic Tracking Module and High-Power Scope Assembly
    MeshData BuildElectronicTrackingScope()
    {
        MeshData ScopeMesh;
        ScopeMesh.MeshSegmentName = "ElectronicScope";

        // Top Continuous Mounting Rail
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 5.8f), Vector3D(18.0f, 2.0f, 1.0f), ScopeMesh);
        // NA-45 Electronic Target Tracking Module (Base under optic)
        AddBoxPrimitive(Vector3D(2.0f, 0.0f, 7.5f), Vector3D(8.0f, 2.8f, 1.2f), ScopeMesh);
        // Tactical Main Scope Body
        AddBoxPrimitive(Vector3D(-1.0f, 0.0f, 10.2f), Vector3D(12.0f, 2.2f, 2.2f), ScopeMesh);
        // Front Objective Housing
        AddBoxPrimitive(Vector3D(9.0f, 0.0f, 10.2f), Vector3D(2.8f, 2.8f, 2.8f), ScopeMesh);

        return ScopeMesh;
    }
};

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <memory>

// Structural Types
struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vector3 operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vector3 operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
    
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
    }
};

struct RaycastHit {
    bool hit = false;
    Vector3 point;
    Vector3 normal;
    float distance = 0.0f;
    std::string objectName = "Environment";
};

enum class AmmoPayload {
    Standard_50BMG,
    Explosive_25mm,
    Thermite_25mm
};

// Rytec AMR Class Definition
class RytecAMR {
public:
    RytecAMR();
    ~RytecAMR() = default;

    // Core Actions
    bool PullTrigger(const Vector3& cameraPosition, const Vector3& cameraForward);
    void Reload();
    void UpdateRecoil(float deltaTime);
    
    // Setters & Getters
    void SetPayload(AmmoPayload payload);
    [[nodiscard]] int GetCurrentAmmo() const { return m_currentAmmo; }
    [[nodiscard]] Vector3 GetRecoilOffset() const { return m_currentRecoil; }

private:
    void ProcessImpact(const RaycastHit& hit);
    RaycastHit SimulateRaycast(const Vector3& origin, const Vector3& direction, float maxDistance);
    float GetRandomRange(float min, float max);

    // Weapon Characteristics
    AmmoPayload m_payload = AmmoPayload::Standard_50BMG;
    int m_magCapacity = 5;
    int m_currentAmmo = 5;
    float m_fireRateRPM = 165.0f;      // CoD MW baseline RPM
    float m_baseDamage = 175.0f;       // High base anti-materiel damage
    float m_splashRadius = 3.5f;       // 25mm splash zone

    // Recoil System Parameters
    Vector3 m_recoilKick{-12.5f, 2.0f, -0.3f}; // Pitch, Yaw jitter, Z-push
    float m_recoilSnappiness = 18.0f;
    float m_recoilReturnSpeed = 7.5f;
    Vector3 m_currentRecoil{0.0f, 0.0f, 0.0f};
    Vector3 m_targetRecoil{0.0f, 0.0f, 0.0f};

    // Timing Control
    std::chrono::steady_clock::time_point m_lastFireTime;
    float m_timeBetweenShots;
};

#include "RytecAMR.hpp"

RytecAMR::RytecAMR() {
    m_timeBetweenShots = 60.0f / m_fireRateRPM;
    m_lastFireTime = std::chrono::steady_clock::now() - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<float>(m_timeBetweenShots)
    );
}

void RytecAMR::SetPayload(AmmoPayload payload) {
    m_payload = payload;
    std::cout << "[Rytec AMR] Payload changed to: ";
    switch (m_payload) {
        case AmmoPayload::Standard_50BMG: std::cout << ".50 BMG Anti-Materiel\n"; break;
        case AmmoPayload::Explosive_25mm: std::cout << "25x59mm Explosive Rounds\n"; break;
        case AmmoPayload::Thermite_25mm:  std::cout << "25x59mm Thermite Incendiary\n"; break;
    }
}

bool RytecAMR::PullTrigger(const Vector3& cameraPosition, const Vector3& cameraForward) {
    auto now = std::chrono::steady_clock::now();
    float elapsedSeconds = std::chrono::duration<float>(now - m_lastFireTime).count();

    // Semi-Auto Throttle & Ammo Check
    if (elapsedSeconds < m_timeBetweenShots) {
        return false; // Trigger blocked by fire-rate governor
    }

    if (m_currentAmmo <= 0) {
        std::cout << "[Rytec AMR] *CLICK* Magazine Empty! Reload required.\n";
        return false;
    }

    // Execute Shot
    m_currentAmmo--;
    m_lastFireTime = now;

    std::cout << "\n>>> BANG! Rytec AMR Fired | Rounds Left: " << m_currentAmmo << "/" << m_magCapacity << " <<<\n";

    // Calculate Dynamic Procedural Recoil Kick
    float yawJitter = GetRandomRange(-m_recoilKick.y, m_recoilKick.y);
    m_targetRecoil = m_targetRecoil + Vector3{m_recoilKick.x, yawJitter, m_recoilKick.z};

    // Process Projectile Physics
    RaycastHit hit = SimulateRaycast(cameraPosition, cameraForward, 1000.0f);
    if (hit.hit) {
        ProcessImpact(hit);
    }

    return true;
}

void RytecAMR::ProcessImpact(const RaycastHit& hit) {
    std::cout << "[Impact Point] Hit: " << hit.objectName 
              << " at Distance: " << hit.distance << "m\n";

    switch (m_payload) {
        case AmmoPayload::Standard_50BMG:
            std::cout << "  -> High-Velocity Armor Penetration: " << m_baseDamage << " Direct Kinetic Damage.\n";
            break;

        case AmmoPayload::Explosive_25mm:
            std::cout << "  -> [BOOM] 25mm Detonation within " << m_splashRadius << "m Radius!\n";
            std::cout << "  -> Direct Payload Damage: " << (m_baseDamage * 0.7f) 
                      << " | Splash Damage: " << (m_baseDamage * 0.5f) << "\n";
            break;

        case AmmoPayload::Thermite_25mm:
            std::cout << "  -> [BURN] Thermite Core Attached to " << hit.objectName << ".\n";
            std::cout << "  -> Applying initial 25 Incendiary damage + 120 damage over 4.5s.\n";
            break;
    }
}

void RytecAMR::UpdateRecoil(float deltaTime) {
    // Dual-stage Recoil Interpolation (Snappiness -> Recovery)
    m_targetRecoil = Vector3::Lerp(m_targetRecoil, Vector3{0.0f, 0.0f, 0.0f}, m_recoilReturnSpeed * deltaTime);
    m_currentRecoil = Vector3::Lerp(m_currentRecoil, m_targetRecoil, m_recoilSnappiness * deltaTime);
}

void RytecAMR::Reload() {
    std::cout << "\n[Rytec AMR] Swapping 5-Round Box Magazine...\n";
    m_currentAmmo = m_magCapacity;
    std::cout << "[Rytec AMR] Reload Complete. Fully Loaded.\n";
}

RaycastHit RytecAMR::SimulateRaycast(const Vector3& origin, const Vector3& direction, float maxDistance) {
    // Simplified environmental raycast simulation
    RaycastHit hit;
    hit.hit = true;
    hit.distance = GetRandomRange(50.0f, 300.0f);
    hit.point = origin + (direction * hit.distance);
    hit.normal = Vector3{0.0f, 1.0f, 0.0f};
    hit.objectName = "Enemy Heavy Vehicle / Wall";
    return hit;
}

float RytecAMR::GetRandomRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

#include "RytecAMR.hpp"
#include <thread>

int main() {
    RytecAMR sniper;
    Vector3 cameraPos{0.0f, 1.7f, 0.0f};
    Vector3 cameraForward{0.0f, 0.0f, 1.0f};

    // 1. Fire Standard .50 BMG Rounds
    sniper.SetPayload(AmmoPayload::Standard_50BMG);
    sniper.PullTrigger(cameraPos, cameraForward);
    
    // Simulate game loop frames & recoil recovery
    for (int i = 0; i < 10; ++i) {
        sniper.UpdateRecoil(0.016f); // 60 FPS Delta
    }

    // 2. Switch Payload to 25mm Explosive
    sniper.SetPayload(AmmoPayload::Explosive_25mm);
    
    // Rapid Fire Test (Semi-auto check)
    sniper.PullTrigger(cameraPos, cameraForward);
    
    // Try firing again immediately (Should be blocked by rate of fire governor)
    if (!sniper.PullTrigger(cameraPos, cameraForward)) {
        std::cout << "[System Check] Semi-Auto Fire Governor active (Trigger pull rejected).\n";
    }

    // Wait for bolt cycle time
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    sniper.PullTrigger(cameraPos, cameraForward);

    // 3. Switch Payload to 25mm Thermite and Reload
    sniper.SetPayload(AmmoPayload::Thermite_25mm);
    sniper.Reload();
    sniper.PullTrigger(cameraPos, cameraForward);

    return 0;
}

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>
#include <iomanip>

// Individual LMG Data Structure
struct LMGStats {
    std::string name;
    int damage;
    int fireRateRPM;
    int magCapacity;
    float mobilityPenalty; // Percentage movement speed reduction
    std::string defaultAmmoType;
};

class LMGManager {
public:
    LMGManager();
    ~LMGManager() = default;

    // Inventory & Selection
    bool EquipLMG(const std::string& lmgName);
    void DisplayAllLMGs() const;
    void DisplayEquippedStats() const;

    // Active Weapon Actions
    bool FireActiveLMG(int roundsFired);
    void ReloadActiveLMG();

    // Query Methods
    [[nodiscard]] const LMGStats* GetLMG(const std::string& lmgName) const;
    [[nodiscard]] std::string GetActiveLMGName() const { return m_activeLMG ? m_activeLMG->name : "None"; }

private:
    void RegisterLMG(const LMGStats& stats);
    std::string ToLower(std::string str) const;

    std::unordered_map<std::string, LMGStats> m_lmgRegistry;
    const LMGStats* m_activeLMG = nullptr;
    int m_currentAmmo = 0;
};

#include "LMGManager.hpp"

LMGManager::LMGManager() {
    // Populate the manager with all 11 Light Machine Guns
    RegisterLMG({"MG42",       22, 1200, 100, 0.15f, "8x57mm IS"});
    RegisterLMG({"Holger 26",  31,  750,  100, 0.08f, "5.56x45mm NATO"});
    RegisterLMG({"RPD",        38,  660,  100, 0.12f, "7.62x39mm"});
    RegisterLMG({"Chopper",    30,  800,  100, 0.14f, "7.62x51mm NATO"});
    RegisterLMG({"UL736",      35,  620,   60, 0.07f, "5.56x45mm NATO"});
    RegisterLMG({"S36",        40,  750,   75, 0.13f, "7.62x51mm NATO"});
    RegisterLMG({"M4LMG",      32,  630,   75, 0.09f, "5.56x45mm NATO"});
    RegisterLMG({"Hades",      29,  840,   84, 0.10f, "5.56x45mm NATO"});
    RegisterLMG({"PKM",        40,  750,  100, 0.15f, "7.62x54mmR"});
    RegisterLMG({"Dingo",      28,  850,   80, 0.08f, "5.56x45mm NATO"});
    RegisterLMG({"Bruen MK9",  34,  750,  100, 0.12f, "5.56x45mm NATO"});
}

void LMGManager::RegisterLMG(const LMGStats& stats) {
    std::string key = ToLower(stats.name);
    m_lmgRegistry[key] = stats;
}

std::string LMGManager::ToLower(std::string str) const {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

bool LMGManager::EquipLMG(const std::string& lmgName) {
    std::string key = ToLower(lmgName);
    auto it = m_lmgRegistry.find(key);

    if (it != m_lmgRegistry.end()) {
        m_activeLMG = &(it->second);
        m_currentAmmo = m_activeLMG->magCapacity;
        std::cout << "\n[LMG Manager] Successfully equipped: " << m_activeLMG->name << "\n";
        return true;
    }

    std::cout << "\n[LMG Manager] Error: LMG '" << lmgName << "' not found in inventory!\n";
    return false;
}

void LMGManager::DisplayAllLMGs() const {
    std::cout << "\n========================================================================================\n";
    std::cout << std::left << std::setw(15) << "LMG Name"
              << std::setw(10) << "Damage"
              << std::setw(12) << "Fire Rate"
              << std::setw(12) << "Magazine"
              << std::setw(15) << "Mobility Penalty"
              << std::setw(18) << "Caliber" << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    for (const auto& [key, lmg] : m_lmgRegistry) {
        std::cout << std::left << std::setw(15) << lmg.name
                  << std::setw(10) << lmg.damage
                  << std::setw(12) << (std::to_string(lmg.fireRateRPM) + " RPM")
                  << std::setw(12) << (std::to_string(lmg.magCapacity) + " rnds")
                  << std::setw(15) << (std::to_string(static_cast<int>(lmg.mobilityPenalty * 100)) + "%")
                  << std::setw(18) << lmg.defaultAmmoType << "\n";
    }
    std::cout << "========================================================================================\n";
}

void LMGManager::DisplayEquippedStats() const {
    if (!m_activeLMG) {
        std::cout << "\n[LMG Manager] No LMG currently equipped.\n";
        return;
    }

    std::cout << "\n=== EQUIPPED WEAPON STATUS ===\n";
    std::cout << "Weapon: " << m_activeLMG->name << "\n";
    std::cout << "Ammo: " << m_currentAmmo << " / " << m_activeLMG->magCapacity << " rounds\n";
    std::cout << "Damage per hit: " << m_activeLMG->damage << "\n";
    std::cout << "Rate of Fire: " << m_activeLMG->fireRateRPM << " RPM\n";
    std::cout << "Caliber: " << m_activeLMG->defaultAmmoType << "\n";
}

bool LMGManager::FireActiveLMG(int roundsFired) {
    if (!m_activeLMG) {
        std::cout << "\n[LMG Manager] Cannot fire: No LMG equipped!\n";
        return false;
    }

    if (m_currentAmmo <= 0) {
        std::cout << "\n[LMG Manager] *CLICK* Out of ammo! Reload " << m_activeLMG->name << ".\n";
        return false;
    }

    int actualFired = std::min(roundsFired, m_currentAmmo);
    m_currentAmmo -= actualFired;
    int totalDamage = actualFired * m_activeLMG->damage;

    std::cout << "\n[FIRING] " << m_activeLMG->name << " fired " << actualFired << " rounds ("
              << m_activeLMG->defaultAmmoType << ") | Deal Damage: " << totalDamage 
              << " | Remaining Ammo: " << m_currentAmmo << "/" << m_activeLMG->magCapacity << "\n";

    return true;
}

void LMGManager::ReloadActiveLMG() {
    if (!m_activeLMG) return;

    std::cout << "\n[RELOADING] Swapping magazine for " << m_activeLMG->name << "...\n";
    m_currentAmmo = m_activeLMG->magCapacity;
    std::cout << "[RELOAD COMPLETE] Ammo restored to " << m_currentAmmo << " rounds.\n";
}

const LMGStats* LMGManager::GetLMG(const std::string& lmgName) const {
    std::string key = ToLower(lmgName);
    auto it = m_lmgRegistry.find(key);
    return (it != m_lmgRegistry.end()) ? &(it->second) : nullptr;
}

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <vector>

// Abstract Base Class for All Firearms
class BaseWeapon {
public:
    BaseWeapon(std::string name, int damage, int rpm, int magSize, float reloadTime, std::string ammo)
        : m_name(std::move(name)), m_damage(damage), m_fireRateRPM(rpm),
          m_magCapacity(magSize), m_currentAmmo(magSize), m_reloadTime(reloadTime),
          m_ammoType(std::move(ammo)) {}

    virtual ~BaseWeapon() = default;

    virtual void PrimaryFire() {
        if (m_currentAmmo <= 0) {
            std::cout << "[" << m_name << "] *CLICK* Out of ammo! Reload required.\n";
            return;
        }
        m_currentAmmo--;
        std::cout << "[" << m_name << "] FIRED 1 round (" << m_ammoType 
                  << ") | Damage: " << m_damage 
                  << " | Ammo: " << m_currentAmmo << "/" << m_magCapacity << "\n";
    }

    virtual void Reload() {
        std::cout << "[" << m_name << "] Reloading... (" << m_reloadTime << "s)\n";
        m_currentAmmo = m_magCapacity;
        std::cout << "[" << m_name << "] Fully Loaded: " << m_currentAmmo << "/" << m_magCapacity << "\n";
    }

    // Getters
    std::string GetName() const { return m_name; }
    int GetDamage() const { return m_damage; }
    int GetRPM() const { return m_fireRateRPM; }
    int GetCurrentAmmo() const { return m_currentAmmo; }

protected:
    std::string m_name;
    int m_damage;
    int m_fireRateRPM;
    int m_magCapacity;
    int m_currentAmmo;
    float m_reloadTime;
    std::string m_ammoType;
};

// ==========================================
// 12 INDIVIDUAL WEAPON CLASS IMPLEMENTATIONS
// ==========================================

class Weapon_MG42 : public BaseWeapon {
public:
    Weapon_MG42() : BaseWeapon("MG42", 22, 1200, 100, 6.2f, "8x57mm IS") {}
    void PrimaryFire() override {
        // High rate-of-fire burst simulation (3 rounds per trigger pull)
        if (m_currentAmmo >= 3) {
            m_currentAmmo -= 3;
            std::cout << "[MG42] *BRRRRT* 3-Round High-RPM Burst! Damage: " << (m_damage * 3) 
                      << " | Ammo: " << m_currentAmmo << "/" << m_magCapacity << "\n";
        } else {
            BaseWeapon::PrimaryFire();
        }
    }
};

class Weapon_Holger26 : public BaseWeapon {
public:
    Weapon_Holger26() : BaseWeapon("Holger 26", 31, 750, 100, 3.8f, "5.56x45mm NATO") {}
};

class Weapon_RPD : public BaseWeapon {
public:
    Weapon_RPD() : BaseWeapon("RPD", 38, 660, 100, 5.0f, "7.62x39mm") {}
};

class Weapon_Chopper : public BaseWeapon {
public:
    Weapon_Chopper() : BaseWeapon("Chopper", 30, 800, 100, 5.5f, "7.62x51mm NATO") {}
};

class Weapon_UL736 : public BaseWeapon {
public:
    Weapon_UL736() : BaseWeapon("UL736", 35, 620, 60, 3.2f, "5.56x45mm NATO") {}
};

class Weapon_S36 : public BaseWeapon {
public:
    Weapon_S36() : BaseWeapon("S36", 40, 750, 75, 4.5f, "7.62x51mm NATO") {}
};

class Weapon_M4LMG : public BaseWeapon {
public:
    Weapon_M4LMG() : BaseWeapon("M4LMG", 32, 630, 75, 3.5f, "5.56x45mm NATO") {}
};

class Weapon_Hades : public BaseWeapon {
public:
    Weapon_Hades() : BaseWeapon("Hades", 29, 840, 84, 4.0f, "5.56x45mm NATO") {}
};

class Weapon_PKM : public BaseWeapon {
public:
    Weapon_PKM() : BaseWeapon("PKM", 40, 750, 100, 7.0f, "7.62x54mmR") {}
};

class Weapon_Dingo : public BaseWeapon {
public:
    Weapon_Dingo() : BaseWeapon("Dingo", 28, 850, 80, 3.6f, "5.56x45mm NATO") {}
};

class Weapon_BruenMK9 : public BaseWeapon {
public:
    Weapon_BruenMK9() : BaseWeapon("Bruen MK9", 34, 750, 100, 4.8f, "5.56x45mm NATO") {}
};

class Weapon_RytecAMR : public BaseWeapon {
public:
    enum class AmmoPayload { Standard50BMG, Explosive25mm, Thermite25mm };
    
    Weapon_RytecAMR() : BaseWeapon("Rytec AMR", 175, 165, 5, 3.9f, ".50 BMG Anti-Materiel") {}

    void SetPayload(AmmoPayload payload) {
        m_payload = payload;
        switch (m_payload) {
            case AmmoPayload::Standard50BMG: m_ammoType = ".50 BMG"; m_damage = 175; break;
            case AmmoPayload::Explosive25mm: m_ammoType = "25mm Explosive Payload"; m_damage = 140; break;
            case AmmoPayload::Thermite25mm:  m_ammoType = "25mm Thermite Incendiary"; m_damage = 120; break;
        }
    }

    void PrimaryFire() override {
        if (m_currentAmmo <= 0) {
            std::cout << "[Rytec AMR] *CLICK* Empty Magazine.\n";
            return;
        }
        m_currentAmmo--;
        std::cout << "[Rytec AMR] BOOM! Detonated " << m_ammoType 
                  << " | Base Damage: " << m_damage 
                  << " | Rounds Left: " << m_currentAmmo << "/" << m_magCapacity << "\n";
    }

private:
    AmmoPayload m_payload = AmmoPayload::Standard50BMG;
};

#pragma once
#include "WeaponSystem.hpp"

class WeaponFactoryManager {
public:
    WeaponFactoryManager() {
        // Register each unique weapon object into memory
        RegisterWeapon(std::make_unique<Weapon_MG42>());
        RegisterWeapon(std::make_unique<Weapon_Holger26>());
        RegisterWeapon(std::make_unique<Weapon_RPD>());
        RegisterWeapon(std::make_unique<Weapon_Chopper>());
        RegisterWeapon(std::make_unique<Weapon_UL736>());
        RegisterWeapon(std::make_unique<Weapon_S36>());
        RegisterWeapon(std::make_unique<Weapon_M4LMG>());
        RegisterWeapon(std::make_unique<Weapon_Hades>());
        RegisterWeapon(std::make_unique<Weapon_PKM>());
        RegisterWeapon(std::make_unique<Weapon_Dingo>());
        RegisterWeapon(std::make_unique<Weapon_BruenMK9>());
        RegisterWeapon(std::make_unique<Weapon_RytecAMR>());
    }

    bool Equip(const std::string& name) {
        std::string key = ToLower(name);
        if (m_armory.find(key) != m_armory.end()) {
            m_equipped = m_armory[key].get();
            std::cout << "\n[Armory] Equipped: " << m_equipped->GetName() << "\n";
            return true;
        }
        std::cout << "\n[Armory] Weapon '" << name << "' not found!\n";
        return false;
    }

    void FireEquipped() {
        if (m_equipped) {
            m_equipped->PrimaryFire();
        } else {
            std::cout << "[Armory] No weapon currently equipped!\n";
        }
    }

    void ReloadEquipped() {
        if (m_equipped) m_equipped->Reload();
    }

    BaseWeapon* GetEquipped() const { return m_equipped; }

private:
    void RegisterWeapon(std::unique_ptr<BaseWeapon> weapon) {
        std::string key = ToLower(weapon->GetName());
        m_armory[key] = std::move(weapon);
    }

    std::string ToLower(std::string str) const {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    std::unordered_map<std::string, std::unique_ptr<BaseWeapon>> m_armory;
    BaseWeapon* m_equipped = nullptr;
};

#include "WeaponFactoryManager.hpp"

struct AutomatedWeaponSystemRunner {
    AutomatedWeaponSystemRunner() {
        WeaponFactoryManager armory;

        std::cout << "=== RUNNING ALL DESIGNED WEAPONS ===\n";

        // 1. Test MG42 (Custom burst override)
        armory.Equip("MG42");
        armory.FireEquipped();

        // 2. Test PKM Heavy LMG
        armory.Equip("PKM");
        armory.FireEquipped();

        // 3. Test Rytec AMR Sniper (With Payload Config)
        armory.Equip("Rytec AMR");
        auto* rytec = dynamic_cast<Weapon_RytecAMR*>(armory.GetEquipped());
        if (rytec) {
            rytec->SetPayload(Weapon_RytecAMR::AmmoPayload::Explosive25mm);
        }
        armory.FireEquipped();

        // 4. Test Chopper
        armory.Equip("Chopper");
        armory.FireEquipped();
        armory.ReloadEquipped();

        std::cout << "\n=== ALL WEAPONS SUCCESSFULLY INITIALIZED & TESTED ===\n";
        std::exit(0);
    }
};

// Static initialization fires execution when binary/dll loads
static AutomatedWeaponSystemRunner g_AutoRunInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Magazine };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_M4LMG {
public:
    Weapon_M4LMG()
        : m_baseName("M4LMG"),
          m_baseDamage(32),
          m_baseRPM(630),
          m_baseMagCapacity(75),
          m_currentAmmo(75),
          m_baseReloadTime(3.5f),
          m_baseAdsTimeMs(310.0f),
          m_ammoType("5.56x45mm NATO") {
        RecalculateEffectiveStats();
    }

    ~Weapon_M4LMG() = default;

    // Attachment Management
    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[M4LMG] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[M4LMG] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    // Firing Mechanics
    void FireBurst(int rounds) {
        if (m_currentAmmo <= 0) {
            std::cout << "[M4LMG] *CLICK* Magazine empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamageDealt = actualFired * m_effectiveDamage;

        std::cout << "[M4LMG FIRING] Fired " << actualFired << " rounds (" << m_ammoType 
                  << ") | Total Damage: " << totalDamageDealt 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[M4LMG] Swapping 5.56mm Drum Magazine... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[M4LMG] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot: " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Mag Capacity    : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Modular Drum)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    std::string m_ammoType;

    // Computed Stats
    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_M4LMG.hpp"

struct M4LMGTestModule {
    M4LMGTestModule() {
        std::cout << "=== INITIALIZING DEDICATED M4LMG WEAPON SYSTEM ===\n";

        Weapon_M4LMG m4lmg;

        // 1. Inspect factory stock M4LMG
        m4lmg.DisplayBlueprint();
        m4lmg.FireBurst(20);

        // 2. Build custom Gunsmith attachments
        auto RTCCompensator = std::make_shared<Attachment>(
            "RTC Compensator", AttachmentType::Muzzle, StatModifiers{0, 0, 0, 0.0f, -0.15f, 10.0f}
        );

        auto Large100RoundDrum = std::make_shared<Attachment>(
            "100-Round Large Drum Mag", AttachmentType::Magazine, StatModifiers{0, 0, 25, 0.6f, 0.05f, 30.0f}
        );

        auto RedDotSight = std::make_shared<Attachment>(
            "Red Dot Sight 1", AttachmentType::Optic, StatModifiers{0, 0, 0, 0.0f, -0.05f, 12.0f}
        );

        std::cout << "\n--- Customizing M4LMG in Gunsmith ---\n";
        m4lmg.EquipAttachment(RTCCompensator);
        m4lmg.EquipAttachment(Large100RoundDrum);
        m4lmg.EquipAttachment(RedDotSight);

        // 3. Display updated blueprint and test firing
        m4lmg.DisplayBlueprint();
        m4lmg.Reload();
        m4lmg.FireBurst(45);

        std::cout << "\n=== M4LMG TEST COMPLETE ===\n";
        std::exit(0);
    }
};

// Global instance triggers execution on load
static M4LMGTestModule g_M4LMGModuleInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Magazine, Underbarrel };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_S36 {
public:
    Weapon_S36()
        : m_baseName("S36"),
          m_baseDamage(40),
          m_baseRPM(750),
          m_baseMagCapacity(75),
          m_currentAmmo(75),
          m_baseReloadTime(4.5f),
          m_baseAdsTimeMs(370.0f),
          m_ammoType("7.62x51mm NATO") {
        RecalculateEffectiveStats();
    }

    ~Weapon_S36() = default;

    // Attachment System
    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[S36] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[S36] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    // Firing Mechanics
    void FireAutomatic(int rounds) {
        if (m_currentAmmo <= 0) {
            std::cout << "[S36] *CLICK* Out of ammo! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamage = actualFired * m_effectiveDamage;

        std::cout << "[S36 FIRING] Fired " << actualFired << " rounds (" << m_ammoType 
                  << ") | Total Damage: " << totalDamage 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[S36] Swapping heavy magazine... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[S36] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " Heavy LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot: " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Mag Capacity    : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Heavy Frame)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    std::string m_ammoType;

    // Modified Stats
    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_S36.hpp"

struct S36TestModule {
    S36TestModule() {
        std::cout << "=== INITIALIZING DEDICATED S36 WEAPON SYSTEM ===\n";

        Weapon_S36 s36;

        // 1. Inspect stock S36
        s36.DisplayBlueprint();
        s36.FireAutomatic(20);

        // 2. Build recoil-control Gunsmith setup
        auto operatorGrip = std::make_shared<Attachment>(
            "Operator Foregrip", AttachmentType::Underbarrel, StatModifiers{0, 0, 0, 0.0f, -0.15f, 20.0f}
        );

        auto tacticalMuzzle = std::make_shared<Attachment>(
            "Tactical Suppressor", AttachmentType::Muzzle, StatModifiers{0, 0, 0, 0.0f, -0.08f, 15.0f}
        );

        auto extendedMag = std::make_shared<Attachment>(
            "100-Round Extended Mag", AttachmentType::Magazine, StatModifiers{0, 0, 25, 0.5f, 0.05f, 30.0f}
        );

        std::cout << "\n--- Customizing S36 in Gunsmith ---\n";
        s36.EquipAttachment(operatorGrip);
        s36.EquipAttachment(tacticalMuzzle);
        s36.EquipAttachment(extendedMag);

        // 3. Inspect updated stats & perform sustained fire test
        s36.DisplayBlueprint();
        s36.Reload();
        s36.FireAutomatic(50);

        std::cout << "\n=== S36 TEST COMPLETE ===\n";
        std::exit(0);
    }
};

// Global instance executes automatically upon module load
static S36TestModule g_S36ModuleInstance;


#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Magazine, Underbarrel };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_Dingo {
public:
    Weapon_Dingo()
        : m_baseName("Dingo"),
          m_baseDamage(28),
          m_baseRPM(720),
          m_baseMagCapacity(80),
          m_currentAmmo(80),
          m_baseReloadTime(3.2f),
          m_baseAdsTimeMs(300.0f),
          m_ammoType("5.56x45mm NATO High-Velocity") {
        RecalculateEffectiveStats();
    }

    ~Weapon_Dingo() = default;

    // Attachment System
    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[Dingo] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[Dingo] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    // Rapid-Fire Automatic Mechanics
    void FireAutomatic(int rounds) {
        if (m_currentAmmo <= 0) {
            std::cout << "[Dingo] *CLICK* Magazine empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamage = actualFired * m_effectiveDamage;

        std::cout << "[DINGO FIRING] Fired " << actualFired << " rounds (" << m_ammoType 
                  << ") | Fire Rate: " << m_effectiveRPM << " RPM"
                  << " | Total Damage: " << totalDamage 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[Dingo] Swapping fast-access box magazine... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[Dingo] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " High-RPM LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot: " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Mag Capacity    : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Rapid Frame)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    std::string m_ammoType;

    // Modified Stats
    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_Dingo.hpp"

struct DingoTestModule {
    DingoTestModule() {
        std::cout << "=== INITIALIZING DEDICATED DINGO WEAPON SYSTEM ===\n";

        Weapon_Dingo dingo;

        // 1. Inspect factory stock Dingo
        dingo.DisplayBlueprint();
        dingo.FireAutomatic(25);

        // 2. Build high-mobility & extended capacity Gunsmith setup
        auto rapidFireBarrel = std::make_shared<Attachment>(
            "Rapid Fire Booster", AttachmentType::Barrel, StatModifiers{0, 60, 0, 0.0f, 0.05f, 10.0f}
        );

        auto extendedBoxMag = std::make_shared<Attachment>(
            "100-Round Extended Box", AttachmentType::Magazine, StatModifiers{0, 0, 20, 0.4f, 0.02f, 20.0f}
        );

        auto quickdrawStock = std::make_shared<Attachment>(
            "Lightweight Stock", AttachmentType::Stock, StatModifiers{0, 0, 0, 0.0f, 0.0f, -40.0f}
        );

        std::cout << "\n--- Customizing Dingo in Gunsmith ---\n";
        dingo.EquipAttachment(rapidFireBarrel);
        dingo.EquipAttachment(extendedBoxMag);
        dingo.EquipAttachment(quickdrawStock);

        // 3. Inspect updated blueprint & perform firing test
        dingo.DisplayBlueprint();
        dingo.Reload();
        dingo.FireAutomatic(60);

        std::cout << "\n=== DINGO TEST COMPLETE ===\n";
        std::exit(0);
    }
};

// Global instance executes automatically upon module load
static DingoTestModule g_DingoModuleInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Belt };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
    float coolingRateDelta = 0.0f;    // Positive = faster heat dissipation
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_MG42 {
public:
    Weapon_MG42()
        : m_baseName("MG42"),
          m_baseDamage(34),
          m_baseRPM(1200),
          m_baseMagCapacity(125),
          m_currentAmmo(125),
          m_baseReloadTime(5.8f),
          m_baseAdsTimeMs(420.0f),
          m_heatLevel(0.0f),
          m_maxHeatThreshold(100.0f),
          m_baseCoolingRate(15.0f),
          m_isOverheated(false),
          m_ammoType("7.92x57mm Mauser") {
        RecalculateEffectiveStats();
    }

    ~Weapon_MG42() = default;

    // Attachment System
    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[MG42] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[MG42] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveCoolingRate = m_baseCoolingRate;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveCoolingRate += mods.coolingRateDelta;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_effectiveCoolingRate = std::max(5.0f, m_effectiveCoolingRate);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    // High-RPM Firing Mechanics with Overheat Simulation
    void FireAutomatic(int rounds) {
        if (m_isOverheated) {
            std::cout << "[MG42 WARN] Barrel is OVERHEATED! Allow it to cool before firing.\n";
            return;
        }

        if (m_currentAmmo <= 0) {
            std::cout << "[MG42] *CLICK* Ammo belt empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamage = actualFired * m_effectiveDamage;

        // Thermal accumulation based on extreme fire rate
        m_heatLevel += actualFired * 1.8f;

        std::cout << "[MG42 FIRING] Discharged " << actualFired << " rounds (" << m_ammoType 
                  << ") at " << m_effectiveRPM << " RPM"
                  << " | Damage: " << totalDamage 
                  << " | Barrel Temp: " << m_heatLevel << "/" << m_maxHeatThreshold << " C"
                  << " | Ammo Left: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";

        if (m_heatLevel >= m_maxHeatThreshold) {
            m_isOverheated = true;
            std::cout << "[MG42 CRITICAL] BARREL OVERHEATED! Quick-change barrel required or cool down.\n";
        }
    }

    void CoolDown(float seconds) {
        float cooledAmount = seconds * m_effectiveCoolingRate;
        m_heatLevel = std::max(0.0f, m_heatLevel - cooledAmount);
        if (m_heatLevel < m_maxHeatThreshold * 0.5f && m_isOverheated) {
            m_isOverheated = false;
            std::cout << "[MG42] Barrel cooled down. Weapon operational.\n";
        }
        std::cout << "[MG42] Cooling for " << seconds << "s... Temperature now: " << m_heatLevel << " C\n";
    }

    void Reload() {
        std::cout << "[MG42] Swapping belt container and locking top cover... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[MG42] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " Extreme-RPM LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot: " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Belt Capacity   : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Cooling Rate    : " << m_effectiveCoolingRate << " units/s\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Heavy Shroud Frame)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    float m_heatLevel;
    float m_maxHeatThreshold;
    float m_baseCoolingRate;
    bool m_isOverheated;
    std::string m_ammoType;

    // Modified Stats
    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveCoolingRate;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_MG42.hpp"

struct MG42TestModule {
    MG42TestModule() {
        std::cout << "=== INITIALIZING DEDICATED MG42 WEAPON SYSTEM ===\n";

        Weapon_MG42 mg42;

        // 1. Inspect factory stock MG42
        mg42.DisplayBlueprint();
        
        // Rapid long burst to trigger barrel heat logic
        mg42.FireAutomatic(35);
        mg42.FireAutomatic(30);

        // Cool the barrel down
        mg42.CoolDown(3.0f);

        // 2. Build sustained-fire Gunsmith setup
        auto heavyFlutedBarrel = std::make_shared<Attachment>(
            "Heavy Fluted Quick-Change Barrel", AttachmentType::Barrel, StatModifiers{2, 0, 0, 0.0f, -0.10f, 25.0f, 10.0f}
        );

        auto extended250Belt = std::make_shared<Attachment>(
            "250-Round Belt Box", AttachmentType::Belt, StatModifiers{0, 0, 125, 1.5f, 0.12f, 60.0f, 0.0f}
        );

        auto RecoilBufferStock = std::make_shared<Attachment>(
            "Heavy Recoil Buffer Stock", AttachmentType::Stock, StatModifiers{0, 0, 0, 0.0f, -0.20f, 30.0f, 0.0f}
        );

        std::cout << "\n--- Customizing MG42 in Gunsmith ---\n";
        mg42.EquipAttachment(heavyFlutedBarrel);
        mg42.EquipAttachment(extended250Belt);
        mg42.EquipAttachment(RecoilBufferStock);

        // 3. Inspect updated blueprint & perform sustained test
        mg42.DisplayBlueprint();
        mg42.Reload();
        mg42.FireAutomatic(50);

        std::cout << "\n=== MG42 TEST COMPLETE ===\n";
        std::exit(0);
    }
};

// Global instance executes automatically upon module load
static MG42TestModule g_MG42ModuleInstance;

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

// Base weapon interface and attachment infrastructure
enum class AttachmentSlot { Optic, Muzzle, Barrel, Magazine, Stock };

struct StatMods {
    int damage = 0;
    int rpm = 0;
    int magSize = 0;
    float reloadTimeSec = 0.0f;
    float recoilPercent = 0.0f; // Negative reduces recoil
    float adsMs = 0.0f;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentSlot slot, StatMods mods)
        : m_name(std::move(name)), m_slot(slot), m_mods(mods) {}

    const std::string& GetName() const { return m_name; }
    AttachmentSlot GetSlot() const { return m_slot; }
    const StatMods& GetMods() const { return m_mods; }

private:
    std::string m_name;
    AttachmentSlot m_slot;
    StatMods m_mods;
};

// Abstract Base Weapon Class
class IWeapon {
public:
    virtual ~IWeapon() = default;

    virtual std::string GetName() const = 0;
    virtual void EquipAttachment(std::shared_ptr<Attachment> att) = 0;
    virtual void Fire(int rounds) = 0;
    virtual void Reload() = 0;
    virtual void PrintStats() const = 0;
};

// Generic Base LMG Class Implementing IWeapon
class BaseLMG : public IWeapon {
public:
    BaseLMG(std::string name, int damage, int rpm, int magCap, float reloadSec, float adsMs)
        : m_name(std::move(name)), m_baseDamage(damage), m_baseRPM(rpm),
          m_baseMagCap(magCap), m_currentAmmo(magCap), m_baseReloadSec(reloadSec),
          m_baseAdsMs(adsMs) {
        RecalculateStats();
    }

    std::string GetName() const override { return m_name; }

    void EquipAttachment(std::shared_ptr<Attachment> att) override {
        if (!att) return;
        m_attachments[att->GetSlot()] = att;
        RecalculateStats();
    }

    void Fire(int rounds) override {
        if (m_currentAmmo <= 0) {
            std::cout << "[" << m_name << "] *CLICK* Out of ammo!\n";
            return;
        }
        int fired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= fired;
        std::cout << "[" << m_name << " FIRING] Fired " << fired 
                  << " rounds | Total Damage: " << (fired * m_effDamage)
                  << " | Ammo Left: " << m_currentAmmo << "/" << m_effMagCap << "\n";
    }

    void Reload() override {
        std::cout << "[" << m_name << "] Reloading... (" << m_effReloadSec << "s)\n";
        m_currentAmmo = m_effMagCap;
        std::cout << "[" << m_name << "] Fully Loaded: " << m_currentAmmo << "/" << m_effMagCap << "\n";
    }

    void PrintStats() const override {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON: " << m_name << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage     : " << m_effDamage << "\n";
        std::cout << " Fire Rate  : " << m_effRPM << " RPM\n";
        std::cout << " Capacity   : " << m_effMagCap << " rounds\n";
        std::cout << " Reload Time: " << m_effReloadSec << "s\n";
        std::cout << " ADS Speed  : " << m_effAdsMs << "ms\n";
        std::cout << " Recoil     : " << (m_effRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";
        if (m_attachments.empty()) {
            std::cout << "   - None\n";
        } else {
            for (const auto& [slot, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

protected:
    void RecalculateStats() {
        m_effDamage = m_baseDamage;
        m_effRPM = m_baseRPM;
        m_effMagCap = m_baseMagCap;
        m_effReloadSec = m_baseReloadSec;
        m_effAdsMs = m_baseAdsMs;
        m_effRecoilMult = 1.0f;

        for (const auto& [slot, att] : m_attachments) {
            const auto& mods = att->GetMods();
            m_effDamage += mods.damage;
            m_effRPM += mods.rpm;
            m_effMagCap += mods.magSize;
            m_effReloadSec += mods.reloadTimeSec;
            m_effAdsMs += mods.adsMs;
            m_effRecoilMult += mods.recoilPercent;
        }

        m_effReloadSec = std::max(1.0f, m_effReloadSec);
        m_effAdsMs = std::max(50.0f, m_effAdsMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effMagCap);
    }

    std::string m_name;
    int m_baseDamage, m_baseRPM, m_baseMagCap, m_currentAmmo;
    float m_baseReloadSec, m_baseAdsMs;

    int m_effDamage, m_effRPM, m_effMagCap;
    float m_effReloadSec, m_effAdsMs;
    float m_effRecoilMult = 1.0f;

    std::unordered_map<AttachmentSlot, std::shared_ptr<Attachment>> m_attachments;
};

// Dedicated Derived Weapon Classes
class RPD : public BaseLMG {
public:
    RPD() : BaseLMG("RPD", 38, 660, 100, 5.2f, 380.0f) {}
};

class M4LMG : public BaseLMG {
public:
    M4LMG() : BaseLMG("M4LMG", 32, 630, 75, 3.5f, 310.0f) {}
};

class S36 : public BaseLMG {
public:
    S36() : BaseLMG("S36", 40, 750, 75, 4.5f, 370.0f) {}
};

class Dingo : public BaseLMG {
public:
    Dingo() : BaseLMG("Dingo", 28, 720, 80, 3.2f, 300.0f) {}
};

class MG42 : public BaseLMG {
public:
    MG42() : BaseLMG("MG42", 34, 1200, 125, 5.8f, 420.0f) {}
};

class BruenMk9 : public BaseLMG {
public:
    BruenMk9() : BaseLMG("Bruen Mk9", 36, 750, 100, 4.8f, 350.0f) {}
};

// Factory pattern to create weapons dynamically
class WeaponFactory {
public:
    static std::unique_ptr<IWeapon> CreateWeapon(const std::string& type) {
        if (type == "RPD") return std::make_unique<RPD>();
        if (type == "M4LMG") return std::make_unique<M4LMG>();
        if (type == "S36") return std::make_unique<S36>();
        if (type == "Dingo") return std::make_unique<Dingo>();
        if (type == "MG42") return std::make_unique<MG42>();
        if (type == "BruenMk9") return std::make_unique<BruenMk9>();
        return nullptr;
    }
};

// Global initializer struct executing tests at load time without main()
struct AutoExecutionEngine {
    AutoExecutionEngine() {
        std::cout << "=== UNIFIED LMG WEAPON FACTORY INITIALIZING ===\n";

        std::vector<std::string> weaponTypes = {"RPD", "M4LMG", "S36", "Dingo", "MG42", "BruenMk9"};
        std::vector<std::unique_ptr<IWeapon>> loadout;

        for (const auto& type : weaponTypes) {
            loadout.push_back(WeaponFactory::CreateWeapon(type));
        }

        // Shared attachment setup
        auto extendedMag = std::make_shared<Attachment>(
            "Extended Drum", AttachmentSlot::Magazine, StatMods{0, 0, 30, 0.5f, 0.05f, 25.0f}
        );

        auto redDot = std::make_shared<Attachment>(
            "Red Dot Sight", AttachmentSlot::Optic, StatMods{0, 0, 0, 0.0f, -0.05f, 10.0f}
        );

        std::cout << "\n--- Iterating and Executing All LMGs via Polymorphism ---\n";
        for (auto& weapon : loadout) {
            if (weapon) {
                weapon->EquipAttachment(extendedMag);
                weapon->EquipAttachment(redDot);
                weapon->PrintStats();
                weapon->Fire(15);
            }
        }

        std::cout << "\n=== ALL WEAPONS SUCCESSFULLY INSTANTIATED & TESTED ===\n";
        std::exit(0);
    }
};

static AutoExecutionEngine g_autoExecutionInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Ammunition, Underbarrel };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
    float penetrationMultDelta = 0.0f; // Armor-penetration modifier
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_PKM {
public:
    Weapon_PKM()
        : m_baseName("PKM"),
          m_baseDamage(42),
          m_baseRPM(600),
          m_baseMagCapacity(100),
          m_currentAmmo(100),
          m_baseReloadTime(5.5f),
          m_baseAdsTimeMs(400.0f),
          m_basePenetrationMult(1.25f),
          m_ammoType("7.62x54mmR Full-Power") {
        RecalculateEffectiveStats();
    }

    ~Weapon_PKM() = default;

    // Attachment Management
    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[PKM] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[PKM] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectivePenetrationMult = m_basePenetrationMult;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectivePenetrationMult += mods.penetrationMultDelta;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_effectivePenetrationMult = std::max(1.0f, m_effectivePenetrationMult);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    // Heavy Automatic Firing Mechanics
    void FireAutomatic(int rounds, bool targetIsArmored = false) {
        if (m_currentAmmo <= 0) {
            std::cout << "[PKM] *CLICK* Belt container empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;

        float perShotDamage = static_cast<float>(m_effectiveDamage);
        if (targetIsArmored) {
            perShotDamage *= m_effectivePenetrationMult;
        }

        int totalDamageDealt = static_cast<int>(actualFired * perShotDamage);

        std::cout << "[PKM FIRING] Fired " << actualFired << " rounds (" << m_ammoType 
                  << ") | Target: " << (targetIsArmored ? "Armored (Penetration Active)" : "Unarmored")
                  << " | Damage: " << totalDamageDealt 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[PKM] Opening receiver feed tray, seating 7.62x54mmR belt box... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[PKM] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " Heavy Belt-Fed LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Base Damage     : " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Mag Capacity    : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Armor Pen Mult  : " << m_effectivePenetrationMult << "x\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Heavy Receiver Frame)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    float m_basePenetrationMult;
    std::string m_ammoType;

    // Modified Stats
    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectivePenetrationMult;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_PKM.hpp"

struct PKMTestModule {
    PKMTestModule() {
        std::cout << "=== INITIALIZING DEDICATED PKM WEAPON SYSTEM ===\n";

        Weapon_PKM pkm;

        // 1. Inspect factory stock PKM
        pkm.DisplayBlueprint();
        pkm.FireAutomatic(20, false);

        // 2. Build heavy anti-armor & control Gunsmith setup
        auto heavy269Barrel = std::make_shared<Attachment>(
            "26.9\" Extended Heavy Barrel", AttachmentType::Barrel, StatModifiers{3, 0, 0, 0.0f, -0.12f, 35.0f, 0.20f}
        );

        auto snatchGrip = std::make_shared<Attachment>(
            "Snatch Grip", AttachmentType::Underbarrel, StatModifiers{0, 0, 0, 0.0f, -0.18f, -25.0f, 0.0f}
        );

        auto extended200Belt = std::make_shared<Attachment>(
            "200-Round Belt Box", AttachmentType::Ammunition, StatModifiers{0, 0, 100, 1.8f, 0.08f, 50.0f, 0.0f}
        );

        std::cout << "\n--- Customizing PKM in Gunsmith ---\n";
        pkm.EquipAttachment(heavy269Barrel);
        pkm.EquipAttachment(snatchGrip);
        pkm.EquipAttachment(extended200Belt);

        // 3. Display updated blueprint and perform sustained anti-armor test
        pkm.DisplayBlueprint();
        pkm.Reload();
        pkm.FireAutomatic(40, true);

        std::cout << "\n=== PKM TEST COMPLETE ===\n";
        std::exit(0);
    }
};

// Global instance triggers execution on module load
static PKMTestModule g_PKMModuleInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Magazine, Handle };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
    bool grantsCrossbarStance = false;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_Hades {
public:
    Weapon_Hades()
        : m_baseName("Hades"),
          m_baseDamage(30),
          m_baseRPM(880),
          m_baseMagCapacity(65),
          m_currentAmmo(65),
          m_baseReloadTime(3.8f),
          m_baseAdsTimeMs(320.0f),
          m_hasCrossbarStance(false),
          m_ammoType("5.56x45mm NATO High-Rate") {
        RecalculateEffectiveStats();
    }

    ~Weapon_Hades() = default;

    // Attachment System
    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[Hades] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[Hades] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveRecoilMult = 1.0f;
        m_hasCrossbarStance = false;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveRecoilMult += mods.recoilPercentDelta;

            if (mods.grantsCrossbarStance) {
                m_hasCrossbarStance = true;
            }
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(0.0f, m_effectiveAdsTimeMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    // Firing Mechanics
    void FireAutomatic(int rounds) {
        if (m_currentAmmo <= 0) {
            std::cout << "[Hades] *CLICK* Magazine empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamage = actualFired * m_effectiveDamage;

        std::string stanceStr = m_hasCrossbarStance ? "Crossbar Hip-Tight Stance" : "Standard ADS/Hip Stance";

        std::cout << "[HADES FIRING] Fired " << actualFired << " rounds (" << m_ammoType 
                  << ") | Mode: " << stanceStr
                  << " | Damage: " << totalDamage 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[Hades] Replacing belt drum housing... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[Hades] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " Rapid Mobility LMG\n";
        std::cout << " Operational Stance: " << (m_hasCrossbarStance ? "Crossbar Operator Handle (Tight Hipfire)" : "Standard Irons/Optic") << "\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot   : " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate         : " << m_effectiveRPM << " RPM\n";
        std::cout << " Capacity          : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time       : " << m_effectiveReloadTime << "s\n";
        if (m_hasCrossbarStance) {
            std::cout << " ADS Speed         : N/A (Crossbar Stance Overrides ADS)\n";
        } else {
            std::cout << " ADS Speed         : " << m_effectiveAdsTimeMs << "ms\n";
        }
        std::cout << " Recoil Scaling    : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Lightweight Receiver)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    bool m_hasCrossbarStance;
    std::string m_ammoType;

    // Modified Stats
    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_Hades.hpp"

struct HadesTestModule {
    HadesTestModule() {
        std::cout << "=== INITIALIZING DEDICATED HADES WEAPON SYSTEM ===\n";

        Weapon_Hades hades;

        // 1. Inspect factory stock Hades
        hades.DisplayBlueprint();
        hades.FireAutomatic(20);

        // 2. Build mobility & Crossbar Operator setup
        auto crossbarHandle = std::make_shared<Attachment>(
            "Crossbar Handle", AttachmentType::Handle, StatModifiers{0, 0, 0, 0.0f, -0.25f, -320.0f, true}
        );

        auto rapidFireBarrel = std::make_shared<Attachment>(
            "Rapid Fire Barrel", AttachmentType::Barrel, StatModifiers{0, 70, 0, 0.0f, 0.08f, 15.0f, false}
        );

        auto hybrid84Belt = std::make_shared<Attachment>(
            "84-Round Hybrid Mag", AttachmentType::Magazine, StatModifiers{0, 0, 19, 0.3f, 0.02f, 20.0f, false}
        );

        std::cout << "\n--- Customizing Hades in Gunsmith ---\n";
        hades.EquipAttachment(crossbarHandle);
        hades.EquipAttachment(rapidFireBarrel);
        hades.EquipAttachment(hybrid84Belt);

        // 3. Inspect updated blueprint & test firing stance
        hades.DisplayBlueprint();
        hades.Reload();
        hades.FireAutomatic(45);

        std::cout << "\n=== HADES TEST COMPLETE ===\n";
        std::exit(0);
    }
};

// Global instance executes automatically upon module load
static HadesTestModule g_HadesModuleInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Magazine, Underbarrel };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_Holger26 {
public:
    Weapon_Holger26()
        : m_baseName("Holger 26"),
          m_baseDamage(31),
          m_baseRPM(710),
          m_baseMagCapacity(100),
          m_currentAmmo(100),
          m_baseReloadTime(4.0f),
          m_baseAdsTimeMs(330.0f),
          m_ammoType("5.56x45mm NATO Double-Drum") {
        RecalculateEffectiveStats();
    }

    ~Weapon_Holger26() = default;

    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[Holger 26] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[Holger 26] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    void FireAutomatic(int rounds) {
        if (m_currentAmmo <= 0) {
            std::cout << "[Holger 26] *CLICK* Magazine empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamage = actualFired * m_effectiveDamage;

        std::cout << "[HOLGER 26 FIRING] Discharged " << actualFired << " rounds (" << m_ammoType 
                  << ") | Total Damage: " << totalDamage 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[Holger 26] Swapping dual-drum / box magazine... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[Holger 26] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " Modular LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot: " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Mag Capacity    : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory 100-Round Double-Drum)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    std::string m_ammoType;

    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_Holger26.hpp"

struct Holger26TestModule {
    Holger26TestModule() {
        std::cout << "=== INITIALIZING DEDICATED HOLGER 26 WEAPON SYSTEM ===\n";

        Weapon_Holger26 holger;

        holger.DisplayBlueprint();
        holger.FireAutomatic(25);

        // Convert Holger 26 into a fast AR hybrid via 30-round mag and ultralight stock
        auto arConversionMag = std::make_shared<Attachment>(
            "30-Round Lightweight Mag", AttachmentType::Magazine, StatModifiers{0, 0, -70, -1.8f, -0.05f, -60.0f}
        );

        auto ultralightBarrel = std::make_shared<Attachment>(
            "MIP Ultralight Barrel", AttachmentType::Barrel, StatModifiers{0, 0, 0, 0.0f, 0.04f, -30.0f}
        );

        std::cout << "\n--- Customizing Holger 26 into AR-Hybrid Setup ---\n";
        holger.EquipAttachment(arConversionMag);
        holger.EquipAttachment(ultralightBarrel);

        holger.DisplayBlueprint();
        holger.Reload();
        holger.FireAutomatic(20);

        std::cout << "\n=== HOLGER 26 TEST COMPLETE ===\n";
    }
};

static Holger26TestModule g_Holger26ModuleInstance;

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

enum class AttachmentType { Optic, Muzzle, Stock, Barrel, Magazine, Underbarrel };

struct StatModifiers {
    int damageDelta = 0;
    int rpmDelta = 0;
    int magSizeDelta = 0;
    float reloadTimeDeltaSec = 0.0f;
    float recoilPercentDelta = 0.0f; // Negative = recoil reduction
    float adsTimeDeltaMs = 0.0f;
};

class Attachment {
public:
    Attachment(std::string name, AttachmentType type, StatModifiers modifiers)
        : m_name(std::move(name)), m_type(type), m_modifiers(modifiers) {}

    std::string GetName() const { return m_name; }
    AttachmentType GetType() const { return m_type; }
    StatModifiers GetModifiers() const { return m_modifiers; }

private:
    std::string m_name;
    AttachmentType m_type;
    StatModifiers m_modifiers;
};

class Weapon_UL736 {
public:
    Weapon_UL736()
        : m_baseName("UL736"),
          m_baseDamage(35),
          m_baseRPM(620),
          m_baseMagCapacity(60),
          m_currentAmmo(60),
          m_baseReloadTime(3.6f),
          m_baseAdsTimeMs(340.0f),
          m_ammoType("5.56x45mm NATO High-Precision") {
        RecalculateEffectiveStats();
    }

    ~Weapon_UL736() = default;

    void EquipAttachment(std::shared_ptr<Attachment> attachment) {
        if (!attachment) return;
        m_attachments[attachment->GetType()] = attachment;
        RecalculateEffectiveStats();
        std::cout << "[UL736] Equipped: " << attachment->GetName() << "\n";
    }

    void DetachAttachment(AttachmentType type) {
        if (m_attachments.erase(type)) {
            RecalculateEffectiveStats();
            std::cout << "[UL736] Detached attachment.\n";
        }
    }

    void RecalculateEffectiveStats() {
        m_effectiveDamage = m_baseDamage;
        m_effectiveRPM = m_baseRPM;
        m_effectiveMagCapacity = m_baseMagCapacity;
        m_effectiveReloadTime = m_baseReloadTime;
        m_effectiveAdsTimeMs = m_baseAdsTimeMs;
        m_effectiveRecoilMult = 1.0f;

        for (const auto& [type, att] : m_attachments) {
            auto mods = att->GetModifiers();
            m_effectiveDamage += mods.damageDelta;
            m_effectiveRPM += mods.rpmDelta;
            m_effectiveMagCapacity += mods.magSizeDelta;
            m_effectiveReloadTime += mods.reloadTimeDeltaSec;
            m_effectiveAdsTimeMs += mods.adsTimeDeltaMs;
            m_effectiveRecoilMult += mods.recoilPercentDelta;
        }

        m_effectiveReloadTime = std::max(1.0f, m_effectiveReloadTime);
        m_effectiveAdsTimeMs = std::max(50.0f, m_effectiveAdsTimeMs);
        m_currentAmmo = std::min(m_currentAmmo, m_effectiveMagCapacity);
    }

    void FireAutomatic(int rounds) {
        if (m_currentAmmo <= 0) {
            std::cout << "[UL736] *CLICK* Magazine empty! Reload required.\n";
            return;
        }

        int actualFired = std::min(rounds, m_currentAmmo);
        m_currentAmmo -= actualFired;
        int totalDamage = actualFired * m_effectiveDamage;

        std::cout << "[UL736 FIRING] Fired " << actualFired << " rounds (" << m_ammoType 
                  << ") | Bullpup Layout | Total Damage: " << totalDamage 
                  << " | Recoil Scale: " << (m_effectiveRecoilMult * 100.0f) << "%"
                  << " | Remaining Ammo: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void Reload() {
        std::cout << "[UL736] Ejecting rear bullpup magazine and inserting fresh box... (" << m_effectiveReloadTime << "s)\n";
        m_currentAmmo = m_effectiveMagCapacity;
        std::cout << "[UL736] Reload Complete: " << m_currentAmmo << "/" << m_effectiveMagCapacity << "\n";
    }

    void DisplayBlueprint() const {
        std::cout << "\n======================================================\n";
        std::cout << " WEAPON BLUEPRINT: " << m_baseName << " Bullpup LMG\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << " Damage per Shot: " << m_effectiveDamage << " (Base: " << m_baseDamage << ")\n";
        std::cout << " Fire Rate       : " << m_effectiveRPM << " RPM\n";
        std::cout << " Mag Capacity    : " << m_effectiveMagCapacity << " rounds\n";
        std::cout << " Reload Time     : " << m_effectiveReloadTime << "s\n";
        std::cout << " ADS Speed       : " << m_effectiveAdsTimeMs << "ms\n";
        std::cout << " Recoil Scaling  : " << (m_effectiveRecoilMult * 100.0f) << "%\n";
        std::cout << " Attachments:\n";

        if (m_attachments.empty()) {
            std::cout << "   - None (Factory Bullpup Frame)\n";
        } else {
            for (const auto& [type, att] : m_attachments) {
                std::cout << "   - " << att->GetName() << "\n";
            }
        }
        std::cout << "======================================================\n";
    }

private:
    std::string m_baseName;
    int m_baseDamage;
    int m_baseRPM;
    int m_baseMagCapacity;
    int m_currentAmmo;
    float m_baseReloadTime;
    float m_baseAdsTimeMs;
    std::string m_ammoType;

    int m_effectiveDamage;
    int m_effectiveRPM;
    int m_effectiveMagCapacity;
    float m_effectiveReloadTime;
    float m_effectiveAdsTimeMs;
    float m_effectiveRecoilMult = 1.0f;

    std::unordered_map<AttachmentType, std::shared_ptr<Attachment>> m_attachments;
};

#include "Weapon_UL736.hpp"

struct UL736TestModule {
    UL736TestModule() {
        std::cout << "=== INITIALIZING DEDICATED UL736 WEAPON SYSTEM ===\n";

        Weapon_UL736 ul736;

        ul736.DisplayBlueprint();
        ul736.FireAutomatic(20);

        // Build a long-range laser-beam setup
        auto RTC254Barrel = std::make_shared<Attachment>(
            "RTC 25.4\" Extended Barrel", AttachmentType::Barrel, StatModifiers{2, 0, 0, 0.0f, -0.15f, 35.0f}
        );

        auto 80RoundMag = std::make_shared<Attachment>(
            "80-Round Extended Mag", AttachmentType::Magazine, StatModifiers{0, 0, 20, 0.5f, 0.02f, 25.0f}
        );

        std::cout << "\n--- Customizing UL736 in Gunsmith ---\n";
        ul736.EquipAttachment(RTC254Barrel);
        ul736.EquipAttachment(80RoundMag);

        ul736.DisplayBlueprint();
        ul736.Reload();
        ul736.FireAutomatic(30);

        std::cout << "\n=== UL736 TEST COMPLETE ===\n";
        std::exit(0);
    }
};

static UL736TestModule g_UL736ModuleInstance;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float r, g, b, a;
    float life; // 1.0 down to 0.0
};

// Simple GLSL Shaders
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;

    out vec4 ParticleColor;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        gl_PointSize = 6.0;
        ParticleColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec4 ParticleColor;
    out vec4 FragColor;

    void main() {
        FragColor = ParticleColor;
    }
)";

class ParticleEmitter {
private:
    std::vector<Particle> particles;
    std::vector<float> vertexData; // x, y, z, r, g, b, a
    GLuint VAO, VBO, shaderProgram;

    GLuint compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        return shader;
    }

public:
    ParticleEmitter() {
        GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fs);
        glLinkProgram(shaderProgram);
        glDeleteShader(vs);
        glDeleteShader(fs);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Position Attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color Attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    ~ParticleEmitter() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }

    void burst(int count) {
        for (int i = 0; i < count; ++i) {
            Particle p;
            p.x = 0.0f; p.y = 0.0f; p.z = 0.0f;
            p.vx = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
            p.vy = ((rand() % 100) / 100.0f) * 2.0f;
            p.vz = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
            p.r = 1.0f; p.g = 0.8f; p.b = 0.2f; p.a = 1.0f;
            p.life = 1.0f;
            particles.push_back(p);
        }
    }

    void update(float dt) {
        const float gravity = -4.0f;

        for (auto& p : particles) {
            p.vy += gravity * dt;
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.z += p.vz * dt;
            p.life -= dt * 1.5f;
            p.a = std::max(0.0f, p.life);
        }

        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                           [](const Particle& p) { return p.life <= 0.0f; }),
            particles.end()
        );
    }

    void render() {
        if (particles.empty()) return;

        vertexData.clear();
        for (const auto& p : particles) {
            vertexData.push_back(p.x);
            vertexData.push_back(p.y);
            vertexData.push_back(p.z);
            vertexData.push_back(p.r);
            vertexData.push_back(p.g);
            vertexData.push_back(p.b);
            vertexData.push_back(p.a);
        }

        glUseProgram(shaderProgram);
        glEnable(GL_PROGRAM_POINT_SIZE);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.size()));
    }
};

#version 430 core

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec4 position; // xyz = pos, w = lifetime (1.0 to 0.0)
    vec4 velocity; // xyz = vel, w = max_lifetime
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

uniform float u_DeltaTime;
uniform vec3 u_Gravity;
uniform float u_Drag;

// Simple GPU Pseudo-random generator
float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= particles.length()) return;

    Particle p = particles[idx];

    if (p.position.w > 0.0) {
        // Update physics
        p.velocity.xyz += u_Gravity * u_DeltaTime;
        p.velocity.xyz *= pow(u_Drag, u_DeltaTime * 60.0);
        p.position.xyz += p.velocity.xyz * u_DeltaTime;

        // Decrease lifetime
        p.position.w -= u_DeltaTime / p.velocity.w;
    } else {
        // Respawn Particle at Origin
        vec3 seed = vec3(idx, gl_Time, p.position.x);
        p.position.xyz = vec3(0.0); // Origin emitter
        
        // Random direction burst
        float rx = rand(seed.xy) * 2.0 - 1.0;
        float ry = rand(seed.yz) * 2.0;
        float rz = rand(seed.xz) * 2.0 - 1.0;
        
        p.velocity.xyz = normalize(vec3(rx, ry, rz)) * (4.0 + rand(seed.xy) * 4.0);
        p.velocity.w = 1.0 + rand(seed.yz) * 2.0; // Random max lifetime (1-3s)
        p.position.w = 1.0; // Reset lifetime
    }

    // Save back to GPU VRAM
    particles[idx] = p;
}

#version 430 core

in float v_Life;
out vec4 FragColor;

void main() {
    if (v_Life <= 0.0) discard;

    // Radial soft point particle
    vec2 circCoord = 2.0 * gl_PointCoord - vec2(1.0);
    float alpha = 1.0 - length(circCoord);
    if (alpha <= 0.0) discard;

    // Color gradient from bright white/yellow to red/fade
    vec3 color = mix(vec3(1.0, 0.2, 0.0), vec3(1.0, 0.9, 0.4), v_Life);
    FragColor = vec4(color, alpha * v_Life);
}

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

struct ParticleGPU {
    glm::vec4 position; // xyz, w = lifetime
    glm::vec4 velocity; // xyz, w = maxLifetime
};

class ComputeParticleSystem {
private:
    GLuint ssbo;
    GLuint computeProgram;
    GLuint renderProgram;
    GLuint emptyVAO; // Dummy VAO for draw call
    uint32_t totalParticles;

public:
    ComputeParticleSystem(uint32_t particleCount) : totalParticles(particleCount) {
        // Initialize particle memory buffer on CPU
        std::vector<ParticleGPU> particles(totalParticles);
        for (auto& p : particles) {
            p.position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Spawns dead, compute shader handles initial seed
            p.velocity = glm::vec4(0.0f, 0.0f, 0.0f, 2.0f);
        }

        // Create SSBO
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, totalParticles * sizeof(ParticleGPU), particles.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        // Dummy VAO required for Core Profile rendering
        glGenVertexArrays(1, &emptyVAO);

        // Load & compile shaders
        computeProgram = createComputeProgram("particle.comp");
        renderProgram = createRenderProgram("particle.vert", "particle.frag");
    }

    ~ComputeParticleSystem() {
        glDeleteBuffers(1, &ssbo);
        glDeleteVertexArrays(1, &emptyVAO);
        glDeleteProgram(computeProgram);
        glDeleteProgram(renderProgram);
    }

    void update(float deltaTime) {
        glUseProgram(computeProgram);

        // Set uniforms
        glUniform1f(glGetUniformLocation(computeProgram, "u_DeltaTime"), deltaTime);
        glUniform3f(glGetUniformLocation(computeProgram, "u_Gravity"), 0.0f, -9.81f, 0.0f);
        glUniform1f(glGetUniformLocation(computeProgram, "u_Drag"), 0.98f);

        // Ensure SSBO is bound
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        // Dispatch Compute Shader (Work Group size = 256)
        GLuint numWorkGroups = (totalParticles + 255) / 256;
        glDispatchCompute(numWorkGroups, 1, 1);

        // Memory barrier to guarantee compute writes finish before rendering reads
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }

    void render(const glm::mat4& viewProjectionMatrix) {
        glUseProgram(renderProgram);

        glUniformMatrix4fv(glGetUniformLocation(renderProgram, "u_ViewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));

        // Enable point sprite parameters
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for fiery glow

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        glBindVertexArray(emptyVAO);

        // Zero data sent from CPU! GPU draws directly from SSBO
        glDrawArrays(GL_POINTS, 0, totalParticles);
    }

private:
    GLuint createComputeProgram(const char* filepath) { /* Compiler implementation */ return 0; }
    GLuint createRenderProgram(const char* vert, const char* frag) { /* Compiler implementation */ return 0; }
};