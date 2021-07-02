/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#include "../include/Board.hpp"

#include <array>
#include <sstream>

namespace DreamChess {
    /**
     * @brief "Constructs a `Board`"
     * @details "Starts with the neutral FEN string, using `init_board()`"
     */
    Board::Board() : m_squares {new uint16_t[64]} {
        init_board();
        generate_legal_moves();
    }

    // TODO Aggiungere linee per scacchiera
    /**
     * @brief "`out-stream` operator overloading"
     * @details "Print each piece and ends line every 8 files"
     * @param stream The new output stream
     * @param board The printed board
     * @return The output stream
     */
    std::ostream &operator<<(std::ostream &stream, const Board &board) {
        for(uint64_t i = 0; i < 64; i++) {
            stream << board.m_piece_repr.at(board.m_squares[i]);

            if((i + 1) % 8 == 0) { stream << std::endl; }
        }

        return stream;
    }

    /**
     * @brief "Used to init the board with the neutral FEN configuration"
     * @details "Parse the FEN string and inits the board"
     */
    void Board::init_board() {
        uint16_t file = 0;
        uint16_t rank = 7;

        std::array<std::string, 6> splitted_fen;
        std::stringstream stream {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

        std::string tmp;

        for(uint64_t i = 0; i < 6; i++) {
            std::getline(stream, tmp, ' ');
            splitted_fen[i] = tmp;
        }

        for(auto &sym : splitted_fen[0]) {
            if(sym == '/') {
                file = 0;
                rank--;
            } else {
                if(isdigit(sym)) {
                    file += sym - '0';
                } else {
                    Piece color = isupper(sym) ? Piece::WHITE : Piece::BLACK;
                    Piece type = Board::m_fen_to_piece.at(
                        static_cast<uint8_t>(std::tolower(sym)));

                    m_squares[rank * 8 + file] = color | type;
                    file++;
                }
            }
        }
    }

    void Board::generate_legal_moves() {
        for(uint64_t i = 0; i < 64; i++) {
            switch(m_squares[i]) {
                case 9:
                case 17:
                    m_legal_moves.push_back(
                        Move {m_squares[i], m_squares[i + 8], Piece::PAWN});
                    m_legal_moves.push_back(
                        Move {m_squares[i], m_squares[i + 16], Piece::PAWN});
            }
        }
    }
} // namespace DreamChess