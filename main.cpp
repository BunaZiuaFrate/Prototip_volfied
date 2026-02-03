#include <iostream>
#include <ctime>
#include "LevelManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Obstacle.h"
#include "GameExceptions.h"

void showMenu() {
    std::cout << "\n=== VOLFIED ===\n";
    std::cout << "1. Move\n2. Wait\n3. Special Event (Dynamic Cast)\n4. Save (Deep Copy)\n5. Error Test\n6. LootBox\n0. Exit\nCmd: ";
}

int main() {
    std::srand(std::time(nullptr));

    LevelManager level;
    // Adaugam entitati folosind make_shared (Smart Pointers)
    level.addEntity(std::make_shared<Player>("Hero", 50, 50));
    level.addEntity(std::make_shared<Enemy>("Sparx", 10, 10, 10));
    level.addEntity(std::make_shared<Obstacle>(20, 20, false));

    bool isRunning = true;
    while (isRunning) {
        level.updateAll();
        showMenu();

        char cmd;
        if (!(std::cin >> cmd)) break;

        // [FEEDBACK] Tratarea separata a exceptiilor
        try {
            Player* p = level.getPlayerRaw();
            if (!p) throw GameException("Player not found!");

            switch (cmd) {
                case '1': {
                    std::cout << "Dir (w/a/s/d): ";
                    char dir; std::cin >> dir;
                    if (dir == 'w') p->move(0, -10);
                    else if (dir == 's') p->move(0, 10);
                    else if (dir == 'a') p->move(-10, 0);
                    else if (dir == 'd') p->move(10, 0);
                    break;
                }
                case '2': p->addScore(10); break;
                case '3': level.triggerSpecialEvent(); break;
                case '4': {
                    LevelManager saved = level;
                    std::cout << "Game Saved (Deep Copy Created)!\n";
                    break;
                }
                case '5': p->move(1000, 1000); break; // Provoaca eroare
                case '6': level.openLootBox(); break;
                case '0': isRunning = false; break;
                default: throw InvalidActionException();
            }
        }
        catch (const OutOfBoundsException& e) {
            std::cout << "\n[HARTA] " << e.what() << "\n";
            // Reset position logic...
        }
        catch (const InvalidActionException& e) {
            std::cout << "\n[INPUT] " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "\n[GENERIC] " << e.what() << "\n";
        }
    }
    return 0;
}/// pentru test