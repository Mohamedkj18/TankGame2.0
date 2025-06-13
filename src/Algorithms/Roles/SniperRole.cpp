
#include "Algorithms/Roles/SniperRole.h"
#include "Algorithms/MyTankAlgorithm.h"

std::vector<std::pair<int, int>> SniperRole::prepareActions(MyTankAlgorithm &algo)
{
    nextMoves.clear();

    Direction currentDirection = algo.getCurrentDirection();
    std::pair<int, int> pos = algo.getCurrentPosition();

    if (algo.shouldShoot(currentDirection, pos))
    {
        nextMoves.push_back(ActionRequest::Shoot);
    }
    else
    {
        nextMoves.push_back(ActionRequest::RotateRight45);
        currentDirection += 0.125;
    }

    algo.setCurrentDirection(currentDirection);
    algo.setNextMoves(nextMoves);
    return std::vector<std::pair<int, int>>{algo.getCurrentPosition()};
}