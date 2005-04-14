/*  DreamChess
**  Copyright (C) 2003-2004  The DreamChess project
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef HISTORY_H
#define HISTORY_H

#include "board.h"

typedef struct step
{
    struct step *prev, *next;
    board_t *board;
    move_t *move;
}
step_t;

typedef struct history
{
    step_t *view, *play;
    result_t *result;
}
history_t;

history_t *history_init(board_t *board);
/* Allocates a new history list containing one step.
** Parameters: (board_t *) board: Pointer to the board to use as the
**                 first step.
** Returns   : (history_t *): Pointer to new history list.
*/

void history_exit(history_t *history);
/* Deallocates a history list and all moves and boards contained therein.
** Parameters: (history_t *) history: History list to deallocate.
** Returns   : (void)
*/

void history_play(history_t *hist, move_t *move, board_t *board);
/* Adds a new move and board state to the end of the history list. Play and
** view pointers will be updated to point to the last step of the updated
** list.
** Parameters: (history_t *) history: History list to update.
**             (move_t *) move: Pointer to the move to add.
**             (board_t *) board: Pointer to the board to add.
** Returns   : (void)
*/

int history_undo(history_t *hist);
/* Undo the last move in the history list. The play and view pointers will
** be updated to point to the step of the updated list.
** Parameters: (history_t *) history: History list to update.
** Returns   : (int): 1, if the undo failed (nothing to undo), 0 otherwise
*/

int history_view_next(history_t *hist);
/* Update the view pointer to point to the next step.
** Parameters: (history_t *) history: History list to update.
** Returns   : (int): 1, if there's no next step, 0 otherwise
*/

int history_view_prev(history_t *hist);
/* Update the view pointer to point to the previous step.
** Parameters: (history_t *) history: History list to update.
** Returns   : (int): 1, if there's no previous step, 0 otherwise
*/

#endif /* HISTORY_H */
