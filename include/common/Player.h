#pragma once

#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <cstddef>

class Player
{
public:
    Player(int ,
           size_t ,
           size_t ,
           size_t ,
           size_t) {}
    virtual ~Player() {}
    virtual void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) = 0;
};
