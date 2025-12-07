#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // std::swap
#include <cstdlib>   // std::rand
#include <ctime>     // std::time
#include <memory>    // std::unique_ptr (pentru modern C++, optional dar bun)

// =========================================================
// 1. MODULE: EXCEPTIONS (Ierarhie proprie)
// =========================================================

class GameException : public std::exception {
protected:
    std::string message;
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class OutOfBoundsException : public GameException {
public:
    explicit OutOfBoundsException(const std::string& entity)
        : GameException("[CRITICAL] " + entity + " a incercat sa iasa din harta!") {}
};

class InvalidActionException : public GameException {
public:
    InvalidActionException() : GameException("[ERROR] Actiune invalida in acest context.") {}
};

// =========================================================
// 2. MODULE: UTILS (Structuri ajutatoare)
// =========================================================

struct Coordinates {
    int x;
    int y;

    bool operator==(const Coordinates& other) const {
        return x == other.x && y == other.y;
    }
};

// =========================================================
// 3. MODULE: ENTITIES (Clasa de baza abstracta)
// =========================================================

class Entity {
protected:
    std::string name;
    Coordinates position;

    // [STATIC] Contor pentru a demonstra cate obiecte sunt in memorie
    static int globalEntityCount;

public:
    Entity(const std::string& n, int x, int y) : name(n), position{x, y} {
        globalEntityCount++;
    }

    // [VIRTUAL] Destructor obligatoriu
    virtual ~Entity() {
        globalEntityCount--;
    }

    // [PURE VIRTUAL] Metode ce trebuie implementate
    virtual void update() = 0;
    virtual void displayStatus(std::ostream& os) const = 0;

    // [VIRTUAL] Clone pattern pentru Deep Copy (Esențial la M2)
    virtual Entity* clone() const = 0;

    // Getters de nivel inalt
    std::string getName() const { return name; }
    Coordinates getPosition() const { return position; }

    // Static getter
    static int getCount() { return globalEntityCount; }

    // Operator << polimorfic
    friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
        e.displayStatus(os);
        return os;
    }
};

int Entity::globalEntityCount = 0;

// =========================================================
// 4. MODULE: CONCRETE CLASSES (Derivate)
// =========================================================

// --- PLAYER ---
class Player : public Entity {
    int hp;
    int score;
    bool shieldActive;

public:
    Player(const std::string& n, int x, int y)
        : Entity(n, x, y), hp(100), score(0), shieldActive(false) {}

    void update() override {
        // Logica complexa: Regenerare pasiva sau scadere energie
        if (hp < 100 && hp > 0) hp++;
    }

    // Functie specifica Player (Move cu validare)
    void move(int dx, int dy) {
        int newX = position.x + dx;
        int newY = position.y + dy;

        // Verificare limite (Arunca exceptie M2)
        if (newX < 0 || newX > 100 || newY < 0 || newY > 100) {
            throw OutOfBoundsException(name);
        }

        position.x = newX;
        position.y = newY;
        std::cout << " -> " << name << " s-a mutat la (" << position.x << ", " << position.y << ")\n";
    }

    // Functie specifica pentru RTTI (Healing)
    void usePotion() {
        hp = 100;
        std::cout << " -> " << name << " a folosit o potiune magica! HP Full.\n";
    }

    void addScore(int points) { score += points; }

    void displayStatus(std::ostream& os) const override {
        os << "[PLAYER] " << name << " | HP:" << hp << " | Score:" << score
           << " | Pos:(" << position.x << "," << position.y << ")";
    }

    // Implementare Clone pentru copiere corecta
    Entity* clone() const override { return new Player(*this); }
};

// --- ENEMY ---
class Enemy : public Entity {
    int damagePower;
    std::string type;

public:
    Enemy(const std::string& n, int x, int y, int dmg, const std::string& t)
        : Entity(n, x, y), damagePower(dmg), type(t) {}

    void update() override {
        // AI Simplu: miscare random
        int dx = (std::rand() % 3) - 1;
        int dy = (std::rand() % 3) - 1;

        // Verificare simpla fara exceptii la inamici
        if (position.x + dx >= 0 && position.x + dx <= 100) position.x += dx;
        if (position.y + dy >= 0 && position.y + dy <= 100) position.y += dy;
    }

    void displayStatus(std::ostream& os) const override {
        os << "[ENEMY-" << type << "] " << name << " | DMG:" << damagePower
           << " | Pos:(" << position.x << "," << position.y << ")";
    }

    Entity* clone() const override { return new Enemy(*this); }
};

// --- OBSTACLE ---
class Obstacle : public Entity {
    bool destructible;

public:
    Obstacle(int x, int y, bool dest) : Entity("Wall", x, y), destructible(dest) {}

    void update() override {
        // Obstacolele nu fac nimic la update
    }

    void displayStatus(std::ostream& os) const override {
        os << "[OBSTACLE] " << (destructible ? "Breakable" : "Solid")
           << " Wall at (" << position.x << "," << position.y << ")";
    }

    Entity* clone() const override { return new Obstacle(*this); }
};

// =========================================================
// 5. MODULE: GAME MANAGER (Level Class)
// =========================================================

