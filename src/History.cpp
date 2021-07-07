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
    History::History() : m_game_history{new std::list<const Step>} {}

    /**
     * @brief "Returns the first move of the game"
     */
    const History::Step History::first() { 
        return m_game_history.get() -> front(); 
    }

    /**
     * @brief "Returns the last played move"
     */
    const History::Step History::last() { 
        return m_game_history.get() -> back(); 
    }

    /**
     * @brief "Adds a step to the `History` list"
     * @params board_view "The `Step`'s view of the board"
     * @params move "The last move made in the referenced board"
     */
    void History::add_step(const Board &board_view, const Board::Move &move) {
        m_game_history.get() -> push_back(Step {board_view, move});
    }

    /**
     * @brief "Constructs the `Step`"
     * @details "Each `Step` is a view of the board and the move which brought us here"
     */
    History::Step::Step(const Board &board_view, const Board::Move &move) 
        : m_board_view{board_view} 
        , m_move{move} {}
}
