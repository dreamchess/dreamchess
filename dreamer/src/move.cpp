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
#include "dreamer.h"
#include "commands.h"
#include "e_comm.h"

Move::Move() :
		_piece(0),
		_source(0),
		_dest(0),
		_type(Type::None),
		_captured(0) { }

Move::Move(unsigned int piece, unsigned int source, unsigned int dest, Type type, unsigned int captured) :
		_piece(piece),
		_source(source),
		_dest(dest),
		_type(type),
		_captured(captured) { }

bool Move::operator==(Move &rhs) const {
	return _type == rhs._type
		   && _piece == rhs._piece
		   && _source == rhs._source
		   && _dest == rhs._dest
		   && _captured == rhs._captured;
}

unsigned int Move::getPiece() const {
	return _piece;
}

unsigned int Move::getPieceKind() const {
	return _piece & PIECE_MASK;
}

unsigned int Move::getPieceColour() const {
	return _piece & 1;
}

unsigned int Move::getSource() const {
	return _source;
}

unsigned int Move::getDest() const {
	return _dest;
}

Move::Type Move::getType() const {
	return _type;
}

unsigned int Move::getCapturedPiece() const {
	return _captured;
}

bool Move::doesCapture() const {
	return static_cast<unsigned char>(_type) & CaptureBit;
}

bool Move::doesPromotion() const {
	return static_cast<unsigned char>(_type) & PromotionBit;
}

bool Move::isRegular() const {
	return static_cast<unsigned char>(_type) & RegularBit;
}

bool Move::isNone() const {
	return _type == Type::None;
}

enum class MoveResult {
	Clear,
	Obstruction,
	Illegal
};

template<int PIECE, int PLAYER>
MoveResult doSingleMove(const Board &board, Move *&move, const int source, const int dest) {
	// If there's a player piece at the destination, skip this possible move.
	if (board.bitboard[ALL + PLAYER] & square_bit[dest])
		return MoveResult::Obstruction;

	// If there's an opponent piece at the destination, this is a capture move.
	if (board.bitboard[ALL + OPPONENT(PLAYER)] & square_bit[dest])	{
		const int piece = board.findPiece<OPPONENT(PLAYER)>(dest);

		// If we are capturing a king, previous board position was illegal.
		if (piece == KING + OPPONENT(PLAYER))
			return MoveResult::Illegal;

		*move++ = Move(PIECE + PLAYER, source, dest, Move::Type::Capture, piece);
		return MoveResult::Obstruction;
	} else {
		// Normal move.
		*move++ = Move(PIECE + PLAYER, source, dest, Move::Type::Normal, 0);
		return MoveResult::Clear;
	}
}

template<int PIECE, int PLAYER>
bool doRayMoves(const Board &board, Move *&move, const int source, const int step, int count) {
	int dest = source;

	while (count-- != 0) {
		dest += step;

		switch(doSingleMove<PIECE, PLAYER>(board, move, source, dest)) {
		case MoveResult::Obstruction:
			return true;
		case MoveResult::Illegal:
			return false;
		case MoveResult::Clear:
			break;
		}
	}

	return true;
}

