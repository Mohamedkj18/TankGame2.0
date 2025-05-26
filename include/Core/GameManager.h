#pragma once

#include <unordered_map>
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
#include "Common/PlayerFactory.h"
#include "Common/TankAlgorithmFactory.h"
#include "Common/ActionRequest.h"

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

    std::unordered_map<int, int> playerTanksCount;
    std::unordered_map<int, std::unique_ptr<Tank>> tanks;
    std::unordered_map<int, std::unique_ptr<Shell>> shells;
    std::set<int> mines;
    std::unordered_map<int, Wall> walls;
    std::set<int> wallsToRemove;
    std::set<int> tanksToRemove;
    std::set<int> shellsToRemove;
    std::unordered_map<int, std::unique_ptr<Shell>> secondaryShells;
    std::unordered_map<int, std::unique_ptr<Tank>> secondaryTanks;
    std::unordered_set<int> shellsFired;
    TankAlgorithmFactory &tankFactory;
    PlayerFactory &playerFactory;

public:
    GameManager(TankAlgorithmFactory &tank_factory,
                PlayerFactory &player_factory);

    int getWidth();
    int getHeight();
    int getGameStep();
    int bijection(int x, int y);

    std::pair<int, int> inverseBijection(int z);
    void processInputFile(const std::string &inputFilePath);

    std::unordered_map<int, Wall> &getWalls() { return walls; }
    std::unordered_map<int, std::unique_ptr<Tank>> &getTanks() { return tanks; }
    std::unordered_map<int, std::unique_ptr<Shell>> &getShells() { return shells; }
    std::set<int> &getMines() { return mines; }
    void getPlayersInput(std::ofstream &file);
    int getWallHealth(int wallPos);
    void incrementGameStep();
    void addTank(std::unique_ptr<Tank> tank);
    void addShell(std::unique_ptr<Shell> shell);
    void addMine(int x, int y);
    void addWall(int x, int y);

    void removeMine(int x);
    void removeWall(int x);
    void removeTank(int tankId);
    void removeShell(int ShellPos);
    void removeTanks();
    void removeShells();
    void removeWalls();

    void hitWall(int x, int y);
    void hitTank(int tankId);

    int readFile(std::string fileName);
    std::vector<std::string> splitByComma(const std::string &input);
    void checkForAMine(int x, int y, int tankId);
    void runGame();
    void printBoard();
    void advanceShells();
    void advanceShellsRecentlyFired();
    void executeTanksMoves();
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
    bool isItATie();
    void outputTankMove(int playerNum, std::string move);
};
