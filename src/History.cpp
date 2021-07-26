/**
 * @copyright Dreamchess++
 * @author Mattia Zorzan
 * @version v1.0
 * @date July, 2021
 */

#include "../include/History.hpp"

namespace DreamChess {
    /**
     * @brief "Constructs the `History`"
     * @details "`History` can be seen as a list of `Step`s"
     */
    History::History()
        : m_game_history {new std::list<Step>} {}

    /**
     * @brief "Copy constructor of `History`"
     */
    History::History(const History &history)
        : m_game_history(history.m_game_history.get()) {}

    /**
     * @brief "Move constructor of `History`"
     */
    History::History(History &&history) noexcept
        : m_game_history(std::move(history.m_game_history)) {}

    /**
     * @brief "Returns the first move of the game"
     */
    History::Step History::first() { return m_game_history->front(); }

    /**
     * @brief "Returns the last played move"
     */
    History::Step History::last() { return m_game_history->back(); }

    /**
     * @brief "Adds a step to the `History` list"
     * @params board_view "The `Step`'s view of the board"
     * @params move "The last move made in the referenced board"
     */
    void History::add_step(const std::string &board_fen,
                           const Board::Move &move) {
        m_game_history->push_back(Step {board_fen, move});
    }

    /**
     * @brief "Constructs the `Step`"
     * @details "Each `Step` is a view of the board and the move which brought
     * us here"
     */
    History::Step::Step(const std::string &board_fen, const Board::Move &move)
        : m_board_fen {board_fen}
        , m_move {move} {}
} // namespace DreamChess