class LevelManager {
    // Vector polimorfic de pointeri
    std::vector<Entity*> entities;

public:
    LevelManager() = default;

    // --- REGULA CELOR 3 (Copy-and-Swap Idiom) ---
    // Aceasta este partea critica pentru Milestone 2

    // 1. Destructor
    ~LevelManager() {
        clear();
    }

    // 2. Copy Constructor (Deep Copy)
    LevelManager(const LevelManager& other) {
        for (const auto& entity : other.entities) {
            entities.push_back(entity->clone()); // Foloseste constructorul virtual
        }
        std::cout << "[DEBUG] LevelManager copied via Deep Copy.\n";
    }

    // 3. Copy Assignment Operator
    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        return *this;
    }
    // ---------------------------------------------

    void addEntity(Entity* e) {
        entities.push_back(e);
    }

    void clear() {
        for (auto e : entities) delete e;
        entities.clear();
    }

    void updateAll() {
        std::cout << "\n--- Updating Game World ---\n";
        for (auto e : entities) {
            e->update();
            std::cout << *e << "\n"; // Afisare polimorfica
        }
    }

    // Functie helper pentru a gasi Player-ul (RTTI - dynamic_cast)
    Player* getPlayer() {
        for (auto e : entities) {
            if (auto p = dynamic_cast<Player*>(e)) {
                return p;
            }
        }
        return nullptr;
    }

    int getEntityCount() const { return entities.size(); }
};

// =========================================================
// 6. MODULE: MAIN APPLICATION LOGIC
// =========================================================

class GameApplication {
    LevelManager currentLevel;
    bool isRunning;

    void initLevel() {
        // Aici se creeaza entitatile (expandable catre Factory Pattern la M3)
        currentLevel.addEntity(new Player("Hero", 50, 50));
        currentLevel.addEntity(new Enemy("Sparx", 10, 10, 20, "Fast"));
        currentLevel.addEntity(new Enemy("Qix", 80, 80, 50, "Boss"));
        currentLevel.addEntity(new Obstacle(20, 20, false));
        currentLevel.addEntity(new Obstacle(25, 25, true));
    }

    void showMenu() {
        std::cout << "\n==============================\n";
        std::cout << "   VOLFIED REMASTERED (C++)   \n";
        std::cout << "==============================\n";
        std::cout << "1. Move Player (WASD)\n";
        std::cout << "2. Wait (Regenerate HP)\n";
        std::cout << "3. Use Potion (Heal)\n";
        std::cout << "4. Save Game (Test Deep Copy)\n";
        std::cout << "5. Test Crash (Exception)\n";
        std::cout << "0. Exit\n";
        std::cout << "> ";
    }

public:
    GameApplication() : isRunning(true) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        initLevel();
    }

    void run() {
        std::cout << "System initialized. Active Objects: " << Entity::getCount() << "\n";

        while (isRunning) {
            currentLevel.updateAll();
            showMenu();

            char cmd;
            std::cin >> cmd;

            try {
                Player* p = currentLevel.getPlayer();
                if (!p) throw GameException("Player entity missing from level!");

                switch (cmd) {
                    case '1': {
                        std::cout << "Direction (w/a/s/d): ";
                        char dir;
                        std::cin >> dir;
                        if (dir == 'w') p->move(0, -10);
                        else if (dir == 's') p->move(0, 10);
                        else if (dir == 'a') p->move(-10, 0);
                        else if (dir == 'd') p->move(10, 0);
                        else std::cout << "Invalid direction.\n";
                        break;
                    }
                    case '2':
                        std::cout << "Waiting...\n";
                        p->addScore(5);
                        break;
                    case '3':
                        p->usePotion();
                        break;
                    case '4': {
                        std::cout << "Creating SavePoint...\n";
                        LevelManager backup = currentLevel; // Test Copy Ctor
                        std::cout << "SavePoint Created! (Objects in RAM doubled: " << Entity::getCount() << ")\n";
                        // Backup is destroyed here (Test Destructor)
                        break;
                    }
                    case '5':
                        std::cout << "Teleporting to void...\n";
                        p->move(1000, 1000); // Throws OutOfBoundsException
                        break;
                    case '0':
                        isRunning = false;
                        std::cout << "Exiting game.\n";
                        break;
                    default:
                        throw InvalidActionException();
                }
            }
            catch (const OutOfBoundsException& e) {
                std::cerr << "\n!!! GAME OVER PREVENTION !!!\n" << e.what() << "\n";
                std::cout << "Reseting player to safe zone...\n";
                if (Player* p = currentLevel.getPlayer()) {
                    Coordinates pos = p->getPosition();
                    p->move(50 - pos.x, 50 - pos.y); // Move back to center
                }
            }
            catch (const GameException& e) {
                std::cerr << "\n[GAME ERROR]: " << e.what() << "\n";
            }
            catch (const std::exception& e) {
                std::cerr << "\n[SYSTEM ERROR]: " << e.what() << "\n";
            }
        }
    }
};

int main() {
    try {
        GameApp app; // Alias pt GameApplication ca sa fie mai scurt in main
        app.run();
    } catch (...) {
        std::cerr << "Fatal unhandled error.\n";
        return -1;
    }
    return 0;
}