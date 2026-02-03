#include "../Headers/Player.h"
#include "../Headers/GameExceptions.h"

Player::Player(const std::string& n, int x, int y)
    : Entity(n, x, y), health(100), score(0) {}

void Player::update() {
    // Regenerare pasiva
    if (health < 100 && health > 0) health += 1;
}

void Player::move(int dx, int dy) {
    int newX = x + dx;
    int newY = y + dy;
    const auto& config = GameConfig::getInstance();

    // Verificam limitele folosind template-ul din Utils
    if (!isWithinBounds<int>(newX, 0, config.getWidth()) ||
        !isWithinBounds<int>(newY, 0, config.getHeight())) {
        throw OutOfBoundsException(name);
        }

    x = newX;
    y = newY;
    std::cout << " -> " << name << " s-a mutat la (" << x << "," << y << ").\n";
}

void Player::addScore(int points) { score += points; }

void Player::heal(int amount) {
    health = std::min(100, health + amount);
    std::cout << " -> " << name << " a primit heal +" << amount << ".\n";
}

Position Player::getPosition() const { return {x, y}; }

void Player::printInfo(std::ostream& os) const {
    os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score;
}

std::shared_ptr<Entity> Player::clone() const {
    return std::make_shared<Player>(*this);
}