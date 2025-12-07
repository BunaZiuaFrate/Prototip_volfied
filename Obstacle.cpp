#include "Obstacle.h"

Obstacle::Obstacle(int x, int y, bool dest) 
    : Entity("Wall", x, y), destructible(dest) {}

void Obstacle::update() { /* Static */ }

void Obstacle::printInfo(std::ostream& os) const {
    os << "[OBSTACLE] " << (destructible ? "Destructible" : "Hard") << " Wall at " << x << "," << y;
}

Entity* Obstacle::clone() const { return new Obstacle(*this); }