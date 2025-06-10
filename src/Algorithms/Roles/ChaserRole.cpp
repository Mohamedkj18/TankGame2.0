#include "Algorithms/Roles/ChaserRole.h"
#include "Algorithms/MyTankAlgorithm.h"

std::vector<std::pair<int, int>> ChaserRole::prepareActions(MyTankAlgorithm &algo)
{
    nextMoves.clear();

    std::pair<int, int> myPos = algo.getCurrentPosition();
    Direction currentDirection = algo.getCurrentDirection();
    int maxMovesPerUpdate = algo.getMaxMovesPerUpdate();
    std::pair<int, int> target = algo.getTargetForTank();
    std::vector<std::pair<int, int>> path = algo.getPath(myPos, target);
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
            step = rotateTowards(desiredDir, step);
            if (step >= maxMovesPerUpdate)
                break;

            currentDirection = desiredDir;
        }

        // Check for shooting opportunity at this direction
        if (algo.shouldShoot())
        {
            nextMoves.push_back(ActionRequest::Shoot);
            step++;
            if (step >= maxMovesPerUpdate)
                break;
        }

        // Move forward toward target
        nextMoves.push_back(ActionRequest::MoveForward);
        step++;

        if (step >= maxMovesPerUpdate)
            break;

        currentPos = pathStep;
    }

    // If no moves planned, do nothing
    if (nextMoves.empty())
        nextMoves.push_back(ActionRequest::DoNothing);

    algo.setCurrentDirection(currentDirection);
    algo.setCurrentPosition(currentPos);
    algo.setNextMoves(nextMoves);
    return path;
}
