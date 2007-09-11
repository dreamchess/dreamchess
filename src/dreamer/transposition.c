/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
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

#include <stdlib.h>

#include "board.h"
#include "hashing.h"
#include "transposition.h"
#include "search.h"

#define ENTRIES (1 << 17)

typedef struct entry
{
    int eval_type;
    int eval;
    int depth;
    long long hash_key;
    int time_stamp;
}
entry_t;

entry_t table[ENTRIES];

void
store_board(board_t *board, int eval, int eval_type, int depth, int ply,
            int time_stamp)
{
    int index = board->hash_key & (ENTRIES - 1);

    if (table[index].eval_type && (table[index].depth > depth) &&
            (table[index].time_stamp >= time_stamp))
        return;

    /* Make mate-in-n values relative to board that's to be stored */
    if (eval < ALPHABETA_MIN + 1000)
        eval -= ply;
    else if (eval > ALPHABETA_MAX - 1000)
        eval += ply;

    table[index].hash_key = board->hash_key;
    table[index].eval = eval;
    table[index].eval_type = eval_type;
    table[index].depth = depth;
    table[index].time_stamp = time_stamp;
}

int
lookup_board(board_t *board, int depth, int ply, int *eval)
{
    int index = board->hash_key & (ENTRIES - 1);

    if (table[index].eval_type == EVAL_NONE)
        return EVAL_NONE;

    if (table[index].hash_key != board->hash_key)
        return EVAL_NONE;

    if (table[index].depth < depth)
        return EVAL_NONE;

    *eval = table[index].eval;

    /* Make mate-in-n values relative to current game position */
    if (*eval < ALPHABETA_MIN + 1000)
        *eval += ply;
    else if (*eval > ALPHABETA_MAX - 1000)
        *eval -= ply;

    return table[index].eval_type;
}

void
clear_table()
{
    int i;
    for (i = 0; i < ENTRIES; i++)
    {
        table[i].eval_type = EVAL_NONE;
    }
}
