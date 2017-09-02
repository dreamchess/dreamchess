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
#include "ttable.h"
#include "move_data.h"
#include "dreamer.h"
#include "commands.h"
#include "e_comm.h"

#define ADD_RAY_MOVES_FUNC(FUNCNAME, MOVES, PIECE, PLAYER, OPPONENT_FIND, LOOP) \
move_t * \
MoveGenerator::FUNCNAME(board_t *board, move_t *move) \
{ \
	int source; \
	bitboard_t bitboard = board->bitboard[PIECE + PLAYER]; \
\
	/* Look for the pieces, starting at player's side of the board. */ \
	for (LOOP) \
	{ \
		int **ray; \
\
		/* If no more pieces of this type are found, we're done. */ \
		if (!bitboard) return move; \
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
					if (piece == KING + OPPONENT(PLAYER)) return NULL; \
\
					*move++ = MOVE(PIECE + PLAYER, source, dest, CAPTURE_MOVE, piece); \
\
					/* Break off the ray. */ \
					break; \
				} \
				else \
				{ \
					/* Normal move. */ \
					*move++ = MOVE(PIECE + PLAYER, source, dest, NORMAL_MOVE, 0); \
				} \
\
			} \
		} \
\
		/* Remove piece from the copy of the bitboard. */ \
		bitboard ^= square_bit[source]; \
	} \
\
	return move; \
}

#define ADD_SINGLE_MOVES_FUNC(FUNCNAME, MOVES, PIECE, PLAYER, OPPONENT_FIND, LOOP) \
move_t * \
MoveGenerator::FUNCNAME(board_t *board, move_t *move) \
{ \
	int source; \
	bitboard_t bitboard = board->bitboard[PIECE + PLAYER]; \
\
	/* Look for the pieces, starting at player's side of the board. */ \
	for (LOOP) \
	{ \
		int *moves = MOVES[source]; \
		int elm; \
\
		/* If no more pieces of this type are found, we're done. */ \
		if (!bitboard) return move; \
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
				if (piece == KING + OPPONENT(PLAYER)) return NULL; \
\
				*move++ = MOVE(PIECE + PLAYER, source, dest, CAPTURE_MOVE, piece); \
			} \
			else \
			{ \
				/* Normal move. */ \
				*move++ = MOVE(PIECE + PLAYER, source, dest, NORMAL_MOVE, 0); \
			} \
\
		} \
\
		/* Remove piece from the copy of the bitboard. */ \
		bitboard ^= square_bit[source]; \
	} \
\
	return move; \
}

#define ADD_PAWN_MOVES_FUNC(FUNCNAME, MOVES, INC, TEST1, TEST2, PLAYER, OPPONENT_FIND, LOOP) \
move_t * \
MoveGenerator::FUNCNAME(board_t *board, move_t *move) \
{ \
	int source; \
	bitboard_t bitboard = board->bitboard[PAWN + PLAYER]; \
	bitboard_t bitboard_all = board->bitboard[WHITE_ALL] | \
		board->bitboard[BLACK_ALL]; \
\
	for (LOOP) \
	{ \
		int dest; \
		int elm; \
\
		if (!bitboard) \
			return move; \
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
\
				if (TEST2) \
				{ \
					dest += INC; \
					if (!(bitboard_all & square_bit[dest])) \
					{ \
						/* Double push. */ \
						*move++ = MOVE(PAWN + PLAYER, source, dest, NORMAL_MOVE, 0); \
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
				if (piece == KING + OPPONENT(PLAYER)) return NULL; \
\
				/* The move is legal. */ \
				if (TEST1) \
				{ \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE, piece); \
				} else { \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_QUEEN, piece); \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_ROOK, piece); \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_BISHOP, piece); \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE | PROMOTION_MOVE_KNIGHT, piece); \
				} \
			} \
			else \
				if (board->en_passant & square_bit[dest]) \
				{ \
					/* En passant capture. */ \
					*move++ = MOVE(PAWN + PLAYER, source, dest, CAPTURE_MOVE_EN_PASSANT, PAWN + OPPONENT(PLAYER)); \
				} \
		} \
\
		/* Remove piece from the copy of the bitboard. */ \
		bitboard ^= square_bit[source]; \
	} \
