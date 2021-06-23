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

		static std::map<char, Piece> fen_to_piece;

		void init_board();
		void set_squares(int pos, uint16_t value);
    public:
        Board();
        ~Board();
    };
};