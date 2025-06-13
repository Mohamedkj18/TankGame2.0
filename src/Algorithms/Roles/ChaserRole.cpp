#include "Algorithms/Roles/ChaserRole.h"
#include "Algorithms/MyTankAlgorithm.h"

std::vector<std::pair<int, int>> ChaserRole::prepareActions(MyTankAlgorithm &algo)
{
    nextMoves.clear();

    std::pair<int, int> myPos = algo.getCurrentPosition();
    Direction currentDirection = algo.getCurrentDirection();
    std::pair<int, int> target = algo.getTargetForTank();
    std::vector<std::pair<int, int>> path = algo.getPath(myPos, target, algo.getBannedPositionsForTank());
    algo.setBFSPath(path);

    if (path.empty())
    {
        nextMoves.push_back(ActionRequest::GetBattleInfo);
        algo.setNextMoves(nextMoves);
        return path;
    }

    int step = 0;
    std::pair<int, int> currentPos = myPos;

    for (const auto &pathStep : path)
    {
        if (step >= maxMovesPerUpdate)
            break;

        // Determine the direction to this path step
        Direction desiredDir = getDirectionFromPosition(currentPos, pathStep);

        // If not facing desired direction, rotate first
        if (currentDirection != desiredDir)
        {
            step = rotateTowards(currentDirection, desiredDir, step);
            if (step >= maxMovesPerUpdate)
                break;
        }

        // Check for shooting opportunity at this direction
        if (algo.shouldShoot(currentDirection, pathStep))
        {
            nextMoves.push_back(ActionRequest::Shoot);
            step++;
            if (step >= maxMovesPerUpdate)
                break;
            else
                nextMoves.push_back(ActionRequest::GetBattleInfo);
        }

        // Move forward toward target
        nextMoves.push_back(ActionRequest::MoveForward);
        currentPos = pathStep;
        step++;

        if (step >= maxMovesPerUpdate)
            break;
    }

    // If no moves planned, do nothing
    if (nextMoves.empty())
        nextMoves.push_back(ActionRequest::DoNothing);

    algo.setCurrentDirection(currentDirection);
    algo.setCurrentPosition(currentPos);
    algo.setNextMoves(nextMoves);
    return path;
}
