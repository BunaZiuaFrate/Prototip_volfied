#include "LevelManager.h"
#include "Enemy.h"
#include <iostream>

// Aici initializam ambele template-uri
LevelManager::LevelManager()
    : bonusPointsBox(500),
      secretMessageBox("SECRET: Codul Konami este Sus-Sus-Jos-Jos...") {}

LevelManager::LevelManager(const LevelManager& other)
    : bonusPointsBox(other.bonusPointsBox),
      secretMessageBox(other.secretMessageBox) { // Copiem si cutia noua
    for (const auto& e : other.entities) {
        entities.push_back(e->clone());
    }
}

void swap(LevelManager& first, LevelManager& second) {
    using std::swap;
    swap(first.entities, second.entities);
    swap(first.bonusPointsBox, second.bonusPointsBox);
    swap(first.secretMessageBox, second.secretMessageBox); // Swap si aici
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

void LevelManager::triggerSpecialEvent() {
    std::cout << "\n--- SPECIAL EVENT! ---\n";
    for (auto& e : entities) {
        if (auto p = std::dynamic_pointer_cast<Player>(e)) {
            p->heal(50);
        }
        else if (auto en = std::dynamic_pointer_cast<Enemy>(e)) {
            en->enrage();
        }
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
        // Deschidem cutia cu int
        int points = bonusPointsBox.open();
        if (points > 0) p->addScore(points);

        // Deschidem cutia cu string (Demonstratie Template #2)
        std::string msg = secretMessageBox.open();
        if (!msg.empty()) {
             std::cout << "[LORE] " << msg << "\n";
        }
    }
}