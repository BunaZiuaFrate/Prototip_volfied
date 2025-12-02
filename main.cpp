#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // Pentru std::swap
#include <cstdlib>   // Pentru rand()

// ==========================================
// 0. STRUCTURI UTILITARE
// ==========================================
struct Position {
    int x;
    int y;
};

// ==========================================
// 1. IERARHIE DE EXCEPȚII [CERINȚĂ TEMA 2]
// ==========================================

// Clasa de bază pentru excepții proprii
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
// 2. IERARHIE DE CLASE (ENTITĂȚI) [CERINȚĂ TEMA 2]
// ==========================================

// Clasa Abstractă de Bază
class Entity {
protected:
    int x, y;
    std::string name;

    // [STATIC] Contor pentru toate entitățile
    static int entityCount;

public:
    Entity(const std::string& n, int startX, int startY) : name(n), x(startX), y(startY) {
        entityCount++;
    }

    // [VIRTUAL] Destructor virtual obligatoriu
    virtual ~Entity() { entityCount--; }

    // [VIRTUAL PUR] Metode ce trebuie implementate de clasele derivate
    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;
    virtual Entity* clone() const = 0; // Pentru Deep Copy

    // Getteri comuni
    std::string getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }

    // Metodă statică
    static int getCount() { return entityCount; }

    // Operator << polimorfic
    friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
        e.printInfo(os);
        return os;
    }
};

// Inițializare membru static
int Entity::entityCount = 0;

// --- CLASA DERIVATĂ 1: PLAYER ---
class Player : public Entity {
    int health;
    int score;
public:
    Player(const std::string& n, int x, int y) : Entity(n, x, y), health(100), score(0) {}

    // Implementare metodă virtuală
    void update() override {
        if (health > 0) health -= 1; // Consumă energie în timp
    }

    // Funcție specifică (nu e virtuală, e doar a Player-ului)
    void move(int dx, int dy) {
        x += dx;
        y += dy;

        // Verificare limite hartă (Aruncă excepție)
        if (x < 0 || x > 100 || y < 0 || y > 100) {
            throw OutOfBoundsException(name);
        }
        std::cout << " -> " << name << " s-a mutat la (" << x << "," << y << ").\n";
    }

    void addScore(int points) { score += points; }

    // Funcție specifică pentru RTTI
    void heal() {
        health = 100;
        std::cout << " -> " << name << " a folosit o potiune! HP Full.\n";
    }

    // [FIX] Getter-ul care lipsea și cauza eroarea
    Position getPosition() const {
        return {x, y};
    }

    void printInfo(std::ostream& os) const override {
        os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score << " | Pos: " << x << "," << y;
    }

    // Clone pentru copiere corectă
    Entity* clone() const override { return new Player(*this); }
};

// --- CLASA DERIVATĂ 2: ENEMY ---
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

// --- CLASA DERIVATĂ 3: OBSTACLE ---
class Obstacle : public Entity {
    bool destructible;
public:
    Obstacle(int x, int y, bool dest) : Entity("Wall", x, y), destructible(dest) {}

    void update() override { /* Obstacolul e static, nu face nimic la update */ }

    void printInfo(std::ostream& os) const override {
        os << "[OBSTACLE] " << (destructible ? "Destructible" : "Hard") << " Wall at " << x << "," << y;
    }

    Entity* clone() const override { return new Obstacle(*this); }
};

// ==========================================
// 3. MANAGER (LEVEL) - GESTIUNE MEMORIE
// ==========================================
class LevelManager {
    // Vector polimorfic (ține pointeri la Entity)
    std::vector<Entity*> entities;

public:
    LevelManager() = default;

    // --- REGULA CELOR 3 (Copy-and-Swap Idiom) ---

    // 1. Destructor
    ~LevelManager() {
        for (auto e : entities) delete e;
        entities.clear();
    }

    // 2. Constructor de Copiere (Deep Copy)
    LevelManager(const LevelManager& other) {
        for (const auto& e : other.entities) {
            entities.push_back(e->clone()); // Folosește clone() virtual
        }
        std::cout << "[DEBUG] LevelManager copiat (Deep Copy).\n";
    }

    // 3. Operator de Atribuire
    LevelManager& operator=(LevelManager other) {
        std::swap(entities, other.entities);
        return *this;
    }
    // --------------------------------------------

    void addEntity(Entity* e) {
        entities.push_back(e);
    }

    void updateAll() {
        std::cout << "\n--- Update Frame ---\n";
        for (auto e : entities) {
            e->update(); // Apel polimorfic
            std::cout << *e << "\n";
        }
    }

    // Funcție care folosește RTTI (dynamic_cast)
    Player* getPlayer() {
        for (auto e : entities) {
            // Încercăm să convertim entitatea în Player
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
        // Initializare nivel cu diverse entități
        level.addEntity(new Player("Hero", 50, 50)); // Start la 50,50 (centru)
        level.addEntity(new Enemy("Sparx", 10, 10, 20));
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
        std::cout << "4. Salveaza Jocul (Test Copy)\n";
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
            std::cin >> cmd;

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
                        else std::cout << "Directie invalida.\n";
                        break;
                    }
                    case '2':
                        std::cout << "Astepti...\n";
                        p->addScore(10);
                        break;
                    case '3':
                        p->heal();
                        break;
                    case '4': {
                        std::cout << "Se salveaza jocul...\n";
                        LevelManager savedGame = level; // Test Copy Constructor
                        std::cout << "Joc salvat! Entitati totale in memorie: " << Entity::getCount() << "\n";
                        break; // Destructorul savedGame se apelează aici
                    }
                    case '5':
                        std::cout << "Teleportare riscanta...\n";
                        p->move(1000, 1000); // Va arunca exceptie
                        break;
                    case '0':
                        isRunning = false;
                        std::cout << "La revedere!\n";
                        break;
                    default:
                        throw InvalidActionException();
                }
            }
            // Catch specific pentru ieșirea din hartă
            catch (const OutOfBoundsException& e) {
                std::cout << "\n!!! GAME OVER !!!\n" << e.what() << "\n";
                std::cout << "Se reseteaza pozitia player-ului la centru (50, 50)...\n";

                if (Player* p = level.getPlayer()) {
                    // Resetăm poziția calculând diferența necesară
                    // Acum funcționează pentru că avem getPosition()
                    int currentX = p->getPosition().x;
                    int currentY = p->getPosition().y;
                    p->move(50 - currentX, 50 - currentY);
                }
            }
            // Catch generic pentru joc
            catch (const GameException& e) {
                std::cout << "\n[EROARE JOC] " << e.what() << "\n";
            }
            // Catch general
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
    try {
        GameApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error in main: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}