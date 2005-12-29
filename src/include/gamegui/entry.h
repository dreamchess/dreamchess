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

#define GG_ENTRY(W) GG_CHECK_CAST(W, gg_entry_get_class_id(), gg_entry_t)

#define GG_ENTRY_DATA \
    GG_WIDGET_DATA \
    char text[ENTRY_MAX_LEN + 1]; \
    int max_len; \
    int cursor_pos; \
    int display_pos;

/** Text entry widget state. */
typedef struct w_entry
{
    GG_ENTRY_DATA
}
gg_entry_t;

void gg_entry_render(gg_widget_t *widget, int x, int y, int focus);

int gg_entry_input(gg_widget_t *widget, gg_event_t event);

void gg_entry_init(gg_entry_t *entry);

gg_widget_t *gg_entry_create();

#endif /* GAMEGUI_ENTRY_H */
