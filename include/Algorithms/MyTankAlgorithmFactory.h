#pragma once
#include <memory>
#include "Common/TankAlgorithm.h"
#include "Common/TankAlgorithmFactory.h"

class MyTankAlgorithmFactory : public TankAlgorithmFactory
{
public:
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override; // ✅ Declaration only
};
