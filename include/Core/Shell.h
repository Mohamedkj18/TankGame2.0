#pragma once

#include "Core/GameObject.hpp"

// ========================= CLASS: Shell =========================

class Shell : public GameObject
{
public:
    Shell(int x, int y, Direction dir, Game *game);
    bool checkForAWall();
};