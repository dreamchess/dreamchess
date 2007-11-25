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

#include "board.h"
#include "move.h"
#include "move_data.h"

/* Global move tables. */
int ***rook_moves;
int ***bishop_moves;
int ***queen_moves;
int **knight_moves;
int **king_moves;
int **white_pawn_capture_moves;
int **black_pawn_capture_moves;

#define add_moves_ray(FUNCNAME, MOVES, PIECE, PLAYER, OPPONENT_FIND, LOOP) \
int \
FUNCNAME(board_t *board, move_t *move) \
{ \
	int source; \
	bitboard_t bitboard = board->bitboard[PIECE + PLAYER]; \
	int move_count = 0; \
\
	/* Look for the pieces, starting at player's side of the board. */ \
	for (LOOP) \
	{ \
		int **ray; \
\
		/* If no more pieces of this type are found, we're done. */ \
		if (!bitboard) return move_count; \
\
		/* If no piece of this type is at this square, continue searching. */ \
		if (!(bitboard & square_bit[source])) continue; \
\
		/* Iterate over rays. */ \
		for (ray = MOVES[source]; *ray; ray++) \
		{ \
			int elm; \
\
			/* Iterate over ray elements. */ \
			for (elm = 1; elm <= (*ray)[0]; elm++) \
			{ \
				int dest = (*ray)[elm]; \
\
				/* If there's a white piece at the destination, break off the ray. */ \
				if (board->bitboard[ALL + PLAYER] & square_bit[dest]) \
					break; \
\
				/* The move is legal. */ \
\
				/* If there's a black piece at the destination, this is a capture move. */ \
				if (board->bitboard[ALL + OPPONENT(PLAYER)] & square_bit[dest]) \
				{ \
					int piece = OPPONENT_FIND(board, dest); \
\
					/* If we are capturing a king, previous board position was illegal. */ \
					if (piece == KING + OPPONENT(PLAYER)) return -1; \
\
					*move++ = MOVE(PIECE + PLAYER, source, dest, CAPTURE_MOVE, piece); \
					move_count++; \
\
					/* Break off the ray. */ \
					break; \
				} \
				else \
				{ \
					/* Normal move. */ \
					*move++ = MOVE(PIECE + PLAYER, source, dest, NORMAL_MOVE, 0); \
					move_count++; \
				} \
\
			} \
		} \
\
		/* Remove piece from the copy of the bitboard. */ \
		bitboard ^= square_bit[source]; \
	} \
\
	return move_count; \
}

#define add_moves_single(FUNCNAME, MOVES, PIECE, PLAYER, OPPONENT_FIND, LOOP) \
int \
FUNCNAME(board_t *board, move_t *move) \
{ \
	int source; \
	bitboard_t bitboard = board->bitboard[PIECE + PLAYER]; \
	int move_count = 0; \
\
	/* Look for the pieces, starting at player's side of the board. */ \
	for (LOOP) \
	{ \
		int *moves = MOVES[source]; \
		int elm; \
\
		/* If no more pieces of this type are found, we're done. */ \
		if (!bitboard) return move_count; \
\
		/* If no piece of this type is at this square, continue searching. */ \
		if (!(bitboard & square_bit[source])) continue; \
\
		/* Iterate over moves. */ \
		for (elm = 1; elm <= moves[0]; elm++) \
		{ \
			int dest = moves[elm]; \
\
			/* If there's a white piece at the destination, skip this possible move. */ \
			if (board->bitboard[ALL + PLAYER] & square_bit[dest]) \
				continue; \
\
			/* The move is legal. */ \
\
			/* If there's a black piece at the destination, this is a capture move. */ \
			if (board->bitboard[ALL + OPPONENT(PLAYER)] & square_bit[dest]) \
			{ \
				int piece = OPPONENT_FIND(board, dest); \
\
				/* If we are capturing a king, previous board position was illegal. */ \
				if (piece == KING + OPPONENT(PLAYER)) return -1; \
\
				*move++ = MOVE(PIECE + PLAYER, source, dest, CAPTURE_MOVE, piece); \
				move_count++; \
			} \
			else \
			{ \
				/* Normal move. */ \
				*move++ = MOVE(PIECE + PLAYER, source, dest, NORMAL_MOVE, 0); \
				move_count++; \
			} \
\
		} \
\
		/* Remove piece from the copy of the bitboard. */ \
		bitboard ^= square_bit[source]; \
	} \
\
	return move_count; \
}

