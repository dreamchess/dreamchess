#pragma once

#include "../include/Piece.hpp"

#include <cstdint>
#include <map>
#include <string>

namespace DreamChess::Base {
    class Board final {
    public:
        Board();
        ~Board();
    private:
        bool turn = Piece::WHITE;
        uint16_t* squares;

		const std::map<uint8_t , Piece> fen_to_piece {
			{'p', Piece::PAWN},
	        {'n', Piece::KNIGHT},
	        {'b', Piece::BISHOP},
	        {'r', Piece::ROOK},
	        {'q', Piece::QUEEN},
			{'k', Piece::KING}
		};

		void init_board();
    };
}
