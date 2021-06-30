#pragma once

#include <cstdint>

namespace DreamChess {
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
}