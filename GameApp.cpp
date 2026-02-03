#include "GameApp.h"
#include "GameExceptions.h"
#include "EntityFactory.h" // [TEMA 3] Include Factory
#include "Player.h"
#include <iostream>
#include <ctime>

GameApp::GameApp() : isRunning(true) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // [TEMA 3] Folosim Factory Pattern pentru a crea obiectele
    level.addEntity(EntityFactory::createEntity(EntityFactory::Type::Player, 50, 50));
    level.addEntity(EntityFactory::createEntity(EntityFactory::Type::EnemySparx, 10, 10));
    level.addEntity(EntityFactory::createEntity(EntityFactory::Type::EnemyQix, 80, 80));
    level.addEntity(EntityFactory::createEntity(EntityFactory::Type::Obstacle, 20, 20));
}

void GameApp::showMenu() const {
    std::cout << "\n=== VOLFIED (M3 - Factory & Templates) ===\n";
    std::cout << "1. Move Player\n";
    std::cout << "2. Wait (Update World)\n";
    std::cout << "3. Special Event (Dynamic Cast)\n";
    std::cout << "4. Save Game (Deep Copy)\n";
    std::cout << "5. Test Error (Exception)\n";
    std::cout << "6. LootBox (Template Instantiations)\n";
    std::cout << "0. Exit\n";
    std::cout << "Cmd: ";
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
            Player* p = level.getPlayerRaw();
            if (!p) throw GameException("Player missing from game!");

            switch (cmd) {
                case '1': {
                    std::cout << "Dir (w/a/s/d): ";
                    char dir;
                    std::cin >> dir;
                    if (dir == 'w') p->move(0, -10);
                    else if (dir == 's') p->move(0, 10);
                    else if (dir == 'a') p->move(-10, 0);
                    else if (dir == 'd') p->move(10, 0);
                    else std::cout << "Directie necunoscuta.\n";
                    break;
                }
                case '2':
                    p->addScore(10);
                    break;
                case '3':
                    level.triggerSpecialEvent();
                    break;
                case '4': {
                    LevelManager saved = level;
                    std::cout << "Game Saved! (Backup created in memory)\n";
                    break;
                }
                case '5':
                    p->move(1000, 1000);
                    break;
                case '6':
                    level.openLootBox();
                    break;
                case '0':
                    isRunning = false;
                    break;
                default:
                    throw InvalidActionException();
            }
        }
        catch (const OutOfBoundsException& e) {
            std::cout << "\n[EROARE HARTA] " << e.what() << "\n";
        }
        catch (const InvalidActionException& e) {
            std::cout << "\n[EROARE INPUT] " << e.what() << "\n";
        }
        catch (const GameException& e) {
            std::cout << "\n[EROARE JOC] " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "\n[CRITICAL SYSTEM ERROR] " << e.what() << "\n";
        }
    }
}