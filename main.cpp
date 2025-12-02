#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm> // Pentru std::swap

// ==========================================
// 1. IERARHIE DE EXCEPȚII [TEMA 2]
// ==========================================

// Clasa de bază pentru excepții
class GameException : public std::exception {
protected:
    std::string message;
public:
    GameException(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

// Excepție 1: Când o entitate iese din hartă
class OutOfBoundsException : public GameException {
public:
    OutOfBoundsException(const std::string& entityName)
        : GameException("Eroare Critica: " + entityName + " a iesit din limitele hartii!") {}
};

// Excepție 2: Când încercăm să adăugăm o entitate invalidă (nullptr)
class InvalidEntityException : public GameException {
public:
    InvalidEntityException() : GameException("Eroare: S-a incercat adaugarea unei entitati NULL!") {}
};

// Excepție 3: Eroare logică generică
class LogicException : public GameException {
public:
    LogicException(const std::string& detail) : GameException("Eroare Logica: " + detail) {}
};

// ==========================================
// 2. IERARHIE DE CLASE (MOȘTENIRE & POLIMORFISM) [TEMA 2]
// ==========================================

// Clasa Abstractă de Bază
class Entity {
protected:
    int x, y;
    std::string name;

    // [TEMA 2] Atribut static (numără câte entități există în total)
    static int entityCount;

public:
    Entity(const std::string& n, int startX, int startY) : name(n), x(startX), y(startY) {
        entityCount++;
    }

    // [TEMA 2] Destructor virtual (OBLIGATORIU la moștenire)
    virtual ~Entity() {
        entityCount--;
    }

    // [TEMA 2] Funcție virtuală pură (specifică temei)
    virtual void update() = 0;

    // [TEMA 2] Funcție virtuală pentru afișare
    virtual void draw(std::ostream& os) const {
        os << "[" << name << "] Pos: (" << x << "," << y << ")";
    }

    // [TEMA 2] Constructor virtual (Clone pattern) - Necesar pentru Deep Copy
    virtual Entity* clone() const = 0;

    // Metodă statică
    static int getCount() { return entityCount; }

    std::string getName() const { return name; }

    // Operator << care apelează funcția virtuală draw
    friend std::ostream& operator<<(std::ostream& os, const Entity& e) {
        e.draw(os);
        return os;
    }
};

// Inițializare membru static
int Entity::entityCount = 0;

// --- CLASA DERIVATĂ 1: PLAYER ---
class Player : public Entity {
    int health;
public:
    Player(const std::string& n, int x, int y, int h) : Entity(n, x, y), health(h) {}

    // Implementarea metodei pure
    void update() override {
        // Simulare mișcare
        x += 1;
        y += 1;
        std::cout << " -> Player se misca la " << x << "," << y << "\n";

        // [TEMA 2] Aruncarea unei excepții
        if (x > 100 || y > 100) {
            throw OutOfBoundsException(name);
        }
    }

    void draw(std::ostream& os) const override {
        Entity::draw(os); // Apelăm afișarea din bază
        os << " | HP: " << health;
    }

    // Implementare Clone pentru Deep Copy
    Entity* clone() const override {
        return new Player(*this);
    }

    void heal() { health += 10; std::cout << " -> Player s-a vindecat!\n"; }
};

// --- CLASA DERIVATĂ 2: ENEMY ---
class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg) : Entity(n, x, y), damage(dmg) {}

    void update() override {
        x -= 1; // Inamicul merge invers
        std::cout << " -> Enemy patruleaza la " << x << "," << y << "\n";
    }

    void draw(std::ostream& os) const override {
        Entity::draw(os);
        os << " | DMG: " << damage;
    }

    Entity* clone() const override {
        return new Enemy(*this);
    }
};

// --- CLASA DERIVATĂ 3: OBSTACLE ---
class Obstacle : public Entity {
public:
    Obstacle(int x, int y) : Entity("Wall", x, y) {}

