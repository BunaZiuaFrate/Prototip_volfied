#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Utils.h"

class Player : public Entity {
    int health;
    int score;
public:
    Player(const std::string& n, int x, int y);

    void update() override;
    void printInfo(std::ostream& os) const override;
    std::shared_ptr<Entity> clone() const override;

    // Metode specifice Player
    void move(int dx, int dy);
    void addScore(int points);
    void heal(int amount);
    Position getPosition() const;
};

#endif //PLAYER_H