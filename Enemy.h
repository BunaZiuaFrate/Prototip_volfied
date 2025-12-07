#pragma once
#include "Entity.h"

class Enemy : public Entity {
    int damage;
public:
    Enemy(const std::string& n, int x, int y, int dmg);
    void update() override;
    void printInfo(std::ostream& os) const override;
    Entity* clone() const override;
};