template<int PIECE, int PLAYER>
bool generateMoves(const Board &board, Move *&move) {
	bitboard_t bitboard = board.bitboard[PIECE + PLAYER];

	constexpr int sourceStart = (PLAYER == SIDE_WHITE ? 0 : 63);
	constexpr int sourceEnd = 63 - sourceStart;
	constexpr int sourceInc = (sourceEnd > sourceStart ? 1 : -1);

	// Look for the pieces, starting at player's side of the board.
	for (int source = sourceStart; source != sourceEnd + sourceInc; source += sourceInc)	{
		// If no more pieces of this type are found, we're done.
		if (!bitboard)
			return true;

		// If no piece of this type is at this square, continue searching.
		if (!(bitboard & SQUARE_BIT(source)))
			continue;

		const int maxLeft = source & 7;
		const int maxRight = 7 - maxLeft;
		const int maxDown = source >> 3;
		const int maxUp = 7 - maxDown;

		if (PIECE == ROOK || PIECE == QUEEN) {
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, -8, maxDown))
				return false;
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, -1, maxLeft))
				return false;
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, 1, maxRight))
				return false;
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, 8, maxUp))
				return false;
		}

		if (PIECE == BISHOP || PIECE == QUEEN) {
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, -9, std::min(maxLeft, maxDown)))
				return false;
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, -7, std::min(maxRight, maxDown)))
				return false;
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, 7, std::min(maxLeft, maxUp)))
				return false;
			if (!doRayMoves<PIECE, PLAYER>(board, move, source, 9, std::min(maxRight, maxUp)))
				return false;
		}

		if (PIECE == KNIGHT) {
			if (maxLeft >= 1 && maxDown >= 2 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 17) == MoveResult::Illegal)
				return false;
			if (maxRight >= 1 && maxDown >= 2 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 15) == MoveResult::Illegal)
				return false;
			if (maxLeft >= 2 && maxDown >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 10) == MoveResult::Illegal)
				return false;
			if (maxRight >= 2 && maxDown >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 6) == MoveResult::Illegal)
				return false;
			if (maxLeft >= 2 && maxUp >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 6) == MoveResult::Illegal)
				return false;
			if (maxRight >= 2 && maxUp >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 10) == MoveResult::Illegal)
				return false;
			if (maxLeft >= 1 && maxUp >= 2 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 15) == MoveResult::Illegal)
				return false;
			if (maxRight >= 1 && maxUp >= 2 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 17) == MoveResult::Illegal)
				return false;
		}

		if (PIECE == KING) {
			if (maxDown >= 1) {
				if (maxLeft >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 9) == MoveResult::Illegal)
					return false;
				if (doSingleMove<PIECE, PLAYER>(board, move, source, source - 8) == MoveResult::Illegal)
					return false;
				if (maxRight >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 7) == MoveResult::Illegal)
					return false;
			}
			if (maxLeft >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source - 1) == MoveResult::Illegal)
				return false;
			if (maxRight >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 1) == MoveResult::Illegal)
				return false;
			if (maxUp >= 1) {
				if (maxLeft >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 7) == MoveResult::Illegal)
					return false;
				if (doSingleMove<PIECE, PLAYER>(board, move, source, source + 8) == MoveResult::Illegal)
					return false;
				if (maxRight >= 1 && doSingleMove<PIECE, PLAYER>(board, move, source, source + 9) == MoveResult::Illegal)
					return false;
			}
		}

		// Remove piece from the copy of the bitboard.
		bitboard ^= SQUARE_BIT(source);
	}

	return true;
}

template<int PLAYER>
bool generatePawnCaptures(const Board &board, Move *&move, const int source, const int dest) {
	// If there's not an opponent piece at the destination, skip this possible move. */
	if (board.bitboard[ALL + OPPONENT(PLAYER)] & square_bit[dest]) {
		const int piece = board.findPiece<OPPONENT(PLAYER)>(dest);

		// If we are capturing a king, previous board position was illegal.
		if (piece == KING + OPPONENT(PLAYER))
			return false;

		// The move is legal.
		if (PLAYER == SIDE_WHITE ? dest <= 55 : dest >= 8) {
			*move++ = Move(PAWN + PLAYER, source, dest, Move::Type::Capture, piece);
		} else {
			*move++ = Move(QUEEN + PLAYER, source, dest, Move::Type::PromotionCapture, piece);
			*move++ = Move(ROOK + PLAYER, source, dest, Move::Type::PromotionCapture, piece);
			*move++ = Move(BISHOP + PLAYER, source, dest, Move::Type::PromotionCapture, piece);
			*move++ = Move(KNIGHT + PLAYER, source, dest, Move::Type::PromotionCapture, piece);
		}
	} else if (board.en_passant & square_bit[dest]) {
		// En passant capture.
		*move++ = Move(PAWN + PLAYER, source, dest, Move::Type::EnPassant, PAWN + OPPONENT(PLAYER));
	}

	return true;
}

