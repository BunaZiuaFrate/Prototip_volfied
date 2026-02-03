#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <vector>
#include <memory>
#include <string> // Necesara pentru string
#include "Entity.h"
#include "Player.h"
#include "Utils.h"

class LevelManager {
    std::vector<std::shared_ptr<Entity>> entities;

    // [CERINTA TEMA 3] Template Class - Instantierea 1 (int)
    LootBox<int> bonusPointsBox;

    // [CERINTA TEMA 3] Template Class - Instantierea 2 (string)
    LootBox<std::string> secretMessageBox;

public:
    LevelManager();
    ~LevelManager() = default;

    LevelManager(const LevelManager& other);
    LevelManager& operator=(LevelManager other);

    void addEntity(const std::shared_ptr<Entity>& e);
    void updateAll();
    void triggerSpecialEvent();

    Player* getPlayerRaw() const;
    void openLootBox();

    friend void swap(LevelManager& first, LevelManager& second);
};

#endif //LEVELMANAGER_H