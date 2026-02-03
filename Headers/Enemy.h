#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg);

    void update() override;
    void printInfo(std::ostream& os) const override;
    std::shared_ptr<Entity> clone() const override;

    // Metoda specifica pentru a demonstra dynamic_cast
    void enrage();
};

#endif //ENEMY_H