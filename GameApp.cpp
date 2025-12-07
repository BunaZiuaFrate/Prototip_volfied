#include "GameApp.h"
#include "Player.h"
#include "Enemy.h"
#include "Obstacle.h"
#include "GameExceptions.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

GameApp::GameApp() : isRunning(true) {
    std::srand(std::time(nullptr));
    level.addEntity(new Player("Hero", 50, 50));
    level.addEntity(new Enemy("Sparx", 10, 10, 20));
    level.addEntity(new Enemy("Qix", 80, 80, 50));
    level.addEntity(new Obstacle(20, 20, false));
}

void GameApp::showMenu() {
    std::cout << "\n=== VOLFIED (M2) ===\n";
    std::cout << "1. Misca Player\n2. Asteapta\n3. Heal\n4. Save Game\n5. Test Error\n0. Exit\nCmd: ";
}

void GameApp::run() {
    while (isRunning) {
        level.updateAll();
        showMenu();

        char cmd;
        if (!(std::cin >> cmd)) { 
            isRunning = false; 
            break; 
        }

        try {
            Player* p = level.getPlayer();
            if (!p) throw GameException("Player missing!");

            switch (cmd) {
                case '1': {
                    std::cout << "Dir (w/a/s/d): ";
                    char dir; std::cin >> dir;
                    if (dir == 'w') p->move(0, -10);
                    else if (dir == 's') p->move(0, 10);
                    else if (dir == 'a') p->move(-10, 0);
                    else if (dir == 'd') p->move(10, 0);
                    else std::cout << "Invalid dir.\n";
                    break;
                }
                case '2': p->addScore(10); break;
                case '3': p->heal(); break;
                case '4': {
                    LevelManager saved = level;
                    std::cout << "Game Saved! (RAM entities: " << Entity::getCount() << ")\n";
                    break;
                }
                case '5': p->move(1000, 1000); break;
                case '0': isRunning = false; break;
                default: throw InvalidActionException();
            }
        }
        catch (const OutOfBoundsException& e) {
            std::cout << "\n[GAME OVER] " << e.what() << "\nResetting...\n";
            if (Player* p = level.getPlayer()) {
                Point pos = p->getPosition();
                p->move(50 - pos.x, 50 - pos.y);
            }
        }
        catch (const std::exception& e) {
            std::cout << "\n[ERROR] " << e.what() << "\n";
        }
    }
}