template<int PLAYER>
bool generatePawnMoves(const Board &board, Move *&move) {
	bitboard_t bitboard = board.bitboard[PAWN + PLAYER];
	const bitboard_t bitboard_all = board.bitboard[WHITE_ALL] | board.bitboard[BLACK_ALL];

	constexpr int sourceStart = (PLAYER == SIDE_WHITE ? 8 : 55);
	constexpr int sourceEnd = 63 - sourceStart;
	constexpr int sourceInc = (sourceEnd > sourceStart ? 1 : -1);
	constexpr int step = (PLAYER == SIDE_WHITE ? 8 : -8);

	for (int source = sourceStart; source != sourceEnd + sourceInc; source += sourceInc) {
		if (!bitboard)
			return move;

		if (!(bitboard & square_bit[source]))
			continue;

		int dest = source + step;

		if (!(bitboard_all & square_bit[dest])) {
			if (PLAYER == SIDE_WHITE ? dest <= 55 : dest >= 8) {
				// Normal move.
				*move++ = Move(PAWN + PLAYER, source, dest, Move::Type::Normal, 0);

				if (PLAYER == SIDE_WHITE ? source <= 15 : source >= 48) {
					dest += step;
					if (!(bitboard_all & square_bit[dest])) {
						// Double push.
						*move++ = Move(PAWN + PLAYER, source, dest, Move::Type::Normal, 0);
					}
				}
			} else {
				// Pawn promotion.
				*move++ = Move(QUEEN + PLAYER, source, dest, Move::Type::Promotion, 0);
				*move++ = Move(ROOK + PLAYER, source, dest, Move::Type::Promotion, 0);
				*move++ = Move(BISHOP + PLAYER, source, dest, Move::Type::Promotion, 0);
				*move++ = Move(KNIGHT + PLAYER, source, dest, Move::Type::Promotion, 0);
			}
		}

		// Check for capture moves.
		const int maxLeft = source & 7;
		const int maxRight = 7 - maxLeft;

		if (maxLeft >= 1 && !generatePawnCaptures<PLAYER>(board, move, source, source + step - 1))
			return false;
		if (maxRight >= 1 && !generatePawnCaptures<PLAYER>(board, move, source, source + step + 1))
			return false;

		// Remove piece from the copy of the bitboard.
		bitboard ^= square_bit[source];
	}

	return true;
}

template<>
bool generateMoves<PAWN, SIDE_WHITE>(const Board &board, Move *&move) {
	return generatePawnMoves<SIDE_WHITE>(board, move);
}

template<>
bool generateMoves<PAWN, SIDE_BLACK>(const Board &board, Move *&move) {
	return generatePawnMoves<SIDE_BLACK>(board, move);
}

void MoveGenerator::addWhiteCastleMoves(const Board &board, Move *&move) {
	// Kingside castle. Check for empty squares.
	if ((board.castle_flags & WHITE_CAN_CASTLE_KINGSIDE) &&
		(!((board.bitboard[BLACK_ALL] | board.bitboard[WHITE_ALL]) &
		WHITE_EMPTY_KINGSIDE))) {
		*move++ = Move(WHITE_KING, SQUARE_E1, SQUARE_G1, Move::Type::KingsideCastle, 0);
	}

	// Queenside castle. Check for empty squares.
	if ((board.castle_flags & WHITE_CAN_CASTLE_QUEENSIDE) &&
		(!((board.bitboard[BLACK_ALL] | board.bitboard[WHITE_ALL]) &
		WHITE_EMPTY_QUEENSIDE))) {
		*move++ = Move(WHITE_KING, SQUARE_E1, SQUARE_C1, Move::Type::QueensideCastle, 0);
	}
}

void MoveGenerator::addBlackCastleMoves(const Board &board, Move *&move) {
	/* Kingside castle. Check for empty squares. */
	if ((board.castle_flags & BLACK_CAN_CASTLE_KINGSIDE) &&
		(!((board.bitboard[BLACK_ALL] | board.bitboard[WHITE_ALL]) &
		BLACK_EMPTY_KINGSIDE))) {
		*move++ = Move(BLACK_KING, SQUARE_E8, SQUARE_G8, Move::Type::KingsideCastle, 0);
	}

	/* Queenside castle. Check for empty squares. */
	if ((board.castle_flags & BLACK_CAN_CASTLE_QUEENSIDE) &&
		(!((board.bitboard[BLACK_ALL] | board.bitboard[WHITE_ALL]) &
		BLACK_EMPTY_QUEENSIDE))) {
		*move++ = Move(BLACK_KING, SQUARE_E8, SQUARE_C8, Move::Type::QueensideCastle, 0);
	}
}

