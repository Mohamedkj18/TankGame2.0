#include <cstddef>
#include "Core/MySatelliteView.h"
#include "utils/DirectionUtils.h"

char MySatelliteView::getObjectAt(size_t x, size_t y)
{
    int bijectionIndex = bijection(x, y);
    if (bijectionIndex == tankPos)
        return '%'; // Current Tank
    if (tanks.count(bijectionIndex))
        return (char)('0' + tanks[bijectionIndex]->getPlayerId()); // Tank
    if (shells.count(bijectionIndex))
        return '*'; // Shell
    if (mines.count(bijectionIndex))
        return '@'; // Mine
    if (walls.count(bijectionIndex))
        return '#'; // Wall

    return ' ';
}