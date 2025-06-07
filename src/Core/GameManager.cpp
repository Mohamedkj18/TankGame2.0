#include <iostream>
#include <fstream>
#include <cmath>
#include <memory>
#include <algorithm>
#include "Core/GameManager.h"
#include "Core/Tank.h"
#include "Core/Shell.h"
// #include "TankAlgorithm/TankChase.h"
// #include "TankAlgorithm/TankEvasion.h"
#include "Core/MySatelliteView.h"
#include "Common/ActionRequest.h"
#include "Core/MyPlayer.h"

std::ofstream visualizationFile("data/visualization.txt");
std::ofstream outputFile;
// ------------------------ GameManager ------------------------

GameManager::GameManager(TankAlgorithmFactory &tank_factory,
                         PlayerFactory &player_factory)
    : tankFactory(tank_factory), playerFactory(player_factory)
{
    gameStep = 0;
    totalShellsRemaining = 0;
    playerTanksCount[1] = 0;
    playerTanksCount[2] = 0;
}

void GameManager::processInputFile(const std::string &inputFilePath)
{
    size_t lastSlash = inputFilePath.find_last_of("/\\");
    std::string inputFileName = (lastSlash == std::string::npos) ? inputFilePath : inputFilePath.substr(lastSlash + 1);

    std::string outputFileName = "data/output_" + inputFileName;

    outputFile.open(outputFileName);
    if (!outputFile.is_open())
    {
        std::cerr << "Error: Could not create output file: " << outputFileName << std::endl;
        return;
    }

    std::cout << "Output written to: " << outputFileName << std::endl;
}

int GameManager::getWidth() { return width; }

int GameManager::getHeight() { return height; }

int GameManager::getGameStep() { return gameStep; }

int GameManager::getWallHealth(int wallPos)
{
    return walls[wallPos].health;
}

void GameManager::incrementGameStep() { gameStep++; }

void GameManager::addTank(std::unique_ptr<Tank> tank)
{
    tanks[bijection(tank->getX(), tank->getY())] = std::move(tank);
}

// Tank *GameManager::getPlayer(int playerId)
// {
//     if (playerId <= 0 || playerId > (int)players.size())
//     {
//         std::cerr << "Invalid player ID: " << playerId << std::endl;
//         return nullptr;
//     }
//     return players[playerId - 1];
// }

void GameManager::addShell(std::unique_ptr<Shell> shell)
{
    int newPos = bijection(shell->getX(), shell->getY());

    if (shellsFired.count(newPos))
    {
        shellsToRemove.insert(newPos);
        outputFile << "shells collided at (" << shells[newPos]->getX() / 2 << ", " << shells[newPos]->getY() / 2 << ") !\n";
    }

    shellsFired.insert(newPos);
    shells[newPos] = std::move(shell);
}

void GameManager::advanceShellsRecentlyFired()
{
    std::unordered_set<int> newFired;

    for (int oldPos : shellsFired)
    {
        auto it = shells.find(oldPos);
        if (it == shells.end())
            continue;

        std::unique_ptr<Shell> &shell = it->second;
        shell->moveForward();
        int newPos = bijection(shell->getX(), shell->getY());

        if (shells.count(newPos))
        {
            shellsToRemove.insert(newPos); // handle collision
        }

        shells[newPos] = std::move(shell); // move it in the map
        newFired.insert(newPos);
        shells.erase(oldPos);
    }

    shellsFired = std::move(newFired);
}

void GameManager::addMine(int x, int y)
{
    mines.insert(bijection(x, y));
}

void GameManager::addWall(int x, int y)
{
    Wall wall = {x, y, 2};
    walls[bijection(x, y)] = wall;
}

void GameManager::removeMine(int x)
{
    mines.erase(x);
}

void GameManager::removeWall(int x)
{
    walls.erase(x);
}

void GameManager::removeTank(int tankPos)
{
    playerTanksCount[tanks[tankPos]->getPlayerId()]--;
    tanks.erase(tankPos);
}

void GameManager::removeShell(int ShellPos)
{
    shells.erase(ShellPos);
}

void GameManager::hitTank(int tankId)
{
    if (tanks.count(tankId))
    {
        tanks[tankId]->hit();
    }
}

std::vector<std::string> GameManager::splitByComma(const std::string &input)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = input.find(',');

    while (end != std::string::npos)
    {
        tokens.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(',', start);
    }
    tokens.push_back(input.substr(start));

    return tokens;
}

