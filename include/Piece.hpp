#pragma once

#include <cstdint>

enum class Piece : uint16_t {
	NONE = 0,

    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
	QUEEN = 5,
	KING = 6,

	WHITE = 8,
	BLACK = 16
};
