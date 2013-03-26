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

#ifndef GAMEGUI_EDIT_H
#define GAMEGUI_EDIT_H

#include <gamegui/system.h>
#include <gamegui/widget.h>
#include <gamegui/container.h>
#include <gamegui/queue.h>
#include <gamegui/scrollbarv.h>

#define EDIT_SPACING 2
#define EDIT_LINE_SPACING 2

#define GG_EDIT(W) GG_CHECK_CAST(W, gg_edit_get_class_id(), gg_edit_t)

struct gg_edit_line {
	TAILQ_ENTRY(gg_edit_line) entries;
	char *text;
};

#define GG_EDIT_DATA \
    GG_CONTAINER_DATA \
    TAILQ_HEAD(lines_head, gg_edit_line) lines; \
    int lines_nr; \
    int display_pos; \
    int line_height;

/** Text entry widget state. */
typedef struct w_edit
{
    GG_EDIT_DATA
}
gg_edit_t;

gg_class_id gg_edit_get_class_id(void);

void gg_edit_render(gg_widget_t *widget, int x, int y, int focus);

int gg_edit_input(gg_widget_t *widget, gg_event_t event);

void gg_edit_init(gg_edit_t *edit, int width, int height);

gg_widget_t *gg_edit_create(int width, int height);

int gg_edit_add_line(gg_edit_t *edit, char *text);

void gg_edit_append(gg_edit_t *edit, char *text);

#endif /* GAMEGUI_EDIT_H */