int GameManager::readFile(std::string fileName)
{
    int tankId1 = 0;
    int tankId2 = 0;

    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cerr << "Failed to open input file: " << fileName << std::endl;
        return -1;
    }

    std::string line;
    std::getline(file, line); // map name/description – ignore

    auto parseConfigLine = [](const std::string &line, const std::string &key)
    {
        auto pos = line.find('=');
        if (pos == std::string::npos)
            throw std::runtime_error("Invalid config line: " + line);
        std::string k = line.substr(0, pos);
        std::string v = line.substr(pos + 1);
        k.erase(remove_if(k.begin(), k.end(), ::isspace), k.end());
        v.erase(remove_if(v.begin(), v.end(), ::isspace), v.end());
        if (k != key)
            throw std::runtime_error("Expected key: " + key + ", got: " + k);
        return std::stoi(v);
    };

    try
    {
        std::getline(file, line);
        maxSteps = parseConfigLine(line, "MaxSteps");

        std::getline(file, line);
        numShellsPerTank = parseConfigLine(line, "NumShells");
        std::cout << "Number of shells per tank: " << numShellsPerTank << std::endl;

        std::getline(file, line);
        height = parseConfigLine(line, "Rows");

        std::getline(file, line);
        width = parseConfigLine(line, "Cols");
    }
    catch (std::exception &e)
    {
        std::cerr << "Error parsing config: " << e.what() << std::endl;
        return -1;
    }
    std::cout << "Game configuration: " << width << "x" << height << ", Max Steps: " << maxSteps
              << ", Shells per Tank: " << numShellsPerTank << std::endl;

    int y = 0;
    std::string tankChars = "0123456789";

    while (std::getline(file, line) && y < height)
    {
        for (int x = 0; x < std::min((int)line.size(), width); ++x)
        {
            char c = line[x];
            if (c == '#')
            {
                addWall(x * 2, y * 2);
            }
            else if (c == '@')
            {
                addMine(x * 2, y * 2);
            }
            else if (tankChars.find(c) != std::string::npos)
            {
                int playerId = (c == '1') ? 1 : 2; // use 1/2 logic or infer player from symbol set
                auto tank = std::make_unique<Tank>(x * 2, y * 2,
                                                   (playerId == 1) ? stringToDirection["L"] : stringToDirection["R"],
                                                   this, playerId, numShellsPerTank, (playerId == 1) ? tankId1++ : tankId2++);

                playerTanksCount[playerId]++;
                totalShellsRemaining += numShellsPerTank;
                addTank(std::move(tank));
            }
        }
        ++y;
    }

    if (checkForAWinner())
    {
        outputFile.close();
        return 1;
    }

    file.close();
    printBoard();

    players[1] = playerFactory.create(1, width, height, maxSteps, numShellsPerTank);
    players[2] = playerFactory.create(2, width, height, maxSteps, numShellsPerTank);

    return 0;
}

void GameManager::checkForAMine(int x, int y, int tankId)
{
    int currTankPos = bijection(x, y);
    if (mines.count(currTankPos))
    {
        outputFile << "Losing step: Tank " << tankId << " hit a mine at " << (int)x / 2 << ", " << (int)y / 2 << "!\n";
        removeMine(bijection(x, y));
        tanksToRemove.insert(currTankPos);
    }
}

void GameManager::tankHitByAShell(int tankPos)
{
    if (shells.count(tankPos) && tanks.count(tankPos))
    {
        outputFile << "Shell hit tank " << tanks[tankPos]->getTankId()
                   << " at (" << shells[tankPos]->getX() / 2 << ", "
                   << shells[tankPos]->getY() / 2 << ")\n";
    }
    shellsToRemove.insert(tankPos);
    tanksToRemove.insert(tankPos);
}

void GameManager::shellHitAWall(int wallPos)
{
    if (shells.count(wallPos))
    {
        outputFile << "Shell hit wall at ("
                   << shells[wallPos]->getX() / 2 << ", " << shells[wallPos]->getY() / 2 << ")\n";
    }
    else
    {
        outputFile << "Unknown Shell hit a wall at unknown position.\n";
    }

    if (getWallHealth(wallPos) <= 0)
    {
        wallsToRemove.insert(wallPos);
    }
}

void GameManager::advanceShells()
{
    Shell *shell;
    int newPos;
    bool didItMove;
    for (const auto &pair : shells)
    {
        shell = shells[pair.first].get();
        didItMove = shell->moveForward();
        newPos = bijection(shell->getX(), shell->getY());
        if (didItMove)
            checkForShellCollision(*shell);
        else
            shellHitAWall(newPos);
    }
    shells = std::move(secondaryShells);
    secondaryShells.clear();
}