MoveGenerator::MoveGenerator() :
		_moves{},
		_movesStart{},
		_movesCur{} {
	clearHistory();
}

int MoveGenerator::computeLegalMoves(const Board &board, int ply) {
	Move *move = &_moves[_movesStart[ply]];

	if (board.current_player == SIDE_WHITE) {
		addWhiteCastleMoves(board, move);

		if (!generateMoves<KING, SIDE_WHITE>(board, move)
			|| !generateMoves<QUEEN, SIDE_WHITE>(board, move)
			|| !generateMoves<ROOK, SIDE_WHITE>(board, move)
			|| !generateMoves<BISHOP, SIDE_WHITE>(board, move)
			|| !generateMoves<KNIGHT, SIDE_WHITE>(board, move)
			|| !generateMoves<PAWN, SIDE_WHITE>(board, move))
			return -1;
	} else {
		addBlackCastleMoves(board, move);

		if (!generateMoves<KING, SIDE_BLACK>(board, move)
			|| !generateMoves<QUEEN, SIDE_BLACK>(board, move)
			|| !generateMoves<ROOK, SIDE_BLACK>(board, move)
			|| !generateMoves<BISHOP, SIDE_BLACK>(board, move)
			|| !generateMoves<KNIGHT, SIDE_BLACK>(board, move)
			|| !generateMoves<PAWN, SIDE_BLACK>(board, move))
			return -1;
}

	_movesStart[ply + 1] = _movesStart[ply] + move - &_moves[_movesStart[ply]];
	_movesCur[ply] = _movesStart[ply];
	return 0;
}

bool MoveGenerator::isCheck(Board &board, int ply) {
	// FIXME
	board.current_player = OPPONENT(board.current_player);
	bool isCheck =  computeLegalMoves(board, ply) < 0;
	board.current_player = OPPONENT(board.current_player);
	return isCheck;
}

Move MoveGenerator::getNextMove(const Board &board, int ply) {
	if (_movesCur[ply] == _movesStart[ply + 1])
		return Move();

	if (_movesCur[ply] == _movesStart[ply]) {
		Move move = g_transTable->lookupBestMove(board);

		if (!move.isNone())
			swapMoveWithFirst(ply, move);
	} else
		sortMove(ply, board.current_player);

	return _moves[_movesCur[ply]++];
}

int MoveGenerator::compareMoves(Move move1, Move move2, int current_side) {
	if (move1.doesCapture() && !move2.doesCapture())
		return -1;

	if (!move1.doesCapture() && move2.doesCapture())
		return 1;

	if (getHistoryCounter(move1, current_side) > getHistoryCounter(move2, current_side))
		return -1;
	else
		return 1;
}

void MoveGenerator::swapMoveWithFirst(int ply, Move move) {
	for (unsigned int i = _movesStart[ply]; i < _movesStart[ply + 1]; i++)
		if (move == _moves[i])
		{
			Move swap = _moves[_movesStart[ply]];
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

	Move swap = _moves[_movesCur[ply]];
	_moves[_movesCur[ply]] = _moves[min];
	_moves[min] = swap;
}

void MoveGenerator::incHistoryCounter(Move move, int side) {
	_history[side << 12 | move.getSource() << 6 | move.getDest()]++;
}

unsigned int MoveGenerator::getHistoryCounter(Move move, int side) {
	return _history[side << 12 | move.getSource() << 6 | move.getDest()];
}

void MoveGenerator::clearHistory() {
	_history = {};
}

void MoveGenerator::ageHistory() {
	for (auto &i : _history)
		i >>= 8;
}

MoveGenerator *g_moveGenerator;