#define add_pawn_moves(FUNCNAME, MOVES, INC, TEST1, TEST2, PLAYER, OPPONENT_FIND, LOOP) \
int \
FUNCNAME(board_t *board, move_t *move) \
{ \
	int source; \
	bitboard_t bitboard = board->bitboard[PAWN + PLAYER]; \
	bitboard_t bitboard_all = board->bitboard[WHITE_ALL] | \
		board->bitboard[BLACK_ALL]; \
	int move_count = 0; \
\
	for (LOOP) \
	{ \
		int dest; \
		int elm; \
\
		if (!bitboard) \
			return move_count; \
\
		if (!(bitboard & square_bit[source])) \
			continue; \
\
		dest = source + INC; \
\
		if (!(bitboard_all & square_bit[dest])) \
		{ \
			if (TEST1) \
			{ \
				/* Normal move. */ \
				*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE, 0); \
				move_count++; \
\
				if (TEST2) \
				{ \
					dest += INC; \
					if (!(bitboard_all & square_bit[dest])) \
					{ \
						/* Double push. */ \
						*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE, 0); \
						move_count++; \
					} \
				} \
			} \
			else \
			{ \
				/* Pawn promotion. */ \
				*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE | PROMOTION_MOVE_QUEEN, 0); \
				*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE | PROMOTION_MOVE_ROOK, 0); \
				*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE | PROMOTION_MOVE_BISHOP, 0); \
				*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE | PROMOTION_MOVE_KNIGHT, 0); \
				move_count += 4; \
			} \
		} \
\
		/* Check for capture moves. */ \
		for (elm = 1; elm <= MOVES[source][0]; elm++) \
		{ \
			int dest = MOVES[source][elm]; \
			int piece; \
\
			/* If there's not a black piece at the destination, skip this possible move. */ \
			if (board->bitboard[ALL + OPPONENT(PLAYER)] & square_bit[dest]) \
			{ \
				piece = OPPONENT_FIND(board, dest); \
\
				/* If we are capturing a king, previous board position was illegal. */ \
				if (piece == KING + OPPONENT(PLAYER)) return -1; \
\
				/* The move is legal. */ \
				if (TEST1) \
				{ \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE, piece); \
					move_count++; \
				} else { \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_QUEEN, piece); \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_ROOK, piece); \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_BISHOP, piece); \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_KNIGHT, piece); \
					move_count += 4; \
				} \
			} \
			else \
				if (board->en_passant & square_bit[dest]) \
				{ \
					/* En passant capture. */ \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE_EN_PASSENT, PAWN + OPPONENT(PLAYER)); \
					move_count++; \
				} \
		} \
\
		/* Remove piece from the copy of the bitboard. */ \
		bitboard ^= square_bit[source]; \
	} \
\
	return move_count; \
}

