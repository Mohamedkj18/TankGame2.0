
#include "Common/SatelliteView.h"
#include "Shell.h"
#include "Tank.h"
#include <unordered_map>
#include <set>
#include <memory>

class MySatelliteView : public SatelliteView
{
public:
    std::unordered_map<int, std::unique_ptr<Tank>> tanks;
    std::unordered_map<int, std::unique_ptr<Shell>> shells;
    std::set<int> mines;
    std::unordered_map<int, Wall> walls;
    int tankPos;
    MySatelliteView(int tankPos, const std::unordered_map<int, std::unique_ptr<Tank>> &tanks, const std::unordered_map<int, std::unique_ptr<Shell>> &shells, std::set<int> &mines, std::unordered_map<int, Wall> &walls)
        : tankPos(tankPos), tanks(tanks), shells(shells), mines(mines), walls(walls) {}

    char getObjectAt(size_t x, size_t y) const override;
};