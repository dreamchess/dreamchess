#pragma once

#include "../include/Piece.hpp"

#include <cstdint>
#include <map>
#include <string>

namespace DreamChess {
    class Board final {
    public:
        Board();
        ~Board();

        friend std::ostream& operator<<(std::ostream&, const Board&);

    private:
        bool turn = Piece::WHITE;
        uint16_t* squares;

        const std::map<uint8_t, Piece> fen_to_piece {
            { 'p', Piece::PAWN },
            { 'n', Piece::KNIGHT },
            { 'b', Piece::BISHOP },
            { 'r', Piece::ROOK },
            { 'q', Piece::QUEEN },
            { 'k', Piece::KING }
        };

        const std::map<uint16_t, uint8_t> piece_repr {
            {9, 'P'},
            {12, 'R'},
            {10, 'N'},
            {11, 'B'},
            {13, 'Q'},
            {14, 'K'},
            {0, ' '},
            {17, 'p'},
            {20, 'r'},
            {18, 'n'},
            {19, 'b'},
            {21, 'q'},
            {22, 'k'}
        };

        void init_board();
    };
}
