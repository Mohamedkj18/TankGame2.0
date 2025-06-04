#pragma once

#include "Core/MyBattleInfo.h"
#include "Common/TankAlgorithm.h"
#include "Common/ActionRequest.h"
#include "Common/BattleInfo.h"
#include <vector>
#include <string>
#include <set>
#include <unordered_map>

class MyTankAlgorithm : public TankAlgorithm
{
private:
    int moveIndex;
    int tankId;
    int playerId;
    std::vector<ActionRequest> plannedMoves;

    // Metadata from BattleInfo
    std::vector<std::pair<int, int>> bfsPath;
    std::string role;
    std::set<int> nearbyFriendlies;
    std::set<int> threats;

    int gameWidth;
    int gameHeight;
    Direction currentDirection;
    std::pair<int, int> currentPos;

public:
    MyTankAlgorithm(int player_index, int tank_index, int numMovesPerUpdate, int range);

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo &info) override;

    // Execution logic
    std::pair<int, int> move(std::pair<int, int> current, Direction direction);
    std::string getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target);
    void rotateTowards(std::string desiredDir);
    void prepareActions();

    bool isThreatAhead();
    bool isFriendlyTooClose();
    bool shouldShoot();
    void setTankId(int id) { tankId = id; };
    int getTankId() const { return tankId; };
    void setPlayerId(int id) { playerId = id; };
    int getPlayerId() const { return playerId; };

private:
    int range;
    int maxMovesPerUpdate;
};