    void update() override {
        // Obstacolul nu face nimic, e static
        // Dar funcția trebuie implementată pentru că e pură în bază
    }

    Entity* clone() const override {
        return new Obstacle(*this);
    }
};

// ==========================================
// 3. CLASA MANAGER (GESTIUNE MEMORIE & RTTI)
// ==========================================

class LevelManager {
    // [TEMA 2] Atribut de tip pointer la clasa de bază (vector polimorfic)
    std::vector<Entity*> entities;

public:
    LevelManager() {}

    // [TEMA 2] Adaugare in vector
    void addEntity(Entity* e) {
        if (e == nullptr) {
            throw InvalidEntityException();
        }
        entities.push_back(e);
    }

    // --- [TEMA 2] REGULA CELOR 3 (CC, Op=, Destructor) ---

    // 1. Destructor
    ~LevelManager() {
        for (auto e : entities) {
            delete e;
        }
        entities.clear();
    }

    // 2. Constructor de Copiere (Deep Copy)
    LevelManager(const LevelManager& other) {
        for (const auto& e : other.entities) {
            // Aici folosim functia virtuala clone() pentru a copia corect tipul
            addEntity(e->clone());
        }
        std::cout << "[Manager] Copy Constructor apelat (Deep Copy)\n";
    }

    // 3. Operator de Atribuire (Copy-and-Swap Idiom)
    LevelManager& operator=(LevelManager other) { // Parametrul e primit prin valoare (copie)
        std::swap(entities, other.entities); // Facem schimb de resurse
        std::cout << "[Manager] Operator= (Swap) apelat\n";
        return *this;
    }
    // ----------------------------------------------------

    void runFrame() {
        std::cout << "\n--- Rulare Cadru (Polimorfism) ---\n";
        for (auto e : entities) {
            // Afisare polimorfica
            std::cout << *e << "\n";

            // Update polimorfic
            e->update();

            // [TEMA 2] dynamic_cast (RTTI) pentru downcast cu sens
            // Verificăm dacă entitatea curentă este Player
            Player* p = dynamic_cast<Player*>(e);
            if (p) {
                // Doar jucatorul se poate vindeca
                p->heal();
            }
        }
    }
};

// ==========================================
// MAIN (Testing & Try-Catch)
// ==========================================

int main() {
    try {
        std::cout << "Numar entitati active: " << Entity::getCount() << "\n";

        // Creăm managerul
        LevelManager level1;

        // Adăugăm entități (Upcasting implicit la Entity*)
        level1.addEntity(new Player("Hero", 10, 10, 100));
        level1.addEntity(new Enemy("Goblin", 50, 50, 15));
        level1.addEntity(new Obstacle(20, 20));

        // Testare Copy Constructor (Tema 2 - Copiere corectă)
        LevelManager level2 = level1;

        std::cout << "Numar entitati active (dupa copiere): " << Entity::getCount() << "\n";

        // Rulăm logica
        level1.runFrame();

        // [TEMA 2] Generare excepție intenționată
        std::cout << "\n--- Testare Exceptie OutOfBounds ---\n";
        Player* badPlayer = new Player("Speedster", 99, 99, 50);

        // Il fortam sa iasa din harta
        badPlayer->update(); // 100, 100
        badPlayer->update(); // 101, 101 -> Arunca exceptie!

        delete badPlayer; // (Nu ajunge aici daca da crash, dar avem try-catch)

    }
    catch (const OutOfBoundsException& e) {
        std::cout << "\n[EXCEPTIE PRINSĂ] " << e.what() << "\n";
    }
    catch (const InvalidEntityException& e) {
        std::cout << "\n[EXCEPTIE PRINSĂ] " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "\n[EXCEPTIE STANDARD] " << e.what() << "\n";
    }

    std::cout << "\nProgram terminat cu succes.\n";
    return 0;
}