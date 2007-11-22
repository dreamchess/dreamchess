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

#ifndef STATE_H
#define STATE_H

#include "board.h"

#define MODE_WHITE 0
#define MODE_BLACK 1
#define MODE_IDLE 2
#define MODE_FORCE 3
#define MODE_QUIT 4

#define FLAG_IGNORE_MOVE 1<<0
#define FLAG_NEW_GAME 1<<1

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
    undo_data_t *undo_data;
    int moves;
    int options;
    struct time_control time;
    int engine_time;
    int opponent_time;
    int move_now_time;
}
state_t;

int my_turn(state_t *state);

#define STATE_NORMAL 0
#define STATE_CHECK 1
#define STATE_MATE 2
#define STATE_STALEMATE 3

#define OPTION_QUIESCE 0

int check_game_state(board_t *board);
void check_game_end(state_t *state);
char *coord_move_str(move_t *move);
void do_move(state_t *state, move_t *move);
void undo_move(state_t *state);
int check_abort();
int get_option(int option);
void set_option(int option, int value);
int get_time();

#endif /* STATE_H */
