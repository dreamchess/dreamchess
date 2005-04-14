/*  DreamChess
**  Copyright (C) 2004-2005  The DreamChess project
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

#ifndef DREAMCHESS_H
#define DREAMCHESS_H

#include "board.h"

typedef struct arguments
{
    int argc;
    char **argv;
}
arguments_t;

#define PLAYER_UI 0
#define PLAYER_ENGINE 1

typedef struct config
{
    int player[2];
    int cpu_level;
}
config_t;

int dreamchess(void *arg);

void game_view_next();
void game_view_prev();
void game_undo();
int game_want_move();
void game_make_move(move_t *move);
void game_retract_move();
void game_move_now();
void game_quit();
void game_get_move_list(char ***list, int *total, int *view);

#endif /* DREAMCHESS_H */
