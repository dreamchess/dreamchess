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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "history.h"

static int history[2][64][64];
static int current_side;

static int
move_compare(move_t *move1, move_t *move2)
{
    if (history[current_side][move1->source][move1->destination]
            > history[current_side][move2->source][move2->destination])
        return -1;
    else
        return 1;
}

void
sort_moves(move_t moves[], int total_moves, int side)
{
    current_side = side;
    qsort(moves, total_moves, sizeof(move_t), (int (*)(const void *, const void *)) move_compare);
}

void
add_count(move_t *move, int side)
{
    history[side][move->source][move->destination]++;
}

void
forget_history()
{
    int i,j,k;
    for (i = 0; i < 2; i++)
        for (j = 0; j < 64; j++)
            for (k = 0; k < 64; k++)
                history[i][j][k] = 0;
}
