#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <memory>
#include "Entity.h"

/// @brief Design Pattern: Factory Method.
class EntityFactory {
public:
    enum class Type { Player, EnemySparx, EnemyQix, Obstacle };

    // Doar declaram functia, nu scriem codul aici
    static std::shared_ptr<Entity> createEntity(Type type, int x, int y);
};

#endif //ENTITYFACTORY_H