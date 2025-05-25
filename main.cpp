#include "Core/GameManager.h"
#include "Core/MyPlayerFactory.h"
#include "Core/MyTankAlgorithmFactory.h"

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

    GameManager game{tankFactory, playerFactory};
    game.processInputFile(inputFile);
    if (game.readFile(inputFile))
    {
        std::cerr << "Failed to read board from file: " << inputFile << std::endl;
        return 1;
    }
    std::cout << "Game initialized successfully." << std::endl;
    game.runGame();

    return 0;
}
