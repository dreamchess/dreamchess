#include "../include/Board.hpp"

Board::Board::Board() : squares{new uint16_t[64]} {
    // TODO Init board using PIECE_COLOR | PIECE_TYPE (bitwise OR)
}

Board::Board::~Board() {
    delete[] squares;
}