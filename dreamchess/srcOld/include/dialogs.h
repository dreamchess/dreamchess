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

#ifndef DIALOGS_DIALOGS_H
#define DIALOGS_DIALOGS_H

#include "dreamchess.h"
#include "gamegui.h"
#include "debug.h"
#include "ui.h"
#include "engine.h"
#include "options.h"

/* Define our booleans */
#define TRUE  1
#define FALSE 0

gg_dialog_style_t *get_ingame_style(void);
gg_dialog_style_t *get_menu_style(void);

gg_dialog_t *dialog_ingame_create(gg_dialog_t *parent);
gg_dialog_t *dialog_system_create(void);
gg_dialog_t *dialog_quit_create(gg_dialog_t *parent);
gg_dialog_t *dialog_message_create(const char *message);
gg_dialog_t *dialog_promote_create(int colour);
gg_dialog_t *dialog_victory_create(result_t *result);
gg_dialog_t *dialog_resolution_create(gg_dialog_t *parent);
gg_dialog_t *dialog_saveload_create(gg_dialog_t *parent, int saving);
void show_message_dialog( const char *message );
gg_dialog_t *dialog_error_create(gg_dialog_t *parent, const char *message1, const char *message2);
gg_dialog_t *dialog_engine_error_create(void);
gg_dialog_t *dialog_time_create(gg_dialog_t *parent);
gg_dialog_t *dialog_chat_create(gg_dialog_t *parent);
gg_dialog_t *dialog_systemopts_create(gg_dialog_t *parent);

/* FIXME */
void set_quit_to_menu( int menu );

#endif
