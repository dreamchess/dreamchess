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
    int difficulty;
}
config_t;

int dreamchess(void *arg);

void game_view_next();
void game_view_prev();
void game_undo();
int game_want_move();
void game_make_move(move_t *move, int ui_update);
void game_retract_move();
void game_move_now();
void game_quit();
void game_get_move_list(char ***list, int *total, int *view);
int game_save( int slot );
int game_load( int slot );
void game_make_move_str(char *move_str, int ui_update);

int set_resolution(int init);
void toggle_fullscreen();

#endif /* DREAMCHESS_H */
