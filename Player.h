#pragma once
#include "Entity.h"
#include "Utils.h"

class Player : public Entity {
    int health;
    int score;
public:
    Player(const std::string& n, int x, int y);

    void update() override;
    void move(int dx, int dy);
    void addScore(int points);
    void heal();
    Point getPosition() const;
    void printInfo(std::ostream& os) const override;
    Entity* clone() const override;
};