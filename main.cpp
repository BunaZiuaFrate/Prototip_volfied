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
// 2. CLASA ABSTRACTĂ DE BAZĂ [CERINȚĂ TEMA 2]
// ==========================================

class Entity {
protected:
    int x, y;
    std::string name;

    // [STATIC] Contor pentru a demonstra cate obiecte sunt in memorie
    static int entityCount;

public:
    Entity(const std::string& n, int startX, int startY) : name(n), x(startX), y(startY) {
        entityCount++;
    }

    // [VIRTUAL] Destructor - Obligatoriu pentru ca vom sterge prin pointeri de baza
    virtual ~Entity() {
        entityCount--;
    }

    // [PURE VIRTUAL] Metode ce trebuie implementate
    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;

    // [VIRTUAL] Clone Pattern - Esențial pentru Deep Copy la Tema 2
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

        // Verificare limite (Hardcodat 100x100 pentru Tema 2)
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

    // Structura ajutatoare pentru pozitie
    struct Pos { int x, y; };
    Pos getPosition() const { return {x, y}; }

    void printInfo(std::ostream& os) const override {
        os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score << " | Pos: " << x << "," << y;
    }

    // Returnam raw pointer (new)
    Entity* clone() const override { return new Player(*this); }
};

// --- ENEMY ---
class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg) : Entity(n, x, y), damage(dmg) {}

    void update() override {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;

        // Verificare simpla fara exceptii
        if (x + dx >= 0 && x + dx <= 100) x += dx;
        if (y + dy >= 0 && y + dy <= 100) y += dy;
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
// 4. MANAGER (REGULA CELOR 3) [CERINȚĂ TEMA 1/2]
// ==========================================

class LevelManager {
    // Vector de pointeri RAW (trebuie gestionați manual)
    std::vector<Entity*> entities;

public:
    LevelManager() = default;

    // --- REGULA CELOR 3 (OBLIGATORIE AICI) ---

    // 1. Destructor
    ~LevelManager() {
        clear();
    }

    // 2. Copy Constructor (Deep Copy)
    LevelManager(const LevelManager& other) {
        for (const auto& e : other.entities) {
            // Aici folosim functia virtuala clone() pentru a copia tipul corect
            entities.push_back(e->clone());
        }
        std::cout << "[DEBUG] LevelManager Deep Copy performed.\n";
    }

    // 3. Copy Assignment Operator (Copy-and-Swap Idiom)
    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        return *this;
    }
    // ------------------------------------------

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
            e->update(); // Polimorfism
            std::cout << *e << "\n";
        }
    }

    // Funcție RTTI (dynamic_cast)
    Player* getPlayer() {
        for (auto e : entities) {
            if (Player* p = dynamic_cast<Player*>(e)) {
                return p;
            }
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

        // [TEMA 2] Creare manuală cu new (fără Factory)
        level.addEntity(new Player("Hero", 50, 50));
        level.addEntity(new Enemy("Sparx", 10, 10, 20));
        level.addEntity(new Enemy("Qix", 80, 80, 50));
        level.addEntity(new Obstacle(20, 20, false));
    }

    void showMenu() {
        std::cout << "\n=== VOLFIED (M1 & M2) ===\n";
        std::cout << "1. Misca Player\n";
        std::cout << "2. Asteapta\n";
        std::cout << "3. Heal\n";
        std::cout << "4. Save Game (Test Deep Copy)\n";
        std::cout << "5. Test Error (Exception)\n";
        std::cout << "0. Exit\n";
        std::cout << "Cmd: ";
    }

    void run() {
        while (isRunning) {
            level.updateAll();
            showMenu();

            char cmd;
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
                        std::cout << "Saving game...\n";
                        LevelManager saved = level; // Testam Copy Constructor
                        std::cout << "Game Saved! (Objects in RAM: " << Entity::getCount() << ")\n";
                        // 'saved' se distruge aici, testand destructorul
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
                    auto pos = p->getPosition();
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