#include <unordered_map>
#include <set>
#include "Core/SatelliteView.h"



class MySatelliteView : public SatelliteView
{
private:
    const std::unordered_map<int, Tank*>& tanks;
    const std::unordered_map<int, Shell*>& shells;
    const std::set<int>& mines;
    const std::unordered_map<int, Wall>& walls;

public:
    int tankPos;

    MySatelliteView(int tankPos,
                    const std::unordered_map<int, Tank*>& tanks,
                    const std::unordered_map<int, Shell*>& shells,
                    const std::set<int>& mines,
                    const std::unordered_map<int, Wall>& walls)
        : tankPos(tankPos), tanks(tanks), shells(shells), mines(mines), walls(walls) {}

    char getObjectAt(size_t x, size_t y) const override;
};
