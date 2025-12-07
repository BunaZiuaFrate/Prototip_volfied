#pragma once
#include <string>
#include <iostream>

class Entity {
protected:
    int x, y;
    std::string name;
    static int entityCount;

public:
    Entity(const std::string& n, int startX, int startY);
    virtual ~Entity();

    virtual void update() = 0;
    virtual void printInfo(std::ostream& os) const = 0;
    virtual Entity* clone() const = 0;

    std::string getName() const;
    int getX() const;
    int getY() const;

    static int getCount();

    friend std::ostream& operator<<(std::ostream& os, const Entity& e);
};