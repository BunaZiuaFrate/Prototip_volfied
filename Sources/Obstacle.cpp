#include "../Headers/Obstacle.h"

Obstacle::Obstacle(int x, int y, bool dest) 
    : Entity("Wall", x, y), destructible(dest) {}

void Obstacle::update() {
    // Obstacolele sunt statice, nu fac nimic
}

void Obstacle::printInfo(std::ostream& os) const {
    os << "[OBSTACLE] " << (destructible ? "Destructible" : "Hard") << " Wall at " << x << "," << y;
}

std::shared_ptr<Entity> Obstacle::clone() const {
    return std::make_shared<Obstacle>(*this);
}