#include "Algorithms/MyTankAlgorithmFactory.h"
#include "Algorithms/MyTankAlgorithm.h"

std::unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const
{
    // Customize per player or tank if needed
    int maxMovesPerUpdate = 8;
    int fireRange = 5;
    Direction dir;
    if (player_index == 1)
    {
        dir = Direction::L;
    }
    else if (player_index == 2)
    {
        dir = Direction::R;
    }
    return std::make_unique<MyTankAlgorithm>(player_index, tank_index, maxMovesPerUpdate, fireRange, dir);
}
