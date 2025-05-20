#include "GameManager.hpp"
#include "MyPlayerFactory.hpp"
#include "MyTankAlgorithmFactory.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: tanks_game <input_file>" << std::endl;
        return 1;
    }

    const std::string inputFile = argv[1];
    GameManager game(MyPlayerFactory(), MyTankAlgorithmFactory());

    if (!game.readBoard(inputFile))
    {
        std::cerr << "Failed to read board from file: " << inputFile << std::endl;
        return 1;
    }

    game.run();

    return 0;
}
