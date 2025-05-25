#include "Core/MyTankAlgorithmFactory.h"

std::unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const
{
    return nullptr; // or your actual algorithm instance
}
