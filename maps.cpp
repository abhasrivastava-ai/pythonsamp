

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
