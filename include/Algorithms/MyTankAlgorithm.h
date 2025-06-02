#include "Common/TankAlgorithm.h"

class MyTankAlgorithm : public TankAlgorithm
{
private:
    int moveNumToBeExecuted;
    std::vector<ActionRequest> movesToBeExecuted;
    int moveToAdd;
    int numOfMovesPerPath;
    int range;

public:
    ActionRequest getAction();
    std::pair<int, int> move(std::pair<int, int> current, Direction direction);
    std::string getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target);
    void setTheMovesToBeExecuted(int playerNum, int playerToChase);
    int getMovesToRotateTank(std::string directionToRotateTo, std::string currentDirection);
    void addMoveToBeExecuted(double angle);
    virtual std::string getNextMove(int playerNum, int playerToChase) = 0;
    bool isTheEnemyInRange(int playerNum, int playerToChase);
    bool isThereAMineOrAWall(int x, int y);
    int isTheSquareSafe(int x, int y, int rangeToCheck);
    void updateBattleInfo(BattleInfo &info);
    virtual setMovesToBeExecuted() = 0;
    virtual std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) = 0;
};