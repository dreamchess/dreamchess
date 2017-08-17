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

#include "move_data.h"

static int knight_moves[8][2] = {{-1, -2}, {1, -2}, {-2, -1}, {2, -1}, {-2, 1}, {2, 1}, {-1, 2}, {1, 2}};
static int king_moves[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
static int white_pawn_captures[2][2] = {{-1, 1}, {1, 1}};
static int black_pawn_captures[2][2] = {{-1, -1}, {1, -1}};
static int rook_moves[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};
static int bishop_moves[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
static int queen_moves[8][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

static int
is_valid(int x, int y)
{
	return (x >= 0 && x <= 7 && y >= 0 && y <= 7);
}

#define generate_moves_single(FUNCNAME, MOVES, SIZE) \
int ** \
FUNCNAME(void) \
{ \
	int x, y, i; \
\
	int **move_table; \
	move_table = malloc(64 * sizeof(int *)); \
	for (y = 0; y < 8; y++) \
		for (x = 0; x < 8; x++) { \
			int count = 0; \
			int source = y * 8 + x; \
\
			/* Determine possible moves from this source square. */ \
			for (i = 0; i < SIZE; i++) \
				if (is_valid(x + MOVES[i][0], y + MOVES[i][1])) count++; \
\
			move_table[source] = malloc((count + 1) * sizeof(int)); \
\
			/* Store number of moves in first element of array. */ \
			move_table[source][0] = count; \
\
			/* Print moves. */ \
			count = 1; \
			for (i = 0; i < SIZE; i++) { \
				if (is_valid(x + MOVES[i][0], y + MOVES[i][1])) { \
					int dest = source + MOVES[i][1] * 8 + MOVES[i][0]; \
					move_table[source][count] = dest; \
					count++; \
				} \
			} \
		} \
	return move_table; \
}

#define generate_moves_ray(FUNCNAME, MOVES, SIZE) \
int *** \
FUNCNAME(void) \
{ \
	int x, y, i; \
\
	int ***move_table; \
	move_table = malloc(64 * sizeof(int **)); \
	for (y = 0; y < 8; y++) \
		for (x = 0; x < 8; x++) { \
			int rays = 0; \
			int curray = 0; \
			int source = y * 8 + x; \
\
			/* Determine number of non-zero length rays for this source square. */ \
			for (i = 0; i < SIZE; i++) \
				if (is_valid(x + MOVES[i][0], y + MOVES[i][1])) rays++; \
\
			move_table[source] = malloc((rays + 1) * sizeof(int *)); \
\
			for (i = 0; i < SIZE; i++) { \
				int raylen = 0; \
				int curraypos = 1; \
				int xinc, yinc; \
\
				/* Determine length of current ray. */ \
				xinc = MOVES[i][0]; \
				yinc = MOVES[i][1]; \
\
				while (is_valid(x + xinc, y + yinc)) { \
					raylen++; \
					xinc += MOVES[i][0]; \
					yinc += MOVES[i][1]; \
				} \
\
				/* If this ray has length 0, skip it. */ \
				if (!raylen) continue; \
\
				move_table[source][curray] = malloc((raylen + 1) * sizeof(int)); \
\
				/* Store ray length in element 0 of the ray. */ \
				move_table[source][curray][0] = raylen; \
\
				xinc = MOVES[i][0]; \
				yinc = MOVES[i][1]; \
\
				/* Print ray elements. */ \
				while (is_valid(x + xinc, y + yinc)) { \
					int dest = source + yinc * 8 + xinc; \
					move_table[source][curray][curraypos] = dest; \
					curraypos++; \
					xinc += MOVES[i][0]; \
					yinc += MOVES[i][1]; \
				} \
\
				curray++; \
			} \
\
			/* Add a NULL pointer as sentinel. */ \
			move_table[source][rays] = NULL; \
		} \
	return move_table; \
}

generate_moves_single(all_knight_moves, knight_moves, 8)
generate_moves_single(all_king_moves, king_moves, 8)
generate_moves_single(all_white_pawn_capture_moves, white_pawn_captures, 2);
generate_moves_single(all_black_pawn_capture_moves, black_pawn_captures, 2);
generate_moves_ray(all_rook_moves, rook_moves, 4)
generate_moves_ray(all_bishop_moves, bishop_moves, 4)
generate_moves_ray(all_queen_moves, queen_moves, 8)

