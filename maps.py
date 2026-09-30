#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Crash Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Crossfire Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Shooting Range Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Shipment 1994 Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Hijacked Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Nuketown Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Killhouse Map - COD Mobile");

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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Standoff Map - COD Mobile");

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

int main() {
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

int main() {
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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Nuketown Russia Map - COD Mobile");

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

int main() {
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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Raid Map - COD Mobile");

    // Load textures
    sf::Texture groundTexture; groundTexture.loadFromFile("raid_ground.png"); // stone courtyard
    sf::Texture houseTexture; houseTexture.loadFromFile("raid_house.png");    // villa walls
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
