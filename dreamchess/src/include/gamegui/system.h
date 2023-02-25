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

/** @file
 *  @brief Core gamegui system header file.
 */

#ifndef GAMEGUI_SYSTEM_H
#define GAMEGUI_SYSTEM_H

/** No children should get focus. */
#define GG_FOCUS_NONE 0
/** At most one child should get focus. */
#define GG_FOCUS_ONE 1
/** All children should get focus. */
#define GG_FOCUS_ALL 2
/** All children disabled. */
#define GG_FOCUS_DISABLED 3

/** Bouncy text amplitude in pixels. */
#define GG_BOUNCE_AMP 2
/** Bouncy text wave length in characters. */
#define GG_BOUNCE_LEN 10
/** Bouncy text speed in bounces per second. */
#define GG_BOUNCE_SPEED 3

#define GG_FLAG_BOUNCY    (1 << 0)
#define GG_FLAG_NO_SHADOW (1 << 1)

/** Unique value used to identify a class. */
typedef int gg_class_id;

/** Colour description. */
typedef struct gg_colour {
	/** Red channel. Ranges from 0.0f to 1.0f. */
	float r;

	/** Green channel. Ranges from 0.0f to 1.0f. */
	float g;

	/** Blue channel. Ranges from 0.0f to 1.0f. */
	float b;

	/** Alpha channel. Ranges from 0.0f (transparent) to 1.0f (opaque). */
	float a;
} gg_colour_t;

/** Rectangle. */
typedef struct gg_rect {
	/** Leftmost x coordinate of the rectangle. */
	int x;
	/** Lowermost y coordinate of the rectangle. */
	int y;
	/** Width of the rectangle in pixels. */
	int width;
	/** Height of the rectangle in pixels. */
	int height;
} gg_rect_t;

/** Tile image to fill target area. */
#define GG_MODE_TILE 0
/** Scale image to fill target area. */
#define GG_MODE_SCALE 1

/** Gamegui driver. Gamegui uses these functions to do all its rendering. */
typedef struct gg_driver {
	void (*draw_rect)(int x, int y, int width, int height, gg_colour_t *colour);
	void (*draw_filled_rect)(int x, int y, int width, int height, gg_colour_t *colour);
	void (*draw_gradient_rect)(int x, int y, int width, int height, gg_colour_t *top_left, gg_colour_t *top_right,
							   gg_colour_t *bottom_left, gg_colour_t *bottom_right);
	void (*draw_image)(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour);
	void (*get_image_size)(void *image, int *width, int *height);
	unsigned int (*get_ticks)(void);
	float (*get_screen_width)(void);

	/* Unicode string interface */
	unsigned int (*get_line_height)(void);
	unsigned int (*get_string_width)(const char *text);
	void (*draw_string)(const char *text, int x, int y, const gg_rect_t *clip, float align, int bounce, int no_shadow, gg_colour_t colour);
} gg_driver_t;

/** Value indicating an unregistered class. */
#define GG_CLASS_ID_NONE -1

/** Prints a typecasting error message and aborts the program. */
#define GG_CAST_ERROR(C) ((C *)gg_cast_error(__FILE__, __LINE__, #C))

#define GG_CHECK_CAST(W, I, C) ((C *)gg_check_cast((gg_widget_t *)W, I, __FILE__, __LINE__, #C))

#define GG_CHILD(C)                                                                                                    \
	static gg_class_id class_id = GG_CLASS_ID_NONE;                                                                    \
	if (class_id == GG_CLASS_ID_NONE)                                                                                  \
		class_id = gg_register_class(C);                                                                               \
	return class_id;

struct gg_widget;

gg_class_id gg_register_class(gg_class_id parent);

int gg_is_parent(gg_class_id parent, gg_class_id child);

struct gg_widget *gg_check_cast(struct gg_widget *widget, gg_class_id id, char *file, int line, char *type);

void gg_system_init(gg_driver_t *d);

void gg_system_exit(void);

void gg_system_draw_rect(int x, int y, int width, int height, gg_colour_t *colour);

void gg_system_draw_filled_rect(int x, int y, int width, int height, gg_colour_t *colour);

void gg_system_draw_gradient_rect(int x, int y, int width, int height, gg_colour_t *top_left, gg_colour_t *top_right,
								  gg_colour_t *bottom_left, gg_colour_t *bottom_right);

void gg_system_draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour);

void gg_system_draw_char(int c, int x, int y, gg_colour_t *colour);

void gg_system_get_image_size(void *image, int *width, int *height);

void gg_system_get_string_size(const char *s, int *width, int *height);

void gg_system_draw_string(char *s, int x, int y, gg_colour_t *colour, int bounce, float align);

unsigned int gg_system_get_ticks(void);

float gg_system_get_screen_width(void);

gg_colour_t gg_colour(float r, float g, float b, float a);

gg_rect_t gg_rect(int x, int y, int w, int h);

#endif
