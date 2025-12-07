#pragma once
#include "LevelManager.h"

class GameApp {
    LevelManager level;
    bool isRunning;

public:
    GameApp();
    void showMenu();
    void run();
};