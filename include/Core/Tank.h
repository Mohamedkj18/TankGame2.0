#pragma once

#include "Core/GameObject.hpp"
#include "Core/Game.hpp"
#include <string>
#include <set>

// ========================= CLASS: Tank =========================

class Tank : public MovingGameObject
{
private:
    int playerId;
    int tabkId;
    int artilleryShells;
    bool destroyed;
    int cantShoot;
    int reverseCharge = 0;
    bool reverseQueued = false;
    bool reverseReady = false;
    ActionRequest lastMove;

public:
    Tank(int x, int y, Direction dir, Game *game, int playerId);

    // Position and state
    int getplayerId();
    int getTankId();
    ActionRequest getLastMove();
    bool checkForAWall();

    // Movement
    void moveBackwards();
    void rotateTank(double angle);
    void setDirection(std::string directionStr);
    void setLastMove(ActionRequest lastMove);

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
