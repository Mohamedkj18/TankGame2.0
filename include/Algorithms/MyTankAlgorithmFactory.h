#pragma once
#include <memory>
#include "common/TankAlgorithm.h"
#include "common/TankAlgorithmFactory.h"

class MyTankAlgorithmFactory : public TankAlgorithmFactory
{
public:
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override; // ✅ Declaration only
    ~MyTankAlgorithmFactory() override = default;
};
