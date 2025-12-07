#include "Player.h"
#include "GameExceptions.h"

Player::Player(const std::string& n, int x, int y) 
    : Entity(n, x, y), health(100), score(0) {}

void Player::update() {
    if (health > 0) health -= 1;
}

void Player::move(int dx, int dy) {
    int newX = x + dx;
    int newY = y + dy;

    if (newX < 0 || newX > 100 || newY < 0 || newY > 100) {
        throw OutOfBoundsException(name);
    }

    x = newX;
    y = newY;
    std::cout << " -> " << name << " s-a mutat la (" << x << "," << y << ").\n";
}

void Player::addScore(int points) { score += points; }

void Player::heal() {
    health = 100;
    std::cout << " -> " << name << " a folosit o potiune! HP Full.\n";
}

Point Player::getPosition() const { return {x, y}; }

void Player::printInfo(std::ostream& os) const {
    os << "[PLAYER] " << name << " | HP: " << health << " | Score: " << score << " | Pos: " << x << "," << y;
}

Entity* Player::clone() const { return new Player(*this); }