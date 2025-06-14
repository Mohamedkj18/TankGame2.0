#include "Core/GameManager.h"
#include "Core/MyPlayerFactory.h"
#include "Algorithms/MyTankAlgorithmFactory.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: tanks_game <input_file>" << std::endl;
        return 1;
    }

    const std::string inputFile = argv[1];
    MyTankAlgorithmFactory tankFactory;
    MyPlayerFactory playerFactory;

    auto game = std::make_shared<GameManager>(tankFactory, playerFactory);
    game->processInputFile(inputFile);
    int rval = game->readFile(inputFile, game);
    if (rval == -1)
    {
        std::cerr << "Failed to read board from file: " << inputFile << std::endl;
        return 1;
    }
    else if (rval == 1)
    {
        std::cout << "Game Ended before step 0." << std::endl;
        return 0;
    }
    std::cout << "Game initialized successfully." << std::endl;
    game->runGame();
    std::cout << "Game finished successfully." << std::endl;

    return 0;
}
