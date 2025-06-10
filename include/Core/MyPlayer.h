#pragma once

#include "common/Player.h"
#include "Core/MyBattleInfo.h"
#include "Core/MySatelliteView.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>
#include <set>
#include <queue>
#include <stack>

class Role;
struct EnemyScanResult
{
    int closestDistance = INT_MAX;
    bool hasLineOfSight = false;
    bool isInOpen = false;
    bool ShouldKeepRole = false;
};
class MyPlayer : public Player
{
protected:
    int player_index;
    size_t playerGameWidth, playerGameHeight;
    size_t max_steps, num_shells;

    std::unordered_map<int, std::vector<std::pair<int, int>>> tanksPlannedPaths;
    std::unordered_map<int, std::pair<int, int>> tankPositions;
    std::unordered_map<int, std::string> tankRoles;
    std::vector<std::vector<char>> lastSatellite;

public:
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);
    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
    std::string getRoleName(int tankId) const
    {
        auto it = tankRoles.find(tankId);
        return (it != tankRoles.end()) ? it->second : "Unknown";
    }

protected:
    EnemyScanResult assignRole(int tankId, Direction currDir, std::pair<int, int> pos);
    std::unique_ptr<Role> createRole(int tankId, Direction currDir, std::pair<int, int> pos, EnemyScanResult scan);

    void updatePlannedMoves();

    bool shouldKeepRole(int tankId, const std::pair<int, int> &pos, const std::string &role, EnemyScanResult scan);
    EnemyScanResult scanVisibleEnemies(int x0, int y0) const;
    int manhattanDistance(int x1, int y1, int x2, int y2) const;
    bool isClearLine(int x0, int y0, int x1, int y1) const;
    bool isInOpen(int x, int y) const;
};

// ------------------------ Player 1 ------------------------
class Player1 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;
    virtual ~Player1();
    // void assignRole(int tankId) override;
};

// ------------------------ Player 2 ------------------------
class Player2 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;
    virtual ~Player2();
    // void assignRole(int tankId) override;
};
