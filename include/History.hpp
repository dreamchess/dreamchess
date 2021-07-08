/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */
#pragma once

#include "Board.hpp"

#include <list>
#include <memory>
#include <string>

namespace DreamChess {
    /**
     * @brief "The current game History"
     */
    class History final {
    private:
        /**
         * @brief "A single step into the History"
         */
        struct Step final {
            explicit Step(const Board &, const Board::Move &);

            /**
             * @brief "The board as it can be seen at this step"
             */
            const Board &m_board_view;

            /**
             * @brief "Last move which has been made"
             */
            const Board::Move &m_move;
        };

        /**
         * @brief "Game history so far"
         */
        std::unique_ptr<std::list<Step>> m_game_history;

    public:
        explicit History();

        Step first();
        Step last();

        void add_step(const Board &, const Board::Move &);
    };
} // namespace DreamChess
