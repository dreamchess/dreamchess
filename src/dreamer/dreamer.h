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

#ifndef DREAMER_H
#define DREAMER_H

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
    move_t move;
}
undo_data_t;

struct time_control
{
    int mps;
    int base;
    int inc;
};

typedef struct state
{
    int done;
    int mode;
    int flags;
    int depth;
    board_t board;
    board_t root_board;
    undo_data_t *undo_data;
    int moves;
    int options;
    struct time_control time;
    timer engine_time;
    timer move_time;
    move_t hint;
    move_t ponder_opp_move;
    move_t ponder_my_move;
    move_t ponder_actual_move;
}
state_t;

int my_turn(state_t *state);

#define STATE_NORMAL 0
#define STATE_CHECK 1
#define STATE_MATE 2
#define STATE_STALEMATE 3

#define OPTION_QUIESCE 0
#define OPTION_POST 1
#define OPTION_PONDER 2

int engine(void);
int check_game_state(board_t *board, int ply);
void check_game_end(state_t *state);
void do_move(state_t *state, move_t move);
void undo_move(state_t *state);
int check_abort(int ply);
int get_option(int option);
void set_option(int option, int value);
int get_time(void);
int is_check(board_t *board, int ply);
void send_move(state_t *state, move_t move);
void set_move_time(void);

#endif /* DREAMER_H */
