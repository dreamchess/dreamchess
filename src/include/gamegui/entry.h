/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GAMEGUI_ENTRY_H
#define GAMEGUI_ENTRY_H

#include <gamegui/system.h>
#include <gamegui/widget.h>

#define ENTRY_SPACING 2
#define ENTRY_MAX_LEN 255
#define ENTRY_CURSOR "|"

#define W_ENTRY(W) CHECK_CAST(W, w_entry_get_class_id(), w_entry_t)

#define W_ENTRY_DATA \
    W_WIDGET_DATA \
    char text[ENTRY_MAX_LEN + 1]; \
    int max_len; \
    int cursor_pos; \
    int display_pos; \
    int display_len;

/** Text entry widget state. */
typedef struct w_entry
{
    W_ENTRY_DATA
}
w_entry_t;

void w_entry_render(w_widget_t *widget, int x, int y, int focus);

int w_entry_input(w_widget_t *widget, ui_event_t event);

void w_entry_init(w_entry_t *entry);

w_widget_t *w_entry_create();

#endif /* GAMEGUI_ENTRY_H */
