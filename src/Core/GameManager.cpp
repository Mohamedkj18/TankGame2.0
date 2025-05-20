#include "Core/GameManager.hpp"
#include "Core/Tank.hpp"
#include "Core/Shell.hpp"
#include "TankAlgorithm/TankChase.hpp"
#include "TankAlgorithm/TankEvasion.hpp"
#include "Common/ActionRequest.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

std::ofstream visualizationFile("data/visualization.txt");
std::ofstream outputFile;
// ------------------------ GameManager ------------------------

GameManager(TankAlgorithmFactory &tank_factory,
            PlayerFactory &player_factory)
{
    processInputFile(inputFile);
    gameStep = 0;
    totalShellsRemaining = 0;
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

std::unordered_map<int, Tank *> GameManager::getTanks() { return tanks; }

std::unordered_map<int, Shell *> GameManager::getShell() { return Shells; }

std::set<int> &GameManager::getMines() { return mines; }

std::unordered_map<int, Wall> &GameManager::getWalls() { return walls; }

void GameManager::incrementGameStep() { gameStep++; }

void GameManager::addTank(Tank *tank)
{
    tanks[bijection(tank->getX(), tank->getY())] = tank;
}

int GameManager::bijection(int x, int y)
{
    return ((int)((x + y) * (x + y + 1)) * 0.5) + y;
}

std::pair<int, int> GameManager::inverseBijection(int z)
{
    int w = static_cast<int>(std::floor((std::sqrt(8 * z + 1) - 1) / 2));
    int t = (w * w + w) / 2;
    int y = z - t;
    int x = w - y;
    return {x, y};
}

Tank *GameManager::getPlayer(int playerId)
{
    if (playerId <= 0 || playerId > (int)players.size())
    {
        std::cerr << "Invalid player ID: " << playerId << std::endl;
        return nullptr;
    }
    return players[playerId - 1];
}

void GameManager::addShell(Shell *Shell)
{
    int newPos = bijection(Shell->getX(), Shell->getY());
    if (ShellsFired.count(newPos))
    {
        shellsToRemove.insert(newPos);
        outputFile << "Shells collided at (" << Shell->getX() / 2 << ", " << Shell->getY() / 2 << ") !\n";
    }
    ShellsFired[newPos] = Shell;
}

void GameManager::advanceShellsRecentlyFired()
{
    for (int shellPos : shellsToRemove)
    {
        removeShell(shellPos);
        ShellsFired.erase(shellPos);
    }
    for (const auto &pair : ShellsFired)
    {
        Shell *Shell = pair.second;
        Shells.erase(pair.first);
        Shell->moveForward();
        int newPos = bijection(Shell->getX(), Shell->getY());
        if (Shells.count(newPos))
            shellsToRemove.insert(newPos);
        else if (tanks.count(newPos))
        {
            Shells[newPos] = Shell;
            tankHitByAShell(newPos);
        }
        else if (walls.count(newPos))
        {
            shellsToRemove.insert(newPos);
            ShellHitAWall(newPos);
        }
        Shells[newPos] = Shell;
    }
    ShellsFired.clear();
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
    delete tanks[tankPos];
    tanks.erase(tankPos);
}

void GameManager::removeShell(int ShellPos)
{
    delete Shells[ShellPos];
    Shells.erase(ShellPos);
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
    int xAxis = 0;
    int yAxis = 0;
    bool foundErrors = false;
    std::ofstream errorsFile;
    std::vector<std::string> errorLogs;

    std::ifstream file(fileName);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }

    getline(file, line);
    std::vector<std::string> params = splitByComma(line);
    height = std::stoi(params[0]);
    width = std::stoi(params[1]);

    int declaredHeight = height;
    int declaredWidth = width;

    while (std::getline(file, line) && yAxis < declaredHeight)
    {
        xAxis = 0;
        for (char c : line)
        {
            if (xAxis >= declaredWidth)
            {
                foundErrors = true;
                errorLogs.push_back("Ignored extra column at row " + std::to_string(yAxis));
                break;
            }

            if (c == '#')
            {
                addWall(xAxis * 2, yAxis * 2);
            }
            else if (c == '@')
            {
                addMine(xAxis * 2, yAxis * 2);
            }
            else if (c == '1')
            {
                if (players[0] == nullptr)
                {
                    totalShellsRemaining += 16;
                    Tank *player1 = new Tank(xAxis * 2, yAxis * 2, stringToDirection["L"], this, 1);
                    addTank(player1);
                    players[0] = player1;
                }
                else
                {
                    foundErrors = true;
                    errorLogs.push_back("Ignored extra tank for Player 1 at position (" + std::to_string(xAxis) + "," + std::to_string(yAxis) + ")");

                    Tank *redundantTank = new Tank(xAxis * 2, yAxis * 2, stringToDirection["L"], this, 1);
                    delete redundantTank; // Prevent memory leak
                }
            }
            else if (c == '2')
            {
                if (players[1] == nullptr)
                {
                    totalShellsRemaining += 16;
                    Tank *player2 = new Tank(xAxis * 2, yAxis * 2, stringToDirection["R"], this, 2);
                    addTank(player2);
                    players[1] = player2;
                }
                else
                {
                    foundErrors = true;
                    errorLogs.push_back("Ignored extra tank for Player 2 at position (" + std::to_string(xAxis) + "," + std::to_string(yAxis) + ")");

                    Tank *redundantTank = new Tank(xAxis * 2, yAxis * 2, stringToDirection["R"], this, 2);
                    delete redundantTank; // Prevent memory leak
                }
            }
            else if (c != ' ')
            {
                foundErrors = true;
                errorLogs.push_back("Ignored unknown character '" + std::string(1, c) + "' at position (" + std::to_string(xAxis) + "," + std::to_string(yAxis) + ")");
            }

            xAxis++;
        }

        if (xAxis < declaredWidth)
        {
            foundErrors = true;
            errorLogs.push_back("Missing columns at row " + std::to_string(yAxis));
        }

        yAxis++;
    }

    if (yAxis < declaredHeight)
    {
        foundErrors = true;
        errorLogs.push_back("Missing rows after line " + std::to_string(yAxis - 1));
    }

    file.close();

    if (foundErrors)
    {
        errorsFile.open("data/input_errors.txt");
        errorsFile << "Recovered errors found in input file:\n";
        for (const auto &err : errorLogs)
        {
            errorsFile << "- " << err << std::endl;
        }
        errorsFile.close();
    }

    if (players[0] == nullptr || players[1] == nullptr)
    {
        return 1;
    }

    printBoard();
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
    if (Shells.count(tankPos) && tanks.count(tankPos))
    {
        outputFile << "Shell hit tank " << tanks[tankPos]->getId()
                   << " at (" << Shells[tankPos]->getX() / 2 << ", " << Shells[tankPos]->getY() / 2 << ")\n";
    }
    shellsToRemove.insert(tankPos);
    tanksToRemove.insert(tankPos);
}

