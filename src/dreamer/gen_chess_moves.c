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

#include <stdio.h>

#define CHS_FILENAME_C "move_data.c"

int knight_moves[8][2] = {{-1, -2}, {1, -2}, {-2, -1}, {2, -1}, {-2, 1}, {2, 1}, {-1, 2}, {1, 2}};
int king_moves[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
int white_pawn_captures[2][2] = {{-1, 1}, {1, 1}};
int black_pawn_captures[2][2] = {{-1, -1}, {1, -1}};
int rook_moves[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};
int bishop_moves[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
int queen_moves[8][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

int
is_valid(int x, int y)
{
	return (x >= 0 && x <= 7 && y >= 0 && y <= 7);
}

#define generate_moves_single(FUNCNAME, TABLE_FUNCTION, MOVES, SIZE) \
void \
FUNCNAME(FILE *f) \
{ \
	int x, y, i; \
\
	fprintf(f, "int **\n" TABLE_FUNCTION "()\n{\n\tint **move_table;\n\n"); \
	fprintf(f, "\tmove_table = malloc(64 * sizeof(int *));\n\n"); \
	for (y = 0; y < 8; y++) \
		for (x = 0; x < 8; x++) { \
			int count = 0; \
			int source = y * 8 + x; \
\
			/* Determine possible moves from this source square. */ \
			for (i = 0; i < SIZE; i++) \
				if (is_valid(x + MOVES[i][0], y + MOVES[i][1])) count++; \
\
			fprintf(f, "\tmove_table[%i] = malloc(%i * sizeof(int));\n", source, count + 1); \
\
			/* Store number of moves in first element of array. */ \
			fprintf(f, "\tmove_table[%i][0] = %i;\n", source, count); \
\
			/* Print moves. */ \
			count = 1; \
			for (i = 0; i < SIZE; i++) { \
				if (is_valid(x + MOVES[i][0], y + MOVES[i][1])) { \
					int dest = source + MOVES[i][1] * 8 + MOVES[i][0]; \
					fprintf(f, "\tmove_table[%i][%i] = %i;\n", \
						source, count, dest); \
					count++; \
				} \
			} \
		} \
	fprintf(f, "\n\treturn move_table;\n}\n\n"); \
}

#define generate_moves_ray(FUNCNAME, TABLE_FUNCTION, MOVES, SIZE) \
void \
FUNCNAME(FILE *f) \
{ \
	int x, y, i; \
\
	fprintf(f, "int ***\n" TABLE_FUNCTION "()\n{\n\tint ***move_table;\n\n"); \
	fprintf(f, "\tmove_table = malloc(64 * sizeof(int **));\n\n"); \
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
			fprintf(f, "\tmove_table[%i] = malloc(%i * sizeof(int *));\n", source, rays + 1); \
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
				fprintf(f, "\tmove_table[%i][%i] = malloc(%i * sizeof(int));\n", source, curray, raylen + 1); \
\
				/* Store ray length in element 0 of the ray. */ \
				fprintf(f, "\tmove_table[%i][%i][0] = %i;\n", source, curray, raylen); \
\
				xinc = MOVES[i][0]; \
				yinc = MOVES[i][1]; \
\
				/* Print ray elements. */ \
				while (is_valid(x + xinc, y + yinc)) { \
					int dest = source + yinc * 8 + xinc; \
					fprintf(f, "\tmove_table[%i][%i][%i] = %i;\n", source, curray, curraypos, dest); \
					curraypos++; \
					xinc += MOVES[i][0]; \
					yinc += MOVES[i][1]; \
				} \
\
				curray++; \
			} \
\
			/* Add a NULL pointer as sentinel. */ \
			fprintf(f, "\tmove_table[%i][%i] = NULL;\n", source, rays); \
		} \
	fprintf(f, "\n\treturn move_table;\n}\n\n"); \
}

generate_moves_single(generate_knight_moves, "all_knight_moves", knight_moves, 8)
generate_moves_single(generate_king_moves, "all_king_moves", king_moves, 8)
generate_moves_single(generate_white_pawn_captures, "all_white_pawn_capture_moves", white_pawn_captures, 2);
generate_moves_single(generate_black_pawn_captures, "all_black_pawn_capture_moves", black_pawn_captures, 2);
generate_moves_ray(generate_rook_moves, "all_rook_moves", rook_moves, 4)
generate_moves_ray(generate_bishop_moves, "all_bishop_moves", bishop_moves, 4)
generate_moves_ray(generate_queen_moves, "all_queen_moves", queen_moves, 8)

int
main()
{
	FILE *f;
	f = fopen(CHS_FILENAME_C, "w");
	if (!f) {
		printf("Error opening file '%s'.\n", CHS_FILENAME_C);
		return 1;
	}
	fprintf(f, "#include <stdlib.h>\n\n");
	generate_knight_moves(f);
	generate_king_moves(f);
	generate_bishop_moves(f);
	generate_rook_moves(f);
	generate_queen_moves(f);
	generate_white_pawn_captures(f);
	generate_black_pawn_captures(f);
	fclose(f);

	return 0;
}
