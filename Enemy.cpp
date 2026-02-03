#include "Enemy.h"
#include "Utils.h"
#include <cstdlib> // rand

Enemy::Enemy(const std::string& n, int x, int y, int dmg)
    : Entity(n, x, y), damage(dmg) {}

void Enemy::update() {
    // Miscare aleatoare
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    const auto& config = GameConfig::getInstance();

    if (isWithinBounds(x + dx, 0, config.getWidth())) x += dx;
    if (isWithinBounds(y + dy, 0, config.getHeight())) y += dy;
}

void Enemy::enrage() {
    damage += 10;
    std::cout << " -> " << name << " este FURIOS! Damage crescut la " << damage << "\n";
}

void Enemy::printInfo(std::ostream& os) const {
    os << "[ENEMY] " << name << " | DMG: " << damage << " | Pos: " << x << "," << y;
}

std::shared_ptr<Entity> Enemy::clone() const {
    return std::make_shared<Enemy>(*this);
}