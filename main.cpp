#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include <thread> // Pentru simulare timp (optional)
#include <chrono>

// ==========================================
// 1. IERARHIE DE EXCEPȚII
// ==========================================
class GameException : public std::exception {
protected:
    std::string message;
public:
    GameException(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class OutOfBoundsException : public GameException {
public:
    OutOfBoundsException(const std::string& name)
        : GameException("CRITIC: " + name + " a incercat sa paraseasca harta!") {}
};

class InvalidActionException : public GameException {
public:
    InvalidActionException() : GameException("Actiune invalida! Comanda necunoscuta.") {}
};

// ==========================================
// 2. IERARHIE DE CLASE (ENTITĂȚI)
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

    // Metode pur virtuale
    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;
    virtual Entity* clone() const = 0;

    // Getteri
    std::string getName() const { return name; }
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
        // Player-ul consumă energie când stă pe loc in acest demo
        if (health > 0) health -= 1;
    }

    void move(int dx, int dy) {
        x += dx;
        y += dy;
        if (x < 0 || x > 100 || y < 0 || y > 100) {
            throw OutOfBoundsException(name);
        }
        std::cout << " -> " << name << " s-a mutat la (" << x << "," << y << ").\n";
    }

    void addScore(int points) { score += points; }

    // Funcție specifică (pentru dynamic_cast)
    void heal() {
        health = 100;
        std::cout << " -> " << name << " a folosit o potiune! HP Full.\n";
    }

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
        // AI Simplu: se mișcă random
        x += (rand() % 3) - 1;
        y += (rand() % 3) - 1;
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

    void update() override { /* Obstacolul e static */ }

    void printInfo(std::ostream& os) const override {
        os << "[OBSTACLE] " << (destructible ? "Destructible" : "Hard") << " Wall at " << x << "," << y;
    }

    Entity* clone() const override { return new Obstacle(*this); }
};

// ==========================================
// 3. MANAGER (LEVEL)
// ==========================================
class LevelManager {
    std::vector<Entity*> entities;

public:
    LevelManager() = default;

    // Regula celor 3
    ~LevelManager() {
        for (auto e : entities) delete e;
        entities.clear();
    }

    LevelManager(const LevelManager& other) {
        for (const auto& e : other.entities) {
            entities.push_back(e->clone());
        }
    }

    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        return *this;
    }

    void addEntity(Entity* e) {
        entities.push_back(e);
    }

    void updateAll() {
        std::cout << "\n--- Update Frame ---\n";
        for (auto e : entities) {
            e->update(); // Polimorfism
            std::cout << *e << "\n";
        }
    }

    // Funcție care returnează pointer la Player (dacă există)
    Player* getPlayer() {
        for (auto e : entities) {
            // RTTI - Dynamic Cast
            if (Player* p = dynamic_cast<Player*>(e)) {
                return p;
            }
        }
        return nullptr;
    }
};

// ==========================================
// 4. APLICAȚIA PRINCIPALĂ (GAME APP)
// ==========================================
class GameApp {
    LevelManager level;
    bool isRunning;

public:
    GameApp() : isRunning(true) {
        // Initializare nivel
        level.addEntity(new Player("Hero", 10, 10));
        level.addEntity(new Enemy("Sparx", 50, 50, 20));
        level.addEntity(new Enemy("Qix", 80, 80, 50));
        level.addEntity(new Obstacle(20, 20, false));
    }

    void showMenu() {
        std::cout << "\n=========================\n";
        std::cout << " VOLFIED - CONSOLE EDITION \n";
        std::cout << "=========================\n";
        std::cout << "1. Misca Player (W/A/S/D)\n";
        std::cout << "2. Asteapta o tura\n";
        std::cout << "3. Foloseste Potiune (Heal)\n";
        std::cout << "4. Salveaza Jocul (Simulare Copy)\n";
        std::cout << "5. Testeaza Eroare Critica (Out of Bounds)\n";
        std::cout << "0. Iesire\n";
        std::cout << "Comanda ta: ";
    }

    void run() {
        std::cout << "Entitati initiale: " << Entity::getCount() << "\n";

        while (isRunning) {
            level.updateAll();
            showMenu();

            char cmd;
            std::cin >> cmd; // Aici va citi din tastatura.txt cand ruleaza pe GitHub Actions

            try {
                Player* p = level.getPlayer();
                if (!p) throw GameException("Player-ul a murit sau nu exista!");

                switch (cmd) {
                    case '1': {
                        std::cout << "Directie (w/a/s/d): ";
                        char dir;
                        std::cin >> dir;
                        if (dir == 'w') p->move(0, -10);
                        else if (dir == 's') p->move(0, 10);
                        else if (dir == 'a') p->move(-10, 0);
                        else if (dir == 'd') p->move(10, 0);
                        else std::cout << "Directie invalida, stai pe loc.\n";
                        break;
                    }
                    case '2':
                        std::cout << "Astepti...\n";
                        p->addScore(10); // Primesti puncte pentru supravietuire
                        break;
                    case '3':
                        p->heal();
                        break;
                    case '4': {
                        std::cout << "Se salveaza jocul...\n";
                        LevelManager savedGame = level; // Test Copy Constructor
                        std::cout << "Joc salvat cu succes! (Obiecte duplicate in memorie)\n";
                        std::cout << "Total entitati acum: " << Entity::getCount() << "\n";
                        break; // savedGame se distruge aici (Test Destructor)
                    }
                    case '5':
                        std::cout << "Teleportare in afara hartii...\n";
                        p->move(1000, 1000); // Arunca OutOfBoundsException
                        break;
                    case '0':
                        isRunning = false;
                        std::cout << "La revedere!\n";
                        break;
                    default:
                        throw InvalidActionException();
                }
            }
            catch (const OutOfBoundsException& e) {
                std::cout << "\n!!! GAME OVER !!!\n" << e.what() << "\n";
                std::cout << "Se reseteaza pozitia player-ului la centru...\n";
                // Putem recupera eroarea
                if (Player* p = level.getPlayer()) {
                    p->move(-1000, -1000); // Hacky reset
                    p->move(50-p->getPosition().x, 50-p->getPosition().y); // Back to center (aprox)
                }
            }
            catch (const GameException& e) {
                std::cout << "\n[EROARE] " << e.what() << "\n";
            }
            catch (...) {
                std::cout << "\n[EROARE NECUNOSCUTA] Ceva rau s-a intamplat.\n";
            }
        }
    }
};

// ==========================================
// MAIN
// ==========================================
int main() {
    // Putem pune un mic try-catch si in main pentru siguranta maxima
    try {
        GameApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
