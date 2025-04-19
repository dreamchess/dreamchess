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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gamegui/clipping.h>
#include <gamegui/signal.h>
#include <gamegui/system.h>
#include <gamegui/widget.h>

static int classes = 0;
static gg_class_id *parent_class = NULL;
static gg_driver_t *driver;

gg_class_id gg_register_class(gg_class_id parent) {
	parent_class = realloc(parent_class, (classes + 1) * sizeof(gg_class_id));

	parent_class[classes] = parent;

	return classes++;
}

int gg_is_parent(gg_class_id parent, gg_class_id child) {
	child = parent_class[child];

	while ((child != GG_CLASS_ID_NONE) && (parent != child))
		child = parent_class[child];

	if (child == GG_CLASS_ID_NONE)
		return 0;

	return 1;
}

gg_widget_t *gg_check_cast(gg_widget_t *widget, gg_class_id id, char *file, int line, char *type) {
	if (!widget) {
		fprintf(stderr, "Fatal error (%s:L%d): Widget is NULL.\n", file, line);
		exit(1);
	}

	if ((widget->id != id) && !gg_is_parent(id, widget->id)) {
		fprintf(stderr, "Fatal error (%s:L%d): Widget is not of type %s.\n", file, line, type);
		exit(1);
	}

	return widget;
}

void gg_system_init(gg_driver_t *d) {
	driver = d;
	gg_signal_init();
}

/* FIXME */
void gg_dialog_close_all(void);
void gg_dialog_cleanup(void);

void gg_system_exit(void) {
	gg_dialog_close_all();
	gg_dialog_cleanup();

	gg_signal_exit();

	if (parent_class)
		free(parent_class);
}

unsigned int gg_system_get_ticks(void) {
	return driver->get_ticks();
}

float gg_system_get_screen_width(void) {
	return driver->get_screen_width();
}

float gg_system_get_screen_height(void) {
	return driver->get_screen_height();
}

void gg_system_draw_rect(int x, int y, int width, int height, gg_colour_t *colour) {
	gg_rect_t dest;
	dest.x = x;
	dest.y = y;
	dest.width = width;
	dest.height = 1;
	gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
	dest.y += height - 1;
	gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
	dest.y = y + 1;
	dest.width = 1;
	dest.height = height - 2;
	gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
	dest.x += width - 1;
	gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
}

void gg_system_draw_filled_rect(int x, int y, int width, int height, gg_colour_t *colour) {
	gg_rect_t *clip = gg_clipping_get();

	if (clip) {
		gg_rect_t dest, dest_c;
		dest.x = x;
		dest.y = y;
		dest.width = width;
		dest.height = height;
		dest_c = gg_clipping_rect(&dest, clip);
		driver->draw_filled_rect(dest_c.x, dest_c.y, dest_c.width, dest_c.height, colour);
	} else
		driver->draw_filled_rect(x, y, width, height, colour);
}

void gg_system_draw_gradient_rect(int x, int y, int width, int height, gg_colour_t *top_left, gg_colour_t *top_right,
								  gg_colour_t *bottom_left, gg_colour_t *bottom_right) {
	gg_rect_t *clip = gg_clipping_get();

	if (clip) {
		gg_rect_t dest, dest_c;
		dest.x = x;
		dest.y = y;
		dest.width = width;
		dest.height = height;
		dest_c = gg_clipping_rect(&dest, clip);
		driver->draw_gradient_rect(dest_c.x, dest_c.y, dest_c.width, dest_c.height, top_left, top_right, bottom_left,
								   bottom_right);
	} else
		driver->draw_gradient_rect(x, y, width, height, top_left, top_right, bottom_left, bottom_right);
}

void gg_system_draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour) {
	gg_rect_t *clip = gg_clipping_get();

	if (clip) {
		gg_rect_t dest_c;
		gg_rect_t source_c;
		int x_offset;
		int y_offset;

		dest_c = gg_clipping_rect(&dest, clip);
		x_offset = dest_c.x - dest.x;
		y_offset = dest.y + dest.height - dest_c.y - dest_c.height;

		if (mode_h == GG_MODE_SCALE) {
			/* Magnification factor. */
			float x_mag = dest.width / (float)source.width;

			source_c.x = source.x + x_offset / x_mag;
			source_c.width = dest_c.width / x_mag;
		} else {
			source_c.x = source.x + x_offset % source.width;
			/* Width is undefined for GG_MODE_TILE. */
		}

		if (mode_v == GG_MODE_SCALE) {
			/* Magnification factor. */
			float y_mag = dest.height / (float)source.height;

			source_c.y = source.y + y_offset / y_mag;
			source_c.height = dest_c.height / y_mag;
		} else {
			source_c.y = source.y + y_offset % source.height;
			/* Height is undefined for GG_MODE_TILE. */
		}

		driver->draw_image(image, source_c, dest_c, mode_h, mode_v, colour);
	} else
		driver->draw_image(image, source, dest, mode_h, mode_v, colour);
}

void gg_system_get_image_size(void *image, int *width, int *height) {
	driver->get_image_size(image, width, height);
}

void gg_system_get_string_size(const char *s, int *width, int *height) {
	if (width)
		*width = driver->get_string_width(s);

	if (height)
		*height = driver->get_line_height();
}

void gg_system_draw_string(char *s, int x, int y, gg_colour_t *colour, int bounce, float align) {
	driver->draw_string(s, x, y, gg_clipping_get(), align, bounce, (colour->a == 2.0f ? 1 : 0), *colour);
}

gg_colour_t gg_colour(float r, float g, float b, float a) {
	gg_colour_t col;

	col.r = r;
	col.g = g;
	col.b = b;
	col.a = a;

	return col;
}

gg_rect_t gg_rect(int x, int y, int w, int h) {
	gg_rect_t rect;

	rect.x = x;
	rect.y = y;
	rect.width = w;
	rect.height = h;

	return rect;
}
