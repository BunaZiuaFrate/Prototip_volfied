#include "EntityFactory.h"
#include "Player.h"
#include "Enemy.h"
#include "Obstacle.h"
#include "Utils.h" // Pentru GameConfig
#include <stdexcept> // Pentru std::invalid_argument

std::shared_ptr<Entity> EntityFactory::createEntity(Type type, int x, int y) {
    switch (type) {
        case Type::Player:
            return std::make_shared<Player>("Hero", x, y);
        case Type::EnemySparx:
            // Folosim GameConfig pentru dificultate
            return std::make_shared<Enemy>("Sparx", x, y, 10 * GameConfig::getInstance().getDifficulty());
        case Type::EnemyQix:
            return std::make_shared<Enemy>("Qix", x, y, 25 * GameConfig::getInstance().getDifficulty());
        case Type::Obstacle:
            return std::make_shared<Obstacle>(x, y, false);
        default:
            throw std::invalid_argument("Tip de entitate necunoscut!");
    }
}