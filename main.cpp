#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // swap
#include <cstdlib>   // rand
#include <ctime>     // time
#include <memory>    // smart pointers (optional, dar bun practice)

// ==========================================
// 0. UTILS & TEMPLATES [TEMA 3]
// ==========================================

struct Position {
    int x, y;
};

// [TEMA 3] Funcție Șablon (Template Function) - Instanțiere 1
// Verifică dacă o valoare e într-un interval.
// Utilizare: coordonate (int), timp (float), etc.
template <typename T>
bool isWithinBounds(T value, T min, T max) {
    return (value >= min && value <= max);
}

// [TEMA 3] Clasă Șablon (Class Template) - Instanțiere 1 & 2
// O cutie de loot care poate conține orice tip de recompensă
template <typename T>
class LootBox {
    T content;
    bool opened;
public:
    LootBox(T item) : content(item), opened(false) {}

    T open() {
        if (!opened) {
            opened = true;
            std::cout << "[LOOT] Ai deschis o cutie! Continut: " << content << "\n";
            return content;
        }
        // Returnăm o valoare default dacă e deja deschisă (simplificare)
        return content;
    }

    bool isOpened() const { return opened; }
};

// ==========================================
// 1. DESIGN PATTERNS [TEMA 3]
// ==========================================

// [TEMA 3] Design Pattern: SINGLETON
// Gestionăm configurația jocului global
class GameConfig {
private:
    int mapWidth;
    int mapHeight;
    int difficulty;

    // Constructor privat
    GameConfig() : mapWidth(100), mapHeight(100), difficulty(1) {}

public:
    // Ștergem metodele de copiere pentru a asigura unicitatea
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    // Metoda statică de acces (Meyers Singleton - Thread Safe in C++11)
    static GameConfig& getInstance() {
        static GameConfig instance;
        return instance;
    }

    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    void setDifficulty(int d) { difficulty = d; }
    int getDifficulty() const { return difficulty; }
};

// ==========================================
// 2. IERARHIE DE EXCEPȚII
// ==========================================

class GameException : public std::exception {
protected:
    std::string message;
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override { return message.c_str(); }
};

class OutOfBoundsException : public GameException {
public:
    explicit OutOfBoundsException(const std::string& name)
        : GameException("CRITIC: " + name + " a incercat sa iasa din harta!") {}
};

class InvalidActionException : public GameException {
public:
    InvalidActionException() : GameException("Actiune invalida! Comanda necunoscuta.") {}
};

// ==========================================
// 3. IERARHIE DE CLASE (ENTITĂȚI)
// ==========================================

class Entity {
protected:
    int x, y;
    std::string name;
    static int entityCount;

public:
    Entity(const std::string& n, int startX, int startY) : name(n), x(startX), y(startY) {
        entityCount++;
    }

    virtual ~Entity() { entityCount--; }

    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;
    virtual Entity* clone() const = 0;

    std::string getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }

    static int getCount() { return entityCount; }

    friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
        e.printInfo(os);
        return os;
    }
};

int Entity::entityCount = 0;

// --- PLAYER ---
class Player : public Entity {
    int health;
    int score;
public:
    Player(const std::string& n, int x, int y) : Entity(n, x, y), health(100), score(0) {}

    void update() override {
        if (health > 0) health -= 1;
    }

    void move(int dx, int dy) {
        int newX = x + dx;
        int newY = y + dy;

        // [TEMA 3] Folosirea Funcției Template (Instanțiere cu int)
        // Luăm limitele din Singleton
        int maxX = GameConfig::getInstance().getWidth();
        int maxY = GameConfig::getInstance().getHeight();

        if (!isWithinBounds<int>(newX, 0, maxX) || !isWithinBounds<int>(newY, 0, maxY)) {
            throw OutOfBoundsException(name);
        }

        x = newX;
        y = newY;
        std::cout << " -> " << name << " s-a mutat la (" << x << "," << y << ").\n";
    }

    void addScore(int points) { score += points; }

    void heal() {
        health = 100;
        std::cout << " -> " << name << " a folosit o potiune! HP Full.\n";
    }

    Position getPosition() const { return {x, y}; }

    void printInfo(std::ostream& os) const override {
        os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score << " | Pos: " << x << "," << y;
    }

    Entity* clone() const override { return new Player(*this); }
};

// --- ENEMY ---
class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg) : Entity(n, x, y), damage(dmg) {}

    void update() override {
        // Mișcare simplă random
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;

        int maxX = GameConfig::getInstance().getWidth();
        int maxY = GameConfig::getInstance().getHeight();

        // Verificăm limitele fără excepție la inamici
        if (isWithinBounds(x + dx, 0, maxX)) x += dx;
        if (isWithinBounds(y + dy, 0, maxY)) y += dy;
    }

    void printInfo(std::ostream& os) const override {
        os << "[ENEMY] " << name << " | DMG: " << damage << " | Pos: " << x << "," << y;
    }

    Entity* clone() const override { return new Enemy(*this); }
};

// --- OBSTACLE ---
class Obstacle : public Entity {
    bool destructible;
public:
    Obstacle(int x, int y, bool dest) : Entity("Wall", x, y), destructible(dest) {}

    void update() override { /* Static */ }

    void printInfo(std::ostream& os) const override {
        os << "[OBSTACLE] " << (destructible ? "Destructible" : "Hard") << " Wall at " << x << "," << y;
    }

    Entity* clone() const override { return new Obstacle(*this); }
};

// ==========================================
// 4. DESIGN PATTERN: FACTORY [TEMA 3]
// ==========================================

enum class EntityType {
    Player,
    EnemySparx,
    EnemyQix,
    Obstacle
};

