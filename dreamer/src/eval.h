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

#ifndef DREAMER_EVAL_H
#define DREAMER_EVAL_H

#include "board.h"

typedef struct eval_data {
	int max_pawn_file_bins[8];
	int max_pawn_color_bins[8];
	int max_total_pawns;
	int pawn_rams;
	int max_most_advanced[8];
	int max_passed_pawns[8];
	int min_pawn_file_bins[8];
	int min_most_backward[8];
} eval_data_t;

int board_eval_quick(const Board &board, int side);

int board_eval_complete(const Board &board, int side, int alpha, int beta);

#endif
