#include <memory>
#include <stdexcept>
#include "Algorithms/MyTankAlgorithmFactory.h"
#include "Algorithms/TankChase.h"
#include "Algorithms/TankEvasion.h"

std::unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const
{
    if (player_index == 1)
    {
        return std::make_unique<TankChase>(player_index, tank_index);
    }
    else if (player_index == 2)
    {
        return std::make_unique<TankEvasion>(player_index, tank_index);
    }
    else
    {
        throw std::invalid_argument("Invalid player index");
    }
}
