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
    int display_pos; \
    gg_signal_t action_pressed;

/** Text entry widget state. */
typedef struct w_entry
{
    GG_ENTRY_DATA
}
gg_entry_t;

gg_class_id gg_entry_get_class_id();

void gg_entry_render(gg_widget_t *widget, int x, int y, int focus);

int gg_entry_input(gg_widget_t *widget, gg_event_t event);

void gg_entry_init(gg_entry_t *entry, int width);

gg_widget_t *gg_entry_create(int width);

char *gg_entry_get_text(gg_entry_t *entry);

int gg_entry_set_text(gg_entry_t *entry, char *text);

void gg_entry_set_max_len(gg_entry_t *entry, int len);

#endif /* GAMEGUI_ENTRY_H */

