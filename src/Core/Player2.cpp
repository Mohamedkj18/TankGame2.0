#include "Core/MyPlayer.h"

std::unique_ptr<Role> Player2::createRole(int tankId, std::pair<int, int> pos, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks, int numOfFriendlyTanks)
{
    int x = pos.first, y = pos.second;

    int remainingShells = tanksRemainingShells[tankId];
    int numOfChasers = 0;

    for (const auto &role : tankRoles)
    {
        if (role.second == "Chaser")
            numOfChasers++;
    }
    if (numOfChasers == 0 && numOfFriendlyTanks >= (int)(tankRoles.size() - 1))
        tankRoles[tankId] = "Chaser";
    return std::make_unique<ChaserRole>(5, playerGameWidth, playerGameHeight);
    // High threat or trapped → Evasior
    if (isInRedZone(x, y, shells, enemyTanks) || scan.closestDistance <= 2)
    {
        tankRoles[tankId] = "Evasior";
        return std::make_unique<EvasiorRole>(5, playerGameWidth, playerGameHeight);
    }

    // Long-range opportunity with ammo → Sniper
    if (scan.hasLineOfSight && scan.closestDistance >= 6 && remainingShells > 0)
    {
        tankRoles[tankId] = "Sniper";
        return std::make_unique<SniperRole>(3, playerGameWidth, playerGameHeight);
    }
    // Enemy is close and tank has ammo → Chaser
    tankRoles[tankId] = "Chaser";
    return std::make_unique<ChaserRole>(5, playerGameWidth, playerGameHeight);
}

bool Player2::shouldKeepRole(int tankId, const std::pair<int, int> &pos, const std::string &role, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks, int numFriendlyTanks)
{
    int remainingShells = tanksRemainingShells[tankId];
    int numOfChasers = 0;

    for (const auto &role : tankRoles)
    {
        if (role.second == "Chaser")
            numOfChasers++;
    }
    if (numOfChasers == 0 && numFriendlyTanks >= (int)tankRoles.size() - 1)
        return false;

    if (isInRedZone(pos.first, pos.second, shells, enemyTanks) || scan.closestDistance < 3)
        return false;

    if (role == "Sniper")
        return scan.hasLineOfSight && scan.closestDistance >= 6 && remainingShells > 0;

    if (role == "Chaser")
        return scan.closestDistance <= 5 && remainingShells > 0;

    if (role == "Evasior")
        return isInRedZone(pos.first, pos.second, shells, enemyTanks) || scan.closestDistance < 4;

    return false;
}

Player2::~Player2() = default;
