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

#include "dreamchess.h"
#include "gamegui.h"
#include "debug.h"
#include "ui.h"
#include "engine.h"
#include "options.h"

/* Define our booleans */
#define TRUE  1
#define FALSE 0

gg_dialog_style_t *get_ingame_style();
gg_dialog_style_t *get_menu_style();

gg_dialog_t *dialog_ingame_create(gg_dialog_t *parent);
gg_dialog_t *dialog_system_create();
gg_dialog_t *dialog_quit_create(gg_dialog_t *parent);
gg_dialog_t *dialog_message_create(char *message);
gg_dialog_t *dialog_promote_create(int colour);
gg_dialog_t *dialog_victory_create(result_t *result);
gg_dialog_t *dialog_resolution_create(gg_dialog_t *parent);
gg_dialog_t *dialog_saveload_create(gg_dialog_t *parent, int saving);
void show_message_dialog( char *message );
gg_dialog_t *dialog_error_create(gg_dialog_t *parent, char *message1, char *message2);
gg_dialog_t *dialog_engine_error_create();

/* FIXME */
void set_quit_to_menu( int menu );
