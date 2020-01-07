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

#include "ui_sdlgl.h"
#include "unicode.h"

float fade_start;
int in_transition = FALSE;

#define SONIC_FADE_SPEED 2.0f

void set_fade_start(float start) {
	fade_start = start;
}

int draw_fade(int inout) {
	float amount = (gg_system_get_ticks() - fade_start) / (FADE_SPEED * 1000);

	gg_colour_t col = {0.0f, 0.0f, 0.0f, 1.0f - amount};

	if (inout == FADE_OUT)
		col.a = amount;

	if (col.a > 1.0f)
		col.a = 1.0f;

	if (col.a >= 0.0f)
		gg_system_draw_filled_rect(0, 0, get_gl_width(), 480, &col);

	if (amount >= 1.0f)
		return FALSE;

	return TRUE;
}

int draw_sonic_fade(int inout) {
	float amount = (gg_system_get_ticks() - fade_start) / (SONIC_FADE_SPEED * 1000);
	int i = 0;

	gg_colour_t col_red = {1.0f, 0.0f, 0.0f, 1.0f};
	gg_colour_t col_blue = {0.0f, 0.0f, 1.0f, 1.0f};
	gg_colour_t col_yellow = {1.0f, 1.0f, 0.0f, 1.0f};
	gg_colour_t col_black = {0.0f, 0.0f, 0.0f, 1.0f};
	gg_colour_t col_white = {1.0f, 1.0f, 1.0f, 1.0f};

	if (amount < 1.0f) {
		if (inout == FADE_IN)
			amount = 1.0f - amount;

		if (amount < 0.7f)
			amount = (amount) / 0.7f;

		else if (amount >= 0.7f)
			amount = 1.0f;

		gg_system_draw_filled_rect(0, 480 - (480 * amount), 640, 480, &col_blue);
		gg_system_draw_filled_rect(640 - (640 * amount), 0, 640, (480 / 3), &col_yellow);

		unicode_string_render("DreamChess the chess game", 640 - (640 * amount) + 280, (480 / 3) - 30, 0.0f, 1.2f, 0, col_white);

		gg_system_draw_filled_rect(0, 0, (((640 / 3) + (480 / 14)) * amount) - (480 / 14), 480, &col_red);

		for (i = 0; i < 14; i++) {
			draw_tri((((640 / 3) + (480 / 14)) * amount) - (480 / 14) + 2, i * (480 / 14) - 2,
					 (((640 / 3) + (480 / 14)) * amount) - (480 / 14) + 2, i * (480 / 14) + (480 / 14) - 2,
					 (((640 / 3) + (480 / 14)) * amount) - (480 / 14) + (480 / 14) + 2,
					 i * (480 / 14) + ((480 / 14) / 2) - 2, &col_black);

			draw_tri((((640 / 3) + (480 / 14)) * amount) - (480 / 14), i * (480 / 14),
					 (((640 / 3) + (480 / 14)) * amount) - (480 / 14), i * (480 / 14) + (480 / 14),
					 (((640 / 3) + (480 / 14)) * amount) - (480 / 14) + (480 / 14), i * (480 / 14) + ((480 / 14) / 2),
					 &col_red);
		}

		unicode_string_render("Chess Hill", 640 - (640 * amount * 2) + 840, 480 - (480 / 3), 0.0f, 3.0f, 0, col_white);

		unicode_string_render("Zone", (640 * amount * 2) - 1000, 480 - (480 / 3) - 50, 0.0f, 3.0f, 0, col_white);
		unicode_string_render("1", (640 * amount * 2) - 860, 480 - (480 / 3) - 60, 0.0f, 4.0f, 0, col_yellow);
	}

	if (amount >= 1.0f) {
		return FALSE;
	}

	return TRUE;
}
