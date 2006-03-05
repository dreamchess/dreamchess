/*  DreamChess
**  Copyright (C) 2004  The DreamChess project
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

typedef struct
{
    bitboard_t en_passent;
    int castle_flags;
    int fifty_moves;
    move_t move;
}
undo_data_t;

typedef struct state
{
    int done;
    int mode;
    int flags;
    int depth;
    board_t board;
    undo_data_t *undo_data;
    int moves;
}
state_t;

int my_turn(state_t *state);

#define STATE_NORMAL 0
#define STATE_CHECK 1
#define STATE_MATE 2
#define STATE_STALEMATE 3

int check_game_state(board_t *board);
void check_game_end(state_t *state);
char *coord_move_str(move_t *move);
void do_move(state_t *state, move_t *move);
void undo_move(state_t *state);
int check_abort();

#endif /* STATE_H */
