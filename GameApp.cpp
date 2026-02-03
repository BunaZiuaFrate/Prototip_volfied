#include "GameApp.h"
#include "GameExceptions.h"
#include "Player.h" // Avem nevoie de Player pentru cast in meniu
#include "Enemy.h"
#include "Obstacle.h"
#include <iostream>
#include <ctime>    // Pentru time()

GameApp::GameApp() : isRunning(true) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Initializam nivelul cu entitati
    level.addEntity(std::make_shared<Player>("Hero", 50, 50));
    level.addEntity(std::make_shared<Enemy>("Sparx", 10, 10, 10));
    level.addEntity(std::make_shared<Obstacle>(20, 20, false));
}

void GameApp::showMenu() const {
    std::cout << "\n=== VOLFIED (Modular Version) ===\n";
    std::cout << "1. Move Player\n";
    std::cout << "2. Wait (Update World)\n";
    std::cout << "3. Special Event (Dynamic Cast)\n";
    std::cout << "4. Save Game (Deep Copy)\n";
    std::cout << "5. Test Error (Exception)\n";
    std::cout << "6. LootBox (Template)\n";
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
            // Obtinem player-ul pentru comenzi directe
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
                    // Testam Copy Constructor-ul
                    LevelManager saved = level;
                    std::cout << "Game Saved! (Backup created in memory)\n";
                    break;
                }
                case '5':
                    p->move(1000, 1000); // Arunca OutOfBoundsException
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
        // [FEEDBACK] Catch-uri specifice
        catch (const OutOfBoundsException& e) {
            std::cout << "\n[EROARE HARTA] " << e.what() << "\n";
            // Logica de resetare pozitie ar putea fi aici
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