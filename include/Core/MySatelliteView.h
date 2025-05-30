
#include "common/SatelliteView.h"
#include "Shell.h"
#include <unordered_map>
#include <set>

class MySatteliteView: public SatteliteView {
    public:
        std::unordered_map<int, std::unique_ptr<Tank>> tanks;
        std::unordered_map<int, std::unique_ptr<Shell>> shells;
        std::set<int> mines;
        std::unordered_map<int, Wall> walls;
        int tankPos;
        MySatteliteView(int tankPos, const &std::unordered_map<int, std::unique_ptr<Tank>> tanks, const &std::unordered_map<int, std::unique_ptr<Shell>> shells, &std::set<int> mines, &std::unordered_map<int, Wall> walls)
            : tankPos(tankPos), tanks(tanks), shells(shells), mines(mines), walls(walls) {}


        char getObjectAt(size_t x, size_t y);
}