#include "Core/MyPlayer.h"

std::unique_ptr<Role> Player1::createRole(int tankId, Direction currDir, std::pair<int, int> pos, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    int x = pos.first, y = pos.second;

    int remainingShells = tanksRemainingShells[tankId];
    // If in red zone or under threat → Evasior
    if (isInRedZone(x, y, shells, enemyTanks) || scan.closestDistance <= 2)
    {
        tankRoles[tankId] = "Evasior";
        return std::make_unique<EvasiorRole>(5, currDir, pos, playerGameWidth, playerGameHeight);
    }

    // If has LOS and is at decent range → Defender
    if (scan.hasLineOfSight && remainingShells > 0)
    {
        tankRoles[tankId] = "Defender";
        return std::make_unique<DefenderRole>(5, currDir, pos, playerGameWidth, playerGameHeight);
    }

    // else → Decoy

    tankRoles[tankId] = "Decoy";
    return std::make_unique<DecoyRole>(5, currDir, pos, playerGameWidth, playerGameHeight);
}

bool Player1::shouldKeepRole(int tankId, const std::pair<int, int> &pos, const std::string &role, EnemyScanResult scan, std::set<int> shells, std::set<int> enemyTanks)
{
    int remainingShells = tanksRemainingShells[tankId];

    if (role == "Defender")
        return scan.hasLineOfSight && scan.closestDistance >= 3 && remainingShells > 0;

    if (role == "Decoy")
        return isInOpen(pos.first, pos.second) && scan.closestDistance > 2;

    if (role == "Evasior")
        return isInRedZone(pos.first, pos.second, shells, enemyTanks) || scan.closestDistance < 4;

    return false;
}

Player1::~Player1() = default;
