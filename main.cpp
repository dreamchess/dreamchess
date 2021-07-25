/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#include "../include/Game.hpp"

#include <iostream>

int main() {
    DreamChess::Game game;

    // TODO Qui da segfault!!!
    std::cout << game.get_board();

    return 0;
}
