#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // swap
#include <cstdlib>   // rand
#include <ctime>     // time

// ==========================================
// 1. IERARHIE DE EXCEPȚII [CERINȚĂ TEMA 2]
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
// 2. CLASA ABSTRACTĂ DE BAZĂ
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

    // [VIRTUAL] Destructor
    virtual ~Entity() {
        entityCount--;
    }

    // [PURE VIRTUAL] Metode ce trebuie implementate
    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;

    // [VIRTUAL] Clone pentru Deep Copy
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

// ==========================================
// 3. CLASE DERIVATE
// ==========================================

// Structura ajutatoare pentru pozitie (Repara eroarea getPosition)
struct Point { int x, y; };

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

        // Verificare limite (100x100)
        if (newX < 0 || newX > 100 || newY < 0 || newY > 100) {
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

    // [FIX] Metoda care lipsea și cauza eroarea de compilare
    Point getPosition() const { return {x, y}; }

    void printInfo(std::ostream& os) const override {
        os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score << " | Pos: " << x << "," << y;
    }

    Entity* clone() const override { return new Player(*this); }
};

class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg) : Entity(n, x, y), damage(dmg) {}

    void update() override {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;

        if (x + dx >= 0 && x + dx <= 100) x += dx;
        if (y + dy >= 0 && y + dy <= 100) y += dy;
    }

    void printInfo(std::ostream& os) const override {
        os << "[ENEMY] " << name << " | DMG: " << damage << " | Pos: " << x << "," << y;
    }

    Entity* clone() const override { return new Enemy(*this); }
};

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
// 4. MANAGER (REGULA CELOR 3)
// ==========================================

class LevelManager {
    std::vector<Entity*> entities;

public:
    LevelManager() = default;

    // 1. Destructor
    ~LevelManager() {
        clear();
    }

    // 2. Copy Constructor
    LevelManager(const LevelManager& other) {
        for (const auto& e : other.entities) {
            entities.push_back(e->clone());
        }
        std::cout << "[DEBUG] LevelManager Deep Copy.\n";
    }

    // 3. Assignment Operator
    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        return *this;
    }

    void addEntity(Entity* e) {
        entities.push_back(e);
    }

    void clear() {
        for (auto e : entities) delete e;
        entities.clear();
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
};

// ==========================================
// 5. MAIN APP
// ==========================================

class GameApp {
    LevelManager level;
    bool isRunning;

public:
    GameApp() : isRunning(true) {
        srand(time(nullptr));
        level.addEntity(new Player("Hero", 50, 50));
        level.addEntity(new Enemy("Sparx", 10, 10, 20));
        level.addEntity(new Enemy("Qix", 80, 80, 50));
        level.addEntity(new Obstacle(20, 20, false));
    }

    void showMenu() {
        std::cout << "\n=== VOLFIED (M2) ===\n";
        std::cout << "1. Misca Player\n2. Asteapta\n3. Heal\n4. Save Game\n5. Test Error\n0. Exit\nCmd: ";
    }

    void run() {
        while (isRunning) {
            level.updateAll();
            showMenu();

            char cmd;
            // [CRITIC PENTRU GITHUB] Daca nu mai e input, oprim jocul
            if (!(std::cin >> cmd)) {
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
                    case '0': isRunning = false; break;
                    default: throw InvalidActionException();
                }
            }
            catch (const OutOfBoundsException& e) {
                std::cout << "\n[GAME OVER] " << e.what() << "\nResetting...\n";
                if (Player* p = level.getPlayer()) {
                    // [FIX] Acum functioneaza pentru ca am definit struct Point si metoda getPosition
                    Point pos = p->getPosition();
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