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
#include "i18n.h"
#include "unicode.h"

void draw_move_lists(coord3_t offset, gg_colour_t *col_normal, gg_colour_t *col_high);
void draw_capture_list(coord3_t offset, gg_colour_t *col);

coord3_t transition;
float transition_speed = 1000;
float transition_start_pos;
float trans_amount = 320;
int trans_in;
int trans_reset;

void reset_transition(int in) {
	trans_in = in;
	trans_reset = TRUE;
}

void transition_update(void) {
	float ticks = SDL_GetTicks();

	if (trans_reset) {
		transition_start_pos = SDL_GetTicks();
		trans_reset = FALSE;
	}

	transition.y = trans_amount * ((ticks - transition_start_pos) / transition_speed);

	if (transition.y > trans_amount)
		transition.y = trans_amount;

	transition.x = trans_amount - transition.y;
}

float get_ui_trans_pos(void) {
	if (trans_in)
		return -transition.x;
	else
		return -transition.y;
}

static void draw_health_bar(coord3_t position, coord3_t size, int white) {
	int health;
	float bar_len;

	if (white) {
		health = 36 - ((get_board()->captured[WHITE_PAWN]) + (get_board()->captured[WHITE_ROOK] * 5) +
					   (get_board()->captured[WHITE_BISHOP] * 3) + (get_board()->captured[WHITE_KNIGHT] * 3) +
					   (get_board()->captured[WHITE_QUEEN] * 9));
	} else {
		health = 36 - ((get_board()->captured[BLACK_PAWN]) + (get_board()->captured[BLACK_ROOK] * 5) +
					   (get_board()->captured[BLACK_BISHOP] * 3) + (get_board()->captured[BLACK_KNIGHT] * 3) +
					   (get_board()->captured[BLACK_QUEEN] * 9));
	}

	if (health > 36)
		health = 36;
	if (health < 2)
		health = 2;

	gg_colour_t top_left_col = {0.59f, 0.60f, 0.86f, 1.0f};
	gg_colour_t bottom_left_col = {0.19f, 0.20f, 0.46f, 1.0f};
	gg_colour_t top_right_col = {0.79f, 0.80f, 1.0f, 1.0f};
	gg_colour_t bottom_right_col = {0.59f, 0.60f, 0.86f, 1.0f};

	bar_len = (size.x) * ((float)health / 36.0f);

	if (white) {
		draw_rect_fill(position.x, position.y, bar_len, size.y, get_col(COL_WHITE));
		draw_rect_fill_gradient(position.x + 1, position.y + 1, bar_len - 2, size.y - 2, &bottom_left_col,
								&bottom_right_col, &top_left_col, &top_right_col);
	} else {
		draw_rect_fill(position.x + (size.x - bar_len), position.y, bar_len, size.y, get_col(COL_WHITE));
		draw_rect_fill_gradient(position.x + 1 + (size.x - bar_len), position.y + 1, bar_len - 2, size.y - 2,
								&bottom_right_col, &bottom_left_col, &top_right_col, &top_left_col);
	}
}

static void draw_player_status(coord3_t offset, int white) {
	const char *tiedStr = _("Tied!");
	const char *checkMateStr = _("Checkmate!");
	const char *checkStr = _("Check!");
	const char *winStr = _("Victory!");

	if (white) { /* UGLY */
		if (get_game_stalemate() == TRUE)
			unicode_string_render(tiedStr, offset.x, offset.y, 0.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_WHITE));
		else if (get_white_in_checkmate() == TRUE)
			unicode_string_render(checkMateStr, offset.x, offset.y, 0.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_RED));
		else if (get_white_in_check() == TRUE)
			unicode_string_render(checkStr, offset.x, offset.y, 0.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_RED));

		if (get_black_in_checkmate() == TRUE)
			unicode_string_render(winStr, offset.x, offset.y, 0.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_WHITE));
	} else {
		if (get_game_stalemate() == TRUE)
			unicode_string_render(tiedStr, get_gl_width() - offset.x, offset.y, 1.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_WHITE));
		else if (get_black_in_checkmate() == TRUE)
			unicode_string_render(checkMateStr, get_gl_width() - offset.x, offset.y, 1.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_RED));
		else if (get_black_in_check() == TRUE)
			unicode_string_render(checkStr, get_gl_width() - offset.x, offset.y, 1.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_RED));

		if (get_white_in_checkmate() == TRUE)
			unicode_string_render(winStr, get_gl_width() - offset.x, offset.y, 1.0f, 1.0f, UNICODE_FLAG_BOUNCY, *get_col(COL_WHITE));
	}
}

