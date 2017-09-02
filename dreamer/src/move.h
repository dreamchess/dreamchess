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

#ifndef DREAMER_MOVE_H
#define DREAMER_MOVE_H

#include <array>

#include "board.h"
#include "dreamer.h"

#define NORMAL_MOVE 0
#define CAPTURE_MOVE 1
#define CAPTURE_MOVE_EN_PASSANT 2
#define CASTLING_MOVE_KINGSIDE 4
#define CASTLING_MOVE_QUEENSIDE 8
#define RESIGN_MOVE 16
#define STALEMATE_MOVE 32
#define PROMOTION_MOVE_KNIGHT 64
#define PROMOTION_MOVE_BISHOP 128
#define PROMOTION_MOVE_ROOK 256
#define PROMOTION_MOVE_QUEEN 512
#define NO_MOVE 4096

#define MOVE_NO_PROMOTION_MASK 63
#define MOVE_PROMOTION_MASK 960

/* New move type */

#define MOVE_TYPE_MASK 0x3ff
#define MOVE_TYPE_SHIFT 0
#define MOVE_SOURCE_MASK 0xfc00
#define MOVE_SOURCE_SHIFT 10
#define MOVE_DEST_MASK 0x3f0000
#define MOVE_DEST_SHIFT 16
#define MOVE_CAPTURED_MASK 0x3c00000
#define MOVE_CAPTURED_SHIFT 22
#define MOVE_PIECE_MASK 0x3c000000
#define MOVE_PIECE_SHIFT 26

#define MOVE_GET(M, P) (((M) & MOVE_##P##_MASK) >>  MOVE_##P##_SHIFT)
#define MOVE_SET(M, P, V) ((M) = ((M) & ~MOVE_##P##_MASK) | ((V) << MOVE_##P##_SHIFT))
#define MOVE(PIECE, SOURCE, DEST, TYPE, CAPTURED) \
	(((PIECE) << MOVE_PIECE_SHIFT) | ((SOURCE) << MOVE_SOURCE_SHIFT) \
	| ((DEST) << MOVE_DEST_SHIFT) | ((TYPE) << MOVE_TYPE_SHIFT) \
	| ((CAPTURED) << MOVE_CAPTURED_SHIFT))

#define MOVE_IS_REGULAR(M) (((M) != NO_MOVE) && ((M) != RESIGN_MOVE) && ((M) != STALEMATE_MOVE))

class MoveGenerator {
public:
	MoveGenerator();
	~MoveGenerator();

	int computeLegalMoves(board_t *board, int ply);
	move_t getNextMove(board_t *board, int ply);
	void clearHistory();
	void ageHistory();
	void incHistoryCounter(move_t move, int side);

private:
	move_t *addWhiteRookMoves(board_t *board, move_t *move);
	move_t *addBlackRookMoves(board_t *board, move_t *move);
	move_t *addWhiteBishopMoves(board_t *board, move_t *move);
	move_t *addBlackBishopMoves(board_t *board, move_t *move);
	move_t *addWhiteQueenMoves(board_t *board, move_t *move);
	move_t *addBlackQueenMoves(board_t *board, move_t *move);
	move_t *addWhiteKnightMoves(board_t *board, move_t *move);
	move_t *addBlackKnightMoves(board_t *board, move_t *move);
	move_t *addWhiteKingMoves(board_t *board, move_t *move);
	move_t *addBlackKingMoves(board_t *board, move_t *move);
	move_t *addWhitePawnMoves(board_t *board, move_t *move);
	move_t *addBlackPawnMoves(board_t *board, move_t *move);
	move_t *addWhiteCastleMoves(board_t *board, move_t *move);
	move_t *addBlackCastleMoves(board_t *board, move_t *move);
	
	void freeRayMoves(int ***moves);
	void freeSingleMoves(int **moves);

	int compareMoves(move_t move1, move_t move2, int current_side);
	void swapMoveWithFirst(int ply, move_t move);
	void sortMove(int ply, int side);

	unsigned int getHistoryCounter(move_t move, int side);

	int ***_rookMoves;
	int ***_bishopMoves;
	int ***_queenMoves;
	int **_knightMoves;
	int **_kingMoves;
	int **_whitePawnCaptureMoves;
	int **_blackPawnCaptureMoves;

	// Add 1 for in_check function
	std::array<move_t, (MAX_DEPTH + 1) * 256> _moves;
	std::array<unsigned int, MAX_DEPTH + 2> _movesStart;
	std::array<unsigned int, MAX_DEPTH + 1> _movesCur;

	// History heurstic
	std::array<unsigned int, 2 * 64 * 64> _history;    
};

// FIXME: temporary global pointer
extern MoveGenerator *g_moveGenerator;

#endif
