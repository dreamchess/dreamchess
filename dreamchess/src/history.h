/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
**  with this source distribution.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file
 *  @brief Game history header.
 */

#ifndef DREAMCHESS_HISTORY_H
#define DREAMCHESS_HISTORY_H

#include "board.h"

/** History list entry. */
typedef struct step {
	/** Linked list pointers. */
	struct step *prev, *next;

	/** Board position. */
	board_t *board;

	/** Move done from this board position (if any), NULL otherwise. */
	move_t *move;
} step_t;

/** History list. The last step never contains a move. */
typedef struct history {
	/** Currently viewed history step. */
	step_t *view;

	/** First step in list. */
	step_t *first;

	/** Last step in list. */
	step_t *last;

	/** Game result (if any), NULL otherwise. */
	result_t *result;
} history_t;

/** @brief Allocates a new history list containing one step.
 *
 *  @param board Board to use as first step.
 *  @return Pointer to dynamically allocated history list.
 */
history_t *history_init(board_t *board);

/** @brief Deallocates a history list and all moves and boards contained
 *         therein.
 *
 *  @param hist History list to deallocate.
 */
void history_exit(history_t *hist);

/** @brief Deallocates a history list.
 *
 *  @param hist History list to deallocate.
 */

/** @brief Sets the move for the last step and adds a new consecutive step
 *         with the resulting board position.
 *
 *  The move and board will be copied. Play and view pointers will be set to
 *  the last step.
 *
 *  @param hist History list to update.
 *  @param move The move to add.
 *  @param board The board to add.
 */
void history_play(history_t *hist, move_t *move, board_t *board);

/** @brief Removes the last move and its resulting board position from the
 *         history list. The play and view pointers will be set to the last
 *         step.
 *
 * @param hist History list to update.
 * @return 1, if undo failed (nothing to undo), 0 otherwise.
 */
int history_undo(history_t *hist);

/** @brief Sets the view pointer to the next step.
 *
 *  @param hist History list to update.
 *  @return 1, if there's no next step, 0 otherwise.
 */
int history_view_next(history_t *hist);

/** @brief Sets the view pointer to the previous step.
 *
 *  @param hist History list to update.
 *  @return 1, if there's no previous step, 0 otherwise.
 */
int history_view_prev(history_t *hist);

/** @brief Saves the history in Portable Game Notation format.
 *
 *  @param hist History to save.
 *  @param filename Filename of file to write to. File will be truncated.
 *  @return 1 on error, 0 on success
 */
int history_save_pgn(history_t *hist, char *filename);

#endif
