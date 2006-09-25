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

#include "board.h"
#include "move.h"

typedef struct rep_list
{
    /* FIXME */
    long long position[101 + 100];
    int head;
}
rep_list_t;

static rep_list_t *hist;
static int hist_idx;
static rep_list_t *cur_list;

void repetition_init(board_t *board)
{
    hist = malloc(sizeof(rep_list_t));
    hist_idx = 0;
    cur_list = &hist[0];
    cur_list->position[0] = board->hash_key;
    cur_list->head = 1;
}

void repetition_exit()
{
    free(hist);
}

void repetition_add(board_t *board, move_t *move)
{
    if ((move->type != NORMAL_MOVE) || ((move->piece & PIECE_MASK) == PAWN))
    {
        hist_idx++;
        hist = realloc(hist, sizeof(rep_list_t) * (hist_idx + 1));
        cur_list = &hist[hist_idx];
        cur_list->position[0] = board->hash_key;
        cur_list->head = 1;
    }
    else
        cur_list->position[cur_list->head++] = board->hash_key;
}

void repetition_remove()
{
    if (cur_list->head > 1)
        cur_list->head--;
    else if (hist_idx > 0)
    {
        hist_idx--;
        cur_list = &hist[hist_idx];
    }
}

int is_repetition(board_t *board, int ply)
{
    int i;
    int count = 0;
    int cur_head = cur_list->head + ply;

    /* We won't go out of bounds here because of the 50-move rule. */
    cur_list->position[cur_head] = board->hash_key;

    if (cur_head < 8)
        return 0;

    /* We only check for two occurrences to prevent transposition table
    ** hits that lead to a third repetition without us knowing about it.
    */
    for (i = cur_head - 2; i >= 0; i -= 2)
        if (board->hash_key == cur_list->position[i])
            return 1;

    return 0;
}

int is_draw(board_t *board)
{
    int i;
    int count = 0;

    /* 50 move rule. */
    if (board->fifty_moves == 100)
        return 2;

    if (cur_list->head < 9)
        return 0;

    for (i = cur_list->head - 3; i >= 0; i -= 2)
    {
        if (cur_list->position[cur_list->head - 1] ==
                cur_list->position[i])
            count++;
        if (count == 2)
            return 1;
    }

    return 0;
}
