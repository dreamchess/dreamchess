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

#include <cstdio>

#include "board.h"
#include "hashing.h"
#include "ttable.h"
#include "search.h"
#include "move.h"
#include "e_comm.h"

TTable::TTable(const size_t size) {
	const size_t maxEntries = (size << 20) / sizeof(Entry);
	size_t entries = 2;

	while (entries <= maxEntries)
		entries <<= 1;

	entries >>= 1;
	_indexMask = entries - 1;

	printf("Hash table size: %i MB\n", entries * sizeof(Entry) >> 20);
	
	_table = new Entry[entries];
	clear();
}

TTable::~TTable() {
	delete[] _table;
}

void TTable::storeBoard(const board_t &board, int eval, const EvalType evalType,
                        const int depth, const int ply, const int time_stamp, const move_t move) {
	const unsigned int index = board.hash_key & _indexMask;

	// Do not overwrite entries for this board at greater depth.
	if ((_table[index].evalType != EvalType::None) && (_table[index].hashKey == board.hash_key)
	    && (_table[index].depth > depth))
	return;

	/* Make mate-in-n values relative to board that's to be stored */
	if (eval < ALPHABETA_MIN + 1000)
		eval -= ply;
	else if (eval > ALPHABETA_MAX - 1000)
		eval += ply;

	_table[index].hashKey = board.hash_key;
	_table[index].eval = eval;
	_table[index].evalType = evalType;
	_table[index].depth = depth;
	_table[index].timestamp = time_stamp;
	_table[index].move = move;
}

void TTable::setBestMove(const board_t &board, const move_t move) {
	const unsigned int index = board.hash_key & _indexMask;

	if ((_table[index].evalType == EvalType::None) || (_table[index].hashKey != board.hash_key))
		storeBoard(board, 0, EvalType::PV, 0, 0, 0, move);
	else
		_table[index].move = move;
}

TTable::EvalType TTable::lookupBoard(const board_t &board, const int depth, const int ply, int &eval) const {
	const unsigned int index = board.hash_key & _indexMask;

#ifdef DEBUG_TTABLE
	_queries++;

	if (_queries % 100000 == 0)
		e_comm_send("#[TTable] Queries: %d; Hits: %d; Rate:: %.2f%%\n", _queries, _hits, _hits / (float) _queries * 100);
#endif

	if (_table[index].evalType == EvalType::None)
		return EvalType::None;

	if (_table[index].hashKey != board.hash_key)
		return EvalType::None;

#ifdef DEBUG_TTABLE
	_hits++;
#endif

	if (_table[index].depth < depth || _table[index].evalType == EvalType::PV)
		return EvalType::None;

	eval = _table[index].eval;

	/* Make mate-in-n values relative to current game position */
	if (eval < ALPHABETA_MIN + 1000)
		eval += ply;
	else if (eval > ALPHABETA_MAX - 1000)
		eval -= ply;

	return _table[index].evalType;
}

move_t TTable::lookupBestMove(const board_t &board) const {
	const unsigned int index = board.hash_key & _indexMask;

	if ((_table[index].evalType == EvalType::None) || (_table[index].hashKey != board.hash_key))
		return NO_MOVE;

	return _table[index].move;
}

void TTable::clear() {
	for (unsigned int i = 0; i <= _indexMask; ++i)
		_table[i].evalType = EvalType::None;

#ifdef DEBUG_TTABLE
	_queries = 0;
	_hits = 0;
#endif
}

// FIXME: temporary global access
TTable *g_transTable;