void GameManager::reverseHandler(Tank &tank, ActionRequest move)
{
    if (move == ActionRequest::MoveForward)
    {
        outputFile << "Tank " << tank.getTankId() << " canceled reverse!\n";
        tank.resetReverseState();
        tank.setLastMove(ActionRequest::DoNothing);
    }
    else if (tank.isReverseQueued())
    {
        outputFile << "Bad step: Tank " << tank.getTankId() << " waiting to move backwards - move ignored!\n";
        tank.incrementReverseCharge();
        if (tank.isReverseReady())
            tank.executeReverse();
    }

    else if (move == ActionRequest::MoveBackward)
    {
        tank.queueReverse();
        tank.incrementReverseCharge();
        outputFile << "Tank " << tank.getTankId() << " queued reverse!\n";
        if (tank.isReverseReady())
        {
            outputFile << "Tank " << tank.getX() << " moved backwards!\n";
            tank.executeReverse();
        }
    }
    checkForAMine(tank.getX(), tank.getY(), tank.getTankId());
}

void GameManager::advanceTank(Tank &tank)
{
    tank.resetReverseState();
    tank.moveForward();
    checkForAMine(tank.getX(), tank.getY(), tank.getTankId());
}

void GameManager::tankShootingShells(Tank &tank)
{
    tank.resetReverseState();
    if (tank.canShoot())
    {
        tank.fire();
        totalShellsRemaining--;
        tank.incrementCantShoot();
    }
    else
    {
        outputFile << "Bad step: Tank " << tank.getTankId() << " can't shoot!\n";
    }
    tank.setLastMove(ActionRequest::DoNothing);
}

void GameManager::rotate(Tank &tank)
{
    tank.resetReverseState();
    ActionRequest move = tank.getLastMove();
    if (move == ActionRequest::RotateLeft90)
        tank.rotateTank(-0.25);
    else if (move == ActionRequest::RotateRight90)
        tank.rotateTank(0.25);
    else if (move == ActionRequest::RotateLeft45)
        tank.rotateTank(-0.125);
    else if (move == ActionRequest::RotateRight45)
        tank.rotateTank(0.125);

    tank.setLastMove(ActionRequest::DoNothing);
}

void GameManager::checkForTankCollision(Tank &tank)
{
    int currTankPos = bijection(tank.getX(), tank.getY());
    if (secondaryTanks.count(currTankPos))
    {
        outputFile << "Losing step: Tank " << secondaryTanks[currTankPos]->getTankId() << " hit a tank at " << (int)tank.getX() / 2 << ", " << (int)tank.getY() / 2 << "!\n";
        tanksToRemove.insert(currTankPos);
    }
    if (shells.count(currTankPos))
    {
        tankHitByAShell(currTankPos);
    }
    secondaryTanks[currTankPos] = std::make_unique<Tank>(std::move(tank));
}

void GameManager::checkForShellCollision(Shell &shell)
{
    int shellPos = bijection(shell.getX(), shell.getY());
    if (tanks.count(shellPos))
        tankHitByAShell(shellPos);
    if (secondaryShells.count(shellPos))
        shellsToRemove.insert(shellPos);
    secondaryShells[shellPos] = std::make_unique<Shell>(shell);
}

void GameManager::executeTanksMoves()
{
    ActionRequest move;
    for (const auto &pair : tanks)
    {

        Tank *tank = pair.second.get(); // pointer from unique_ptr
        move = tank->getLastMove();
        if (tank->getCantShoot())
        {
            tank->incrementCantShoot();
            if (tank->getCantShoot() == 8)
                tank->resetCantShoot();
        }

        if (tank->isReverseQueued() || move == ActionRequest::MoveBackward)
        {
            reverseHandler(*tank, move); // pass by reference
        }
        else if (move == ActionRequest::MoveForward)
        {
            advanceTank(*tank);
        }
        else if (move == ActionRequest::Shoot)
        {
            tankShootingShells(*tank);
        }
        else
        {
            rotate(*tank);
        }

        checkForTankCollision(*tank);
    }

    tanks = std::move(secondaryTanks);
    secondaryTanks.clear();
}

void GameManager::executeBattleInfoRequests()
{
    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second.get();

        if (tank->getLastMove() == ActionRequest::GetBattleInfo)
        {
            TankAlgorithm *tankAlgorithm = tank->getTankAlgorithm();
            int pos = bijection(tank->getX(), tank->getY());
            MySatelliteView satelliteView(pos, tanks, shells, mines, walls);
            players[tank->getPlayerId()]->updateTankWithBattleInfo(*tankAlgorithm, satelliteView);
            tank->setLastMove(ActionRequest::DoNothing);
        }
    }
}

