#pragma once

#include "../include/Piece.hpp"

#include <cstdint>
#include <map>
#include <string>

namespace Board {
    class Board final {
    private:
        bool turn = 0;
        uint16_t* squares;

		const std::map<char, Piece> fen_to_piece {
			{'p', Piece::PAWN},
	        {'n', Piece::KNIGHT},
	        {'b', Piece::BISHOP},
	        {'r', Piece::ROOK},
	        {'q', Piece::QUEEN},
			{'k', Piece::KING}
		};

		void init_board();
    public:
        Board();
        ~Board();
    };
};