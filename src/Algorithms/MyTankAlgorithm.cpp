#include "include/Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"


MyTankAlgorithm::MyTankAlgorithm(int numOfMovesPerPath, int range)
{
    this->range = range;
    this->moveToAdd = 0;
    this->moveNumToBeExecuted = numOfMovesPerPath;
    this->numOfMovesPerPath = numOfMovesPerPath;
};

std::pair<int, int> MyTankAlgorithm::move(std::pair<int, int> current, Direction direction)
{
    auto offset = stringToIntDirection[direction];
    return {(current.first + offset[0] + gameWidth) % gameWidth, (current.second + offset[1] + gameHeight) % gameHeight};
}

void MyTankAlgorithm::setTheMovesToBeExecuted(MyBattleInfo &info)
{
    int i = 0;
    moveNumToBeExecuted = 0;
    std::string directionOfMove;
    std::pair<int, int> target = {i,i};
    std::pair<int, int> start = {info.getMyXPosition() / 2, info.getMyYPosition() / 2};
    std::string directionOfTank = directionToString[currentDirection];
    std::vector<std::pair<int, int>> positionsToGetTo = getPath(start, target);
    while (moveToAdd < numOfMovesPerPath)
    {
        if (i >= (int)positionsToGetTo.size())
        {
            movesToBeExecuted[moveToAdd++] = ActionRequest::DoNothing;
            continue;
        }
        if(isTheEnemyInRange(info))movesToBeExecuted[moveToAdd++] = ActionRequest::Shoot; 
        directionOfMove = getDirectionFromPosition(start, positionsToGetTo[i]);
        if(moveToAdd < numOfMovesPerPath){
            if (directionOfMove != directionOfTank)
                getMovesToRotateTank(directionOfMove, directionOfTank);
        }
        if (moveToAdd < numOfMovesPerPath)
            movesToBeExecuted[moveToAdd++] = ActionRequest::MoveForward;
        start = positionsToGetTo[i];
        directionOfTank = directionOfMove;
        i++;
    }
    moveToAdd = 0;
}

void MyTankAlgorithm::addMoveToBeExecuted(double angle)
{
    if (angle == 0.125)
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateRight45;
    else if (angle == 0.25)
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateRight90;
    else if (angle == 0.375)
    {
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateRight90;
        if (moveToAdd >= numOfMovesPerPath)
            return;
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateRight45;
    }
    else if (angle == 0.5)
    {
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateRight90;
        if (moveToAdd >= numOfMovesPerPath)
            return;
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateRight90;
    }
    else if (angle == 0.625)
    {
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateLeft90;
        if (moveToAdd >= numOfMovesPerPath)
            return;
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateLeft45;
    }
    else if (angle == 0.75)
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateLeft90;
    else if (angle == 0.875)
        movesToBeExecuted[moveToAdd++] = ActionRequest::RotateLeft45;
}

std::string MyTankAlgorithm::getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target)
{
    int xDiff = target.first - current.first;
    xDiff = xDiff > 1 || xDiff == -1 ? -1 : (xDiff + gameWidth) % gameWidth;
    int yDiff = target.second - current.second;
    yDiff = yDiff > 1 || yDiff == -1 ? -1 : (yDiff + gameHeight) % gameHeight;
    return pairToDirections[{xDiff, yDiff}];
}

int MyTankAlgorithm::getMovesToRotateTank(std::string directionToRotateTo, std::string currentDirection)
{
    int i, movesToRotate = 0;
    double rotate = 0.125, angle;
    Direction dir = stringToDirection[currentDirection];
    Direction dirToCheck;
    for (i = 0; i < 8; i++)
    {
        angle = rotate * i;
        dirToCheck = dir;
        dirToCheck += (angle);
        if (directionToString[dirToCheck] == directionToRotateTo)
        {
            if (angle == 0.0)
                return 0;

            else if (angle == 0.125 || angle == 0.25 || angle == 0.875 || angle == 0.75)
                movesToRotate++;
            else if (angle == 0.375 || angle == 0.5 || angle == 0.625)
                movesToRotate += 2;
            addMoveToBeExecuted(angle);
            break;
        }
    }
    return movesToRotate;
}

bool MyTankAlgorithm::isThereAMineOrAWall(int x, int y,MyBattleInfo &info)
{
    return info.isMine(x,y) || info.isWall(x,y);
}

bool MyTankAlgorithm::isTheEnemyInRange(MyBattleInfo &info)
{
    int i, width, height;
    std::pair<int, int> start = {info.getMyXPosition() / 2, info.getMyYPosition() / 2};
    std::array<int, 2> offset = stringToIntDirection[currentDirection];
    for (i = 1; i < range + 1; i++)
    {
        std::pair<int, int> newPos = {(start.first + offset[0] * i + gameWidth * i) % gameWidth, (start.second + offset[1] * i + gameHeight * i) % gameHeight};
        if (((MyBattleInfo)info).isEnemyTank(newPos.first, newPos.second))return true;
    }
    return false;
}

int MyTankAlgorithm::isTheSquareSafe(int x, int y, int rangeToCheck, MyBattleInfo &info)
{
    int xPos, yPos, currPos;
    Shell *shell;
    for (int i = -rangeToCheck; i <= rangeToCheck; ++i)
    {
        for (Direction dir : directions)
        {
            xPos = (x + i * (stringToIntDirection[dir][0] + gameWidth)) % gameWidth;
            yPos = (y + i * (stringToIntDirection[dir][1] + gameHeight)) % gameHeight;
            currPos = bijection(2 * xPos, 2 * yPos);
            if (info.isShell(xPos, yPos))return currPos;
        }
    }
    return -1;
}

void MyTankAlgorithm::updateBattleInfo(BattleInfo &info)
{
    gameWidth = static_cast<MyBattleInfo&>(info).getWidth();
    gameHeight = static_cast<MyBattleInfo&>(info).getHeight();
    setTheMovesToBeExecuted(static_cast<MyBattleInfo&>(info));
}

ActionRequest MyTankAlgorithm::getAction()
{
    if (moveNumToBeExecuted == -1)return ActionRequest::GetBattleInfo;
    if(numOfMovesPerPath == moveNumToBeExecuted){
        moveNumToBeExecuted = 0;
        return ActionRequest::GetBattleInfo;
    }
    return movesToBeExecuted[moveNumToBeExecuted++];
}



std::vector<std::pair<int,int>> MyTankAlgorithm::getPathToClosestTarget(
    std::pair<int,int> start,
    const std::vector<std::pair<int,int>>& targets, MyBattleInfo &info) 
{
    std::unordered_set<std::pair<int, int>, pair_hash> targetSet(targets.begin(), targets.end());
    std::queue<std::pair<int, int>> queue;
    std::unordered_map<std::pair<int, int>, bool, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    queue.push(start);
    visited[start] = true;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        if (targetSet.find(current) != targetSet.end()) {
            std::vector<std::pair<int,int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& dir : directions) {
            auto next = move(current, dir);
            if (!visited[next] && !isThereAMineOrAWall(next.first, next.second,info)) {
                visited[next] = true;
                parent[next] = current;
                queue.push(next);
            }
        }
    }

    return {}; // No path to any target
}


