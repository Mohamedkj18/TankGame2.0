#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include "Core/Direction.hpp"
#include <fstream>

class Tank;
class Artillery;
class TankChase;

extern std::ofstream outputFile;
// ========================= CLASS: GameManager =========================
class GameManager
{
private:
    int width;
    int height;
    int gameStep;
    int totalShellsRemaining;

    std::array<Tank *, 2> players;
    std::unordered_map<int, Tank *> tanks;
    std::unordered_map<int, Shell *> shells;
    std::set<int> mines;
    std::unordered_map<int, Wall> walls;
    std::set<int> wallsToRemove;
    std::set<int> tanksToRemove;
    std::set<int> shellsToRemove;
    std::unordered_map<int, Shell *> secondaryshells;
    std::unordered_map<int, Tank *> secondaryTanks;
    std::unordered_map<int, Shell *> shellsFired;
    TankAlgorithmFactory &tankFactory;
    PlayerFactory &playerFactory;

protected:
public:
    GameManager(TankAlgorithmFactory &tank_factory,
                PlayerFactory &player_factory);

    int getWidth();
    int getHeight();
    int getGameStep();
    int bijection(int x, int y);
    std::pair<int, int> inverseBijection(int z);
    Tank *getPlayer(int playerId);
    std::unordered_map<int, Tank *> getTanks();
    std::unordered_map<int, Shell *> getShell();
    std::set<int> &getMines();
    std::unordered_map<int, Wall> &getWalls();
    void processInputFile(const std::string &inputFilePath);

    void getPlayersInput(std::ofstream &file);
    int getWallHealth(int wallPos);
    void incrementGameStep();
    void addTank(Tank *tank);
    void addShell(Shell *shell);
    void addMine(int x, int y);
    void addWall(int x, int y);

    void removeMine(int x);
    void removeWall(int x);
    void removeTank(int tankId);
    void removeShell(int ShellPos);
    void removeTanks();
    void removeshells();
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
    void reverseHandler(Tank *tank, std::string move);
    void advanceTank(Tank *tank);
    void tankShootingShells(Tank *tank);
    void rotate(Tank *tank);
    void checkForTankCollision(Tank *tank);
    void checkForShellCollision(Shell *shell);
    void tankHitByAShell(int tankPos);
    void ShellHitAWall(int shellPos);

    bool checkForAWinner();
    bool isItATie();
    void outputTankMove(int playerNum, std::string move);
};