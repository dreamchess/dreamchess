/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */

#include "../include/Game.hpp"

namespace DreamChess {
    /**
     * @brief "Creates a `Game` object"
     */
    Game::Game()
        : m_board(Board {})
        , m_history(History {}) {}

    /**
     * @brief "Updates the `Game`'s history"
     */
    void Game::update_history(const Board &board, const Board::Move &move) {
        m_history.add_step(board, move);
    }
} // namespace DreamChess