void draw_ui_elements(void) {
	int namew, nameh;

	/* Enable/disable elements, set positions/sizes. */
	int avatars = TRUE;
	coord3_t avatar_size = {100, 100};
	coord3_t avatar_offset = {0 + get_ui_trans_pos(), 480 - avatar_size.y};

	int shadows = TRUE;
	coord3_t shadow_offset = {2, 2};

	int names = TRUE;
	char *white_name, *black_name;
	coord3_t name_offset = {100 + get_ui_trans_pos(), 480 - 60};
	coord3_t white_name_size, black_name_size;

	int clocks = FALSE;
	char *white_clock, *black_clock;
	coord3_t clock_offset = {300 + get_ui_trans_pos(), 480 - 60};
	coord3_t white_clock_size, black_clock_size;

	int health_bars = FALSE;
	coord3_t health_bar_offset = {100 + get_ui_trans_pos(), 480 - 40};
	coord3_t health_bar_size = {200, 15};

	int move_lists = TRUE;
	coord3_t move_list_offset = {30 + get_ui_trans_pos(), 350};

	int capture_lists = TRUE;
	coord3_t capture_list_offset = {10 + get_ui_trans_pos(), 180};

	int player_status = TRUE;
	coord3_t player_status_offset = {25 + get_ui_trans_pos(), 480 - 80};

	/* Get name sizes, string */
	white_name = get_white_name();
	gg_system_get_string_size(white_name, &namew, &nameh);
	white_name_size.x = namew;
	white_name_size.y = nameh;
	black_name = get_black_name();
	gg_system_get_string_size(black_name, &namew, &nameh);
	black_name_size.x = namew;
	black_name_size.y = nameh;

	/* Get clock sizes, string */
	white_clock = "00:00";
	gg_system_get_string_size(white_clock, &namew, &nameh);
	white_clock_size.x = namew;
	white_clock_size.y = nameh;

	black_clock = "00:00";
	gg_system_get_string_size(black_clock, &namew, &nameh);
	black_clock_size.x = namew;
	black_clock_size.y = nameh;

	/* Draw the avatars. */
	if (avatars) {
		if (shadows) {
			draw_texture(get_white_piece(GUI_PIECE_AVATAR), avatar_offset.x + shadow_offset.x,
						 avatar_offset.y - shadow_offset.y, avatar_size.x, avatar_size.y, 1.0f, get_col(COL_BLACK));

			draw_texture(get_black_piece(GUI_PIECE_AVATAR),
						 get_gl_width() - avatar_size.x - avatar_offset.x + shadow_offset.x,
						 avatar_offset.y - shadow_offset.y, avatar_size.x, avatar_size.y, 1.0f, get_col(COL_BLACK));
		}
		draw_texture(get_white_piece(GUI_PIECE_AVATAR), avatar_offset.x, avatar_offset.y, avatar_size.x, avatar_size.y,
					 1.0f, get_col(COL_WHITE));

		draw_texture(get_black_piece(GUI_PIECE_AVATAR), get_gl_width() - avatar_size.x - avatar_offset.x,
					 avatar_offset.y, avatar_size.x, avatar_size.y, 1.0f, get_col(COL_WHITE));
	}

	/* Draw the names */
	if (names) {
		unicode_string_render(white_name, name_offset.x, name_offset.y, 0.0f, 1.0f, 0, *get_col(COL_WHITE));
		unicode_string_render(black_name, get_gl_width() - name_offset.x, name_offset.y, 1.0f, 1.0f, 0, *get_col(COL_WHITE));
	}

	/* Draw the clocks */
	if (clocks) {
		unicode_string_render(white_clock, clock_offset.x, clock_offset.y, 0.0f, 1.0f, 0, *get_col(COL_WHITE));
		unicode_string_render(black_clock, get_gl_width() - clock_offset.x, clock_offset.y, 1.0f, 1.0f, 0, *get_col(COL_WHITE));
	}

	/* Draw the health bars. */
	if (health_bars) {
		coord3_t black;
		draw_health_bar(health_bar_offset, health_bar_size, TRUE);

		black = health_bar_offset;
		black.x = get_gl_width() - black.x - health_bar_size.x;
		draw_health_bar(black, health_bar_size, FALSE);
	}

	/* Draw the move lists. */
	if (move_lists)
		draw_move_lists(move_list_offset, get_col(COL_WHITE), get_col(COL_YELLOW));

	if (capture_lists)
		draw_capture_list(capture_list_offset, get_col(COL_WHITE));

	if (player_status) {
		draw_player_status(player_status_offset, TRUE);
		draw_player_status(player_status_offset, FALSE);
	}
}

/** @brief Renders the in-game backdrop. */
void draw_backdrop(void) {
	draw_texture_fullscreen(get_backdrop(), -1.0f);
}

/** @brief Renders the move list.
 *
 *  Only the last 5 moves (max) for each side are shown to prevent the screen
 *  from getting cluttered. The last move before the current board position
 *  is highlighted.
 *
 *  @param col_normal Text colour for move list.
 *  @param col_high Text colour for highlighting the last move.
 */
