#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // swap
#include <cstdlib>   // rand
#include <ctime>     // time
#include <memory>    // OBLIGATORIU PENTRU SMART POINTERS

// ==========================================
// 0. UTILS & TEMPLATES
// ==========================================

struct Position {
    int x, y;
};

// Template Function
template <typename T>
bool isWithinBounds(T value, T min, T max) {
    return (value >= min && value <= max);
}

// Template Class
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
        return content;
    }
    bool isOpened() const { return opened; }
};

// ==========================================
// 1. SINGLETON (GameConfig)
// ==========================================

class GameConfig {
private:
    int mapWidth;
    int mapHeight;
    int difficulty;

    GameConfig() : mapWidth(100), mapHeight(100), difficulty(1) {}

public:
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

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
// 2. EXCEPTII
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
// 3. ENTITATI (Cu Smart Pointers)
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

    // Returnam shared_ptr pentru clone
    virtual std::shared_ptr<Entity> clone() const = 0;

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

    // Aceasta metoda TREBUIE sa existe pentru a repara eroarea de compilare din poza
    Position getPosition() const { return {x, y}; }

    void printInfo(std::ostream& os) const override {
        os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score << " | Pos: " << x << "," << y;
    }

    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Player>(*this);
    }
};

// --- ENEMY ---
class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg) : Entity(n, x, y), damage(dmg) {}

    void update() override {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;
        int maxX = GameConfig::getInstance().getWidth();
        int maxY = GameConfig::getInstance().getHeight();

        if (isWithinBounds(x + dx, 0, maxX)) x += dx;
        if (isWithinBounds(y + dy, 0, maxY)) y += dy;
    }

    void printInfo(std::ostream& os) const override {
        os << "[ENEMY] " << name << " | DMG: " << damage << " | Pos: " << x << "," << y;
    }

    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Enemy>(*this);
    }
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

    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Obstacle>(*this);
    }
};

// ==========================================
// 4. FACTORY (Returneaza Smart Pointers)
// ==========================================

enum class EntityType { Player, EnemySparx, EnemyQix, Obstacle };

class EntityFactory {
public:
    static std::shared_ptr<Entity> createEntity(EntityType type, int x, int y) {
        switch (type) {
            case EntityType::Player:
                return std::make_shared<Player>("Hero", x, y);
            case EntityType::EnemySparx:
                return std::make_shared<Enemy>("Sparx", x, y, 10 * GameConfig::getInstance().getDifficulty());
            case EntityType::EnemyQix:
                return std::make_shared<Enemy>("Qix", x, y, 25 * GameConfig::getInstance().getDifficulty());
            case EntityType::Obstacle:
                return std::make_shared<Obstacle>(x, y, false);
            default:
                throw std::invalid_argument("Unknown Entity Type");
        }
    }
};

// ==========================================
// 5. MANAGER (LEVEL) - FARA Memory Leaks
// ==========================================
class LevelManager {
    // Vector de Smart Pointers! Se sterge singur.
    std::vector<std::shared_ptr<Entity>> entities;
    LootBox<int> bonusPointsBox;
    LootBox<std::string> secretMessageBox;

public:
    LevelManager() : bonusPointsBox(500), secretMessageBox("Secret: Invinge Qix primul!") {}

    // Destructorul e implicit (nu mai trebuie sa stergem nimic manual)
    ~LevelManager() = default;

    // Copy Constructor - Folosim clone()
    LevelManager(const LevelManager& other)
        : bonusPointsBox(other.bonusPointsBox), secretMessageBox(other.secretMessageBox) {
        for (const auto& e : other.entities) {
            entities.push_back(e->clone());
        }
    }

    // Copy Assignment
    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        std::swap(bonusPointsBox, other.bonusPointsBox);
        std::swap(secretMessageBox, other.secretMessageBox);
        return *this;
    }

    void addEntity(std::shared_ptr<Entity> e) {
        entities.push_back(e);
    }

    void updateAll() {
        std::cout << "\n--- Update Frame ---\n";
        for (auto& e : entities) {
            e->update();
            std::cout << *e << "\n";
        }
    }

    // Returnam raw pointer doar pentru observare (nu ownership)
    Player* getPlayer() {
        for (auto& e : entities) {
            // dynamic_pointer_cast pentru shared_ptr
            if (auto p = std::dynamic_pointer_cast<Player>(e)) {
                return p.get();
            }
        }
        return nullptr;
    }

    void openLootBox() {
        if (Player* p = getPlayer()) {
            int points = bonusPointsBox.open();
            p->addScore(points);
            std::string msg = secretMessageBox.open();
            std::cout << "Mesaj: " << msg << "\n";
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
        GameConfig::getInstance().setDifficulty(2);

        // Factory returneaza acum shared_ptr, totul e safe
        level.addEntity(EntityFactory::createEntity(EntityType::Player, 50, 50));
        level.addEntity(EntityFactory::createEntity(EntityType::EnemySparx, 10, 10));
        level.addEntity(EntityFactory::createEntity(EntityType::EnemyQix, 80, 80));
        level.addEntity(EntityFactory::createEntity(EntityType::Obstacle, 20, 20));
    }

    void showMenu() {
        std::cout << "\n=== VOLFIED (M3 - Smart Ptr) ===\n";
        std::cout << "1. Misca Player\n2. Asteapta\n3. Heal\n4. Save Game\n5. Test Error\n6. LootBox\n0. Exit\nCmd: ";
    }

    void run() {
        while (isRunning) {
            level.updateAll();
            showMenu();

            char cmd;
            if (!(std::cin >> cmd)) { // Protectie daca se termina inputul
                isRunning = false;
                break;
            }

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
                    case '2': p->addScore(10); break;
                    case '3': p->heal(); break;
                    case '4': {
                        LevelManager saved = level;
                        std::cout << "Game Saved! (RAM entities: " << Entity::getCount() << ")\n";
                        break;
                    }
                    case '5': p->move(1000, 1000); break;
                    case '6': level.openLootBox(); break;
                    case '0': isRunning = false; break;
                    default: throw InvalidActionException();
                }
            }
            catch (const OutOfBoundsException& e) {
                std::cout << "\n[GAME OVER] " << e.what() << "\nResetting...\n";
                if (Player* p = level.getPlayer()) {
                    Position pos = p->getPosition(); // Aici nu va mai da eroare
                    p->move(50 - pos.x, 50 - pos.y);
                }
            }
            catch (const std::exception& e) {
                std::cout << "\n[ERROR] " << e.what() << "\n";
            }
        }
    }
};

int main() {
    try {
        GameApp app;
        app.run();
    } catch (...) {
        return 1;
    }
    return 0;
}