#pragma once

#ifndef MY_BATTLE_INFO_H
#define MY_BATTLE_INFO_H

#include <set>
#include <vector>
#include <string>
#include "Common/BattleInfo.h"
#include "utils/DirectionUtils.h"

class MyBattleInfo : public BattleInfo
{
private:
    int width, height;
    int myX, myY;
    std::string role;
    std::set<int> friendlyTanks;
    std::set<int> enemyTanks;
    std::set<int> mines;
    std::set<int> walls;
    std::set<int> shells;
    std::vector<std::pair<int, int>> bfsPath;

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

    void setMyXPosition(int x);
    void setMyYPosition(int y);
    int getMyXPosition() const;
    int getMyYPosition() const;

    void setRole(const std::string &role);
    std::string getRole() const;

    void setBFSPath(std::vector<std::pair<int, int>> &path);
    std::vector<std::pair<int, int>> getBFSPath();

    bool isMine(int x, int y) const;
    bool isWall(int x, int y) const;

    bool isShell(int x, int y) const;
    bool isEnemyTank(int x, int y) const;
    bool isFriendlyTank(int x, int y) const;

    int bijection(int x, int y) const;
};

#endif
