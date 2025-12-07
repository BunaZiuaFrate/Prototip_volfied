#pragma once
#include "Entity.h"

class Obstacle : public Entity {
    bool destructible;
public:
    Obstacle(int x, int y, bool dest);
    void update() override;
    void printInfo(std::ostream& os) const override;
    Entity* clone() const override;
};