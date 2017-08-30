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

#ifndef DREAMER_TTABLE_H
#define DREAMER_TTABLE_H

#include "board.h"

// #define DEBUG_TTABLE

/** Transposition table. */

class TTable {
public:
	/** Evaluation type. */
	enum class EvalType {
		None,       ///< Empty table slot
		Lowerbound, ///< Lowerbound score
		Accurate,   ///< Accurate score
		Upperbound, ///< Upperbound score
		PV          ///< PV move only, no score
	};

	/**
	 * Transposition table constructor.
	 *
	 * @param size Desired size of transposition table in megabytes
	 */
	TTable(const size_t size);
	~TTable();

	/**
	 * Stores a board evaluation in the transposition table, unless a
	 * greater-depth entry for this board is already in the table.
	 *
	 * @param board    The evaluated board
	 * @param eval     The evaluation score for #board
	 * @param evalType The evaluation type for #eval
	 * @param depth    Search depth in plies, from #board
	 * @param ply      Number of plies between the current game position and #board
	 * @param move     The best move for #board
	 */
	void storeBoard(const board_t &board, int eval, const EvalType evalType, const int depth,
					const int ply, const int timestamp, const move_t move);

	/**
	 * Updates the best move for a board position. If the board is not
	 * present in the transposition table, it will be added.
	 *
	 * @param board The evaluated board
	 * @param move  The best move for #board
	 */
	void setBestMove(const board_t &board, const move_t move);

	/**
	 * Looks up a board evaluation in the transposition table.
	 *
	 * @param board     The board position to search for
	 * @param depth     The required search depth in plies
	 * @param ply       Number of plies between the current game position and #board
	 * @param[out] eval The evaluation score for #board, if return value is not EvalType::None
	 *
	 * @return          The evaluation type found, or EvalType::None
	 */
	EvalType lookupBoard(const board_t &board, const int depth, const int ply, int &eval) const;

	/**
	 * Looks up the best move for a board position.
	 *
	 * @param board The board position to search for
	 *
	 * @return The best move, or NO_MOVE if no entry was found
	 */
	move_t lookupBestMove(const board_t &board) const;

	/** Clears the transposition table. */
	void clear();

private:
	struct Entry {
		EvalType evalType;
		int eval;
		int depth;
		unsigned long long hashKey;
		int timestamp;
		move_t move;
	};

	Entry *_table;
	size_t _indexMask;
#ifdef DEBUG_TTABLE
	mutable unsigned int _queries;
	mutable unsigned int _hits;
#endif
};

// FIXME: temporary global pointer
extern TTable *g_transTable;

#endif // DREAMER_TTABLE_H
