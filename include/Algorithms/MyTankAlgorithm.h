#include "Core/MyBattleInfo.h"
#include "Common/TankAlgorithm.h"
#include "Common/ActionRequest.h"
#include "Common/BattleInfo.h"
#include <vector>
#include <string>

class MyTankAlgorithm : public TankAlgorithm
{
private:
    int moveNumToBeExecuted;
    std::vector<ActionRequest> movesToBeExecuted;
    int moveToAdd;
    int numOfMovesPerPath;
    int range;
    int gameWidth;
    int gameHeight;
    Direction currentDirection;

public:
    MyTankAlgorithm(int numOfMovesPerPath, int range);

    ActionRequest getAction();
    std::pair<int, int> move(std::pair<int, int> current, Direction direction);
    std::string getDirectionFromPosition(std::pair<int, int> current, std::pair<int, int> target);
    void setTheMovesToBeExecuted(MyBattleInfo &info);
    int getMovesToRotateTank(std::string directionToRotateTo, std::string currentDirection);
    void addMoveToBeExecuted(double angle);
    virtual std::string getNextMove(int playerNum, int playerToChase) = 0;
    bool isTheEnemyInRange(MyBattleInfo &info);
    bool isThereAMineOrAWall(int x, int y,MyBattleInfo &info);
    int isTheSquareSafe(int x, int y, int rangeToCheck, MyBattleInfo &info);
    void updateBattleInfo(BattleInfo &info);
    std::vector<std::pair<int, int>> getPathToClosestTarget(std::pair<int, int> start, const std::vector<std::pair<int, int>>& targets, MyBattleInfo &info);
    virtual std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> target) = 0;

};