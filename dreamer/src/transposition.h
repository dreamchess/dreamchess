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

#ifndef DREAMER_TRANSPOSITION_H
#define DREAMER_TRANSPOSITION_H

#include "board.h"

#define EVAL_NONE 0
#define EVAL_LOWERBOUND 1
#define EVAL_ACCURATE 2
#define EVAL_UPPERBOUND 3
#define EVAL_PV 4

void
store_board(board_t *board, int eval, int eval_type, int depth, int ply,
            int time_stamp, move_t best_move);

int
lookup_board(board_t *board, int depth, int ply, int *eval);

void
set_best_move(board_t *board, move_t move);

void
clear_table(void);

void transposition_init(int megabytes);
void transposition_exit(void);
move_t lookup_best_move(board_t *board);

#endif
