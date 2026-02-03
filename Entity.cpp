#include "Entity.h"

int Entity::entityCount = 0;

Entity::Entity(const std::string& n, int startX, int startY) 
    : x(startX), y(startY), name(n) {
    entityCount++;
}

Entity::~Entity() {
    entityCount--;
}

std::string Entity::getName() const { return name; }
int Entity::getX() const { return x; }
int Entity::getY() const { return y; }
int Entity::getCount() { return entityCount; }

std::ostream& operator<<(std::ostream& os, const Entity& e) {
    e.printInfo(os);
    return os;
}