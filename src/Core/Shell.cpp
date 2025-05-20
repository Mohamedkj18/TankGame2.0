#include "Core/Shell.hpp"
#include "Core/Game.hpp"
#include <iostream>
#include <unordered_map>

// ------------------------ Shell ------------------------

Shell::Shell(int x, int y, Direction dir, Game *game)
    : MovingGameObject(x, y, dir, game) {}

bool Shell::checkForAWall()
{
    Wall *currWall;
    int wallPos;
    std::unordered_map<int, Wall> &walls = game->getWalls();
    wallPos = game->bijection(x, y);
    if (walls.count(wallPos))
    {
        outputFile << "Shell at " << x << ", " << y << "hit a wall!" << std::endl;
        currWall = &walls[wallPos];
        currWall->health -= 1;
        updatePosition(direction);
        return true;
    }
    return false;
}
