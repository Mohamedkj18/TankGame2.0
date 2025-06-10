#pragma once

#include <string>
#include "Common/ActionRequest.h"
#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"

class MyTankAlgorithm;

class Role
{

public:
    virtual ~Role() = default;
    Role(int maxMovesPerUpdate, Direction initialDirection, std::pair<int, int> initialPosition, int gameWidth = 0, int gameHeight = 0)
        : maxMovesPerUpdate(maxMovesPerUpdate), currentDirection(initialDirection), currentPosition(initialPosition), gameWidth(gameWidth), gameHeight(gameHeight) {}
    virtual std::vector<std::pair<int, int>> prepareActions(MyTankAlgorithm &algo) = 0;

    virtual std::string getRoleName() const = 0;
    virtual std::unique_ptr<Role> clone() const = 0;

    ActionRequest getNextAction();

protected:
    int maxMovesPerUpdate;
    Direction currentDirection;
    std::vector<ActionRequest> nextMoves;
    std::pair<int, int> currentPosition;
    int gameWidth;
    int gameHeight;

    int rotateTowards(Direction desiredDir, int step);
    Direction getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target);
    double getAngleFromDirections(Direction &directionStr, Direction &desiredDir);
};
