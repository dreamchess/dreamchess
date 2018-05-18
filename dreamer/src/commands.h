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

#ifndef DREAMER_COMMANDS_H
#define DREAMER_COMMANDS_H

#include "dreamer.h"

void command_handle(Dreamer *state, const char *command);
int command_check_abort(Dreamer *state, int ply, const char *command);
int command_usermove(Dreamer *state, const char *command);

char *san_move_str(Board &board, int ply, Move move);
char *coord_move_str(Move move);

int parse_move(Board &board, int ply, const char *command, Move *move);

#endif
