#include <queue>
#include <memory>
#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"
#include "Algorithms/Roles/ChaserRole.h"

MyTankAlgorithm::MyTankAlgorithm(int player_index, int tank_index, int numMovesPerUpdate, int range, Direction initialDirection)
    : playerId(player_index), tankId(tank_index), moveIndex(0), range(range), maxMovesPerUpdate(numMovesPerUpdate), currentDirection(initialDirection) {}

void MyTankAlgorithm::updateBattleInfo(BattleInfo &info)
{
    auto &myInfo = static_cast<MyBattleInfo &>(info);

    if (!myInfo.getShouldKeepRole() || !role)
        setRole(std::move(myInfo.extractRole()));

    threats = myInfo.getEnemyTanks();
    nearbyFriendlies = myInfo.getFriendlyTanks();
    mines = myInfo.getMines();
    walls = myInfo.getWalls();

    gameWidth = myInfo.getWidth();
    gameHeight = myInfo.getHeight();
    currentPos = {myInfo.getMyXPosition(), myInfo.getMyYPosition()};
    std::pair<int, int> target = getTargetForTank();

    bfsPath = role->prepareActions(*this);
    myInfo.setPath(tankId, bfsPath);
    moveIndex = 0;
}

ActionRequest MyTankAlgorithm::getAction()
{
    if (!role)
    {
        return ActionRequest::GetBattleInfo;
    }

    return role->getNextAction();
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
        if (nearbyFriendlies.count(id))
            return false; // don't friendly fire
    }
    return true;
}

bool MyTankAlgorithm::isSquareValid(int x, int y, int step)
{
    if (x < 0 || y < 0 || x >= static_cast<int>(gameWidth) || y >= static_cast<int>(gameHeight))
        return false;

    int pos = bijection(x, y);
    if (mines.count(pos) > 0 || walls.count(pos) > 0)
        return false;

    return true;
}

std::pair<int, int> MyTankAlgorithm::findFirstLegalLocationToFlee(int x, int y)
{
    for (Direction dir : directions)
    {
        int nx = (x + stringToIntDirection[dir][0] + gameWidth) % gameWidth;
        int ny = (y + stringToIntDirection[dir][1] + gameHeight) % gameHeight;
        if (isSquareValid(nx, ny, 0))
            return {nx, ny};
    }
    return {x, y};
}

std::pair<int, int> MyTankAlgorithm::moveTank(std::pair<int, int> pos, Direction dir)
{
    auto offset = stringToIntDirection[dir];
    return {
        (pos.first + offset[0] + static_cast<int>(gameWidth)) % static_cast<int>(gameWidth),
        (pos.second + offset[1] + static_cast<int>(gameHeight)) % static_cast<int>(gameHeight)};
}

std::pair<int, int> MyTankAlgorithm::getTargetForTank()
{
    // In case of equal distance, prefer the one with lower rotation cost
    std::pair<int, int> myPos = currentPos;
    std::pair<int, int> bestTarget = {-1, -1};
    int minDist = INT_MAX;

    for (int y = 0; y < static_cast<int>(gameHeight); ++y)
    {
        for (int x = 0; x < static_cast<int>(gameWidth); ++x)
        {
            int pos = bijection(x, y);
            if (threats.count(pos) > 0)
            {
                int dist = manhattanDistance(myPos.first, myPos.second, x, y);

                if (dist < minDist)
                {
                    minDist = dist;
                    bestTarget = {x, y};
                }
            }
        }
    }
    return (bestTarget.first == -1) ? std::pair<int, int>{0, 0} : bestTarget;
}

std::vector<std::pair<int, int>> MyTankAlgorithm::getPath(std::pair<int, int> start, std::pair<int, int> target)
{
    std::queue<std::pair<int, int>> queue;
    std::unordered_map<std::pair<int, int>, bool, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    queue.push(start);
    visited[start] = true;

    int step = 0;

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        if (current == target)
        {
            std::vector<std::pair<int, int>> path;
            while (current != start)
            {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());

            return path;
        }

        for (const auto &dir : directions)
        {
            auto next = moveTank(current, dir);
            if (!visited[next] && isSquareValid(next.first, next.second, step))
            {
                visited[next] = true;
                parent[next] = current;
                queue.push(next);
            }
        }
        step++;
    }

    return {};
}

int MyTankAlgorithm::manhattanDistance(int x1, int y1, int x2, int y2) const
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

bool MyTankAlgorithm::isInOpen(std::pair<int, int> pos) const
{
    int wallCount = 0;
    int x = pos.first;
    int y = pos.second;

    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0)
                continue;

            int nx = (x + dx + gameWidth) % gameWidth;
            int ny = (y + dy + gameHeight) % gameHeight;

            if (walls.count(bijection(nx, ny)) > 0)
                wallCount++;
        }
    }

    return wallCount <= 3;
}

bool MyTankAlgorithm::isThreatWithinRange(int range) const
{
    for (int id : threats)
    {
        std::pair<int, int> enemyPos = inverseBijection(id);
        int dx = std::abs(enemyPos.first - currentPos.first);
        int dy = std::abs(enemyPos.second - currentPos.second);

        // Apply toroidal wrapping
        dx = std::min(dx, static_cast<int>(gameWidth) - dx);
        dy = std::min(dy, static_cast<int>(gameHeight) - dy);

        if (dx + dy <= range)
            return true;
    }
    return false;
}

std::pair<int, int> MyTankAlgorithm::findNearestFriendlyTank(std::pair<int, int> from) const
{
    int bestDist = INT_MAX;
    std::pair<int, int> bestPos = from;

    for (int id : nearbyFriendlies)
    {
        std::pair<int, int> pos = inverseBijection(id);
        int dx = std::abs(pos.first - from.first);
        int dy = std::abs(pos.second - from.second);

        // Toroidal wrapping
        dx = std::min(dx, static_cast<int>(gameWidth) - dx);
        dy = std::min(dy, static_cast<int>(gameHeight) - dy);

        int dist = dx + dy;
        if (dist < bestDist)
        {
            bestDist = dist;
            bestPos = pos;
        }
    }

    return bestPos;
}

void MyTankAlgorithm::setRole(std::unique_ptr<Role> newRole)
{
    role = std::move(newRole);
}
