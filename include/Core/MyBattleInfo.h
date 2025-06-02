#include "Common/BattleInfo.h"
#include "Core/Tank.h"
#include <set>
#include "Core/GameObject.h"

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

    bool isMine(int x, int y);
    bool isWall(int x, int y);
    bool isShell(int x, int y);
    bool isFriendlyTank(int x, int y);
    bool isEnemyTank(int x, int y);
};