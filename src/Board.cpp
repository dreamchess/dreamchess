#include "../include/Board.hpp"

#include <sstream>
#include <array>

namespace DreamChess::Base {
    Board::Board() : squares{new uint16_t[64]} {
        init_board();
    }

    Board::~Board() {
        delete[] squares;
    }

    void Board::init_board() {
        uint16_t file = 0;
        uint16_t rank = 7;

        std::array<std::string, 6> splitted_fen;
        std::stringstream stream{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
        std::string tmp;

        for(uint64_t i = 0; i < 6; i++) {
            std::getline(stream, tmp, ' ');
            splitted_fen[i] = tmp;
        }

        for (auto& sym : splitted_fen[0]) {
            if (sym == '/') {
                file = 0;
                rank--;
            } else {
                if (isdigit(sym)) {
                    file += sym - '0';
                } else {
                    Piece color = isupper(sym) ? Piece::WHITE : Piece::BLACK;
                    Piece type = Board::fen_to_piece.at(static_cast<uint8_t>(std::tolower(sym)));

                    squares[rank * 8 + file] = color | type;
                    file++;
                }
            }
        }
    }
}