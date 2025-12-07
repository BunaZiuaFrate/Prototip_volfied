#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // swap
#include <cstdlib>   // rand
#include <ctime>     // time

// ==========================================
// 1. IERARHIE DE EXCEPȚII [MILESTONE 2]
// ==========================================

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
    explicit OutOfBoundsException(const std::string& entityName)
        : GameException("CRITIC: " + entityName + " a incercat sa iasa din harta!") {}
};

// ==========================================
// 2. CLASA DE BAZĂ ABSTRACTĂ [MILESTONE 2]
// ==========================================

class Entity {
protected:
    int x, y;
    std::string name;

    // [MILESTONE 2] Membru Static
    static int entityCount;

public:
    Entity(const std::string& n, int startX, int startY)
        : name(n), x(startX), y(startY) {
        entityCount++;
    }

    // [MILESTONE 2] Destructor Virtual (Obligatoriu la moștenire)
    virtual ~Entity() {
        entityCount--;
    }

    // [MILESTONE 2] Metode Virtuale Pure (Interfață)
    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;

    // [MILESTONE 2] Constructor Virtual (Clone Pattern)
    // Esențial pentru Deep Copy (Regula celor 3)
    virtual Entity* clone() const = 0;

    // Getteri clasici (fără template momentan)
    std::string getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }

    static int getCount() { return entityCount; }

    // Supraincarcare operator <<
    friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
        e.printInfo(os);
        return os;
    }
};

int Entity::entityCount = 0;

// ==========================================
// 3. CLASE DERIVATE [MILESTONE 2]
// ==========================================

class Player : public Entity {
    int health;
    int score;
public:
    Player(const std::string& n, int x, int y)
        : Entity(n, x, y), health(100), score(0) {}

    void update() override {
        if (health > 0) health -= 1;
    }

    // Această metodă ar putea folosi Template în M3 (pentru coordonate float/double)
    // Dar momentan rămânem la int pentru M1/M2
    void move(int dx, int dy) {
        x += dx;
        y += dy;

        // Verificare limite (fără Singleton config momentan, hardcodat 100)
        if (x < 0 || x > 100 || y < 0 || y > 100) {
            throw OutOfBoundsException(name);
        }
    }

    void heal() { health = 100; std::cout << " -> Player healed.\n"; }
    void addScore(int s) { score += s; }

    void printInfo(std::ostream& os) const override {
        os << "[Player] " << name << " HP:" << health << " Score:" << score
           << " Pos:(" << x << "," << y << ")";
    }

    Entity* clone() const override { return new Player(*this); }
};

class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg)
        : Entity(n, x, y), damage(dmg) {}

    void update() override {
        // AI simplu
        int moveX = (rand() % 3) - 1;
        int moveY = (rand() % 3) - 1;

        // Verificare simpla fara exceptii
        if (x + moveX >= 0 && x + moveX <= 100) x += moveX;
        if (y + moveY >= 0 && y + moveY <= 100) y += moveY;
    }

    void printInfo(std::ostream& os) const override {
        os << "[Enemy] " << name << " Dmg:" << damage << " Pos:(" << x << "," << y << ")";
    }

    Entity* clone() const override { return new Enemy(*this); }
};

class Obstacle : public Entity {
public:
    Obstacle(int x, int y) : Entity("Wall", x, y) {}

    void update() override { /* Static */ }

    void printInfo(std::ostream& os) const override {
        os << "[Obstacle] Wall at (" << x << "," << y << ")";
    }

    Entity* clone() const override { return new Obstacle(*this); }
};

// ==========================================
// 4. MANAGER (GESTIUNE MEMORIE) [MILESTONE 1 & 2]
// ==========================================

class Level {
    // Aici folosim vector de pointeri Entity* (Polimorfism)
    std::vector<Entity*> entities;

public:
    Level() = default;

    // --- [MILESTONE 1 & 2] REGULA CELOR 3 ---
    // (Avem pointeri raw, deci trebuie să gestionăm manual memoria)

    // 1. Destructor
    ~Level() {
        for (auto e : entities) delete e;
        entities.clear();
    }

