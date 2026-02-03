#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Entity.h"

class Obstacle : public Entity {
    bool destructible;
public:
    Obstacle(int x, int y, bool dest);

    // Trebuie implementata chiar daca e goala, ca sa nu fie clasa abstracta
    void update() override;
    void printInfo(std::ostream& os) const override;
    std::shared_ptr<Entity> clone() const override;
};

#endif //OBSTACLE_H