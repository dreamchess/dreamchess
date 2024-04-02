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

#ifndef GUI_UI_SDLGL_H
#define GUI_UI_SDLGL_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_thread.h"

#include <GL/glew.h>

//#define NO_SDL_GLEXT
//#include "SDL_opengl.h"
//#include "SDL_joystick.h"

#include "credits.h"
#include "debug.h"
#include "dir.h"
#include "dreamchess.h"
#include "fen.h"
#include "history.h"
#include "options.h"
#include "theme.h"
#include "ui.h"
#include "ui_sdlgl_3d.h"
#include <gamegui.h>

#define GAME_TYPE_HUMAN_VS_CPU 0
#define GAME_TYPE_CPU_VS_HUMAN 1
#define GAME_TYPE_HUMAN_VS_HUMAN 2

#define LEFT (1 << 0)
#define RIGHT (1 << 1)
#define UP (1 << 2)
#define DOWN (1 << 3)

/* Define our booleans */
#define TRUE 1
#define FALSE 0

#define FADE_SPEED 0.5f
#define FADE_OUT 0
#define FADE_IN 1

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 16

/** Desired frames per second. */
#define FPS 200

/* Speed that text types, characters per second. */
#define STRING_TYPE_SPEED 20

/** Bouncy text amplitude. */
#define BOUNCE_AMP 2
/** Bouncy text wave length in characters. */
#define BOUNCE_LEN 10
/** Bouncy text speed in bounces per second. */
#define BOUNCE_SPEED 3

/** Focussed image scale value. */
#define IMAGE_SCALE -0.3f
/** Focussed image enlargement speed in enlargements per second. */
#define IMAGE_SPEED 2.0f

#define GUI_PIECE_PAWN 0
#define GUI_PIECE_ROOK 3
#define GUI_PIECE_BISHOP 2
#define GUI_PIECE_KNIGHT 1
#define GUI_PIECE_QUEEN 4
#define GUI_PIECE_KING 5
#define GUI_PIECE_AVATAR 6

/** Describes a texture. The OpenGL texture ID is paired with texture
 *  coordinates to allow for only a small portion of the image to be used.
 */
typedef struct texture {
	/** OpenGL Texture ID. */
	GLuint id;

	/** Upper-left u-coordinate. Ranges from 0.0f to 1.0f. */
	float u1;

	/** Upper-left v-coordinate. Ranges from 0.0f to 1.0f. */
	float v1;

	/** Lower-right u-coordinate. Ranges from 0.0f to 1.0f. */
	float u2;

	/** Lower-right v-coordinate. Ranges from 0.0f to 1.0f. */
	float v2;

	/** Width of texture in pixels. */
	int width;

	/** Height of texture in pixels. */
	int height;
} texture_t;

int get_show_egg(void);
void set_show_egg(int set);
int get_egg_req(void);
int get_screen_width(void);
int get_screen_height(void);

/* dialogs */
gg_dialog_t *dialog_saveload_create(gg_dialog_t *parent, int saving);
gg_dialog_t *dialog_system_create(void);
gg_dialog_t *dialog_quit_create(gg_dialog_t *parent);
gg_dialog_t *dialog_ingame_create(gg_dialog_t *parent);
gg_dialog_t *dialog_systemopts_create(gg_dialog_t *parent);
gg_dialog_t *dialog_message_create(char *message);
gg_dialog_t *dialog_promote_create(int colour);
gg_dialog_t *dialog_victory_create(result_t *result);
void show_message_dialog(char *message);

/* title_dialog.c */
gg_dialog_t *dialog_title_root_create(void);
gg_dialog_t *dialog_title_newgame_create(gg_dialog_t *parent);
void open_title_root_dialog(void);

/* transitions.c */
void set_fade_start(float start);
int draw_fade(int inout);
int draw_sonic_fade(int inout);
float get_ui_trans_pos(void);

/* xmlsavefile.c */
void load_saves_xml(void);
int write_save_xml(int slot);
int get_slot_used_status(int slot);
void set_slot_used_status(int slot, int status);
char *get_time_save(int index);
config_t *get_config_save(int index);
board_t *get_saved_board(int index);

/* gamegui_driver.c */
gg_event_t convert_event(SDL_Event *event);
gg_driver_t *get_gg_driver_sdlgl(void);

