#pragma once
#include <vector>
#include "Entity.h"
#include "Player.h" // Necesar pentru getPlayer

class LevelManager {
    std::vector<Entity*> entities;

public:
    LevelManager() = default;
    ~LevelManager();
    LevelManager(const LevelManager& other);
    LevelManager& operator=(LevelManager other);

    void addEntity(Entity* e);
    void clear();
    void updateAll();
    Player* getPlayer();
};