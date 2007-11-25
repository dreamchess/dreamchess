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
move_compare(move_t move1, move_t move2)
{
    if (history[current_side][MOVE_GET(move1, SOURCE)][MOVE_GET(move1, DEST)]
            > history[current_side][MOVE_GET(move2, SOURCE)][MOVE_GET(move2, DEST)])
        return -1;
    else
        return 1;
}

static void best_first(move_t moves[], int total_moves, move_t move)
{
    int i;

    for (i = 0; i < total_moves; i++)
        if (move == moves[i])
        {
            move_t swap = moves[0];
            moves[0] = moves[i];
            moves[i] = swap;
            return;
        }
}

void
sort_moves(move_t moves[], int total_moves, int side, move_t move)
{
    current_side = side;
    if (move != NO_MOVE)
        best_first(moves, total_moves, move);
    qsort(moves + 1, total_moves - 1, sizeof(move_t), (int (*)(const void *, const void *)) move_compare);
}

void
add_count(move_t move, int side)
{
    history[side][MOVE_GET(move, SOURCE)][MOVE_GET(move, DEST)]++;
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
