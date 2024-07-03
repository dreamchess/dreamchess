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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "fen.h"

char *fen_encode(board_t *board) {
	const size_t fen_max_size = 255;
	char *fen = malloc(fen_max_size + 1);
	int feni = 0;
	int file, rank;

	for (rank = 7; rank >= 0; rank--) {
		int empty = 0;
		for (file = 0; file < 8; file++) {
			int piece = board->square[rank * 8 + file];
			int c = 0;

			if (piece == NONE) {
				empty++;
				continue;
			}

			if (empty) {
				if (feni < fen_max_size)
					fen[feni++] = '0' + empty;
				empty = 0;
			}

			switch (PIECE(piece)) {
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
		if (empty) {
			if (feni < fen_max_size)
				fen[feni++] = '0' + empty;
			empty = 0;
		}
		if (rank > 0 && feni < fen_max_size)
			fen[feni++] = '/';
	}

	fen[feni] = '\0';
	strcat(fen, board->turn == WHITE ? " w " : " b ");

	if (board->can_castle_kingside[WHITE])
		strcat(fen, "K");
	if (board->can_castle_queenside[WHITE])
		strcat(fen, "Q");
	if (board->can_castle_kingside[BLACK])
		strcat(fen, "k");
	if (board->can_castle_queenside[BLACK])
		strcat(fen, "q");

	if (!board->can_castle_kingside[WHITE] && !board->can_castle_queenside[WHITE] &&
		!board->can_castle_kingside[BLACK] && !board->can_castle_queenside[BLACK])
		strcat(fen, "-");

	strcat(fen, " ");

	if (board->en_passant != -1) {
		const int file = board->en_passant % 8;
		const int rank = board->en_passant / 8;
		const char square[3] = { 'a' + file, '1' + rank, '\0' };
		strcat(fen, square);
	} else {
		strcat(fen, "-");
	}

	// FIXME: Add move counters
	strcat(fen, " 0 1");

	return fen;
}

board_t *fen_decode(const char *fen) {
	board_t *board = malloc(sizeof(board_t));
	memset(board, 0, sizeof(board_t));
	board->en_passant = -1;

	for (int i = 0; i < 64; ++i)
		board->square[i] = NONE;

	int rank = 7;
	int file = 0;
	const char *ptr = fen;

	// Parse board position
	while (*ptr && *ptr != ' ') {
		if (*ptr == '/') {
			rank--;
			file = 0;
		} else if (isdigit(*ptr)) {
			file += *ptr - '0';
		} else {
			int piece = NONE;

			switch (toupper(*ptr)) {
				case 'P': piece = PAWN; break;
				case 'N': piece = KNIGHT; break;
				case 'B': piece = BISHOP; break;
				case 'R': piece = ROOK; break;
				case 'Q': piece = QUEEN; break;
				case 'K': piece = KING; break;
			}

			if (islower(*ptr))
				piece |= BLACK;

			if (rank < 0 || file > 7)
				goto error;

			board->square[8 * rank + file] = piece;
			file++;
		}
		ptr++;
	}

	// Skip space
	if (*ptr++ != ' ')
		goto error;

	// Parse turn
	char turn = *ptr++;
	if (turn == 'w')
		board->turn = WHITE;
	else if (turn == 'b')
		board->turn = BLACK;
	else
		goto error;

	// Skip space
	if (*ptr++ != ' ')
		goto error;

	// Parse castling availability
	while (*ptr && *ptr != ' ') {
		switch (*ptr++) {
			case 'K': board->can_castle_kingside[WHITE] = true; break;
			case 'Q': board->can_castle_queenside[WHITE] = true; break;
			case 'k': board->can_castle_kingside[BLACK] = true; break;
			case 'q': board->can_castle_queenside[BLACK] = true; break;
			case '-': break;
		}
	}

	// Skip space
	if (*ptr++ != ' ')
		goto error;

	// Parse en passant target square
	if (*ptr != '-') {
		if (*ptr < 'a' || *ptr > 'h')
			goto error;

		const int file = *ptr++ - 'a';

		if (*ptr != '3' && *ptr != '6')
			goto error;

		const int rank = *ptr++ - '1';

		board->en_passant = 8 * rank + file;
	} else {
		ptr++; // Skip the dash
	}

	// FIXME: Parse move counters

	return board;

error:
	free(board);
	return NULL;
}
