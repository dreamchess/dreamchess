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

#ifndef DREAMER_MOVE_H
#define DREAMER_MOVE_H

#include <array>

#include "board.h"
#include "dreamer.h"

class MoveGenerator {
public:
	MoveGenerator();

	int computeLegalMoves(const Board &board, int ply);
	bool isCheck(Board &board, int ply);
	int checkGameState(Board &board, int ply);
	Move getNextMove(const Board &board, int ply);
	void clearHistory();
	void ageHistory();
	void incHistoryCounter(Move move, int side);

private:
	void addWhiteCastleMoves(const Board &board, Move *&move);
	void addBlackCastleMoves(const Board &board, Move *&move);

	void freeRayMoves(int ***moves);
	void freeSingleMoves(int **moves);

	int compareMoves(Move move1, Move move2, int current_side);
	void swapMoveWithFirst(int ply, Move move);
	void sortMove(int ply, int side);

	unsigned int getHistoryCounter(Move move, int side);

	// Add 1 for in_check function
	std::array<Move, (MAX_DEPTH + 1) * 256> _moves;
	std::array<unsigned int, MAX_DEPTH + 2> _movesStart;
	std::array<unsigned int, MAX_DEPTH + 1> _movesCur;

	// History heurstic
	std::array<unsigned int, 2 * 64 * 64> _history;    
};

// FIXME: temporary global pointer
extern MoveGenerator *g_moveGenerator;

#endif
