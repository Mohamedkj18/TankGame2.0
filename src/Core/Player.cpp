#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "Core/MyPlayer.h"
#include "Core/MyBattleInfo.h"
#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells),
      player_index(player_index), playerGameWidth(x), playerGameHeight(y), max_steps(max_steps), num_shells(num_shells) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view)
{
    std::set<int> friendlyTanks, enemyTanks, mines, walls, shells;

    int myX = -1, myY = -1;
    lastSatellite.assign(playerGameHeight, std::vector<char>(playerGameWidth, ' '));
    for (int i = 0; i < static_cast<int>(playerGameHeight); ++i)
    {
        for (int j = 0; j < static_cast<int>(playerGameWidth); ++j)
        {
            char object = satellite_view.getObjectAt(2 * j, 2 * i);
            int id = bijection(j, i);
            lastSatellite[i][j] = object;
            if (object == '%')
            {
                friendlyTanks.insert(id);
                myX = 2*j;
                myY = 2*i;
            }
            else if (object == '@')
            {
                mines.insert(id);
            }
            else if (object == '#')
            {
                walls.insert(id);
            }
            else if (object == '*')
            {
                shells.insert(id);
            }
            else if (object >= '0' && object <= '9')
            {
                int tankOwner = object - '0';

                if (tankOwner == player_index)
                {
                    friendlyTanks.insert(id);
                }
                else
                {
                    enemyTanks.insert(id);
                }
            }
        }
    }

    MyBattleInfo info(playerGameWidth, playerGameHeight, friendlyTanks, enemyTanks, mines, walls, shells);

    info.setMyXPosition(myX);
    info.setMyYPosition(myY);

    MyTankAlgorithm *algo = dynamic_cast<MyTankAlgorithm *>(&tank);
    int tankId = algo ? algo->getTankId() : 0;

    std::string role = assignRole(tankId);
    info.setRole(role);

    std::pair<int, int> myPos = {myX / 2, myY / 2};
    std::pair<int, int> target = getTargetForTank(tankId);
    std::vector<std::pair<int, int>> path = getPath(myPos, target);
    info.setBFSPath(path);
    

    plannedPositions[tankId] = path;

    tank.updateBattleInfo(info);
}

void MyPlayer::updatePlannedPositions()
{
    for (auto &[_, path] : plannedPositions)
    {
        if (!path.empty())
            path.erase(path.begin());
    }
}

bool MyPlayer::isSquareValid(int x, int y, int step)
{
    if (x < 0 || y < 0 || x >= static_cast<int>(playerGameWidth) || y >= static_cast<int>(playerGameHeight))
        return false;
    char cell = lastSatellite[y][x];
    if (cell == '#' || cell == '@')
        return false;

    int idx = bijection(x, y);
    for (const auto &[_, positions] : plannedPositions)
    {
        if (step < static_cast<int>(positions.size()) &&
            bijection(positions[step].first, positions[step].second) == idx)
            return false;
    }
    return true;
}

std::pair<int, int> MyPlayer::findFirstLegalLocationToFlee(int x, int y)
{
    for (Direction dir : directions)
    {
        int nx = (x + stringToIntDirection[dir][0] + playerGameWidth) % playerGameWidth;
        int ny = (y + stringToIntDirection[dir][1] + playerGameHeight) % playerGameHeight;
        if (isSquareValid(nx, ny, 0))
            return {nx, ny};
    }
    return {x, y};
}

std::pair<int, int> MyPlayer::moveTank(std::pair<int, int> pos, Direction dir)
{
    auto offset = stringToIntDirection[dir];
    return {
        (pos.first + offset[0] + static_cast<int>(playerGameWidth)) % static_cast<int>(playerGameWidth),
        (pos.second + offset[1] + static_cast<int>(playerGameHeight)) % static_cast<int>(playerGameHeight)};
}

std::string MyPlayer::assignRole(int tankId)
{
    if (tankRoles.count(tankId))
        return tankRoles[tankId];
    std::string role = (tankId % 2 == 0) ? "chaser" : "chaser";
    tankRoles[tankId] = role;
    return role;
}

std::pair<int, int> MyPlayer::getTargetForTank(int /*tankId*/)
{
    for (int i = 0; i < static_cast<int>(playerGameWidth); ++i)
    {
        for (int j = 0; j < static_cast<int>(playerGameHeight); ++j)
        {
            char obj = lastSatellite[j][i];
            if (obj >= '0' && obj <= '9' && (obj - '0') != player_index)
                return {i, j};
        }
    }
    return {0, 0}; // fallback
}

// ------------------------ Player 1 ------------------------
std::vector<std::pair<int, int>> Player1::getPath(std::pair<int, int> start, std::pair<int, int> target)
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

// ------------------------ Player 2 ------------------------
std::vector<std::pair<int, int>> Player2::getPath(std::pair<int, int> start, std::pair<int, int> /*target*/)
{
    std::stack<std::pair<int, int>> stack;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;

    stack.push(start);
    visited.insert(start);

    std::pair<int, int> fallback = findFirstLegalLocationToFlee(start.first, start.second);

    while (!stack.empty())
    {
        auto current = stack.top();
        stack.pop();

        if (current == fallback)
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
            if (!visited.count(next) && isSquareValid(next.first, next.second, 0))
            {
                visited.insert(next);
                parent[next] = current;
                stack.push(next);
            }
        }
    }

    return {};
}
