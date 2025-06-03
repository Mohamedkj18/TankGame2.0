#include "Core/MyBattleInfo.h"

MyBattleInfo::MyBattleInfo(int width, int height,
                           const std::set<int>& friendlyTanks,
                           const std::set<int>& enemyTanks,
                           const std::set<int>& mines,
                           const std::set<int>& walls,
                           const std::set<int>& shells)
    : width(width), height(height),
      friendlyTanks(friendlyTanks), enemyTanks(enemyTanks),
      mines(mines), walls(walls), shells(shells),
      myX(0), myY(0), myDirection(Direction::UP) {}

int MyBattleInfo::getWidth() const { return width; }
int MyBattleInfo::getHeight() const { return height; }

void MyBattleInfo::setMyXPosition(int x) { myX = x; }
void MyBattleInfo::setMyYPosition(int y) { myY = y; }
void MyBattleInfo::setMyDirection(Direction dir) { myDirection = dir; }

int MyBattleInfo::getMyXPosition() const { return myX; }
int MyBattleInfo::getMyYPosition() const { return myY; }
Direction MyBattleInfo::getMyDirection() const { return myDirection; }

void MyBattleInfo::setBFSPath(const std::vector<std::pair<int, int>>& path) {
    bfsPath = path;
}
const std::vector<std::pair<int, int>>& MyBattleInfo::getBFSPath() const {
    return bfsPath;
}

void MyBattleInfo::setRole(const std::string& r) { role = r; }
const std::string& MyBattleInfo::getRole() const { return role; }

void MyBattleInfo::setFriendlies(const std::set<int>& f) { friendliesNearby = f; }
const std::set<int>& MyBattleInfo::getFriendlies() const { return friendliesNearby; }

void MyBattleInfo::setThreats(const std::set<int>& t) { knownThreats = t; }
const std::set<int>& MyBattleInfo::getThreats() const { return knownThreats; }

bool MyBattleInfo::isMine(int x, int y) const {
    return mines.count(bijection(x, y)) > 0;
}

bool MyBattleInfo::isWall(int x, int y) const {
    return walls.count(bijection(x, y)) > 0;
}

bool MyBattleInfo::isShell(int x, int y) const {
    return shells.count(bijection(x, y)) > 0;
}

bool MyBattleInfo::isEnemyTank(int x, int y) const {
    return enemyTanks.count(bijection(x, y)) > 0;
}

bool MyBattleInfo::isFriendlyTank(int x, int y) const {
    return friendlyTanks.count(bijection(x, y)) > 0;
}

int MyBattleInfo::bijection(int x, int y) const {
    return ((x + y) * (x + y + 1)) / 2 + y;
}