void GameManager::ShellHitAWall(int wallPos)
{
    if (Shells.count(wallPos))
    {
        outputFile << "Shell hit wall at ("
                   << Shells[wallPos]->getX() / 2 << ", " << Shells[wallPos]->getY() / 2 << ")\n";
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
    Shell *Shell;
    int newPos;
    bool didItMove;
    for (const auto &pair : Shells)
    {
        Shell = Shells[pair.first];
        didItMove = Shell->moveForward();
        newPos = bijection(Shell->getX(), Shell->getY());
        if (didItMove)
            checkForShellCollision(Shell);
        else
            ShellHitAWall(newPos);
    }
    Shells = secondaryShells;
    secondaryShells.clear();
}

void GameManager::reverseHandler(Tank *tank, ActionRequest move)
{
    if (move == MoveForward)
    {
        outputFile << "Tank " << tank->getId() << " canceled reverse!\n";
        tank->resetReverseState();
        tank->setLastMove(DoNothing);
    }
    else if (tank->isReverseQueued())
    {
        outputFile << "Bad step: Tank " << tank->getId() << " waiting to move backwards - move ignored!\n";
        tank->incrementReverseCharge();
        if (tank->isReverseReady())
            tank->executeReverse();
    }

    else if (move == MoveBackward)
    {
        tank->queueReverse();
        tank->incrementReverseCharge();
        outputFile << "Tank " << tank->getId() << " queued reverse!\n";
        if (tank->isReverseReady())
        {
            outputFile << "Tank " << tank->getX() << " moved backwards!\n";
            tank->executeReverse();
        }
    }
    checkForAMine(tank->getX(), tank->getY(), tank->getId());
}

void GameManager::advanceTank(Tank *tank)
{
    tank->resetReverseState();
    tank->moveForward();
    checkForAMine(tank->getX(), tank->getY(), tank->getId());
}

void GameManager::tankShootingShells(Tank *tank)
{
    tank->resetReverseState();
    if (tank->canShoot())
    {
        tank->fire();
        totalShellsRemaining--;
        outputFile << "Tank " << tank->getId() << " fired!\n";
        tank->incrementCantShoot();
    }
    else
    {
        outputFile << "Bad step: Tank " << tank->getId() << " can't shoot!\n";
    }
    tank->setLastMove(DoNothing);
}

void GameManager::rotate(Tank *tank)
{
    tank->resetReverseState();
    ActionRequest move = tank->getLastMove();
    if (move == RotateLeft90)
        tank->rotateTank(90);
    else if (move == RotateRight90)
        tank->rotateTank(-90);
    else if (move == RotateLeft45)
        tank->rotateTank(45);
    else if (move == RotateRight45)
        tank->rotateTank(-45);

    tank->setLastMove(DoNothing);
}

void GameManager::checkForTankCollision(Tank *tank)
{
    int currTankPos = bijection(tank->getX(), tank->getY());
    if (secondaryTanks.count(currTankPos))
    {
        outputFile << "Losing step: Tank " << secondaryTanks[currTankPos]->getId() << " hit a tank at " << (int)tank->getX() / 2 << ", " << (int)tank->getY() / 2 << "!\n";
        tanksToRemove.insert(currTankPos);
    }
    if (Shells.count(currTankPos))
    {
        tankHitByAShell(currTankPos);
    }
    secondaryTanks[currTankPos] = tank;
}

void GameManager::checkForShellCollision(Shell *Shell)
{
    int shellPos = bijection(Shell->getX(), Shell->getY());
    if (tanks.count(shellPos))
        tankHitByAShell(shellPos);
    if (secondaryShells.count(shellPos))
        shellsToRemove.insert(shellPos);
    secondaryShells[shellPos] = Shell;
}

void GameManager::executeTanksMoves()
{
    ActionRequest move;

    for (const auto pair : tanks)
    {

        Tank *tank = pair.second;

        move = tank->getLastMove();
        if (tank->getCantShoot())
        {
            tank->incrementCantShoot();
            if (tank->getCantShoot() == 8)
                tank->resetCantShoot();
        }
        if ((tank->isReverseQueued() || move == MoveBackward))
        {
            reverseHandler(tank, move);
        }
        else if (move == MoveForward)
            advanceTank(tank);
        else if (move == Shoot)
            tankShootingShells(tank);

        else
        {
            rotate(tank);
        }
        checkForTankCollision(tank);
    }

    tanks = secondaryTanks;
    secondaryTanks.clear();
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
    if (tanks.size() == 1)
    {
        outputFile << "Game Over! Player " << tanks.begin()->second->getId() << " wins!\n";
        return true;
    }
    return false;
}

bool GameManager::isItATie()
{
    if (tanks.empty())
    {
        outputFile << "Game Over! It's a tie!\n";
        return true;
    }
    return false;
}

void GameManager::outputTankMove(int playerNum, std::string move)
{
    outputFile << "Player " << playerNum << " moved: " << moveMap[move] << std::endl;
}

void GameManager::runGame()
{

    int count = 0;
    std::string move, n;
    TankChase *tankChase = new TankChase(this, 8);
    TankEvasion *tankEvasion = new TankEvasion(this, 8);

    while (true)
    {

        outputFile << "Game step: " << gameStep << std::endl;
        move = tankChase->getNextMove(1, 2);
        getPlayer(1)->setLastMove(move);
        outputTankMove(1, move);
        move = tankEvasion->getNextMove(2, 1);
        getPlayer(2)->setLastMove(move);
        outputTankMove(2, move);

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
        else if (isItATie())
        {
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
        printBoard();
        gameStep++;
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

    for (const auto &pair : Shells)
    {
        Shell *a = pair.second;
        int x = a->getX() / 2;
        int y = a->getY() / 2;
        board[y][x] = '*';
    }

    for (const auto &pair : tanks)
    {
        Tank *tank = pair.second;
        int x = tank->getX() / 2;
        int y = tank->getY() / 2;
        char symbol = '0' + (tank->getId() % 10);
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
