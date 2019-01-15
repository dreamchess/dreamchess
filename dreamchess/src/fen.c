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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "board.h"
#include "fen.h"

/* FIXME: Add support for castling rights and move counters. */
/* FIXME: Error checking. */

char *fen_encode(board_t *board)
{
	const size_t fen_max_size = 255;
	char *fen = malloc(fen_max_size + 1);
	int feni = 0;
	int file, rank;

	for (rank = 7; rank >= 0; rank--)
	{
		int empty = 0;
		for (file = 0; file < 8; file++)
		{
			int piece = board->square[rank*8+file];
			int c=0;

			if (piece == NONE)
			{
				empty++;
				continue;
			}

			if (empty)
			{
				if (feni < fen_max_size)
					fen[feni++] = '0' + empty;
				empty = 0;
			}

			switch(PIECE(piece))
			{
				case PAWN:
					c = 'p';
					break;
				case KNIGHT:
					c = 'n';
					break;
				case BISHOP:
					c = 'b';
					break;
				case ROOK:
					c = 'r';
					break;
				case QUEEN:
					c = 'q';
					break;
				case KING:
					c = 'k';
			}

			if (IS_WHITE(piece))
				c = toupper(c);

			if (feni < fen_max_size)
				fen[feni++] = c;
		}
		if (empty)
		{
			if (feni < fen_max_size)
				fen[feni++] = '0' + empty;
			empty = 0;
		}
		if (rank > 0 && feni < fen_max_size)
			fen[feni++] = '/';
	}
	snprintf(fen + feni, fen_max_size + 1 - feni, " %c KQkq - 0 1", board->turn == WHITE ? 'w' : 'b');

	return fen;
}

board_t *fen_decode(char *fen)
{
	board_t *board;
	char *space = strchr(fen, ' ');
	int i;
	int square = 56;

	if (!space)
		return NULL;

	board = malloc(sizeof(board_t));

	for (i = 0; i < space - fen; i++)
	{
		int j;
		char c = fen[i];
		char piece=0;

		if ((c >= '1') && (c <= '9'))
		{
			for (j = 0; j < c - '0'; j++)
				board->square[square++] = NONE;
			continue;
		}

		switch(toupper(c))
		{
			case 'P':
				piece = PAWN;
				break;
			case 'N':
				piece = KNIGHT;
				break;
			case 'B':
				piece = BISHOP;
				break;
			case 'R':
				piece = ROOK;
				break;
			case 'Q':
				piece = QUEEN;
				break;
			case 'K':
				piece = KING;
				break;
			case '/':
				square -= 16;
				continue;
		}

		if (c >= 'a')
			piece |= BLACK;

		board->square[square++] = piece;
	}

	if (*(space + 1) == 'w')
		board->turn = 0;
	else
		board->turn = 1;

	for (i = 0; i < 10; i++)
		board->captured[i] = 0;

	board->state = BOARD_NORMAL;

	return board;
}
