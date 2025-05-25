#include <cstddef>
#include <memory>
#include "Common/Player.h"
#include "Core/MyPlayerFactory.h"

class DummyPlayer : public Player
{
public:
    DummyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : Player(player_index, x, y, max_steps, num_shells)
    {
        (void)player_index;
        (void)x;
        (void)y;
        (void)max_steps;
        (void)num_shells;
    }

    void updateTankWithBattleInfo(TankAlgorithm &, SatelliteView &) override {}
};

std::unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y,
                                                size_t max_steps, size_t num_shells) const
{
    return std::make_unique<DummyPlayer>(player_index, x, y, max_steps, num_shells);
}
