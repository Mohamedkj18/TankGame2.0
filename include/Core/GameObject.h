#pragma once
#include "Core/Direction.hpp"

class Game; // Forward declaration
// ========================= CLASS: GameObject =========================
class GameObject
{
protected:
    int x;
    int y;
    Game *game;
    Direction direction;

public:
    GameObject(int x, int y, Direction dir, Game *game) : x(x), y(y), game(game), direction(dir) {}
    int getX() { return x; }
    int getY() { return y; }
    virtual ~GameObject() = default;

    Direction getDirection() { return direction; }
    virtual bool checkForAWall() = 0;
    bool moveForward();
    void updatePosition(Direction dir);
};
