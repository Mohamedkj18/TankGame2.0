#include "Core/MyPlayer.h"

std::unique_ptr<Role> Player1::createRole(int tankId, Direction currDir, std::pair<int, int> pos, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    int x = pos.first, y = pos.second;

    // If in red zone or under threat → Evasior
    if (isInRedZone(x, y, shells, enemyTanks) || scan.closestDistance <= 2)
        return std::make_unique<EvasiorRole>();

    // If has LOS and is at decent range → Defender
    if (scan.hasLineOfSight && num_shells > 0)
        return std::make_unique<DefenderRole>();

    // If exposed in open space → Decoy
    if (isInOpen(x, y))
        return std::make_unique<DecoyRole>();

    // Fallback: Evasior
    return std::make_unique<EvasiorRole>();
}

bool Player1::shouldKeepRole(int tankId, const std::pair<int, int> &pos, const std::string &role, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    if (role == "Defender")
        return scan.hasLineOfSight && scan.closestDistance >= 3 && num_shells > 0;

    if (role == "Decoy")
        return isInOpen(pos.first, pos.second) && scan.closestDistance > 2;

    if (role == "Evasior")
        return isInRedZone(pos.first, pos.second, shells, enemyTanks) || scan.closestDistance < 4;

    return false;
}
