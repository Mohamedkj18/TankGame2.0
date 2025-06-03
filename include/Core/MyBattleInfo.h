#pragma once

#include "Common/BattleInfo.h"
#include "utils/DirectionUtils.h"
#include <set>
#include <vector>
#include <string>
#include <utility>

class MyBattleInfo : public BattleInfo {
private:
    int width, height;
    int myX, myY;
    Direction myDirection;

    std::set<int> friendlyTanks;
    std::set<int> enemyTanks;
    std::set<int> mines;
    std::set<int> walls;
    std::set<int> shells;

    std::vector<std::pair<int, int>> bfsPath;
    std::string role;
    std::set<int> friendliesNearby;
    std::set<int> knownThreats;

public:
    MyBattleInfo(int width, int height,
                 const std::set<int>& friendlyTanks,
                 const std::set<int>& enemyTanks,
                 const std::set<int>& mines,
                 const std::set<int>& walls,
                 const std::set<int>& shells);

    int getWidth() const;
    int getHeight() const;

    void setMyXPosition(int x);
    void setMyYPosition(int y);
    void setMyDirection(Direction dir);

    int getMyXPosition() const;
    int getMyYPosition() const;
    Direction getMyDirection() const;

    void setBFSPath(const std::vector<std::pair<int, int>>& path);
    const std::vector<std::pair<int, int>>& getBFSPath() const;

    void setRole(const std::string& r);
    const std::string& getRole() const;

    void setFriendlies(const std::set<int>& f);
    const std::set<int>& getFriendlies() const;

    void setThreats(const std::set<int>& t);
    const std::set<int>& getThreats() const;

    bool isMine(int x, int y) const;
    bool isWall(int x, int y) const;
    bool isShell(int x, int y) const;
    bool isEnemyTank(int x, int y) const;
    bool isFriendlyTank(int x, int y) const;

private:
    int bijection(int x, int y) const;
};
