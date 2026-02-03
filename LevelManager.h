#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <vector>
#include <memory>
#include "Entity.h"
#include "Player.h"
#include "Utils.h"

class LevelManager {
    // [FEEDBACK] Smart Pointers in vector
    std::vector<std::shared_ptr<Entity>> entities;
    LootBox<int> bonusPointsBox;

public:
    LevelManager();
    ~LevelManager() = default; // Smart pointers curata automat memoria

    // Copy Constructor (Deep Copy)
    LevelManager(const LevelManager& other);
    LevelManager& operator=(LevelManager other);

    void addEntity(const std::shared_ptr<Entity>& e);
    void updateAll();

    // [FEEDBACK] Functie care foloseste dynamic_cast cu un scop real
    void triggerSpecialEvent();

    Player* getPlayerRaw() const;
    void openLootBox();

    friend void swap(LevelManager& first, LevelManager& second);
};

#endif //LEVELMANAGER_H