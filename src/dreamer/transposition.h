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

#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "board.h"

#define EVAL_NONE 0
#define EVAL_LOWERBOUND 1
#define EVAL_ACCURATE 2
#define EVAL_UPPERBOUND 3
#define EVAL_PV 4

void
store_board(board_t *board, int eval, int eval_type, int depth, int ply,
            int time_stamp, move_t *best_move);

int
lookup_board(board_t *board, int depth, int ply, int *eval, move_t **best_move);

void
clear_table();

void transposition_init(int megabytes);
void transposition_exit();

#endif /* TRANSPOSITION_H */
