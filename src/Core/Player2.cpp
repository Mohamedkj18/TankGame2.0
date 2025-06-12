#include "Core/MyPlayer.h"

std::unique_ptr<Role> Player2::createRole(int tankId, Direction currDir, std::pair<int, int> pos, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    int x = pos.first, y = pos.second;

    // High threat or trapped → Evasior
    if (isInRedZone(x, y, shells, enemyTanks) || scan.closestDistance <= 2)
        return std::make_unique<EvasiorRole>();

    // Long-range opportunity with ammo → Sniper
    if (scan.hasLineOfSight && scan.closestDistance >= 6 && num_shells > 0)
        return std::make_unique<SniperRole>();

    // Enemy is close and tank has ammo → Chaser
    if (scan.closestDistance <= 5 && num_shells > 0)
        return std::make_unique<ChaserRole>();

    // Fallback: Evasior
    return std::make_unique<EvasiorRole>();
}

bool Player2::shouldKeepRole(int tankId, const std::pair<int, int> &pos, const std::string &role, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    if (role == "Sniper")
        return scan.hasLineOfSight && scan.closestDistance >= 6 && num_shells > 0;

    if (role == "Chaser")
        return scan.closestDistance <= 5 && num_shells > 0;

    if (role == "Evasior")
        return isInRedZone(pos.first, pos.second, shells, enemyTanks) || scan.closestDistance < 4;

    return false;
}
