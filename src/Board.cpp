/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#include "../include/Board.hpp"

#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace DreamChess {
    /**
     * @brief "Constructs a `Board`"
     * @details "Starts with the neutral FEN string, using `init_board()`"
     */
    Board::Board()
        : m_squares {new uint16_t[64]}
        , m_captured {new uint16_t[10]} {
        init_board();
    }

    /**
     * @brief "Copy constructor of `Board`"
     */
    Board::Board(const Board &board)
        : m_squares(board.m_squares.get())
        , m_captured(board.m_captured.get()) {}

    /**
     * @brief "Move constructor of `Board`"
     */
    Board::Board(Board &&board) noexcept
        : m_squares(std::move(board.m_squares))
        , m_captured(std::move(board.m_captured)) {}

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
     * @details "Parses the FEN string and inits the `Board`"
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

    /**
     * @brief "Makes a move in the current `Board`"
     * @details "Checks if the `Move` is a "special move", makes a "normal move"
     * otherwise"
     * @param move The `Move` to make
     */
    void Board::make_move(const Move &move) {
        if(move.is_valid()) {
            // En-passant
            if(move.m_piece == Piece::PAWN
               && m_squares[move.m_destination] == 0) {
                uint16_t en_passant
                    = move.m_destination
                    - 8 * (move.m_destination > move.m_source ? 1 : -1);
                m_captured[m_squares[en_passant]]++;
                m_squares[en_passant] = 0;
            }

            // Updating captured pieces
            if(move.m_destination != 0) {
                m_captured[m_squares[move.m_destination]]++;
            }

            // kingside castle
            if(move.m_piece == Piece::KING
               && move.m_destination - move.m_source == 2) {
                m_squares[move.m_destination - 1]
                    = m_squares[move.m_destination + 1];
                m_squares[move.m_destination + 1] = 0;
            }

            // Queenside castle
            if(move.m_piece == Piece::KING
               && move.m_source - move.m_destination == 2) {
                m_squares[move.m_destination + 1]
                    = m_squares[move.m_destination - 2];
                m_squares[move.m_destination - 2] = 0;
            }

            if(move.is_promotion()) {
                // Promotion
                m_squares[move.m_destination] = move.m_promotion_piece;
            } else {
                // The actual "common" move
                m_squares[move.m_destination] = m_squares[move.m_source];
            }
        } else {
            throw std::logic_error("Invalid move!");
        }
    }

    /**
     * @brief "Converts the board to FEN notation"
     * @details "First the method constructs the actual state of the Board, then
     *          replaces consecutive whitespaces with the number of ' ' chars.
     *          Finally it adds Move, Castle, En Passant, Semi-moves end Number
     *          of Moves since the beginning"
     * @return The FEN representation of the Board
     */
    std::string Board::to_fen() const {
        std::string fen;

        for(uint64_t i = 0; i < 64; i++) {
            fen.push_back(static_cast<char>(m_piece_repr.at(m_squares[i])));

            if((i + 1) % 8 == 0 && i != 63) { fen.push_back('/'); }
        }

        uint16_t counter = 0;
        for(uint64_t i = 0; i < fen.length(); i++) {
            if(fen.at(i) == ' ') {
                counter++;

                for(uint64_t j = i; j < i + 7; j++) {
                    if(isblank(fen.at(j))) {
                        counter++;
                    } else {
                        break;
                    }
                }

                fen.replace(i, counter, std::to_string(counter));
                i++;
                counter = 0;
            }
        }

        std::string turn = m_turn ? "w" : "b";
        fen.append(" " + turn + " ");

        return fen;
    }

    /**
     * @brief "Checks if the move is valid"
     * @details "A `Move` is valid if it's in the `Board` and actually moves the
     * `Piece`"
     */
    bool Board::Move::is_valid() const {
        return m_source >= 0 && m_source < 64 && m_destination >= 0
            && m_destination < 64 && m_source != m_destination;
    }

    /**
     * @brief "Checks if the move is a promotion move"
     * @details "A `Move` is promotion if it's made by a pawn and the
     * destination it's in the opposite player first file"
     */
    bool Board::Move::is_promotion() const {
        return m_piece == Piece::PAWN
            && (m_destination < 8 || m_destination > 55);
    }
} // namespace DreamChess