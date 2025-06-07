#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"

MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index, int numMovesPerUpdate, int range, Direction initialDirection)
    : playerId(player_index), tankId(tank_index), moveIndex(0), range(range), maxMovesPerUpdate(numMovesPerUpdate), currentDirection(initialDirection) {}

void MyTankAlgorithm::updateBattleInfo(BattleInfo &info)
{
    auto &myInfo = static_cast<MyBattleInfo &>(info);

    bfsPath = myInfo.getBFSPath();
    role = myInfo.getRole();
    threats = myInfo.getEnemyTanks();
    nearbyFriendlies = myInfo.getFriendlyTanks();

    gameWidth = myInfo.getWidth();
    gameHeight = myInfo.getHeight();
    currentPos = {myInfo.getMyXPosition(), myInfo.getMyYPosition()};

    prepareActions();
    moveIndex = 0;
}

ActionRequest MyTankAlgorithm::getAction()
{
    if (moveIndex >= (int)plannedMoves.size())
    {
        return ActionRequest::GetBattleInfo;
    }

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
        if (role == "sniper" && shouldShoot())
        {
            plannedMoves.push_back(ActionRequest::Shoot);
            steps++;
            if (steps >= maxMovesPerUpdate)
                break;
        }

        if (targetDir != currentDirStr)
        {

            steps = rotateTowards(targetDir, steps);

            currentDirStr = targetDir;
            currentDirection = stringToDirection[targetDir];
        }
        // && !isThreatAhead() && !isFriendlyTooClose()
        if (steps < maxMovesPerUpdate)
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

int MyTankAlgorithm::rotateTowards(std::string desiredDir, int step)
{
    Direction desired = stringToDirection[desiredDir];

    double angle = getAngleFromDirections(directionToString[currentDirection], desiredDir);

    if (angle == 0.125)
        plannedMoves.push_back(ActionRequest::RotateRight45);

    else if (angle == 0.25)
        plannedMoves.push_back(ActionRequest::RotateRight90);

    else if (angle == 0.375)
    {
        plannedMoves.push_back(ActionRequest::RotateRight45);
        step++;
        if (step >= maxMovesPerUpdate)
            return step;
        plannedMoves.push_back(ActionRequest::RotateRight90);
    }
    else if (angle == 0.5)
    {

        plannedMoves.push_back(ActionRequest::RotateRight90);
        step++;
        if (step >= maxMovesPerUpdate)
            return step;
        plannedMoves.push_back(ActionRequest::RotateRight90);
    }
    else if (angle == 0.625)
    {
        plannedMoves.push_back(ActionRequest::RotateLeft90);
        step++;
        if (step >= maxMovesPerUpdate)
            return step;
        plannedMoves.push_back(ActionRequest::RotateLeft45);
    }
    else if (angle == 0.75)
        plannedMoves.push_back(ActionRequest::RotateLeft90);
    else if (angle == 0.875)
        plannedMoves.push_back(ActionRequest::RotateLeft45);
    else
        plannedMoves.push_back(ActionRequest::DoNothing);

    return step++;
}

std::string MyTankAlgorithm::getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target)
{
    int xDiff = target.first - current.first;
    xDiff = xDiff > 1 || xDiff == -1 ? -1 : (xDiff + gameWidth) % gameWidth;
    int yDiff = target.second - current.second;
    yDiff = yDiff > 1 || yDiff == -1 ? -1 : (yDiff + gameHeight) % gameHeight;
    return pairToDirections[{xDiff, yDiff}];
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

double MyTankAlgorithm::getAngleFromDirections(const std::string &directionStr, const std::string &desiredDir)
{
    Direction orgDir = stringToDirection[directionStr], desDir = stringToDirection[desiredDir], dirToCheck;
    double angle, rotate = 0.125;
    for (int i = 0; i < 8; ++i)
    {
        angle = i * rotate;
        dirToCheck = orgDir;
        dirToCheck += (angle);
        if (dirToCheck == desDir){
            return angle;
    }
    }
}