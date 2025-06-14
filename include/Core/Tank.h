#pragma once

#include <string>
#include <set>
#include "Core/GameObject.h"
#include "Core/GameManager.h"
#include "common/ActionRequest.h"
#include "common/TankAlgorithm.h"

class GameManager;
// ========================= CLASS: Tank =========================

class Tank : public GameObject
{
private:
    int playerId;
    int artilleryShells;
    int tankId;
    int tankGlobalId;
    bool destroyed;
    int cantShoot;
    int reverseCharge = 0;
    bool reverseQueued = false;
    bool reverseReady = false;
    std::unique_ptr<TankAlgorithm> tankAlgorithm;
    ActionRequest lastMove;

public:
    Tank(int x, int y, Direction dir, std::shared_ptr<GameManager> game, int playerId, int shells, int tankId, int tankGlobalId);

    // Position and state
    int getPlayerId();
    int getTankId();
    int getTankGlobalId();
    ActionRequest getLastMove();
    bool checkForAWall();

    // Movement
    void moveBackwards();
    void rotateTank(double angle);
    void setDirection(std::string directionStr);
    void setLastMove(ActionRequest currentMove);
    TankAlgorithm *getTankAlgorithm();
    void setTankAlgorithm(std::unique_ptr<TankAlgorithm> algorithm);

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
