#include <iostream>
#include <string>
#include <vector>

// --- Player class ---
class Player {
public:
    std::string name;
    int health;
    int maxHealth;
    float regenRate;
    float sprintSpeed;
    float fallDamageMultiplier;
    float explosiveDamageMultiplier;
    float crouchSpeed;
    float adsSpeed;

    Player(std::string n)
        : name(n), health(100), maxHealth(100),
          regenRate(2.0f), sprintSpeed(10.0f),
          fallDamageMultiplier(1.0f), explosiveDamageMultiplier(1.0f),
          crouchSpeed(5.0f), adsSpeed(6.0f) {}
};

// --- Base Perk interface ---
class Perk {
public:
    virtual void apply(Player &p) = 0;
    virtual std::string getName() = 0;
};

// --- RED Perks ---
class FastRecover : public Perk { public: void apply(Player &p) override { p.regenRate *= 2.0f; } std::string getName() override { return "Fast Recover"; } };
class Lightweight : public Perk { public: void apply(Player &p) override { p.sprintSpeed *= 1.10f; p.fallDamageMultiplier *= 0.75f; } std::string getName() override { return "Lightweight"; } };
class FlakJacket : public Perk { public: void apply(Player &p) override { p.explosiveDamageMultiplier *= 0.65f; } std::string getName() override { return "Flak Jacket"; } };
class Skulker : public Perk { public: void apply(Player &p) override { p.crouchSpeed *= 1.12f; p.adsSpeed *= 1.12f; } std::string getName() override { return "Skulker"; } };
class Agile : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Agile! Faster aim after sprint.\n"; } std::string getName() override { return "Agile"; } };
class Persistence : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Persistence! Scorestreak progress persists after death.\n"; } std::string getName() override { return "Persistence"; } };
class Martyrdom : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Martyrdom! Drops grenade on death.\n"; } std::string getName() override { return "Martyrdom"; } };
class Overclock : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Overclock! Faster operator skill charge.\n"; } std::string getName() override { return "Overclock"; } };
class Demolition : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Demolition! Extra lethal equipment.\n"; } std::string getName() override { return "Demolition"; } };

// --- GREEN Perks ---
class Vulture : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Vulture! Can pick up ammo.\n"; } std::string getName() override { return "Vulture"; } };
class Toughness : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Toughness! Reduced flinch.\n"; } std::string getName() override { return "Toughness"; } };
class Tracker : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Tracker! Can see footprints.\n"; } std::string getName() override { return "Tracker"; } };
class Ghost : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Ghost! Invisible to UAVs.\n"; } std::string getName() override { return "Ghost"; } };
class ColdBlooded : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Cold-Blooded! Immune to AI targeting.\n"; } std::string getName() override { return "Cold-Blooded"; } };
class Shrapnel : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Shrapnel! Extra tactical equipment.\n"; } std::string getName() override { return "Shrapnel"; } };
class Restock : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Restock! Equipment regenerates.\n"; } std::string getName() override { return "Restock"; } };
class Amped : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Amped! Faster weapon swap.\n"; } std::string getName() override { return "Amped"; } };
class HighAlert : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped High Alert! Screen flashes when seen.\n"; } std::string getName() override { return "High Alert"; } };

// --- BLUE Perks ---
class Hardline : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Hardline! Scorestreaks earned faster.\n"; } std::string getName() override { return "Hardline"; } };
class DemoExpert : public Perk { public: void apply(Player &p) override { p.explosiveDamageMultiplier *= 1.25f; std::cout << p.name << " equipped Demo Expert! Explosives stronger.\n"; } std::string getName() override { return "Demo Expert"; } };
class Engineer : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Engineer! Detects enemy equipment.\n"; } std::string getName() override { return "Engineer"; } };
class TacticalMask : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Tactical Mask! Reduced tactical effects.\n"; } std::string getName() override { return "Tactical Mask"; } };
class Alert : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Alert! Radar shows nearby enemies.\n"; } std::string getName() override { return "Alert"; } };
class DeadSilence : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Dead Silence! Silent footsteps.\n"; } std::string getName() override { return "Dead Silence"; } };
class GungHo : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Gung-Ho! Can fire while sprinting.\n"; } std::string getName() override { return "Gung-Ho"; } };
class Tactician : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Tactician! Extra tactical grenade.\n"; } std::string getName() override { return "Tactician"; } };
class ManOWar : public Perk { public: void apply(Player &p) override { std::cout << p.name << " equipped Man-O-War! Weapon damage boosted.\n"; } std::string getName() override { return "Man-O-War"; } };

// --- Main ---
int main() {
    Player globalPlayer("PlayerOne");

    // Example loadout: 1 Red, 1 Green, 1 Blue
    FastRecover fr;
    Ghost gh;
    DeadSilence ds;

    std::vector<Perk*> loadout = {&fr, &gh, &ds};

    for (auto perk : loadout) {
        perk->apply(globalPlayer);
        std::cout << globalPlayer.name << " equipped " << perk->getName() << "\n";
    }

    std::cout << "\nFinal stats:\n"
              << "Regen rate: " << globalPlayer.regenRate << "\n"
              << "Sprint speed: " << globalPlayer.sprintSpeed << "\n"
              << "Fall damage multiplier: " << globalPlayer.fallDamageMultiplier << "\n"
              << "Explosive damage multiplier: " << globalPlayer.explosiveDamageMultiplier << "\n"
              << "Crouch speed: " << globalPlayer.crouchSpeed << "\n"
              << "ADS speed: " << globalPlayer.adsSpeed << "\n";

    return 0;
}