add_moves_ray(add_black_rook_moves, rook_moves, ROOK, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
add_moves_ray(add_white_rook_moves, rook_moves, ROOK, SIDE_WHITE,
 find_black_piece, source = 0; source < 64; source++)
add_moves_ray(add_black_bishop_moves, bishop_moves, BISHOP, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
add_moves_ray(add_white_bishop_moves, bishop_moves, BISHOP, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
add_moves_ray(add_white_queen_moves, queen_moves, QUEEN, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
add_moves_ray(add_black_queen_moves, queen_moves, QUEEN, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
add_moves_single(add_white_knight_moves, knight_moves, KNIGHT, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
add_moves_single(add_black_knight_moves, knight_moves, KNIGHT, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
add_moves_single(add_white_king_moves, king_moves, KING, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
add_moves_single(add_black_king_moves, king_moves, KING, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
add_pawn_moves(add_white_pawn_moves, white_pawn_capture_moves, +8, dest <= 55, !(source & ~15),
	SIDE_WHITE, find_black_piece, source = 8; source <= 55; source++)
add_pawn_moves(add_black_pawn_moves, black_pawn_capture_moves, -8, dest >= 8, source >= 48,
	SIDE_BLACK, find_white_piece, source = 55; source >= 8; source--)

int
add_white_castle_moves(board_t *board, move_t *move)
{
	int move_count = 0;

	/* Kingside castle. Check for empty squares. */
	if ((board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		WHITE_EMPTY_KINGSIDE)))
	{
		*move++ = MOVE(WHITE_KING, SQUARE_E1, SQUARE_G1, CASTLING_MOVE_KINGSIDE, 0); \
		move_count++;
	}

	/* Queenside castle. Check for empty squares. */
	if ((board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		WHITE_EMPTY_QUEENSIDE)))
	{
		*move++ = MOVE(WHITE_KING, SQUARE_E1, SQUARE_C1, CASTLING_MOVE_QUEENSIDE, 0); \
		move_count++;
	}

	return move_count;
}

int
add_black_castle_moves(board_t *board, move_t *move)
{
	int move_count = 0;

	/* Kingside castle. Check for empty squares. */
	if ((board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		BLACK_EMPTY_KINGSIDE)))
	{
		*move++ = MOVE(BLACK_KING, SQUARE_E8, SQUARE_G8, CASTLING_MOVE_KINGSIDE, 0); \
		move_count++;
	}

	/* Queenside castle. Check for empty squares. */
	if ((board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		BLACK_EMPTY_QUEENSIDE)))
	{
		*move++ = MOVE(BLACK_KING, SQUARE_E8, SQUARE_C8, CASTLING_MOVE_QUEENSIDE, 0); \
		move_count++;
	}

	return move_count;
}

int
compute_legal_moves(board_t *board, move_t *move)
{
	int total_moves;
	int moves;

	if (board->current_player == SIDE_WHITE)
	{
		moves = add_white_castle_moves(board, move);
		move += moves;
		total_moves = moves;

		if ((moves = add_white_king_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_white_queen_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_white_rook_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_white_bishop_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_white_knight_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_white_pawn_moves(board, move)) < 0)
			return -1;
		total_moves += moves;
	}
	else
	{
        moves = add_black_castle_moves(board, move);
		move += moves;
		total_moves = moves;

		if ((moves = add_black_king_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_black_queen_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_black_rook_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_black_bishop_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_black_knight_moves(board, move)) < 0)
			return -1;
		move += moves;
		total_moves += moves;

		if ((moves = add_black_pawn_moves(board, move)) < 0)
			return -1;
		total_moves += moves;
	}

	return total_moves;
}

void
move_init()
{
	rook_moves = all_rook_moves();
	knight_moves = all_knight_moves();
	king_moves = all_king_moves();
	bishop_moves = all_bishop_moves();
	queen_moves = all_queen_moves();
	white_pawn_capture_moves = all_white_pawn_capture_moves();
	black_pawn_capture_moves = all_black_pawn_capture_moves();
}

void
free_moves_ray(int ***moves)
{
	int **ray;
	int source;

	for (source = 0; source < 64; source++)
	{

		/* Iterate over rays. */
		for (ray = moves[source]; *ray; ray++)
			free(*ray);

		free (moves[source]);
	}

	free(moves);
}

void
free_moves_single(int **moves)
{
	int source;

	for (source = 0; source < 64; source++)
	{
		free (moves[source]);
	}

	free(moves);
}

void
move_exit()
{
	free_moves_ray(queen_moves);
	free_moves_ray(bishop_moves);
	free_moves_ray(rook_moves);
	free_moves_single(knight_moves);
	free_moves_single(king_moves);
	free_moves_single(white_pawn_capture_moves);
	free_moves_single(black_pawn_capture_moves);
}
