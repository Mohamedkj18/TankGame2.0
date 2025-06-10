#pragma once
#include "Common/SatelliteView.h"
#include "Core/Tank.h"
#include "Core/Shell.h"
#include "Core/GameManager.h"
#include <unordered_map>
#include <map>
#include <set>
#include <cstddef>

class MySatelliteView : public SatelliteView
{
private:
    int tankPos;
    const std::map<int, std::unique_ptr<Tank>> &tanks;
    const std::unordered_map<int, std::unique_ptr<Shell>> &shells;
    const std::set<int> &mines;
    const std::unordered_map<int, Wall> &walls;

public:
    MySatelliteView(int tankPos,
                    const std::map<int, std::unique_ptr<Tank>> &tanks,
                    const std::unordered_map<int, std::unique_ptr<Shell>> &shells,
                    const std::set<int> &mines,
                    const std::unordered_map<int, Wall> &walls);

    char getObjectAt(size_t x, size_t y) const override;
};
