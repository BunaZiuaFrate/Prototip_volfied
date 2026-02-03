#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "LevelManager.h"

/// @brief Clasa principala care controleaza bucla jocului (Game Loop).
class GameApp {
    LevelManager level;
    bool isRunning;

    void showMenu() const;

public:
    GameApp();
    void run();
};

#endif //GAMEAPP_H