#pragma once

#include <string>
#include <set>
#include "Core/GameObject.h"
#include "Core/GameManager.h"
#include "Common/ActionRequest.h"

class GameManager;
// ========================= CLASS: Tank =========================

class Tank : public GameObject
{
private:
    int playerId;
    int tankId;
    int artilleryShells;
    bool destroyed;
    int cantShoot;
    int reverseCharge = 0;
    bool reverseQueued = false;
    bool reverseReady = false;
    ActionRequest lastMove;

public:
    Tank(int x, int y, Direction dir, GameManager *game, int playerId, int shells, int tankId);

    // Position and state
    int getPlayerId();
    int getTankId();
    ActionRequest getLastMove();
    bool checkForAWall();

    // Movement
    void moveBackwards();
    void rotateTank(double angle);
    void setDirection(std::string directionStr);
    void setLastMove(ActionRequest currentMove);

    // Firing
    void fire();

    // Damage
    void hit();

    // Shooting cooldown
    void incrementCantShoot();
    void resetCantShoot();
    bool canShoot();
    int getCantShoot();

    // 🔁 Reverse state
    bool isReverseQueued() const;
    bool isReverseReady() const;
    int getReverseCharge() const;
    void queueReverse();
    void incrementReverseCharge();
    void resetReverseState();
    void executeReverse();
    void ignoreMove();
};
