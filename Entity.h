#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <iostream>
#include <memory> // [FEEDBACK] Smart Pointers

/// @brief Clasa abstracta de baza pentru toate obiectele din joc.
class Entity {
protected:
    int x, y;
    std::string name;
    static int entityCount;

public:
    // [FEEDBACK] Const reference la string
    Entity(const std::string& n, int startX, int startY);

    virtual ~Entity();

    /// @brief Metoda pur virtuala pentru actualizarea starii.
    virtual void update() = 0;

    /// @brief Metoda pur virtuala pentru afisare.
    virtual void printInfo(std::ostream& os) const = 0;

    /// @brief Returneaza un smart pointer catre o copie a obiectului.
    virtual std::shared_ptr<Entity> clone() const = 0;

    // Getteri const
    std::string getName() const;
    int getX() const;
    int getY() const;
    static int getCount();

    friend std::ostream& operator<<(std::ostream& os, const Entity& e);
};

#endif //ENTITY_H