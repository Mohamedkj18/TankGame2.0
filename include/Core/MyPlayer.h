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

class MyPlayer : public Player
{
protected:
    int player_index;
    size_t playerGameWidth, playerGameHeight;
    size_t max_steps, num_shells;

    std::unordered_map<int, std::vector<std::pair<int, int>>> plannedPositions;
    std::unordered_map<int, std::string> tankRoles;
    std::vector<std::vector<char>> lastSatellite;

public:
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
    virtual std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) = 0;

    void updatePlannedPositions();
    bool isSquareValid(int x, int y, int step);
    std::pair<int, int> findFirstLegalLocationToFlee(int x, int y);

protected:
    std::string assignRole(int tankId);
    std::pair<int, int> getTargetForTank(int tankId);
    std::pair<int, int> moveTank(std::pair<int, int> pos, Direction dir);
};

// ------------------------ Player 1 ------------------------
class Player1 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;
    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) override;
};

// ------------------------ Player 2 ------------------------
class Player2 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;
    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) override;
};
