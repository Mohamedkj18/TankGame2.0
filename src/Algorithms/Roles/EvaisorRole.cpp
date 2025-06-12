#include "Algorithms/Roles/EvasiorRole.h"

std::vector<std::pair<int, int>> EvasiorRole::prepareActions(MyTankAlgorithm &algo)
{
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> myPos = algo.getCurrentPosition();
    Direction currentDirection = algo.getCurrentDirection();
    int RANGE = 7; // 14x14 square

    std::set<std::pair<int, int>> shells = algo.getShells();
    if (shells.empty())
    {
        nextMoves.push_back(ActionRequest::GetBattleInfo);
        path = {myPos};
    }
    else
    {
        std::set<std::pair<int, int>> redZone = createRedZone(shells);
        concatenateSets(redZone, algo.getBannedPositionsForTank());
        std::optional<std::pair<int, int>> target = algo.findFirstLegalLocationToFlee(myPos, redZone);
        if (target.has_value())
        {
            path = algo.getPath(myPos, target.value(), redZone);
            nextMoves = getNextMoves(path, target.value(), algo);
        }
    }

    algo.setNextMoves(nextMoves);
    return path;
}

std::set<std::pair<int, int>> EvasiorRole::createRedZone(std::set<std::pair<int, int>> shells)
{

    std::set<std::pair<int, int>> redZone;
    for (auto pos : shells)
    {
        for (int k = 1; k < 5; k++)
        {
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (i == 0 && j == 0)
                        continue;

                    int x = (pos.first + i * k + gameWidth * k) % gameWidth;
                    int y = (pos.second + j * k + gameHeight * k) % gameHeight;
                    redZone.insert(std::make_pair(x, y));
                }
            }
        }
    }
    return redZone;
}

std::vector<ActionRequest> EvasiorRole::getNextMoves(std::vector<std::pair<int, int>> path, std::pair<int, int> target, MyTankAlgorithm &algo)
{
    std::pair<int, int> pos = algo.getCurrentPosition();
    Direction currentDirection = algo.getCurrentDirection();
    int maxMovesPerUpdate = algo.getMaxMovesPerUpdate();
    int step = 0;

    for (const auto &pathStep : path)
    {
        if (step >= maxMovesPerUpdate)
            break;

        // Determine the direction to this path step
        Direction desiredDir = getDirectionFromPosition(pos, pathStep);

        // If not facing desired direction, rotate first
        if (currentDirection != desiredDir)
        {
            step = rotateTowards(currentDirection, desiredDir, step);
            if (step >= maxMovesPerUpdate)
                break;

            currentDirection = desiredDir;
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
    algo.setCurrentDirection(currentDirection);
    return nextMoves;
}

void EvasiorRole::concatenateSets(std::set<std::pair<int, int>> targetSet, std::set<std::pair<int, int>> setToBeAdded)
{
    for (const auto pos : setToBeAdded)
    {
        targetSet.insert(pos);
    }
}
