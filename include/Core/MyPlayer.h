#include "common/Player.h"

class MyPlayer : public Player
{
private:
    int player_index;
    size_t x;
    size_t y;
    size_t max_steps;
    size_t num_shells;

public:
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
};

// ------------------------ Player 1 ------------------------
class Player1 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;

    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override
    {
        MyPlayer::updateTankWithBattleInfo(tank, satellite_view);
    }
};
// ------------------------ Player 2 ------------------------
class Player2 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;

    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override
    {
        MyPlayer::updateTankWithBattleInfo(tank, satellite_view);
    }
};