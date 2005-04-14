/*  DreamChess
**  Copyright (C) 2003-2005  The DreamChess project
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

#ifndef EVAL_H
#define EVAL_H

#include "board.h"

typedef struct eval_data
{
	int max_pawn_file_bins[8];
	int max_pawn_color_bins[8];
	int max_total_pawns;
	int pawn_rams;
	int max_most_advanced[8];
	int max_passed_pawns[8];
	int min_pawn_file_bins[8];
	int min_most_backward[8];
} eval_data_t;

int
board_eval_quick(board_t *board);

int
board_eval_complete(board_t *board);

#endif /* EVAL_H */
