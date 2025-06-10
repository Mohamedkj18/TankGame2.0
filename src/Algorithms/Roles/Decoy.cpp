#include "Algorithms/Roles/DecoyRole.h"
#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"

std::vector<std::pair<int, int>> DecoyRole::prepareActions(MyTankAlgorithm &algo)
{
    nextMoves.clear();

    std::vector<std::pair<int, int>> path;
    std::pair<int, int> myPos = algo.getCurrentPosition();
    Direction currentDirection = algo.getCurrentDirection();
    int maxMovesPerUpdate = algo.getMaxMovesPerUpdate();
    int step = 0;
    bool isInOpenArea = algo.isInOpen(myPos);
    bool threatIsClose = algo.isThreatWithinRange(2); // use Manhattan distance

    if (isInOpenArea && !threatIsClose)
    {
        // Stay visible, don't move
        nextMoves.push_back(ActionRequest::GetBattleInfo);
        step++;
        path = {myPos};
    }
    else
    {
        std::pair<int, int> center = {algo.getGameWidth() / 2, algo.getGameHeight() / 2};
        std::pair<int, int> target = threatIsClose ? algo.findNearestFriendlyTank(myPos) : center;

        path = algo.getPath(myPos, target);
        nextMoves = getNextMoves(path, target, algo);
    }

    // for (const auto &move : nextMoves)
    // {
    //     std::cout << "Decoy role: Planned move: " << to_string(move) << std::endl;
    // }
    algo.setNextMoves(nextMoves);
    return path;
}

std::vector<ActionRequest> DecoyRole::getNextMoves(std::vector<std::pair<int, int>> path, std::pair<int, int> target, MyTankAlgorithm &algo)
{
    std::pair<int, int> pos = algo.getCurrentPosition();
    Direction currDir = algo.getCurrentDirection();
    int maxMovesPerUpdate = algo.getMaxMovesPerUpdate();
    int step = 0;

    for (const auto &pathStep : path)
    {
        if (step >= maxMovesPerUpdate)
            break;

        // Determine the direction to this path step
        Direction desiredDir = getDirectionFromPosition(pos, pathStep);

        // If not facing desired direction, rotate first
        if (currDir != desiredDir)
        {
            step = rotateTowards(desiredDir, step);
            if (step >= maxMovesPerUpdate)
                break;

            currDir = desiredDir;
        }
        if (step < maxMovesPerUpdate)
        {
            nextMoves.push_back(ActionRequest::MoveForward);
            step++;
        }

        pos = pathStep;
    }
    nextMoves.push_back(ActionRequest::GetBattleInfo);
    algo.setCurrentPosition(pos);
    algo.setCurrentDirection(currDir);
    return nextMoves;
}