int GameManager::bijection(int x, int y)
{
    return ((x + y) * (x + y + 1)) / 2 + y;
}

void GameManager::removeTanks()
{
    for (int object : tanksToRemove)
    {
        removeTank(object);
    }
    tanksToRemove.clear();
}

void GameManager::removeShells()
{
    for (int object : shellsToRemove)
    {
        removeShell(object);
    }
    shellsToRemove.clear();
}

void GameManager::removeWalls()
{
    for (int object : wallsToRemove)
    {
        removeWall(object);
    }
    wallsToRemove.clear();
}

void GameManager::removeObjectsFromTheBoard()
{
    removeTanks();
    removeShells();
    removeWalls();
}

bool GameManager::checkForAWinner()
{
    if (playerTanksCount[1] == 0 && playerTanksCount[2] > 0)
    {
        outputFile << "Game Over! Player 2 wins!\n";
        return true;
    }
    else if (playerTanksCount[2] == 0 && playerTanksCount[1] > 0)
    {
        outputFile << "Game Over! Player 1 wins!\n";
        return true;
    }
    else if (playerTanksCount[1] == 0 && playerTanksCount[2] == 0)
    {
        outputFile << "Game Over! Both players have no tanks left!\n";
        return true;
    }

    return false;
}

void GameManager::outputTankMove(int playerNum, ActionRequest move, int tankId)
{

    outputFile << "Player " << playerNum << " tank " << tankId <<" moved: " << to_string(move) << std::endl;
}

void GameManager::runGame()
{

    int count = 0;
    std::string move;

    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second.get();
        std::unique_ptr<TankAlgorithm> algo = tankFactory.create(tank->getPlayerId(), tank->getTankId());
        tank->setTankAlgorithm(std::move(algo));
    }
    while (true)
    {
        outputFile << "Game step: " << gameStep << std::endl;

        for (const auto &pair : tanks)
        {
            Tank *tank = pair.second.get();
            TankAlgorithm *algo = tank->getTankAlgorithm();
            ActionRequest move = algo->getAction();
            tank->setLastMove(move);
            outputTankMove(tank->getPlayerId(), move, tank->getTankId());
        }
        executeBattleInfoRequests();

        advanceShells();
        removeShells();
        advanceShells();
        removeObjectsFromTheBoard();

        executeTanksMoves();
        advanceShellsRecentlyFired();
        removeTanks();
        removeShells();
        executeTanksMoves();
        removeObjectsFromTheBoard();

        advanceShells();
        removeShells();
        advanceShells();
        removeObjectsFromTheBoard();

        if (checkForAWinner())
        {
            outputFile.close();
            return;
        }
        else if (gameStep >= maxSteps)
        {
            outputFile << "Game Over! It's a tie due to max steps reached!\n";
            outputFile.close();
            return;
        }
        else if (totalShellsRemaining <= 0 && gameStep >= maxSteps / 2)
        {
            outputFile << "Game Over! It's a tie due to no shells remaining!\n";
            outputFile.close();
            return;
        }
        else if (totalShellsRemaining <= 0)
        {
            count++;
            if (count == 40)
            {
                outputFile << "Game Over! It's a tie due to time out!\n";
                outputFile.close();
                return;
            }
        }
        gameStep++;
        printBoard();
    }
}

void GameManager::printBoard()
{
    std::vector<std::vector<char>> board(height, std::vector<char>(width, '.'));
    std::pair<int, int> xy;

    for (const auto &pair : walls)
    {
        int x = pair.second.x / 2;
        int y = pair.second.y / 2;
        board[y][x] = '#';
    }

    for (const auto &mine : mines)
    {
        xy = inverseBijection(mine);
        int x = xy.first / 2;
        int y = xy.second / 2;
        board[y][x] = '@';
    }

    for (const auto &pair : shells)
    {
        Shell *a = pair.second.get();
        int x = a->getX() / 2;
        int y = a->getY() / 2;
        board[y][x] = '*';
    }

    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second.get();
        int x = tank->getX() / 2;
        int y = tank->getY() / 2;
        char symbol = '0' + (tank->getPlayerId() % 10);
        board[y][x] = symbol;
    }

    visualizationFile << "\n=== Game Step " << gameStep << " ===\n";
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            visualizationFile << board[y][x];
        }
        visualizationFile << '\n';
    }
    visualizationFile << std::endl;
}