\
	return move; \
}

ADD_RAY_MOVES_FUNC(addBlackRookMoves, _rookMoves, ROOK, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
ADD_RAY_MOVES_FUNC(addWhiteRookMoves, _rookMoves, ROOK, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
ADD_RAY_MOVES_FUNC(addBlackBishopMoves, _bishopMoves, BISHOP, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
ADD_RAY_MOVES_FUNC(addWhiteBishopMoves, _bishopMoves, BISHOP, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
ADD_RAY_MOVES_FUNC(addWhiteQueenMoves, _queenMoves, QUEEN, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
ADD_RAY_MOVES_FUNC(addBlackQueenMoves, _queenMoves, QUEEN, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
ADD_SINGLE_MOVES_FUNC(addWhiteKnightMoves, _knightMoves, KNIGHT, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
ADD_SINGLE_MOVES_FUNC(addBlackKnightMoves, _knightMoves, KNIGHT, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
ADD_SINGLE_MOVES_FUNC(addWhiteKingMoves, _kingMoves, KING, SIDE_WHITE,
	find_black_piece, source = 0; source < 64; source++)
ADD_SINGLE_MOVES_FUNC(addBlackKingMoves, _kingMoves, KING, SIDE_BLACK,
	find_white_piece, source = 63; source >= 0; source--)
ADD_PAWN_MOVES_FUNC(addWhitePawnMoves, _whitePawnCaptureMoves, +8, dest <= 55, !(source & ~15),
	SIDE_WHITE, find_black_piece, source = 8; source <= 55; source++)
ADD_PAWN_MOVES_FUNC(addBlackPawnMoves, _blackPawnCaptureMoves, -8, dest >= 8, source >= 48,
	SIDE_BLACK, find_white_piece, source = 55; source >= 8; source--)

move_t *MoveGenerator::addWhiteCastleMoves(board_t *board, move_t *move) {
	/* Kingside castle. Check for empty squares. */
	if ((board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		WHITE_EMPTY_KINGSIDE)))
	{
		*move++ = MOVE(WHITE_KING, SQUARE_E1, SQUARE_G1, CASTLING_MOVE_KINGSIDE, 0); \
	}

	/* Queenside castle. Check for empty squares. */
	if ((board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		WHITE_EMPTY_QUEENSIDE)))
	{
		*move++ = MOVE(WHITE_KING, SQUARE_E1, SQUARE_C1, CASTLING_MOVE_QUEENSIDE, 0); \
	}

	return move;
}

move_t *MoveGenerator::addBlackCastleMoves(board_t *board, move_t *move) {
	/* Kingside castle. Check for empty squares. */
	if ((board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		BLACK_EMPTY_KINGSIDE)))
	{
		*move++ = MOVE(BLACK_KING, SQUARE_E8, SQUARE_G8, CASTLING_MOVE_KINGSIDE, 0); \
	}

	/* Queenside castle. Check for empty squares. */
	if ((board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE) &&
		(!((board->bitboard[BLACK_ALL] | board->bitboard[WHITE_ALL]) &
		BLACK_EMPTY_QUEENSIDE)))
	{
		*move++ = MOVE(BLACK_KING, SQUARE_E8, SQUARE_C8, CASTLING_MOVE_QUEENSIDE, 0); \
	}

	return move;
}

MoveGenerator::MoveGenerator() :
		_moves{},
		_movesStart{},
		_movesCur{} {
	_rookMoves = all_rook_moves();
	_bishopMoves = all_bishop_moves();
	_queenMoves = all_queen_moves();
	_knightMoves = all_knight_moves();
	_kingMoves = all_king_moves();
	_whitePawnCaptureMoves = all_white_pawn_capture_moves();
	_blackPawnCaptureMoves = all_black_pawn_capture_moves();
	clearHistory();
}

MoveGenerator::~MoveGenerator() {
	freeRayMoves(_rookMoves);
	freeRayMoves(_bishopMoves);
	freeRayMoves(_queenMoves);
	freeSingleMoves(_knightMoves);
	freeSingleMoves(_kingMoves);
	freeSingleMoves(_whitePawnCaptureMoves);
	freeSingleMoves(_blackPawnCaptureMoves);
}

void MoveGenerator::freeRayMoves(int ***moves) {
	for (unsigned int source = 0; source < 64; source++) {
		/* Iterate over rays. */
		for (int **ray = moves[source]; *ray; ray++)
			free(*ray);
		free (moves[source]);
	}

	free(moves);
}

void MoveGenerator::freeSingleMoves(int **moves) {
	for (unsigned int source = 0; source < 64; source++)
		free (moves[source]);

	free(moves);
}

int MoveGenerator::computeLegalMoves(board_t *board, int ply) {
	move_t *move = &_moves[_movesStart[ply]];

	if (board->current_player == SIDE_WHITE)
	{
		move = addWhiteCastleMoves(board, move);

		if (!(move = addWhiteKingMoves(board, move))
			|| !(move = addWhiteQueenMoves(board, move))
			|| !(move = addWhiteRookMoves(board, move))
			|| !(move = addWhiteBishopMoves(board, move))
			|| !(move = addWhiteKnightMoves(board, move))
			|| !(move = addWhitePawnMoves(board, move)))
			return -1;
	}
	else
	{
		move = addBlackCastleMoves(board, move);

		if (!(move = addBlackKingMoves(board, move))
			|| !(move = addBlackQueenMoves(board, move))
			|| !(move = addBlackRookMoves(board, move))
			|| !(move = addBlackBishopMoves(board, move))
			|| !(move = addBlackKnightMoves(board, move))
			|| !(move = addBlackPawnMoves(board, move)))
			return -1;
	}

	_movesStart[ply + 1] = _movesStart[ply] + move - &_moves[_movesStart[ply]];
	_movesCur[ply] = _movesStart[ply];
	return 0;
}

move_t MoveGenerator::getNextMove(board_t *board, int ply) {
	if (_movesCur[ply] == _movesStart[ply + 1])
		return NO_MOVE;

	if (_movesCur[ply] == _movesStart[ply]) {
		move_t move = g_transTable->lookupBestMove(*board);

		if (move != NO_MOVE)
			swapMoveWithFirst(ply, move);
	} else
		sortMove(ply, board->current_player);

	return _moves[_movesCur[ply]++];
}

int MoveGenerator::compareMoves(move_t move1, move_t move2, int current_side) {
	if ((move1 & (CAPTURE_MOVE | CAPTURE_MOVE_EN_PASSANT)) && !(move2 & (CAPTURE_MOVE | CAPTURE_MOVE_EN_PASSANT)))
		return -1;

	if (!(move1 & (CAPTURE_MOVE | CAPTURE_MOVE_EN_PASSANT)) && (move2 & (CAPTURE_MOVE | CAPTURE_MOVE_EN_PASSANT)))
		return 1;

	if (getHistoryCounter(move1, current_side) > getHistoryCounter(move2, current_side))
		return -1;
	else
		return 1;
}

void MoveGenerator::swapMoveWithFirst(int ply, move_t move) {
	for (unsigned int i = _movesStart[ply]; i < _movesStart[ply + 1]; i++)
		if (move == _moves[i])
		{
			move_t swap = _moves[_movesStart[ply]];
			_moves[_movesStart[ply]] = _moves[i];
			_moves[i] = swap;
			return;
		}
}

void MoveGenerator::sortMove(int ply, int side) {
	unsigned int min = _movesCur[ply];

	for (unsigned int i = _movesCur[ply] + 1; i < _movesStart[ply + 1]; i++)
	   if (compareMoves(_moves[i], _moves[min], side) < 0)
		  min = i;

	move_t swap = _moves[_movesCur[ply]];
	_moves[_movesCur[ply]] = _moves[min];
	_moves[min] = swap;
}

void MoveGenerator::incHistoryCounter(move_t move, int side) {
	_history[side << 12 | MOVE_GET(move, SOURCE) << 6 | MOVE_GET(move, DEST)]++;
}

unsigned int MoveGenerator::getHistoryCounter(move_t move, int side) {
	return _history[side << 12 | MOVE_GET(move, SOURCE) << 6 | MOVE_GET(move, DEST)];
}

void MoveGenerator::clearHistory() {
	_history = {};
}

void MoveGenerator::ageHistory() {
	for (auto &i : _history)
		i >>= 8;
}

MoveGenerator *g_moveGenerator;
