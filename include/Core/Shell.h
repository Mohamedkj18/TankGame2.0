#pragma once

#include "Core/GameObject.h"

// ========================= CLASS: Shell =========================

class Shell : public GameObject
{
public:
    Shell(int x, int y, Direction dir, GameManager *game);
    bool checkForAWall();
};