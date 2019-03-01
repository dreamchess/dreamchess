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

#include <gamegui/label.h>

static gg_colour_t col_grey = {0.5f, 0.5f, 0.5f, 1.0f};

static gg_colour_t col_trans = {0.0f, 0.0f, 0.0f, 0.0f};

static gg_colour_t col_texthighlight = {0.55f, 0.65f, 0.95f, 1.0f};

static gg_colour_t col_text = {1.0f, 1.0f, 1.0f, 1.0f};

gg_class_id gg_label_get_class_id(void) {
	GG_CHILD(gg_align_get_class_id())
}

/** Implements widget::render for text widgets. */
void gg_label_render(gg_widget_t *widget, int x, int y, int focus) {
	gg_label_t *label = GG_LABEL(widget);

	if (label->bgcolour.a != 0.0f)
		gg_system_draw_filled_rect(x, y, label->width_a, label->height_a, &label->bgcolour);

	x += label->xalign * (label->width_a - label->width);
	y += (1.0f - label->yalign) * (label->height_a - label->height);

	/* TODO Fix temporary hack */
	if (!widget->enabled)
		gg_system_draw_string(label->label, x, y, &col_grey, 0, 0);
	else if (focus != GG_FOCUS_NONE)
		gg_system_draw_string(label->label, x, y, &col_texthighlight, label->bouncy, 0);
	else
		gg_system_draw_string(label->label, x, y, &label->colour, 0, 0);
}

void gg_label_set_bouncy(gg_label_t *label, int bouncy) {
	label->bouncy = bouncy;
}

void gg_label_set_colour(gg_label_t *label, gg_colour_t *colour, gg_colour_t *bgcolour) {
	if (colour)
		label->colour = *colour;

	if (bgcolour)
		label->bgcolour = *bgcolour;
}

/** @brief Destroys a text widget.
 *
 *  @param widget The text widget.
 */
void gg_label_destroy(gg_widget_t *widget) {
	gg_label_t *label = GG_LABEL(widget);

	if (label->label)
		free(label->label);

	gg_widget_destroy(widget);
}

void gg_label_init(gg_label_t *label, char *text) {
	gg_align_init((gg_align_t *)label);

	label->render = gg_label_render;
	label->destroy = gg_label_destroy;
	label->id = gg_label_get_class_id();
	label->label = strdup(text);
	label->bouncy = 0;
	label->enabled = 1;
	label->colour = col_text;
	label->bgcolour = col_trans;
	gg_system_get_string_size(text, &label->width, &label->height);
	label->height += GG_BOUNCE_AMP;
}

/** @brief Creates a text widget.
 *
 *  A text widget consists of a single label. This widget has no input
 *  functionality.
 *
 *  @param string The text for the widget.
 *  @return The created text widget.
 */
gg_widget_t *gg_label_create(char *string) {
	gg_label_t *label = malloc(sizeof(gg_label_t));

	gg_label_init(label, string);

	return GG_WIDGET(label);
}
