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

#include <stdlib.h>
#include <string.h>

#include <gamegui/clipping.h>
#include <gamegui/edit.h>

static gg_colour_t col_grey = {0.5f, 0.5f, 0.5f, 1.0f};

static gg_colour_t col_texthighlight = {0.55f, 0.65f, 0.95f, 1.0f};

static gg_colour_t col_text = {1.0f, 1.0f, 1.0f, 1.0f};

gg_class_id gg_edit_get_class_id(void) {
	GG_CHILD(gg_widget_get_class_id())
}

#if 0
static int string_width(char *s, int n)
{
    int retval;
    char c = s[n];

    s[n] = 0;
    gg_system_get_string_size(s, &retval, NULL);
    s[n] = c;
    return retval;
}
#endif

void gg_edit_render(gg_widget_t *widget, int x, int y, int focus) {
	gg_edit_t *edit = GG_EDIT(widget);
	gg_rect_t rect;
	gg_colour_t *colour = &col_text;
	struct gg_edit_line *line;

	switch (focus) {
	case GG_FOCUS_DISABLED:
		colour = &col_grey;
		break;
	case GG_FOCUS_ONE:
	case GG_FOCUS_ALL:
		colour = &col_texthighlight;
	}

	/* TODO Fix temporary hack */
	if (!widget->enabled)
		colour = &col_grey;

	gg_system_draw_rect(x, y, edit->width_a, edit->height_a, colour);

	x += EDIT_SPACING;
	y += EDIT_SPACING;

	rect.x = x;
	rect.y = y;
	rect.width = edit->width_a - 2 * EDIT_SPACING;
	rect.height = edit->height_a - 2 * EDIT_SPACING;
	gg_clipping_adjust(&rect);

	TAILQ_FOREACH(line, &edit->lines, entries) {
		gg_system_draw_string(line->text, x, y, colour, 0, 0);
		y += edit->line_height;
	}

	gg_clipping_undo();
}

int gg_edit_input(gg_widget_t *widget, gg_event_t event) {
	return 1;
}

void gg_edit_append(gg_edit_t *edit, char *text) {
	struct gg_edit_line *line = malloc(sizeof(struct gg_edit_line));
	line->text = strdup(text);
	TAILQ_INSERT_TAIL(&edit->lines, line, entries);
}

void gg_edit_init(gg_edit_t *edit, int width, int height) {
	gg_container_init((gg_container_t *)edit);

	edit->render = gg_edit_render;
	edit->input = gg_edit_input;
	edit->id = gg_edit_get_class_id();
	TAILQ_INIT(&edit->lines);
	edit->enabled = 1;
	edit->width = width + EDIT_SPACING * 2;
	edit->height = height + EDIT_SPACING * 2;
	edit->display_pos = 0;
	gg_system_get_string_size("W", NULL, &edit->line_height);
	edit->line_height += EDIT_LINE_SPACING;
	gg_container_append(GG_CONTAINER(edit), gg_scrollbarv_create(edit->height));
}

gg_widget_t *gg_edit_create(int width, int height) {
	gg_edit_t *edit = malloc(sizeof(gg_edit_t));

	gg_edit_init(edit, width, height);

	return GG_WIDGET(edit);
}
