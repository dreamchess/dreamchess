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
#include "hashing.h"

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
store_board(board_t *board, int eval, int eval_type, int depth, int
            time_stamp)
{
    int index = board->hash_key & (ENTRIES - 1);

    if (table[index].eval_type && (table[index].depth > depth) &&
            (table[index].time_stamp >= time_stamp))
        return;

    table[index].hash_key = board->hash_key;
    table[index].eval = eval;
    table[index].eval_type = eval_type;
    table[index].depth = depth;
    table[index].time_stamp = time_stamp;
}

entry_t *
lookup_board(board_t *board)
{
    int index = board->hash_key & (ENTRIES - 1);

    if (!table[index].eval_type)
        return NULL;

    if (table[index].hash_key != board->hash_key)
        return NULL;

    return &table[index];
}

void
clear_table()
{
    int i;
    for (i = 0; i < ENTRIES; i++)
    {
        table[i].eval_type = 0;
    }
}
