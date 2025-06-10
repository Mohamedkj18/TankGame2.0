
#include "Algorithms/Roles/SniperRole.h"

std::vector<std::pair<int, int>> SniperRole::prepareActions(MyTankAlgorithm &algo)
{
    nextMoves.clear();

    Direction currentDirection = algo.getCurrentDirection();

    if (algo.shouldShoot())
    {
        nextMoves.push_back(ActionRequest::Shoot);
    }
    else
    {
        nextMoves.push_back(ActionRequest::RotateRight45);
    }

    algo.setNextMoves(nextMoves);
    return std::vector<std::pair<int, int>>{algo.getCurrentPosition()};
}