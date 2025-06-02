
#include "Core/MyBattleInfo.hpp"
#include "utils/DirectionUtils.h"

MyBattleInfo::MyBattleInfo(int width, int height, const std::set<int> &friendlyTanks, const std::set<int> &enemyTanks,
                           const std::set<int> &mines, const std::set<int> &walls, const std::set<int> &shells)
    : width(width), height(height), friendlyTanks(friendlyTanks), enemyTanks(enemyTanks),
      mines(mines), walls(walls), shells(shells) {}

int MyBattleInfo::getWidth() const { return width; }
int MyBattleInfo::getHeight() const { return height; }

std::set<int> &MyBattleInfo::getFriendlyTanks() { return friendlyTanks; }
std::set<int> &MyBattleInfo::getEnemyTanks() { return enemyTanks; }
std::set<int> &MyBattleInfo::getMines() { return mines; }
std::set<int> &MyBattleInfo::getWalls() { return walls; }
std::set<int> &MyBattleInfo::getShells() { return shells; }

bool MyBattleInfo::isMine(int x, int y)
{
    return mines.count(bijection(x, y)) > 0;
}
bool MyBattleInfo::isWall(int x, int y)
{
    return walls.count(bijection(x, y)) > 0;
}
bool MyBattleInfo::isShell(int x, int y)
{
    return shells.count(bijection(x, y)) > 0;
}

bool MyBattleInfo::isFriendlyTank(int x, int y)
{
    return friendlyTanks.count(bijection(x, y)) > 0;
}
bool MyBattleInfo::isEnemyTank(int x, int y)
{
    return enemyTanks.count(bijection(x, y)) > 0;
}