void draw_move_lists(coord3_t offset, gg_colour_t *col_normal, gg_colour_t *col_high) {
	char **list;
	int entries, view, i;
	int y;
	int last_white, last_black;

	gg_colour_t col_normal2 = *col_normal;
	gg_colour_t col_high2 = *col_normal;

	game_get_move_list(&list, &entries, &view);

	if (IS_BLACK(get_board()->turn)) {
		last_white = view;
		last_black = view - 1;
	} else {
		last_black = view;
		last_white = view - 1;
	}

	y = offset.y;
	for (i = last_white; i >= 0 && i >= last_white - 8; i -= 2) {
		char s[11];
		if (snprintf(s, 11, "%i.%s", (i >> 1) + 1, list[i]) >= 11)
			exit(1);
		if (i != view)
			unicode_string_render(s, offset.x + 5, y - 5, 0.0f, 1.0f, 0, col_normal2);
		else
			unicode_string_render(s, offset.x + 5, y - 5, 0.0f, 1.0f, 0, col_high2);
		y -= unicode_get_font_height();
		col_normal2.a -= 0.15f;
		col_high2.a -= 0.15f;
	}
	col_normal2 = *col_normal;
	col_high2 = *col_normal;
	y = offset.y;
	if (IS_BLACK(get_board()->turn)) {
		y -= unicode_get_font_height();
		col_normal2.a -= 0.15f;
		col_high2.a -= 0.15f;
	}
	for (i = last_black; i >= 0 && i >= last_black - (IS_BLACK(get_board()->turn) ? 6 : 8); i -= 2) {
		if (i != view)
			unicode_string_render(list[i], get_gl_width() - offset.x - 5, y - 5, 1.0f, 1.0f, 0, col_normal2);
		else
			unicode_string_render(list[i], get_gl_width() - offset.x - 5, y - 5, 1.0f, 1.0f, 0, col_high2);
		y -= unicode_get_font_height();
		col_normal2.a -= 0.15f;
		col_high2.a -= 0.15f;
	}
}

/** @brief Renders the list of captured pieces for both sides.
 *
 *  @param col The text colour to use.
 */
void draw_capture_list(coord3_t offset, gg_colour_t *col) {
	int blackVOffset = offset.y;
	int whiteVOffset = offset.y;	
	int i,j;
	int blackHealth, whiteHealth;
	int strLen = sizeof(int)+2; 
	char healthStr[strLen];

	for (i = 1; i <= 9; i += 2) {
		for (j = 0; j < get_board()->captured[i]; j++)
			draw_texture(get_black_piece(i / 2), offset.x + (j*8), whiteVOffset, 24, 24, 1.0f, get_col(COL_WHITE));
		if (get_board()->captured[i] > 0)
			whiteVOffset -= 28;
		
		for (j = 0; j < get_board()->captured[i - 1]; j++)
			draw_texture(get_white_piece((i - 1) / 2), get_gl_width() - offset.x - 24 - (j*8), blackVOffset, 24, 24, 1.0f, get_col(COL_WHITE));
		if (get_board()->captured[i - 1] > 0)
			blackVOffset -= 28;
	}

	whiteHealth = 36 - ((get_board()->captured[WHITE_PAWN]) + (get_board()->captured[WHITE_ROOK] * 5) +
		(get_board()->captured[WHITE_BISHOP] * 3) + (get_board()->captured[WHITE_KNIGHT] * 3) +
		(get_board()->captured[WHITE_QUEEN] * 9));
	blackHealth = 36 - ((get_board()->captured[BLACK_PAWN]) + (get_board()->captured[BLACK_ROOK] * 5) +
	   (get_board()->captured[BLACK_BISHOP] * 3) + (get_board()->captured[BLACK_KNIGHT] * 3) +
	   (get_board()->captured[BLACK_QUEEN] * 9));
	
	if (blackHealth > whiteHealth) {
		snprintf(healthStr, strLen, "+%i", blackHealth - whiteHealth) ;		
		unicode_string_render(healthStr, get_gl_width() - offset.x - 12 - (unicode_get_string_width(healthStr)/2.0f), blackVOffset+(unicode_get_font_height()/2.0f)-4, 0.0f, 1.0f, 0, *get_col(COL_WHITE));
	}	
	if (whiteHealth > blackHealth) {
		snprintf(healthStr, strLen, "+%i", whiteHealth - blackHealth);		
		unicode_string_render(healthStr, offset.x + 12 - (unicode_get_string_width(healthStr)/2.0f), whiteVOffset+(unicode_get_font_height()/2.0f)-4, 0.0f, 1.0f, 0, *get_col(COL_WHITE));
	}	
}

