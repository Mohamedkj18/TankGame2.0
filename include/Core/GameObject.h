#pragma once
#include "utils/DirectionUtils.h"

class GameManager; // Forward declaration
// ========================= CLASS: GameObject =========================
class GameObject
{
protected:
    int x;
    int y;
    Direction direction;
    GameManager *game;
    

public:
    GameObject(int x, int y, Direction dir, GameManager *game);
    int getX();
    int getY();
    virtual ~GameObject() = default;

    Direction getDirection() { return direction; }
    virtual bool checkForAWall() = 0;
    bool moveForward();
    void updatePosition(Direction dir);
};
