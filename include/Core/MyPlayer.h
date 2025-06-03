#include "common/Player.h"

class MyPlayer : public Player
{
private:
    int player_index;
    size_t x;
    size_t y;
    size_t max_steps;
    size_t num_shells;
    std::unordered_map<int,std::vector<std::pair<int, int>>> plannedPositions;

public:
    MyPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;
    virtual std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) = 0;
    void updatePlanedPositions();
    bool isSquareValid(int x, int y, int step);
    std::pair<int,int> findFirstLegalLocationToFlee(int x, int y);

};

// ------------------------ Player 1 ------------------------
class Player1 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;

    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) override;
};
// ------------------------ Player 2 ------------------------
class Player2 : public MyPlayer
{
public:
    using MyPlayer::MyPlayer;

    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) override;
};