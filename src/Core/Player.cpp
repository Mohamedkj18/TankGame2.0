#include "Core/MyPlayer.h"
#include "Core/MyBattleInfo.h"
#include "Core/MySatelliteView.h"

// ------------------------ Abstract Player ------------------------

MyPlayer::MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : Player(player_index, x, y, max_steps, num_shells), player_index(player_index), x(x), y(y), max_steps(max_steps), num_shells(num_shells) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view)
{

    std::set<int> friendlyTanks;
    std::set<int> enemyTanks;
    std::set<int> mines;
    std::set<int> walls;
    std::set<int> shells;

    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            char object = satellite_view.getObjectAt(i, j);
            int bijectionIndex = bijection(i, j);

            if (object == '%') // Current Tank
            {
                friendlyTanks.insert(bijectionIndex);
            }
            else if (object >= '0' && object <= '9') // Other Tanks
            {
                if (object - '0' == player_index)
                    friendlyTanks.insert(bijectionIndex);
                else
                    enemyTanks.insert(bijectionIndex);
            }
            else if (object == '@') // Mine
            {
                mines.insert(bijectionIndex);
            }
            else if (object == '#') // Wall
            {
                walls.insert(bijectionIndex);
            }
            else if (object == '*') // Shell
            {
                shells.insert(bijectionIndex);
            }
        }
    }
    MyBattleInfo battleInfo(x, y,
                            friendlyTanks, enemyTanks,
                            mines, walls, shells);

    tank.updateBattleInfo(battleInfo);
}
