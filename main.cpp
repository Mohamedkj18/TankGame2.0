int main(int argc, char **argv)
{
    GameManager game(MyPlayerFactory(), MyTankAlgorithmFactory());
    game.readBoard(<game_board_input_file from command line>);
    game.run();
}
