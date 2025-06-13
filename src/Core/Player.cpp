#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "Algorithms/Roles/ChaserRole.h"
#include "Algorithms/Roles/DecoyRole.h"
#include "Algorithms/Roles/EvasiorRole.h"
#include "Algorithms/Roles/SniperRole.h"
#include "Algorithms/Roles/DefenderRole.h"
#include "Core/MyPlayer.h"
#include "Core/MyBattleInfo.h"
#include "Algorithms/MyTankAlgorithm.h"
#include "utils/DirectionUtils.h"

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells),
      player_index(player_index), playerGameWidth(x), playerGameHeight(y), max_steps(max_steps), num_shells(num_shells) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view)
{

    std::cout << "[DEBUG PLAYER] PlayerId: " << player_index << std::endl;
    std::set<int> friendlyTanks, enemyTanks, mines, walls, shells;

    int myX = -1, myY = -1;
    std::pair<int, int> tankPos = prepareInfoForBattleInfo(mines, walls, shells, friendlyTanks, enemyTanks, satellite_view);

    myX = tankPos.first;
    myY = tankPos.second;

    MyBattleInfo info(playerGameWidth, playerGameHeight, friendlyTanks, enemyTanks, mines, walls, shells);

    info.setMyXPosition(myX);
    info.setMyYPosition(myY);

    MyTankAlgorithm *algo = dynamic_cast<MyTankAlgorithm *>(&tank);
    int tankId = algo ? algo->getTankId() : 0;
    EnemyScanResult scan = assignRole(tankId, algo->getCurrentDirection(), {myX, myY}, shells, enemyTanks);
    if (!scan.ShouldKeepRole)
    {
        info.setRole(createRole(tankId, algo->getCurrentDirection(), {myX, myY}, scan, shells, enemyTanks));
        info.setShouldKeepRole(false);
    }
    else
    {
        info.setShouldKeepRole(true);
    }

    std::set<std::pair<int, int>> planned = getCalculatedPathsSet();
    info.setPlannedPositions(planned);
    tank.updateBattleInfo(info);
    tanksPlannedActions[tankId] = info.getPlannedActions();
    tanksPlannedPaths[tankId] = info.getPath();

    if (tanksRemainingShells.count(tankId) == 0)
    {
        tanksRemainingShells[tankId] = num_shells;
    }
}

void MyPlayer::updatePlannedPaths()
{
    for (auto pair : tanksPlannedActions)
    {
        int tankId = pair.first;
        if (pair.second.empty())
        {
            continue;
        }
        ActionRequest action = pair.second.front();
        pair.second.erase(pair.second.begin());
        if (action == ActionRequest::Shoot)
        {
            tanksRemainingShells[tankId]--;
        }
        if (action == ActionRequest::MoveBackward || action == ActionRequest::MoveForward)
        {
            if (tanksPlannedPaths[tankId].empty())
            {
                continue;
            }
            tanksPlannedPaths[tankId].erase(tanksPlannedPaths[tankId].begin());
        }
    }
}

EnemyScanResult MyPlayer::assignRole(int tankId, Direction currDir, std::pair<int, int> pos, std::set<int> shells, std::set<int> enemyTanks)
{

    EnemyScanResult scan = scanVisibleEnemies(pos.first, pos.second);

    auto it = tankRoles.find(tankId);
    if (it != tankRoles.end())
    {

        if (shouldKeepRole(tankId, pos, it->second, scan, shells, enemyTanks))
        {
            tankPositions[tankId] = pos;
            scan.ShouldKeepRole = true;
        }
    }

    return scan;
}

std::set<std::pair<int, int>> MyPlayer::getCalculatedPathsSet()
{
    std::set<std::pair<int, int>> bannedPositionsSet;
    for (const auto &entry : tanksPlannedPaths)
    {
        const auto &path = entry.second;
        for (const auto &pos : path)
        {
            bannedPositionsSet.insert(pos);
        }
    }

    return bannedPositionsSet;
}

EnemyScanResult MyPlayer::scanVisibleEnemies(int x0, int y0) const
{
    const char enemyChar = (player_index == 1) ? '2' : '1';
    EnemyScanResult result;
    for (int y = 0; y < (int)lastSatellite.size(); ++y)
    {
        for (int x = 0; x < (int)lastSatellite[0].size(); ++x)
        {
            if (lastSatellite[y][x] == enemyChar)
            {
                int dist = manhattanDistance(x0, y0, x, y);
                if (dist < result.closestDistance)
                    result.closestDistance = dist;
                if ((x == x0 || y == y0) && isClearLine(x0, y0, x, y))
                    result.hasLineOfSight = true;
            }
        }
    }
    return result;
}

int MyPlayer::manhattanDistance(int x1, int y1, int x2, int y2) const
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

bool MyPlayer::isClearLine(int x1, int y1, int x2, int y2) const
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Normalize direction to one of 8 (if not aligned, return false)
    if (dx != 0)
        dx /= std::abs(dx);
    if (dy != 0)
        dy /= std::abs(dy);

    if ((x1 + dx + playerGameWidth) % playerGameWidth == x2 &&
        (y1 + dy + playerGameHeight) % playerGameHeight == y2)
    {
        return true; // adjacent cell
    }

    int x = (x1 + dx + playerGameWidth) % playerGameWidth;
    int y = (y1 + dy + playerGameHeight) % playerGameHeight;

    while (x != x2 || y != y2)
    {
        if (lastSatellite[y][x] == '#')
            return false;

        x = (x + dx + playerGameWidth) % playerGameWidth;
        y = (y + dy + playerGameHeight) % playerGameHeight;
    }
    return true;
}

bool MyPlayer::isInOpen(int x, int y) const
{
    int wallCount = 0;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0)
                continue;

            int nx = (x + dx + playerGameWidth) % playerGameWidth;
            int ny = (y + dy + playerGameHeight) % playerGameHeight;

            if (lastSatellite[ny][nx] == '#')
                wallCount++;
        }
    }
    return wallCount <= 3;
}

std::pair<int, int> MyPlayer::prepareInfoForBattleInfo(std::set<int> &mines, std::set<int> &walls, std::set<int> &shells, std::set<int> &friendlyTanks, std::set<int> &enemyTanks, SatelliteView &satellite_view)
{
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
                myX = j;
                myY = i;
            }
            else if (object == '@')
                mines.insert(id);
            else if (object == '#')
                walls.insert(id);
            else if (object == '*')
                shells.insert(id);
            else if (object >= '0' && object <= '9')
            {
                int tankOwner = object - '0';

                if (tankOwner == player_index)
                    friendlyTanks.insert(id);
                else
                    enemyTanks.insert(id);
            }
        }
    }
    return std::make_pair(myX, myY);
}

bool MyPlayer::isInRedZone(int x, int y, std::set<int> shellsPositions, std::set<int> enemies) const
{
    std::pair<int, int> pos = {x, y};

    for (const auto &shellPos : shellsPositions)
    {
        std::pair<int, int> shell = inverseBijection(shellPos);
        if (manhattanDistance(pos.first, pos.second, shell.first, shell.second) <= 3)
            return true;
    }

    for (const auto &enemyId : enemies)
    {
        std::pair<int, int> enemyPos = inverseBijection(enemyId);
        if (manhattanDistance(x, y, enemyPos.first, enemyPos.second) <= 4)
            return true;
    }

    return false;
}