// [TEMA 3] Factory Method Pattern
// Centralizează crearea obiectelor
class EntityFactory {
public:
    static Entity* createEntity(EntityType type, int x, int y) {
        switch (type) {
            case EntityType::Player:
                return new Player("Hero", x, y);
            case EntityType::EnemySparx:
                // Dificultatea din Singleton influențează damage-ul
                return new Enemy("Sparx", x, y, 10 * GameConfig::getInstance().getDifficulty());
            case EntityType::EnemyQix:
                return new Enemy("Qix", x, y, 25 * GameConfig::getInstance().getDifficulty());
            case EntityType::Obstacle:
                return new Obstacle(x, y, false);
            default:
                throw std::invalid_argument("Tip entitate necunoscut in Factory!");
        }
    }
};

// ==========================================
// 5. MANAGER (LEVEL)
// ==========================================
class LevelManager {
    std::vector<Entity*> entities;

    // [TEMA 3] Instanțiere Clasă Template #1 (int)
    LootBox<int> bonusPointsBox;
    // [TEMA 3] Instanțiere Clasă Template #2 (string)
    LootBox<std::string> secretMessageBox;

public:
    LevelManager() : bonusPointsBox(500), secretMessageBox("Secret: Invinge Qix primul!") {}

    ~LevelManager() {
        for (auto e : entities) delete e;
        entities.clear();
    }

    LevelManager(const LevelManager& other)
        : bonusPointsBox(other.bonusPointsBox), secretMessageBox(other.secretMessageBox) {
        for (const auto& e : other.entities) {
            entities.push_back(e->clone());
        }
    }

    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        std::swap(bonusPointsBox, other.bonusPointsBox);
        std::swap(secretMessageBox, other.secretMessageBox);
        return *this;
    }

    void addEntity(Entity* e) {
        entities.push_back(e);
    }

    void updateAll() {
        std::cout << "\n--- Update Frame ---\n";
        for (auto e : entities) {
            e->update();
            std::cout << *e << "\n";
        }
    }

    Player* getPlayer() {
        for (auto e : entities) {
            if (Player* p = dynamic_cast<Player*>(e)) return p;
        }
        return nullptr;
    }

    // Funcție demo pentru Template Class
    void openLootBox() {
        if (Player* p = getPlayer()) {
            // Deschidem cutia cu int
            int points = bonusPointsBox.open();
            p->addScore(points);

            // Deschidem cutia cu string
            std::string msg = secretMessageBox.open();
            std::cout << "Mesaj descifrat: " << msg << "\n";
        }
    }
};

// ==========================================
// 6. MAIN APP
// ==========================================
class GameApp {
    LevelManager level;
    bool isRunning;

public:
    GameApp() : isRunning(true) {
        srand(time(nullptr));

        // Setare dificultate via Singleton
        GameConfig::getInstance().setDifficulty(2);

        // [TEMA 3] Folosire Factory pentru populare
        level.addEntity(EntityFactory::createEntity(EntityType::Player, 50, 50));
        level.addEntity(EntityFactory::createEntity(EntityType::EnemySparx, 10, 10));
        level.addEntity(EntityFactory::createEntity(EntityType::EnemyQix, 80, 80));
        level.addEntity(EntityFactory::createEntity(EntityType::Obstacle, 20, 20));
    }

    void showMenu() {
        std::cout << "\n=== VOLFIED (M3) ===\n";
        std::cout << "1. Misca Player\n";
        std::cout << "2. Asteapta\n";
        std::cout << "3. Heal\n";
        std::cout << "4. Save Game (Copy)\n";
        std::cout << "5. Test OutOfBounds\n";
        std::cout << "6. Deschide Loot Box [TEMA 3]\n";
        std::cout << "0. Exit\n";
        std::cout << "Cmd: ";
    }

    void run() {
        std::cout << "Start Game. Entities: " << Entity::getCount() << "\n";
        std::cout << "Map Size (Singleton): " << GameConfig::getInstance().getWidth() << "x" << GameConfig::getInstance().getHeight() << "\n";

        while (isRunning) {
            level.updateAll();
            showMenu();

            char cmd;
            std::cin >> cmd;

            try {
                Player* p = level.getPlayer();
                if (!p) throw GameException("Player missing!");

                switch (cmd) {
                    case '1': {
                        std::cout << "Dir (w/a/s/d): ";
                        char dir; std::cin >> dir;
                        if (dir == 'w') p->move(0, -10);
                        else if (dir == 's') p->move(0, 10);
                        else if (dir == 'a') p->move(-10, 0);
                        else if (dir == 'd') p->move(10, 0);
                        else std::cout << "Invalid dir.\n";
                        break;
                    }
                    case '2': std::cout << "Wait...\n"; p->addScore(10); break;
                    case '3': p->heal(); break;
                    case '4': {
                        LevelManager saved = level;
                        std::cout << "Game Saved! (Entities in RAM: " << Entity::getCount() << ")\n";
                        break;
                    }
                    case '5': p->move(1000, 1000); break;
                    case '6':
                        // Testam LootBox (Template Class)
                        level.openLootBox();
                        break;
                    case '0': isRunning = false; break;
                    default: throw InvalidActionException();
                }
            }
            catch (const OutOfBoundsException& e) {
                std::cout << "\n[GAME OVER] " << e.what() << "\nResetting player...\n";
                if (Player* p = level.getPlayer()) {
                    Position pos = p->getPosition();
                    p->move(50 - pos.x, 50 - pos.y);
                }
            }
            catch (const GameException& e) {
                std::cout << "\n[ERROR] " << e.what() << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "\n[STD ERROR] " << e.what() << "\n";
            }
        }
    }
};

int main() {
    try {
        GameApp app;
        app.run();
    } catch (...) {
        std::cerr << "Fatal Crash.\n";
        return 1;
    }
    return 0;
}