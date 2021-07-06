/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#pragma once

#include "../include/Piece.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <string>

namespace DreamChess {
    /**
     * @brief "Define Board, Pieces and moves for the game"
     */
    class Board final {
    public:
        explicit Board();
        ~Board() = default;

        friend std::ostream &operator<<(std::ostream &, const Board &);

        /**
         * @brief "Describes a `Move` for any piece in the board"
         */
        struct Move final {
            uint16_t m_source;
            uint16_t m_destination;

            Piece m_piece;
            Piece m_promotion_piece;

            bool is_valid() const {
                return m_source >= 0 && m_source < 64 && m_destination >= 0
                    && m_destination < 64 && m_source != m_destination;
            }

            bool is_promotion() const {
                return m_piece == Piece::PAWN
                    && (m_destination < 8 || m_destination >= 56);
            }
        };

    private:
        /**
         * @brief "'false' for BLACK's or 'true' for WHITE's turn"
         */
        bool m_turn = true;
        /**
         * @brief "Array describing the board's state"
         */
        std::unique_ptr<uint16_t[]> m_squares;
        /**
         * @brief "Keeps track of captured pieces"
         */
        std::unique_ptr<uint16_t[]> m_captured;

        /**
         * @brief "Convention for the FEN to DreamChess::Piece mapping"
         */
        const std::map<uint8_t, Piece> m_fen_to_piece {
            {'p', Piece::PAWN}, {'n', Piece::KNIGHT}, {'b', Piece::BISHOP},
            {'r', Piece::ROOK}, {'q', Piece::QUEEN},  {'k', Piece::KING}};

        /**
         * @brief "Used in Piece to char conversion, while printing the board"
         */
        const std::map<uint16_t, uint8_t> m_piece_repr {
            {0, ' '},  {9, 'P'},  {10, 'N'}, {11, 'B'}, {12, 'R'},
            {13, 'Q'}, {14, 'K'}, {17, 'p'}, {18, 'n'}, {19, 'b'},
            {20, 'r'}, {21, 'q'}, {22, 'k'}};

        void init_board();
        void make_move(const Move&);
    };
} // namespace DreamChess
