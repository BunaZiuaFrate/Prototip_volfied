#include "LevelManager.h"
#include <iostream>
#include <algorithm> // swap

LevelManager::~LevelManager() {
    clear();
}

LevelManager::LevelManager(const LevelManager& other) {
    for (const auto& e : other.entities) {
        entities.push_back(e->clone());
    }
    std::cout << "[DEBUG] LevelManager Deep Copy.\n";
}

LevelManager& LevelManager::operator=(LevelManager other) {
    std::swap(entities, other.entities);
    return *this;
}

void LevelManager::addEntity(Entity* e) {
    entities.push_back(e);
}

void LevelManager::clear() {
    for (auto e : entities) delete e;
    entities.clear();
}

void LevelManager::updateAll() {
    std::cout << "\n--- Update Frame ---\n";
    for (auto e : entities) {
        e->update();
        std::cout << *e << "\n";
    }
}

Player* LevelManager::getPlayer() {
    for (auto e : entities) {
        if (Player* p = dynamic_cast<Player*>(e)) return p;
    }
    return nullptr;
}