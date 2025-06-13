#include "Core/MyPlayer.h"

std::unique_ptr<Role> Player2::createRole(int tankId, Direction currDir, std::pair<int, int> pos, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    int x = pos.first, y = pos.second;

    int remainingShells = tanksRemainingShells[tankId];

    // High threat or trapped → Evasior
    if (isInRedZone(x, y, shells, enemyTanks) || scan.closestDistance <= 2)
    {
        tankRoles[tankId] = "Evasior";
        return std::make_unique<EvasiorRole>(5, currDir, pos, playerGameWidth, playerGameHeight);
    }

    // Long-range opportunity with ammo → Sniper
    if (scan.hasLineOfSight && scan.closestDistance >= 6 && remainingShells > 0)
    {
        tankRoles[tankId] = "Sniper";
        return std::make_unique<SniperRole>(3, currDir, pos, playerGameWidth, playerGameHeight);
    }
    // Enemy is close and tank has ammo → Chaser
    tankRoles[tankId] = "Chaser";
    return std::make_unique<ChaserRole>(5, currDir, pos, playerGameWidth, playerGameHeight);
}

bool Player2::shouldKeepRole(int tankId, const std::pair<int, int> &pos, const std::string &role, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    int remainingShells = tanksRemainingShells[tankId];

    if (role == "Sniper")
        return scan.hasLineOfSight && scan.closestDistance >= 6 && remainingShells > 0;

    if (role == "Chaser")
        return scan.closestDistance <= 5 && remainingShells > 0;

    if (role == "Evasior")
        return isInRedZone(pos.first, pos.second, shells, enemyTanks) || scan.closestDistance < 4;

    return false;
}

Player2::~Player2() = default;
