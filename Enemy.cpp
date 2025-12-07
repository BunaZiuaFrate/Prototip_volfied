#include "Enemy.h"
#include <cstdlib> // rand

Enemy::Enemy(const std::string& n, int x, int y, int dmg) 
    : Entity(n, x, y), damage(dmg) {}

void Enemy::update() {
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    
    if (x + dx >= 0 && x + dx <= 100) x += dx;
    if (y + dy >= 0 && y + dy <= 100) y += dy;
}

void Enemy::printInfo(std::ostream& os) const {
    os << "[ENEMY] " << name << " | DMG: " << damage << " | Pos: " << x << "," << y;
}

Entity* Enemy::clone() const { return new Enemy(*this); }