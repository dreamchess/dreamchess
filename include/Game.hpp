/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#pragma once

#include "Board.hpp"
#include "History.hpp"

namespace DreamChess {
    class Game final {
    public:
        explicit Game();
        Board get_board() const;
        History get_history() const;

    private:
        Board m_board;
        History m_history;

        void update_history(const Board &, const Board::Move &);
    };
} // namespace DreamChess