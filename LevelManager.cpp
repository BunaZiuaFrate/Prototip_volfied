#include "LevelManager.h"
#include "Enemy.h" // Necesar pentru dynamic_cast la Enemy
#include <iostream>

LevelManager::LevelManager() : bonusPointsBox(500) {}

LevelManager::LevelManager(const LevelManager& other)
    : bonusPointsBox(other.bonusPointsBox) {
    for (const auto& e : other.entities) {
        entities.push_back(e->clone());
    }
}

void swap(LevelManager& first, LevelManager& second) {
    using std::swap;
    swap(first.entities, second.entities);
    swap(first.bonusPointsBox, second.bonusPointsBox);
}

LevelManager& LevelManager::operator=(LevelManager other) {
    swap(*this, other);
    return *this;
}

void LevelManager::addEntity(const std::shared_ptr<Entity>& e) {
    entities.push_back(e);
}

void LevelManager::updateAll() {
    std::cout << "\n--- Update Frame ---\n";
    for (auto& e : entities) {
        e->update();
        std::cout << *e << "\n";
    }
}

// [FEEDBACK] Dynamic Cast cu sens
void LevelManager::triggerSpecialEvent() {
    std::cout << "\n--- SPECIAL EVENT! ---\n";
    for (auto& e : entities) {
        // Daca e Player, primeste Heal
        if (auto p = std::dynamic_pointer_cast<Player>(e)) {
            p->heal(50);
        }
        // Daca e Enemy, devine Furios (Enrage)
        else if (auto en = std::dynamic_pointer_cast<Enemy>(e)) {
            en->enrage();
        }
        // Obstacolele sunt ignorate
    }
    std::cout << "----------------------\n";
}

Player* LevelManager::getPlayerRaw() const {
    for (const auto& e : entities) {
        if (auto p = std::dynamic_pointer_cast<Player>(e)) {
            return p.get();
        }
    }
    return nullptr;
}

void LevelManager::openLootBox() {
    if (Player* p = getPlayerRaw()) {
        int points = bonusPointsBox.open();
        p->addScore(points);
    }
}