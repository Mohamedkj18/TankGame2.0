#pragma once

#include <unordered_map>
#include <map>

#include <unordered_set>

#include <vector>
#include <string>
#include <set>
#include <utility>
#include "utils/DirectionUtils.h"
#include <fstream>
#include <memory>
#include "Core/Tank.h"
#include "Core/Shell.h"
#include "common/PlayerFactory.h"
#include "common/TankAlgorithmFactory.h"
#include "common/ActionRequest.h"

class Tank;
class Shell;

extern std::ofstream outputFile;

class GameManager
{
private:
    int width;
    int height;
    int gameStep;
    int totalShellsRemaining;
    int maxSteps;
    int numShellsPerTank;
    int totalTanks;

    std::vector<std::string> movesOfTanks;
    std::unordered_map<int, std::unique_ptr<Player>> players;
    std::unordered_map<int, int> playerTanksCount;
    std::map<int, std::unique_ptr<Tank>> tanks;
    std::unordered_map<int, std::unique_ptr<Shell>> shells;
    std::set<int> mines;
    std::unordered_map<int, Wall> walls;
    std::set<int> wallsToRemove;
    std::set<int> tanksToRemove;
    std::set<int> shellsToRemove;
    std::unordered_map<int, std::unique_ptr<Shell>> secondaryShells;
    std::map<int, std::unique_ptr<Tank>> secondaryTanks;
    std::unordered_set<int> shellsFired;
    TankAlgorithmFactory &tankFactory;
    PlayerFactory &playerFactory;

public:
    GameManager(TankAlgorithmFactory &tank_factory,
                PlayerFactory &player_factory);

    int getWidth();
    int getHeight();
    int bijection(int x, int y);

    std::unordered_map<int, Wall> &getWalls() { return walls; }
    void removeTank(int tankId);
    void removeShell(int ShellPos);
    void addShell(std::unique_ptr<Shell> shell);

    int readFile(const std::string &fileName, std::shared_ptr<GameManager> self);
    void processInputFile(const std::string &inputFilePath);
    void runGame();

private:
    int getGameStep();

    std::map<int, std::unique_ptr<Tank>> &getTanks() { return tanks; }
    std::unordered_map<int, std::unique_ptr<Shell>> &getShells() { return shells; }
    std::set<int> &getMines() { return mines; }
    void getPlayersInput(std::ofstream &file);
    int getWallHealth(int wallPos);
    void incrementGameStep();
    void addTank(std::unique_ptr<Tank> tank);

    void addMine(int x, int y);
    void addWall(int x, int y);

    void removeMine(int x);
    void removeWall(int x);

    void removeTanks();
    void removeShells();
    void removeWalls();

    void hitWall(int x, int y);
    void hitTank(int tankId);

    void checkForAMine(int x, int y);
    void printBoard();

    void advanceShells();
    void advanceShellsRecentlyFired();
    void executeTanksMoves(bool firstPass);
    void executeBattleInfoRequests();
    void removeObjectsFromTheBoard();
    void reverseHandler(Tank &tank, ActionRequest move);
    void advanceTank(Tank &tank);
    void tankShootingShells(Tank &tank);
    void rotate(Tank &tank);
    void checkForTankCollision(Tank &tank);
    void checkForShellCollision(Shell &shell);
    void tankHitByAShell(int tankPos);
    void shellHitAWall(int shellPos);

    bool checkForAWinner();

    std::vector<std::string> splitByComma(const std::string &input);
    void sortTanks();
    void outputTankMoves();
};
