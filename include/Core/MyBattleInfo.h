#include "Common/BattleInfo.h"
#include "Core/Tank.h"
#include <set>
#include "Core/GameObject.h"
#include <optional>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>

class MyBattleInfo : public BattleInfo
{
private:
    int width;
    int height;

    std::set<int> friendlyTanks;
    std::set<int> enemyTanks;
    std::set<int> mines;
    std::set<int> walls;
    std::set<int> shells;
    std::pair<int,int> closestEnemyTank;

public:
    MyBattleInfo(int width, int height,
                 const std::set<int> &friendlyTanks,
                 const std::set<int> &enemyTanks,
                 const std::set<int> &mines,
                 const std::set<int> &walls,
                 const std::set<int> &shells);

    int getWidth() const;
    int getHeight() const;

    std::set<int> &getFriendlyTanks();
    std::set<int> &getEnemyTanks();
    std::set<int> &getMines();
    std::set<int> &getWalls();
    std::set<int> &getShells();

    int getMyXPosition();
    int getMyYPosition();
    bool isMine(int x, int y);
    bool isWall(int x, int y);
    bool isShell(int x, int y);
    bool isFriendlyTank(int x, int y);
    bool isEnemyTank(int x, int y);
    std::optional<std::pair<int, int>> getClosestTarget(std::pair<int, int> start,const std::vector<std::pair<int, int>>& targets);
};