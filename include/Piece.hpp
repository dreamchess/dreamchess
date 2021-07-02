/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#pragma once

#include <cstdint>

namespace DreamChess {
    /**
     * @brief "Describes every piece in the board"
     * @details "Each piece is ORed with the color to code its value"
     */
    enum Piece : uint16_t {
        PAWN = 1,
        KNIGHT = 2,
        BISHOP = 3,
        ROOK = 4,
        QUEEN = 5,
        KING = 6,

        WHITE = 8,
        BLACK = 16
    };
} // namespace DreamChess