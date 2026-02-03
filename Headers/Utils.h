#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

/// @brief Singleton pentru configuratia jocului.
class GameConfig {
    int mapWidth;
    int mapHeight;
    int difficulty;

    GameConfig() : mapWidth(100), mapHeight(100), difficulty(1) {}

public:
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    static GameConfig& getInstance() {
        static GameConfig instance;
        return instance;
    }

    // [FEEDBACK] Const correctness
    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }
    int getDifficulty() const { return difficulty; }
    void setDifficulty(int d) { difficulty = d; }
};

/// @brief Structura simpla pentru coordonate.
struct Position {
    int x, y;
};

/// @brief Template function pentru verificarea limitelor.
template <typename T>
bool isWithinBounds(const T& value, const T& min, const T& max) {
    return (value >= min && value <= max);
}

/// @brief Template class pentru LootBox.
template <typename T>
class LootBox {
    T content;
    bool opened;
public:
    explicit LootBox(const T& item) : content(item), opened(false) {}

    T open() {
        if (!opened) {
            opened = true;
            std::cout << "[LOOT] Ai deschis cutia! Continut: " << content << "\n";
        }
        return content;
    }

    bool isOpened() const { return opened; }
};

#endif //UTILS_H