    // 2. Copy Constructor (Deep Copy)
    Level(const Level& other) {
        for (const auto& e : other.entities) {
            entities.push_back(e->clone()); // Aici e cheia polimorfismului
        }
        std::cout << "[DEBUG] Level copied via Deep Copy.\n";
    }

    // 3. Assignment Operator (Copy-and-Swap)
    Level& operator=(Level other) {
        std::swap(entities, other.entities);
        return *this;
    }
    // ----------------------------------------

    // Aici nu folosim încă Factory Pattern (M3).
    // Primim pointeri creați cu "new" din main.
    void addEntity(Entity* e) {
        entities.push_back(e);
    }

    void updateAll() {
        std::cout << "\n--- Level Update ---\n";
        for (auto e : entities) {
            e->update();
            std::cout << *e << "\n";
        }
    }

    // Funcție helper pentru RTTI
    Player* getPlayer() {
        for (auto e : entities) {
            // [MILESTONE 2] dynamic_cast (RTTI)
            if (Player* p = dynamic_cast<Player*>(e)) {
                return p;
            }
        }
        return nullptr;
    }

    // Funcție pentru a demonstra const correctness
    int getEntityCount() const {
        return entities.size();
    }
};

// ==========================================
// 5. APLICAȚIA (MAIN LOOP)
// ==========================================

class GameApp {
    Level currentLevel;
    bool running;

public:
    GameApp() : running(true) {
        srand(time(nullptr));

        // [EXPANDABLE] Aici cream obiectele manual.
        // Pentru Milestone 3, vom inlocui aceste linii cu EntityFactory::create(...)
        currentLevel.addEntity(new Player("Hero", 50, 50));
        currentLevel.addEntity(new Enemy("Sparx", 10, 10, 15));
        currentLevel.addEntity(new Enemy("Qix", 80, 80, 30));
        currentLevel.addEntity(new Obstacle(20, 20));
    }

    void showMenu() {
        std::cout << "\n=== VOLFIED (M1/M2) ===\n";
        std::cout << "1. Move Player\n";
        std::cout << "2. Wait\n";
        std::cout << "3. Heal (RTTI Test)\n";
        std::cout << "4. Save Game (Copy Ctor Test)\n";
        std::cout << "5. Trigger Error\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
    }

    void run() {
        std::cout << "Active Entities: " << Entity::getCount() << "\n";

        while (running) {
            currentLevel.updateAll();
            showMenu();

            char cmd;
            std::cin >> cmd;

            try {
                Player* p = currentLevel.getPlayer();
                if (!p) throw GameException("Player died or missing!");

                switch(cmd) {
                    case '1': {
                        // [EXPANDABLE] Aici am putea folosi o functie template
                        // in viitor: move<int>(...)
                        std::cout << "Dir (w/a/s/d): ";
                        char d; std::cin >> d;
                        if (d == 'w') p->move(0, -10);
                        else if (d == 's') p->move(0, 10);
                        else if (d == 'a') p->move(-10, 0);
                        else if (d == 'd') p->move(10, 0);
                        break;
                    }
                    case '2':
                        p->addScore(10);
                        break;
                    case '3':
                        // Testam RTTI
                        if (p) p->heal();
                        break;
                    case '4': {
                        std::cout << "Saving...\n";
                        Level saved = currentLevel; // Testam Copy Constructor
                        std::cout << "Saved! (Temp copy destroyed after scope)\n";
                        break;
                    }
                    case '5':
                        p->move(1000, 1000); // Va arunca exceptie
                        break;
                    case '0':
                        running = false;
                        break;
                    default:
                        std::cout << "Invalid command.\n";
                }
            }
            catch (const OutOfBoundsException& ex) {
                std::cout << "\n[GAME OVER] " << ex.what() << "\n";
                // Resetare simpla pentru demo
                if (Player* p = currentLevel.getPlayer()) {
                    p->move(50 - p->getX(), 50 - p->getY());
                }
            }
            catch (const GameException& ex) {
                std::cout << "\n[ERROR] " << ex.what() << "\n";
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