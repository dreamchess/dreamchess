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

#ifndef DREAMER_DREAMER_H
#define DREAMER_DREAMER_H

#include "board.h"
#include "timer.h"

#define MODE_WHITE 0
#define MODE_BLACK 1
#define MODE_IDLE 2
#define MODE_FORCE 3
#define MODE_QUIT 4

#define FLAG_IGNORE_MOVE (1<<0)
#define FLAG_NEW_GAME (1<<1)
#define FLAG_PONDER (1<<2)
#define FLAG_DELAY_MOVE (1<<2)

#define MAX_DEPTH 30

typedef struct
{
    bitboard_t en_passant;
    int castle_flags;
    int fifty_moves;
    Move move;
}
undo_data_t;

struct time_control
{
    int mps;
    int base;
    int inc;
};

class Dreamer {
public:
    Dreamer();

    void run();
    static int checkGameState(Board &board, int ply);
    void checkGameEnd();
    void doMove(Move move);
    void undoMove();
    int checkAbort(int ply);
    int getOption(int option);
    void setOption(int option, int value);
    int getTime(void);
    void sendMove(Move move);
    void setMoveTime();
    bool isMyTurn();
        
    int done;
    int mode;
    int flags;
    int depth;
    Board board;
    Board root_board;
    undo_data_t *undo_data;
    int moves;
    int options;
    struct time_control time;
    Timer engineTime;
    Timer moveTime;
    Timer searchTime;
    Move hint;
    Move ponder_opp_move;
    Move ponder_my_move;
    Move ponder_actual_move;

private:
    void setStartTime();
    void updateClock();
};

extern Dreamer *g_dreamer;

#define STATE_NORMAL 0
#define STATE_CHECK 1
#define STATE_MATE 2
#define STATE_STALEMATE 3

#define OPTION_QUIESCE 0
#define OPTION_POST 1
#define OPTION_PONDER 2

#endif