/* ui_sdlgl_3d.c */
void start_piece_move(int source, int dest);
int get_piece_moving_done(void);
void set_theme(struct theme_struct *theme, texture_t texture);

/* load_data.c */
void load_pieces(void);
void load_border(texture_t border[9], char *filename);
void unload_border(texture_t border[9]);

/* get_move.c */
int get_move(void);

/* ui_sdlgl.c */
void handle_system_events(SDL_Event *event);
int get_fading_out(void);
void set_fading_out(int fade);
void blit_fbo(void);

/* ingame_ui.c */
void draw_name_dialog(float xpos, float ypos, char *name, int left, int white);
void draw_health_bars(void);
void draw_backdrop(void);
void reset_transition(int in);
void transition_update(void);
void draw_ui_elements(void);

/* theme.c */
void load_theme(struct theme_struct *theme);
texture_t *get_white_pieces(void);
texture_t *get_black_pieces(void);
texture_t *get_white_piece(int index);
texture_t *get_black_piece(int index);
texture_t *get_backdrop(void);
texture_t *get_selector_tex(void);
char *get_white_name(void);
char *get_black_name(void);
texture_t *get_mouse_cursor(void);
texture_t *get_menu_mouse_cursor(void);
void load_theme_xml(char *xmlfile);
texture_t *get_menu_border(void);
texture_t *get_border(void);
void unload_theme(void);

/* draw_scene.c */
void draw_scene(board_t *b, GLuint fb, int reflections);
float get_mouse_square(void);

/* credits.c */
void draw_credits(int init);

/* ui_driver.c */
int get_turn_counter(void);
gg_dialog_style_t *get_ingame_style(void);
gg_dialog_style_t *get_menu_style(void);
int get_white_in_check(void);
int get_black_in_check(void);
int get_black_in_checkmate(void);
int get_white_in_checkmate(void);
int get_game_stalemate(void);
void set_switch_to_menu(int sw);
board_t *get_board(void);
void set_is_coordinates(int);
int get_is_coordinates(void);
int get_fading_out(void);
config_t *get_config(void);
void set_pgn_slot(int slot);
void set_quit_to_menu(int menu);
void set_set_loading(int set);
void set_title_process_retval(int ret);
void set_dialog_promote_piece(int piece);
int get_dialog_promote_piece(void);

/* system.c */
void go_3d(int width, int height);
void resize_window(int width, int height);
void set_mouse_pos(int x, int y);
int get_mouse_x(void);
int get_mouse_y(void);
int get_true_mouse_x(void);
int get_true_mouse_y(void);
float get_zerodepth(void);
void gl_swap(void);
void toggle_show_fps(void);
void init_gl(void);
float get_fps(void);
void update_fps_time(void);
int power_of_two(int input);
float get_gl_width();
float get_gl_height();
void set_is_minimized(bool minimized);

/* texture.c */
texture_t SDL_GL_LoadTexture(SDL_Surface *surface, SDL_Rect *area, int alpha, int clamp);
void draw_texture_uv(texture_t *texture, float xpos, float ypos, float width, float height, float zpos,
					 gg_colour_t *col, float u1, float v1, float u2, float v2, GLenum mode_h, GLenum mode_v);
void draw_texture(texture_t *texture, float xpos, float ypos, float width, float height, float zpos, gg_colour_t *col);
void draw_texture_fullscreen(texture_t *texture, float zpos);
void load_texture_png(texture_t *texture, char *filename, int alpha, int clamp);

/* draw_shape.c */
void draw_rect(int x, int y, int w, int h, gg_colour_t *col);
void draw_rect_fill_gradient(int x, int y, int w, int h, gg_colour_t *top_left, gg_colour_t *top_right,
							 gg_colour_t *bottom_left, gg_colour_t *bottom_right);
void draw_rect_fill(int x, int y, int w, int h, gg_colour_t *col);
void draw_tri(int x1, int y1, int x2, int y2, int x3, int y3, gg_colour_t *col);

/* colours.c */
#define COL_BLACK 0
#define COL_WHITE 1
#define COL_RED 2
#define COL_YELLOW 3

gg_colour_t *get_col(int colour);

#define SAVEGAME_SLOTS 99

#endif
