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
            explicit Step(const std::string &, const Board::Move &);

            /**
             * @brief "The board as it can be seen at this step"
             */
            const std::string &m_board_fen;

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
        History(const History &);
        History(History &&) noexcept;
        ~History() = default;

        // TODO: Aggiungere operator= per move e copy

        Step first();
        Step last();

        void add_step(const std::string &, const Board::Move &);
    };
} // namespace DreamChess
