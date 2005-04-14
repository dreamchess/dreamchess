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

#include <stdlib.h>

#include "history.h"
#include "board.h"

history_t *history_init(board_t *board)
{
    history_t *hist = (history_t *) malloc(sizeof(history_t));
    step_t *step = (step_t *) malloc(sizeof(step_t));

    step->board = (board_t *) malloc(sizeof(board_t));
    step->prev = NULL;
    step->next = NULL;
    *step->board = *board;
    step->move = NULL;
    hist->play = step;
    hist->view = step;
    hist->result = NULL;

    return hist;
}

void history_exit(history_t *hist)
{
    step_t *step = hist->play;

    while (step)
    {
        if (step->move)
            free(step->move);
        if (step->board)
            free(step->board);
        step = step->prev;
    }

    free(hist);
}

void history_play(history_t *hist, move_t *move, board_t *board)
{
    step_t *step = (step_t *) malloc(sizeof(step_t));

    step->board = (board_t *) malloc(sizeof(board_t));
    hist->play->move = (move_t *) malloc(sizeof(move_t));
    *step->board = *board;
    step->move = NULL;
    step->prev = hist->play;
    step->next = NULL;
    *hist->play->move = *move;
    hist->play->next = step;
    hist->play = step;
    hist->view = step;
}

int history_undo(history_t *hist)
{
    if (!hist->play->prev)
        return 1;

    free(hist->play->board);
    hist->play = hist->play->prev;
    free(hist->play->move);
    hist->play->move = NULL;
    free(hist->play->next);
    hist->play->next = NULL;
    hist->view = hist->play;
    return 0;
}

int history_view_next(history_t *hist)
{
    if (!hist->view->next)
        return 1;

    hist->view = hist->view->next;

    return 0;
}

int history_view_prev(history_t *hist)
{
    if (!hist->view->prev)
        return 1;

    hist->view = hist->view->prev;

    return 0;
}
