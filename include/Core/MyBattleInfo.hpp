#include "Common/BattleInfo.h"
#include "Core/Tank.h"
#include <unordered_map>
#include "Core/GameObject.h"

class MyBattleInfo: public BattleInfo{
private:
    int width;
    int height;

    std::unordered_map<int, Tank *> tanks;
    std::unordered_map<int, Shell *> shells;
};