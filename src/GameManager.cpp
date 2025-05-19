#include "common/TankAlgorithmFactory.h"
#include "common/PlayerFactory.h"

class GameManager
{
public:
    GameManager(TankAlgorithmFactory &tank_factory,
                PlayerFactory &player_factory);
    ~GameManager();

    void startGame();
    void endGame();
    void resetGame();
}