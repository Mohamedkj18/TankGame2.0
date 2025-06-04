#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"

MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index, int numMovesPerUpdate, int range)
    : playerId(player_index), tankId(tank_index), moveIndex(0), range(range), maxMovesPerUpdate(numMovesPerUpdate) {}

void MyTankAlgorithm::updateBattleInfo(BattleInfo &info)
{
    auto &myInfo = static_cast<MyBattleInfo &>(info);

    bfsPath = myInfo.getBFSPath();
    role = myInfo.getRole();
    threats = myInfo.getEnemyTanks();
    nearbyFriendlies = myInfo.getFriendlyTanks();

    gameWidth = myInfo.getWidth();
    gameHeight = myInfo.getHeight();
    currentPos = {myInfo.getMyXPosition() / 2, myInfo.getMyYPosition() / 2};
    currentDirection = myInfo.getMyDirection();

    prepareActions();
    moveIndex = 0;
}

ActionRequest MyTankAlgorithm::getAction()
{
    if (moveIndex >= (int)plannedMoves.size())
        return ActionRequest::GetBattleInfo;
    return plannedMoves[moveIndex++];
}

void MyTankAlgorithm::prepareActions()
{
    plannedMoves.clear();
    std::string currentDirStr = directionToString[currentDirection];
    int steps = 0;

    for (auto &nextPos : bfsPath)
    {
        if (steps >= maxMovesPerUpdate)
            break;

        std::string targetDir = getDirectionFromPosition(currentPos, nextPos);
        if (targetDir != currentDirStr)
        {
            rotateTowards(targetDir);
            currentDirStr = targetDir;
            currentDirection = stringToDirection[targetDir];
        }

        if (role == "sniper" && shouldShoot())
        {
            plannedMoves.push_back(ActionRequest::Shoot);
            steps++;
            if (steps >= maxMovesPerUpdate)
                break;
        }

        if (!isThreatAhead() && !isFriendlyTooClose())
        {
            plannedMoves.push_back(ActionRequest::MoveForward);
            currentPos = nextPos;
            steps++;
        }
        else
        {
            plannedMoves.push_back(ActionRequest::DoNothing);
            steps++;
        }
    }

    while ((int)plannedMoves.size() < maxMovesPerUpdate)
        plannedMoves.push_back(ActionRequest::DoNothing);
}

void MyTankAlgorithm::rotateTowards(std::string desiredDir)
{
    Direction desired = stringToDirection[desiredDir];
    double angle = (desired - currentDirection + 8) % 8 * 0.125;
    if (angle == 0.125)
        plannedMoves.push_back(ActionRequest::RotateRight45);
    else if (angle == 0.25)
        plannedMoves.push_back(ActionRequest::RotateRight90);
    else if (angle == 0.875)
        plannedMoves.push_back(ActionRequest::RotateLeft45);
    else if (angle == 0.75)
        plannedMoves.push_back(ActionRequest::RotateLeft90);
    else
        plannedMoves.push_back(ActionRequest::DoNothing); // fallback
}

std::string MyTankAlgorithm::getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target)
{
    int dx = (target.first - current.first + gameWidth) % gameWidth;
    int dy = (target.second - current.second + gameHeight) % gameHeight;
    dx = (dx == gameWidth - 1) ? -1 : (dx == 1 ? 1 : 0);
    dy = (dy == gameHeight - 1) ? -1 : (dy == 1 ? 1 : 0);
    return pairToDirections[{dx, dy}];
}

std::pair<int, int> MyTankAlgorithm::move(std::pair<int, int> pos, Direction dir)
{
    auto offset = stringToIntDirection[dir];
    return {(pos.first + offset[0] + gameWidth) % gameWidth, (pos.second + offset[1] + gameHeight) % gameHeight};
}

bool MyTankAlgorithm::isThreatAhead()
{
    std::pair<int, int> front = move(currentPos, currentDirection);
    int id = bijection(front.first, front.second);
    return threats.count(id) > 0;
}

bool MyTankAlgorithm::isFriendlyTooClose()
{
    for (Direction d : directions)
    {
        std::pair<int, int> adj = move(currentPos, d);
        int id = bijection(adj.first, adj.second);
        if (nearbyFriendlies.count(id))
            return true;
    }
    return false;
}

bool MyTankAlgorithm::shouldShoot()
{
    std::pair<int, int> look = currentPos;
    for (int i = 0; i < range; ++i)
    {
        look = move(look, currentDirection);
        int id = bijection(look.first, look.second);
        if (threats.count(id))
            return false; // don't shoot toward mines
        if (nearbyFriendlies.count(id))
            return false; // don't friendly fire
    }
    return true;
}
