/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#include "../include/Game.hpp"

#include <iostream>

int main() {
    DreamChess::Board board {};

    std::cout << board.to_fen() << std::endl;

    return 0;
}
