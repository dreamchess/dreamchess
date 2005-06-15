/*  DreamChess
 *  Copyright (C) 2003-2005  The DreamChess project
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

/** @file
 *  @brief User interface that uses the SDL and OpenGL libraries.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef WITH_UI_SDLGL

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_thread.h"
#include "SDL_opengl.h"
#include "SDL_joystick.h"

#include "dreamchess.h"
#include "history.h"
#include "ui.h"
#include "datadir.h"
#include "credits.h"
#include "ui_sdlgl_3d.h"

#include <gamegui.h>

/* Define our booleans */
#define TRUE  1
#define FALSE 0

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

/** Desired frames per second. */
#define FPS 9999

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

#ifdef _arch_dreamcast
#define AXIS_CURSOR_DISABLED
#define AXIS_VIEW_X 0
#define AXIS_VIEW_Y 1
#else
#define AXIS_CURSOR_X 0
#define AXIS_CURSOR_Y 1
#define AXIS_VIEW_X 2
#define AXIS_VIEW_Y 3
#endif

static void poll_move();

int piece_moving_done=1;
int piece_moving_start;
int piece_moving_dest;
int piece_moving_source;
int piece_moving_x_done;
int piece_moving_y_done;
float piece_moving_source_xpos;
float piece_moving_source_ypos;
float piece_moving_dest_xpos;
float piece_moving_dest_ypos;
float piece_moving_xpos;
float piece_moving_ypos;

#ifdef _arch_dreamcast
float dc_z;
#endif

int text_draw_char( float xpos, float ypos, float scale, int character, w_colour_t *col );

void start_piece_move( int source, int dest )
{
    piece_moving_start=SDL_GetTicks();

    piece_moving_done=0;

    piece_moving_dest=dest;
    piece_moving_source=source;

    piece_moving_source_xpos=(float)(source%8);
    piece_moving_source_ypos=(float)(source/8);

    piece_moving_dest_xpos=(float)(dest%8);
    piece_moving_dest_ypos=(float)(dest/8);

    piece_moving_xpos=piece_moving_source_xpos;
    piece_moving_ypos=piece_moving_source_ypos;

    piece_moving_x_done=0;
    piece_moving_y_done=0;
}

/* Some predefined colours. */

static w_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_grey =
    {
        0.5f, 0.5f, 0.5f, 1.0f
    };

static w_colour_t col_red =
    {
        1.0f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

static w_colour_t col_yellow =
    {
        1.0f, 1.0f, 0.0f, 1.0f
    };

void draw_rect(int x, int y, int w, int h, w_colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + h + 0.5f, 1.0f);
    glVertex3f(x + 0.5f, y + h + 0.5f, 1.0f);
    glEnd();
}

#ifdef _arch_dreamcast
void draw_rect_fill(int x, int y, int w, int h, w_colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    y = SCREEN_HEIGHT - y;
    glBegin(GL_NT_QUADS);
    glVertex3f(x, y, dc_z);
    glVertex3f(x + w, y, dc_z);
    glVertex3f(x + w, y - h, dc_z);
    glVertex3f(x, y - h, dc_z);
    glEnd();
}
#else
void draw_rect_fill(int x, int y, int w, int h, w_colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_QUADS);
    glVertex3f(x, y, 1.0f);
    glVertex3f(x + w, y, 1.0f);
    glVertex3f(x + w, y + h, 1.0f);
    glVertex3f(x, y + h, 1.0f);
    glEnd();
}
#endif
/*
   Key table?
*/
ui_event_t keys[94];

int string_type_pos=0;

int turn_counter_start=0;

void reset_turn_counter()
{
    turn_counter_start=SDL_GetTicks();
}

void PopulateKeyTable()
{
    int i;

    for ( i=0; i<94; i++ )
    {
        keys[i]=i+33;
    }
}

/** Describes a texture. The OpenGL texture ID is paired with texture
 *  coordinates to allow for only a small portion of the image to be used.
 */
typedef struct texture
{
    /** OpenGL Texture ID. */
    GLuint id;

    /** Lower-left u-coordinate. Ranges from 0.0f to 1.0f. */
    float u1;

    /** Lower-left v-coordinate. Ranges from 0.0f to 1.0f. */
    float v1;

    /** Upper-right u-coordinate. Ranges from 0.0f to 1.0f. */
    float u2;

    /** Upper-right v-coordinate. Ranges from 0.0f to 1.0f. */
    float v2;

    /** Width of texture in pixels. */
    int width;

    /** Height of texture in pixels. */
    int height;
}
texture_t;

/* Menu stuff */
static texture_t menu_title_tex;

static texture_t backdrop;
static texture_t border[9];
static texture_t menu_border[9];

#define LEFT (1 << 0)
#define RIGHT (1 << 1)
#define UP (1 << 2)
#define DOWN (1 << 3)

static ui_event_t convert_event(SDL_Event *event)
{
    static unsigned int pressed;

    switch (event->type)
    {
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
        case SDLK_RIGHT:
            return UI_EVENT_RIGHT;
        case SDLK_LEFT:
            return UI_EVENT_LEFT;
        case SDLK_UP:
            return UI_EVENT_UP;
        case SDLK_DOWN:
            return UI_EVENT_DOWN;
        case SDLK_RETURN:
            return UI_EVENT_ACTION;
        default:
            if (event->key.keysym.unicode <= 0xff)
                return event->key.keysym.unicode;
        }
        break;

    case SDL_JOYHATMOTION:
        switch (event->jhat.value)
        {
        case SDL_HAT_RIGHT:
            return UI_EVENT_RIGHT;
        case SDL_HAT_LEFT:
            return UI_EVENT_LEFT;
        case SDL_HAT_UP:
            return UI_EVENT_UP;
        case SDL_HAT_DOWN:
            return UI_EVENT_DOWN;
        }
        break;

    case SDL_JOYBUTTONDOWN:
        switch (event->jbutton.button)
        {
        case 0:
            return UI_EVENT_ACTION;
        case 1:
            return UI_EVENT_ESCAPE;
        case 2:
            return UI_EVENT_EXTRA3;
        case 3:
            return UI_EVENT_EXTRA2;
        case 4:
            return UI_EVENT_ESCAPE;
        }

#ifndef AXIS_CURSOR_DISABLED
    case SDL_JOYAXISMOTION:
        switch (event->jaxis.axis)
        {
        case AXIS_CURSOR_X:
            if (event->jaxis.value >= -15000 && event->jaxis.value <= 15000)
            {
                pressed &= ~(LEFT | RIGHT);
                return UI_EVENT_NONE;
            }

            if (!(pressed & LEFT) && (event->jaxis.value < 0))
            {
                pressed |= LEFT;
                return UI_EVENT_LEFT;
            }
            if (!(pressed & RIGHT) && (event->jaxis.value > 0))
            {
                pressed |= RIGHT;
                return UI_EVENT_RIGHT;
            }
            break;
        case AXIS_CURSOR_Y:
            if (event->jaxis.value >= -15000 && event->jaxis.value <= 15000)
            {
                pressed &= ~(UP | DOWN);
                return UI_EVENT_NONE;
            }
            if (!(pressed & UP) && (event->jaxis.value < 0))
            {
                pressed |= UP;
                return UI_EVENT_UP;
            }
            if (!(pressed & DOWN) && (event->jaxis.value > 0))
            {
                pressed |= DOWN;
                return UI_EVENT_DOWN;
            }
        }
#endif

    case SDL_MOUSEBUTTONDOWN:
        switch (event->button.button)
        {
        case SDL_BUTTON_LEFT:
            return UI_EVENT_ACTION;
        }
    }
    if ((event->type == SDL_KEYDOWN) && (event->key.keysym.unicode <= 0xff))
        return event->key.keysym.unicode;

    return UI_EVENT_NONE;
}

#define DC_PI 3.14159265358979323846

void set_perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * DC_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;


    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

int string_type_cur=0;
int string_type_start=0;

void reset_string_type_length()
{
    Uint32 ticks = SDL_GetTicks();
    string_type_pos=0;
    string_type_cur=0;
    string_type_start=ticks;
}

void update_string_type_length()
{
    string_type_pos=999;
    //Uint32 ticks = SDL_GetTicks();
    // float phase = ((ticks % (1000 / BOUNCE_SPEED)) / (float) (1000 / BOUNCE_SPEED));
    //printf( "In type loop thingy. %i %i\n\r", string_type_cur, string_type_pos );

    // printf( "FART %i\n\r", (ticks-string_type_start) );

    // string_type_pos=((ticks-string_type_start))/(STRING_TYPE_SPEED*1000);

    /*    if ( string_type_cur > STRING_TYPE_SPEED )
        {
     
            string_type_cur=0;
            if ( string_type_pos < 100 )
                string_type_pos++;
        }
        else
            string_type_cur++;*/
}

static void go_3d(int width, int height)
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    set_perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


/** @brief Renders a textured quad.
 *
 *  @param texture The texture to use.
 *  @param xpos The leftmost x-coordinate.
 *  @param ypos The lowermost y-coordinate.
 *  @param width The width in pixels.
 *  @param height The height in pixels.
 *  @param zpos The z-coordinate.
 *  @param col The colour to render with.
 */
#ifdef _arch_dreamcast
static void draw_texture( texture_t *texture, float xpos,
                          float ypos, float width, float height, float zpos,
                          w_colour_t *col )
{
    dc_z += 0.00001f;
    zpos = dc_z;
    glEnable( GL_TEXTURE_2D );

    glColor4f( col->r, col->g, col->b, col->a );
    glBindTexture(GL_TEXTURE_2D, texture->id);

    ypos = SCREEN_HEIGHT - ypos;

    glBegin(GL_NT_QUADS);
    glTexCoord2f(texture->u1, texture->v1);
    glVertex3f( xpos, ypos-height, zpos );
    glTexCoord2f(texture->u2, texture->v1);
    glVertex3f( xpos+width,  ypos-height, zpos );
    glTexCoord2f(texture->u2, texture->v2);
    glVertex3f( xpos+width,  ypos, zpos );
    glTexCoord2f(texture->u1, texture->v2);
    glVertex3f( xpos, ypos, zpos );
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
#else
static void draw_texture( texture_t *texture, float xpos,
                          float ypos, float width, float height, float zpos,
                          w_colour_t *col )
{
    glEnable( GL_TEXTURE_2D );

    glColor4f( col->r, col->g, col->b, col->a );
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glBegin(GL_QUADS);
    glTexCoord2f(texture->u1, texture->v1);
    glVertex3f( xpos, ypos+height, zpos );
    glTexCoord2f(texture->u2, texture->v1);
    glVertex3f( xpos+width,  ypos+height, zpos );
    glTexCoord2f(texture->u2, texture->v2);
    glVertex3f( xpos+width,  ypos, zpos );
    glTexCoord2f(texture->u1, texture->v2);
    glVertex3f( xpos, ypos, zpos );
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
#endif

#ifdef _arch_dreamcast
static void draw_texture_uv( texture_t *texture, float xpos,
                             float ypos, float width, float height, float zpos,
                             w_colour_t *col, float u1, float v1, float u2, float v2, GLenum mode_h, GLenum mode_v)
{
    dc_z += 0.00001f;
    zpos = dc_z;
    glEnable( GL_TEXTURE_2D );

    glColor4f( col->r, col->g, col->b, col->a );
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_h);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_v);

    ypos = SCREEN_HEIGHT - ypos;

    glBegin(GL_NT_QUADS);
    glTexCoord2f(u1, v1);
    glVertex3f( xpos, ypos-height, zpos );
    glTexCoord2f(u2, v1);
    glVertex3f( xpos+width,  ypos-height, zpos );
    glTexCoord2f(u2, v2);
    glVertex3f( xpos+width,  ypos, zpos );
    glTexCoord2f(u1, v2);
    glVertex3f( xpos, ypos, zpos );
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
#else
static void draw_texture_uv( texture_t *texture, float xpos,
                             float ypos, float width, float height, float zpos,
                             w_colour_t *col, float u1, float v1, float u2, float v2, GLenum mode_h, GLenum mode_v)
{
    glEnable( GL_TEXTURE_2D );

    glColor4f( col->r, col->g, col->b, col->a );
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_h);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_v);

    glBegin(GL_QUADS);
    glTexCoord2f(u1, v1);
    glVertex3f( xpos, ypos+height, zpos );
    glTexCoord2f(u2, v1);
    glVertex3f( xpos+width,  ypos+height, zpos );
    glTexCoord2f(u2, v2);
    glVertex3f( xpos+width,  ypos, zpos );
    glTexCoord2f(u1, v2);
    glVertex3f( xpos, ypos, zpos );
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
#endif

void text_draw_string( float xpos, float ypos, unsigned char *text, float scale, w_colour_t *col, int length );
void text_draw_string_right( float xpos, float ypos, unsigned char *text, float scale, w_colour_t *col, int length );
void text_draw_string_bouncy( float xpos, float ypos, unsigned char *text, float scale, w_colour_t *col, int length );
static int text_width(unsigned char *text);
static int text_height();
static int quit_to_menu;
static int title_process_retval;
static int set_loading=FALSE;

static int fps_enabled = 0;
static int vkeyboard_enabled = 0;
static int frames = 0;
static Uint32 fps_time = 0;
static float fps;

static char* themelist[25];
static int num_theme;
static int cur_theme;
static char** pieces_list;
static int pieces_list_total;
static int pieces_list_cur;
static char** board_list;
static int board_list_total;
static int board_list_cur;
static int flip_board;
static int dialog_promote_piece;

#define GUI_PIECE_PAWN     0
#define GUI_PIECE_ROOK     3
#define GUI_PIECE_BISHOP   2
#define GUI_PIECE_KNIGHT   1
#define GUI_PIECE_QUEEN    4
#define GUI_PIECE_KING     5
#define GUI_PIECE_AVATAR   6


static texture_t white_pieces[7];
static texture_t black_pieces[7];

static texture_t text_characters[256];

static config_t *config;

static dialog_style_t style_ingame, style_menu;

#if 0
/** Style used for all dialog boxes except the title menu. */
static style_t style_ingame =
    {
        1, 5, 20, 10,
        {0.0f, 0.0f, 0.0f, 0.5f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.8f, 0.8f, 0.8f, 1.0f}
    };

/** Position used for all dialog boxes except the title menu. */
static position_t pos_ingame =
    {
        320, 240,
        ALIGN_CENTER, ALIGN_CENTER
    };

/** Position used for the virtual keyboard. */
static position_t pos_vkeyboard =
    {
        320, 460,
        ALIGN_CENTER, ALIGN_TOP
    };

/** Style used for the title menu. */
static style_t style_title =
    {
        1, 0, 50, 10,
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.7f, 0.7f, 0.7f, 0.85f}
    };
#endif
/** Position used for the title menu. */
static position_t pos_title =
    {
        320, 100,
        ALIGN_CENTER, ALIGN_CENTER
    };

/* In-game dialog. */

/** The in-game dialog. Provides a set of gameplay-related actions to the
 *  user.
 */

static void retract_move(w_widget_t *widget, void *data)
{
    game_retract_move();
}

static void move_now(w_widget_t *widget, void *data)
{
    game_move_now();
}

static void view_prev(w_widget_t *widget, void *data)
{
    game_view_prev();
}

static void view_next(w_widget_t *widget, void *data)
{
    game_view_next();
}

/** @brief Creates the in-game dialog.
 *
 *  @return The created dialog.
 */
static w_dialog_t *dialog_ingame_create()
{
    w_widget_t *dialog;
    w_widget_t *vbox = w_vbox_create(0);

    w_widget_t *widget = w_action_create_with_label("Retract Move", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), retract_move, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("Move Now", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), move_now, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("View Previous Move", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), view_prev, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("View Next Move", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), view_next, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    dialog = w_dialog_create(vbox);
    return W_DIALOG(dialog);
}


/* Quit dialog. */

/** @brief Quits the current game.
 *
 *  Closes the dialog and causes the game to go back to the title menu.
 */
static void dialog_quit_ok(w_widget_t *widget, void *data)
{
    dialog_close();
    dialog_close();
    quit_to_menu = 1;
}

static void dialog_close_cb(w_widget_t *widget, void *data)
{
    dialog_close();
}

/** The quit dialog. Asks the user to confirm that he wants to quit the game.
 */

/** @brief Creates the quit confirmation dialog.
 *
 *  @return The created dialog.
 */
static w_dialog_t *dialog_quit_create()
{
    w_widget_t *dialog;
    w_widget_t *vbox = w_vbox_create(0);

    w_widget_t *widget = w_label_create("You don't really want to quit do ya?");
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_label_create("");
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("Yeah.. I suck..", 0.5f, 0.0f);
    w_action_set_callback(W_ACTION(widget), dialog_quit_ok, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("Of course not!", 0.5f, 0.0f);
    w_action_set_callback(W_ACTION(widget), dialog_close_cb, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    dialog = w_dialog_create(vbox);
    return W_DIALOG(dialog);
}


/* System dialog. */

/** The system dialog. Provides a set of system-related actions to the user.
 *  Currently this dialog only contains an item to quit the game. In the
 *  future this will be extended with load/save game items and possibly
 *  other items as well.
 */

/** @brief Opens the quit dialog. */
static void dialog_quit_open(w_widget_t *widget, void *data)
{
    dialog_open(dialog_quit_create());
}

/** @brief Creates the system dialog.
 *
 *  @return The created dialog.
 */
static w_dialog_t *dialog_system_create()
{
    w_widget_t *dialog;
    w_widget_t *vbox = w_vbox_create(0);
    w_widget_t *widget;

    widget = w_action_create_with_label("Return To Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), dialog_close_cb, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("Quit Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), dialog_quit_open, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    dialog = w_dialog_create(vbox);
    return W_DIALOG(dialog);
}


/* Victory dialog. */

static w_dialog_t *dialog_victory_create(result_t *result)
{
    w_widget_t *dialog;
    w_widget_t *hbox = w_hbox_create(20);
    w_widget_t *vbox = w_vbox_create(0);
    w_widget_t *image_l, *image_r;
    w_widget_t *action;
    w_widget_t *text;

    switch (result->code)
    {
    case RESULT_WHITE_WINS:
        image_l = w_image_create(&white_pieces[GUI_PIECE_KING]);
        image_r = w_image_create(&white_pieces[GUI_PIECE_QUEEN]);
        text = w_label_create("White won the match!");
        break;

    case RESULT_BLACK_WINS:
        image_l = w_image_create(&black_pieces[GUI_PIECE_KING]);
        image_r = w_image_create(&black_pieces[GUI_PIECE_QUEEN]);
        text = w_label_create("Black won the match!");
        break;

    default:
        image_l = w_image_create(&white_pieces[GUI_PIECE_KING]);
        image_r = w_image_create(&black_pieces[GUI_PIECE_KING]);
        text = w_label_create("The game ended in a draw!");
    }

    w_container_append(W_CONTAINER(vbox), text);
    text = w_label_create(result->reason);
    w_container_append(W_CONTAINER(vbox), text);
    text = w_label_create("");
    w_container_append(W_CONTAINER(vbox), text);
    action = w_action_create_with_label("Ok", 0.5f, 0.5f);
    w_action_set_callback(W_ACTION(action), dialog_close_cb, NULL);
    w_container_append(W_CONTAINER(vbox), action);
    w_container_append(W_CONTAINER(hbox), image_l);
    w_container_append(W_CONTAINER(hbox), vbox);
    w_container_append(W_CONTAINER(hbox), image_r);
    dialog = w_dialog_create(hbox);
    w_dialog_set_modal(W_DIALOG(dialog), 1);
    return W_DIALOG(dialog);
}

/* Title dialog. */

#define GAME_TYPE_HUMAN_VS_CPU 0
#define GAME_TYPE_CPU_VS_HUMAN 1
#define GAME_TYPE_HUMAN_VS_HUMAN 2

/** @brief Triggers gameplay start based on currently selected options. */
static void menu_title_start(w_widget_t *widget, void *data)
{
    set_loading=TRUE;
    dialog_close();
}

/** @brief Triggers DreamChess exit. */
static void menu_title_quit(w_widget_t *widget, void *data)
{
    title_process_retval = 1;
    dialog_close();
}

void dialog_title_players(w_widget_t *widget, void *data)
{
    switch (w_option_get_selected(W_OPTION(widget)))
    {
    case GAME_TYPE_HUMAN_VS_CPU:
        config->player[WHITE] = PLAYER_UI;
        config->player[BLACK] = PLAYER_ENGINE;
        flip_board = 0;
        break;
    case GAME_TYPE_CPU_VS_HUMAN:
        config->player[WHITE] = PLAYER_ENGINE;
        config->player[BLACK] = PLAYER_UI;
        flip_board = 1;
        break;
    case GAME_TYPE_HUMAN_VS_HUMAN:
        config->player[WHITE] = PLAYER_UI;
        config->player[BLACK] = PLAYER_UI;
        flip_board = 0;
    }
}

static void dialog_title_level(w_widget_t *widget, void *data)
{
    config->cpu_level = w_option_get_selected(W_OPTION(widget)) + 1;
}

static void dialog_title_theme(w_widget_t *widget, void *data)
{
    cur_theme = w_option_get_selected(W_OPTION(widget));
}

static void dialog_title_pieces(w_widget_t *widget, void *data)
{
    pieces_list_cur = w_option_get_selected(W_OPTION(widget));
}

static void dialog_title_board(w_widget_t *widget, void *data)
{
    board_list_cur = w_option_get_selected(W_OPTION(widget));
}

static w_dialog_t *dialog_title_create()
{
    w_widget_t *dialog;
    w_widget_t *vbox;
    w_widget_t *widget;
    w_widget_t *vbox2;
    w_widget_t *hbox;
    w_widget_t *label;
    int i;

    config = malloc(sizeof(config_t));
    config->player[WHITE] = PLAYER_UI;
    config->player[BLACK] = PLAYER_ENGINE;
    config->cpu_level = 1;
    cur_theme = 0;
    pieces_list_cur = 0;
    board_list_cur = 0;
    flip_board = 0;

    widget = w_action_create_with_label("Start Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), menu_title_start, NULL);
    vbox = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox), widget);

    label = w_label_create("Players:");
    w_align_set_alignment(W_ALIGN(label), 0.0f, 0.0f);
    vbox2 = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Difficulty:");
    w_align_set_alignment(W_ALIGN(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Theme:");
    w_align_set_alignment(W_ALIGN(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Chess Set:");
    w_align_set_alignment(W_ALIGN(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Board:");
    w_align_set_alignment(W_ALIGN(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    /*   label = w_label_create("Name:");
       w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
       w_container_append(W_CONTAINER(vbox2), label);*/

    hbox = w_hbox_create(20);
    w_container_append(W_CONTAINER(hbox), vbox2);

    widget = w_option_create();
    w_option_append_label(W_OPTION(widget), "Human vs. CPU", 0.5f, 0.0f);
    w_option_append_label(W_OPTION(widget), "CPU vs. Human", 0.5f, 0.0f);
    w_option_append_label(W_OPTION(widget), "Human vs. Human", 0.5f, 0.0f);
    w_option_set_callback(W_OPTION(widget), dialog_title_players, NULL);
    vbox2 = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox2), widget);

    widget = w_option_create();
    w_option_append_label(W_OPTION(widget), "Level 1", 0.5f, 0.0f);
    w_option_append_label(W_OPTION(widget), "Level 2", 0.5f, 0.0f);
    w_option_append_label(W_OPTION(widget), "Level 3", 0.5f, 0.0f);
    w_option_append_label(W_OPTION(widget), "Level 4", 0.5f, 0.0f);
    w_option_set_callback(W_OPTION(widget), dialog_title_level, NULL);
    w_container_append(W_CONTAINER(vbox2), widget);

    widget = w_option_create();
    for (i = 0; i < num_theme; i++)
        w_option_append_label(W_OPTION(widget), themelist[i], 0.5f, 0.0f);
    w_option_set_callback(W_OPTION(widget), dialog_title_theme, NULL);
    w_container_append(W_CONTAINER(vbox2), widget);

    widget = w_option_create();
    for (i = 0; i < pieces_list_total; i++)
        w_option_append_label(W_OPTION(widget), pieces_list[i], 0.5f, 0.0f);
    w_option_set_callback(W_OPTION(widget), dialog_title_pieces, NULL);
    w_container_append(W_CONTAINER(vbox2), widget);

    widget = w_option_create();
    for (i = 0; i < board_list_total; i++)
        w_option_append_label(W_OPTION(widget), board_list[i], 0.5f, 0.0f);
    w_option_set_callback(W_OPTION(widget), dialog_title_board, NULL);
    w_container_append(W_CONTAINER(vbox2), widget);

    /*  widget = w_entry_create();
      w_container_append(W_CONTAINER(vbox2), widget);*/

    w_container_append(W_CONTAINER(hbox), vbox2);
    w_container_append(W_CONTAINER(vbox), hbox);

    widget = w_action_create_with_label("Quit Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), menu_title_quit, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    dialog = w_dialog_create(vbox);
    w_dialog_set_modal(W_DIALOG(dialog), 1);
    w_dialog_set_position(W_DIALOG(dialog), 320, 0, ALIGN_CENTER, ALIGN_BOTTOM);
    w_dialog_set_style(W_DIALOG(dialog), &style_menu);
    return W_DIALOG(dialog);
}

static void dialog_vkeyboard_key(w_widget_t *widget, void *data)
{
    if (dialog_current())
        dialog_input(*(ui_event_t *) data);

    printf( "Pressed a keyyy... it was uh.. '%c' .. right?\n\r", *(ui_event_t *)data);
}

static w_dialog_t *dialog_vkeyboard_create()
{
    w_widget_t *dialog;
    w_widget_t *label;
    w_widget_t *action;
    w_widget_t *hbox;
    w_widget_t *vbox2;
    static ui_event_t key;
    int i,j,k;
    int max_width = 0;

    hbox=w_hbox_create(0);
    label=w_label_create("Type stuff, k?" );
    w_container_append(W_CONTAINER(hbox), label);
    vbox2=w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox2), hbox );

    for (i = 0; i < 256; i++)
    {
        int cur_width = text_characters[i].width;
        if (cur_width > max_width)
            max_width = cur_width;
    }

    k=0;
    for ( j=0; j<6; j++ )
    {
        hbox = w_hbox_create(2);
        for ( i=0; i<16; i++ )
        {
            if ( k < 127-33 )
            {
                char key_str[2];
                key=k+33;
                key_str[0] = key;
                key_str[1] = '\0';
                action = w_action_create_with_label(key_str, 0.5f, 0.5f);
                w_set_requested_size(action, max_width, 0);
                w_action_set_callback(W_ACTION(action), dialog_vkeyboard_key, &keys[k]);

                w_container_append(W_CONTAINER(hbox), action);
                k++;
            }
        }
        w_container_append(W_CONTAINER(vbox2), hbox);
    }

    dialog = w_dialog_create(vbox2);
    return W_DIALOG(dialog);
}

/* This is our SDL surface */
static SDL_Surface *surface;

static void generate_text_chars();
static void draw_backdrop();

static void init_gl();
static void resize_window( int width, int height );
static void load_theme(char* name, char *pieces, char *board);
static int GetMove();
void load_texture_png( texture_t *texture, char *filename, int alpha );
static void draw_name_dialog( float xpos, float ypos, char* name, int left, int white );

static int mouse_x_pos, mouse_y_pos;
static int can_load=FALSE;

static int white_in_check;
static int black_in_check;

static board_t board;
static volatile int event_flag;
static float board_xpos, board_ypos;
static int game_difficulty;
static int game_type;
static SDL_Joystick *joy;
static int wait_menu = 1;

/** @brief Computes smallest power of two that's larger than the input value.
 *
 *  @param input Input value.
 *  @return Smallest power of two that's larger than input.
 */
static int power_of_two(int input)
{
    int value = 1;

    while ( value < input )
    {
        value <<= 1;
    }
    return value;
}

/** @brief Renders an animation frame of the credits display.
 *
 *  This function has a hidden state to progress the animation. The animation
 *  is time-based and its speed should be the same regardless of the frame
 *  rate.
 *
 *  @param init 1 = reset animation (no rendering takes place), 0 = continue
 *              animation.
 */
static void draw_credits(int init)
{
    static int section, nr, state;
    int diff;
    static Uint32 start;
    char ***credits;
    Uint32 now;
    int x = 620;
    int y = 270;
    w_colour_t col_cap = {0.55f, 0.75f, 0.95f, 0.0f};
    w_colour_t col_item = {1.0f, 1.0f, 1.0f, 0.0f};

    now = SDL_GetTicks();
    credits = get_credits();

    if (init)
    {
        section = 0;
        nr = 1;
        state = 0;
        start = now;
        return;
    }

    switch (state)
    {
    case 0:
        diff = now - start;

        if (diff < 1000)
            col_cap.a = diff / (float) 1000;
        else
        {
            col_cap.a = 1.0f;
            start = now;
            state = 1;
        }

        text_draw_string_right(x, y, credits[section][0], 1, &col_cap, string_type_pos);

        break;

    case 1:
        col_cap.a = 1.0f;
        text_draw_string_right(x, y, credits[section][0], 1, &col_cap, string_type_pos);

        diff = now - start;

        if (diff < 1000)
            col_item.a = diff / (float) 1000;
        else if (diff < 2000)
            col_item.a = 1.0f;
        else if (diff < 3000)
            col_item.a = 1.0f - (diff - 2000) / (float) 1000;
        else
        {
            start = now;
            nr++;
            if (!credits[section][nr])
            {
                nr = 1;
                state = 2;
            }
            return;
        }

        text_draw_string_right(x, y - 40, credits[section][nr], 1, &col_item, string_type_pos);

        break;

    case 2:
        diff = now - start;

        if (diff < 1000)
            col_cap.a = 1.0f - diff / (float) 1000;
        else
            if (credits[section + 1])
            {
                section++;
                start = now;
                state = 0;
            }
            else
            {
                state = 3;
                return;
            }

        text_draw_string_right(x, y, credits[section][0], 1, &col_cap, string_type_pos);

        break;
    }
}

/** @brief Creates a texture from an SDL surface.
 *
 *  @param surface The SDL surface to transform.
 *  @param alpha 1 = Create texture with alpha channel (taken from surface),
 *               0 = Create texture without alpha channel.
 *  @return Texture created from surface.
 */
static texture_t SDL_GL_LoadTexture(SDL_Surface *surface, SDL_Rect *area, int alpha)
{
    texture_t texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect dest;
    Uint32 saved_flags;
    Uint8  saved_alpha;

    /* Use the surface width and height expanded to powers of 2 */
    w = power_of_two(area->w);
    h = power_of_two(area->h);

    image = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                w, h,
                32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
                0x000000FF,
                0x0000FF00,
                0x00FF0000,
                0xFF000000
#else
                0xFF000000,
                0x00FF0000,
                0x0000FF00,
                0x000000FF
#endif
            );
    if ( image == NULL )
    {
        exit(0);
    }

    /* Save the alpha blending attributes */
    saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
    saved_alpha = surface->format->alpha;
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA )
    {
        SDL_SetAlpha(surface, 0, 0);
    }

    /* Copy the surface into the GL texture image */
    dest.x = 0;
    dest.y = 0;
    dest.w = area->w;
    dest.h = area->h;
    SDL_BlitSurface(surface, area, image, &dest);

    /* Restore the alpha blending attributes */
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA )
    {
        SDL_SetAlpha(surface, saved_flags, saved_alpha);
    }

    /* Create an OpenGL texture for the image */
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 (alpha ? 4 : 3),
                 w, h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    texture.u1 = 0;
    texture.v1 = 0;
    texture.u2 = area->w / (float) w;
    texture.v2 = area->h / (float) h;
    texture.width = area->w;
    texture.height = area->h;

    return texture;
}


static void draw_name_dialog( float xpos, float ypos, char* name, int left, int white )
{
    float width, height;

    width=100;
    height=30;

    /* draw avatar */
    if ( white == 1 )
        draw_texture( &white_pieces[GUI_PIECE_AVATAR], xpos-45, ypos-50, 100, 100, 1.0f, &col_white);
    else
        draw_texture( &black_pieces[GUI_PIECE_AVATAR], xpos+45, ypos-50, 100, 100, 1.0f, &col_white);

    /* Draw the text stuff */
    if (!left) /* UGLY */
        text_draw_string( xpos, ypos+10, name, 1, &col_black, 999 );
    else
        text_draw_string( xpos+width-(strlen(name)*8), ypos+10, name, 1, &col_black,
                          999 );
}

void dialog_promote_cb(w_widget_t *widget, void *data)
{
    dialog_promote_piece = *(int *)data;
    dialog_close();
}

w_dialog_t *dialog_promote_create(int colour)
{
    static int cb_pieces[4];

    texture_t *pieces;
    w_widget_t *dialog;
    w_widget_t *action;
    w_widget_t *vbox = w_vbox_create(0);
    w_widget_t *hbox = w_hbox_create(0);
    w_widget_t *w_image;
    w_widget_t *text = w_label_create("Promotion! Choose new piece!");

    dialog_promote_piece = NONE;
    cb_pieces[0] = QUEEN + colour;
    cb_pieces[1] = ROOK + colour;
    cb_pieces[2] = BISHOP + colour;
    cb_pieces[3] = KNIGHT + colour;

    w_container_append(W_CONTAINER(vbox), text);

    if (IS_WHITE(colour))
        pieces = white_pieces;
    else
        pieces = black_pieces;

    w_image = w_image_create(&pieces[GUI_PIECE_QUEEN]);
    action = w_action_create(w_image);
    w_action_set_callback(W_ACTION(action), dialog_promote_cb, &cb_pieces[0]);
    w_container_append(W_CONTAINER(hbox), action);

    w_image = w_image_create(&pieces[GUI_PIECE_ROOK]);
    action = w_action_create(w_image);
    w_action_set_callback(W_ACTION(action), dialog_promote_cb, &cb_pieces[1]);
    w_container_append(W_CONTAINER(hbox), action);

    w_image = w_image_create(&pieces[GUI_PIECE_BISHOP]);
    action = w_action_create(w_image);
    w_action_set_callback(W_ACTION(action), dialog_promote_cb, &cb_pieces[2]);
    w_container_append(W_CONTAINER(hbox), action);

    w_image = w_image_create(&pieces[GUI_PIECE_KNIGHT]);
    action = w_action_create(w_image);
    w_action_set_callback(W_ACTION(action), dialog_promote_cb, &cb_pieces[3]);
    w_container_append(W_CONTAINER(hbox), action);
    w_container_append(W_CONTAINER(vbox), hbox);

    dialog = w_dialog_create(vbox);
    w_dialog_set_modal(W_DIALOG(dialog), 1);
    return W_DIALOG(dialog);
}

w_dialog_t *dialog_message_create(char *message)
{
    w_widget_t *dialog;
    w_widget_t *widget;

    w_widget_t *vbox = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox), w_label_create("Important message from engine"));
    w_container_append(W_CONTAINER(vbox), w_label_create(""));
    w_container_append(W_CONTAINER(vbox), w_label_create(message));
    w_container_append(W_CONTAINER(vbox), w_label_create(""));
    widget = w_action_create_with_label("Ok", 0.5f, 0.5f);
    w_action_set_callback(W_ACTION(widget), dialog_close_cb, NULL);
    w_container_append(W_CONTAINER(vbox), widget);
    dialog = w_dialog_create(vbox);
    w_dialog_set_modal(W_DIALOG(dialog), 1);

    return W_DIALOG(dialog);
}

/** @brief Swaps the OpenGL buffer.
 *
 *  Also maintains the frames-per-second counter.
 */
static void gl_swap()
{
    static Uint32 last = 0;
    Uint32 now;

    if (fps_enabled)
    {
        char fps_s[16];

        snprintf(fps_s, 16, "FPS: %.2f", fps);
        text_draw_string(10, 10, fps_s, 1, &col_red, 999 );
    }

    SDL_GL_SwapBuffers();
    now = SDL_GetTicks();
    if (now - last < 1000 / FPS)
        SDL_Delay(1000 / FPS - (now - last));
    last = SDL_GetTicks();

    frames++;
    if (frames == 10)
    {
        fps = 10000 / (float) (last - fps_time);
        frames = 0;
        fps_time = last;
    }

#ifdef _arch_dreamcast
    dc_z = 1.0f;
#endif
}

float starscroll_pos=320;

/** Implements ui_driver::menu */
static config_t *do_menu()
{
    GLuint ticks;
    float fadehuh;
    w_dialog_t *keyboard = dialog_vkeyboard_create();
    SDL_Event event;
    game_difficulty=1;
    game_type=GAME_TYPE_HUMAN_VS_CPU;
    title_process_retval=2;
    w_colour_t stars = { 1.0f, 1.0f, 1.0f, 1.0f };

    board_xpos=128;
    board_ypos=30;
    can_load=FALSE;
    set_loading=FALSE;

    white_in_check=FALSE;
    black_in_check=FALSE;

    draw_credits(1);
    dialog_open(dialog_title_create());

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while ( 1 )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Precess input */
        while ( SDL_PollEvent( &event ) )
        {
            ui_event_t ui_event;

            if (event.type == SDL_MOUSEMOTION)
            {
                w_dialog_t *dialog = dialog_current();
                if (dialog)
                    w_dialog_mouse_movement(dialog, event.motion.x, 479 - event.motion.y);

                continue;
            }

            ui_event = convert_event(&event);

            if (wait_menu)
            {
                if (ui_event != UI_EVENT_NONE)
                {
                    reset_string_type_length();
                    wait_menu = 0;
                }
                continue;
            }

            if (ui_event == 0x06)
            {
                fps_enabled = 1 - fps_enabled;
                continue;
            }

            if (ui_event == 0x0b)
            {
                vkeyboard_enabled = 1 - vkeyboard_enabled;
                continue;
            }

            if (vkeyboard_enabled)
                keyboard->input(W_WIDGET(keyboard), ui_event);
            else
                dialog_input(ui_event);

            if (title_process_retval == 1)
            {
                free(config);
                return NULL;
            }
        }

        /* Draw the menu.. */
        draw_texture( &menu_title_tex, 0, 0, 640, 480, 1.0f, &col_white );
        update_string_type_length();

        if ( can_load == TRUE )
        {
            load_theme(themelist[cur_theme], pieces_list[pieces_list_cur],
                       board_list[board_list_cur]);
            reset_3d();
            return config;
        }

        if ( set_loading == FALSE )
        {
            if (wait_menu)
                text_draw_string_bouncy( 140, 30, "Press any key or button to start",
                                         1.5, &col_white, string_type_pos );
            else
            {
                w_dialog_t *dialog = dialog_current();
                w_dialog_render(dialog);
            }

            if (vkeyboard_enabled)
                w_dialog_render(keyboard);

            draw_credits(0);
        }
        else
        {
            text_draw_string( 390, 30, "Loading...", 3, &col_white, string_type_pos );
            can_load = TRUE;
        }

        gl_swap();
    }
}

/** @brief Loads a PNG file and turns it into a texture.
 *
 *  @param texture Texture to write to.
 *  @param filename The PNG file to load.
 *  @param alpha 1 = Create texture with alpha channel (taken from image),
 *               0 = Create texture without alpha channel.
 */
void load_texture_png( texture_t *texture, char *filename, int alpha )
{
    /* Create storage space for the texture */
    SDL_Surface *texture_image;

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( ( texture_image = IMG_Load( filename ) ) )
    {
        SDL_Rect area;
        area.x = 0;
        area.y = 0;
        area.w = texture_image->w;
        area.h = texture_image->h;
        *texture = SDL_GL_LoadTexture(texture_image, &area, alpha);
    }
    else
    {
        fprintf(stderr, "Could not load texture: %s!\n", filename);
        exit(1);
    }

    /* Free up any memory we may have used */
    if ( texture_image )
        SDL_FreeSurface( texture_image );
}

void draw_image(void *image, w_rect_t source, w_rect_t dest, int mode_h, int mode_v)
{
    texture_t *texture = image;
    float hsize = texture->u2 - texture->u1;
    float vsize = texture->v2 - texture->v1;
    float tex_h = texture->width / hsize;
    float tex_v = texture->height / vsize;
    float xsrc = texture->u1 + source.x / tex_h;
    float ysrc = texture->v1 + source.y / tex_v;
    float width, height;
    GLenum en_h, en_v;

    if (mode_h == GG_MODE_TILE)
    {
        en_h = GL_REPEAT;
        width = dest.width / tex_h;
    }
    else
    {
        en_h = GL_CLAMP;
        width = source.width / tex_h;
    }

    if (mode_v == GG_MODE_TILE)
    {
        en_v = GL_REPEAT;
        height = dest.height / tex_v;
    }
    else
    {
        en_v = GL_CLAMP;
        height = source.height / tex_v;
    }

    draw_texture_uv(texture, dest.x,
                    dest.y, dest.width, dest.height, 1.0f,
                    &col_white, xsrc,
                    ysrc,
                    xsrc + width, ysrc + height, en_h, en_v);
}

void draw_char(int c, int x, int y, w_colour_t *colour)
{
    text_draw_char(x, y, 1.0f, c, colour);
}

void get_image_size(void *image, int *width, int *height)
{
    texture_t *texture = image;

    if (width)
        *width = texture->width;

    if (height)
        *height = texture->height;
}

void get_char_size(int c, int *width, int *height)
{
    if (width)
        *width = text_characters[c].width;

    if (height)
        *height = text_characters[c].height;
}

w_driver_t w_driver_sdlgl =
    {
        draw_rect,
        draw_rect_fill,
        draw_image,
        draw_char,
        get_image_size,
        get_char_size
    };

static void load_border(texture_t border[9], char *filename)
{
    /* Create storage space for the texture */
    SDL_Surface *surface;

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ((surface = IMG_Load(filename)))
    {
        int i;
        for (i = 0; i < 9; i++)
        {
            SDL_Rect rect;
            rect.x = (i % 3) * surface->w / 3;
            rect.y = (i / 3) * surface->h / 3;
            rect.w = surface->w / 3;
            rect.h = surface->h / 3;
            border[i] = SDL_GL_LoadTexture(surface, &rect, 1);
        }
        /* Free up any memory we may have used */
        SDL_FreeSurface(surface);
    }
    else
    {
        fprintf(stderr, "Could not load texture: %s!\n", filename);
        exit(1);
    }
}

static void unload_border(texture_t border[9])
{
    int i;

    for (i = 0; i < 9; i++)
    {
        glDeleteTextures(1, &border[i].id);
    }
}

/** Implements ui_driver::init. */
static void init_gui()
{
    int video_flags;
    const SDL_VideoInfo *video_info;
    int i;
    DIR* themedir;
    struct dirent* themedir_entry;

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    SDL_EnableUNICODE(1);

    video_info = SDL_GetVideoInfo( );

    if ( !video_info )
    {
        fprintf( stderr, "Video query failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    video_flags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    video_flags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    video_flags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    /* video_flags |= SDL_RESIZABLE; */      /* Enable window resizing */
    /* video_flags |= SDL_FULLSCREEN; */

    if ( video_info->hw_available )
        video_flags |= SDL_HWSURFACE;
    else
        video_flags |= SDL_SWSURFACE;

    if ( video_info->blit_hw )
        video_flags |= SDL_HWACCEL;

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
                                video_flags );
    if ( !surface )
    {
        fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
        exit(1);
    }

    if( SDL_NumJoysticks()>0 )
        joy=SDL_JoystickOpen(0);

    SDL_WM_SetCaption( "DreamChess", NULL );

    init_gl();

    w_system_init(&w_driver_sdlgl);

    ch_datadir();

    /* New text stuff. */
    generate_text_chars();

    /* For the menu.. */
    load_texture_png( &menu_title_tex, "menu_title.png" , 0);

    /* Fill theme list. */
    if ( (themedir=opendir("themes")) != NULL )
    {
        num_theme = 0;
        themedir_entry=readdir(themedir);
        while ( themedir_entry != NULL )
        {
            if ( themedir_entry->d_name[0] != '.' )
                themelist[num_theme++]=strdup( themedir_entry->d_name );
            themedir_entry=readdir(themedir);
        }
    }

    chdir("themes");
    chdir("default");
    load_border(menu_border, "border.png");

    /* Fill pieces list. */
    ch_datadir();

    if ((themedir=opendir("pieces")) != NULL )
    {
        pieces_list_total = 0;
        while ((themedir_entry = readdir(themedir)) != NULL)
        {
            if (themedir_entry->d_name[0] != '.')
            {
                pieces_list = realloc(pieces_list, (pieces_list_total + 1) *
                                      sizeof(char *));
                pieces_list[pieces_list_total++] =
                    strdup(themedir_entry->d_name);
            }
        }
    }

    style_ingame.textured = 1;
    style_ingame.fade_col = (w_colour_t)
                            {
                                0.0f, 0.0f, 0.0f, 0.5f
                            };
    style_ingame.hor_pad = 20;
    style_ingame.vert_pad = 10;

    for (i = 0; i < 9; i++)
        style_ingame.border.textured.image[i] = &border[i];

    style_menu.textured = 1;
    style_menu.fade_col = (w_colour_t)
                          {
                              0.0f, 0.0f, 0.0f, 0.0f
                          };
    style_menu.hor_pad = 20;
    style_menu.vert_pad = 10;

    for (i = 0; i < 9; i++)
        style_menu.border.textured.image[i] = &menu_border[i];

    /* Fill board list. */
    ch_datadir();

    /* Make key table? */
    PopulateKeyTable();

    if ((themedir=opendir("boards")) != NULL )
    {
        board_list_total = 0;
        while ((themedir_entry = readdir(themedir)) != NULL)
        {
            if (themedir_entry->d_name[0] != '.')
            {
                board_list = realloc(board_list, (board_list_total + 1) *
                                     sizeof(char *));
                board_list[board_list_total++] =
                    strdup(themedir_entry->d_name);
            }
        }
    }
    fps_time = SDL_GetTicks();
}

/** @brief Loads the textures for the chess pieces. */
void load_pieces()
{
    int i, j;
    texture_t texture;
    int ypos = 0;
    int tex_height, tex_width;

    load_texture_png(&texture, "pieces.png", 1);

    tex_height = power_of_two(texture.height);
    tex_width = power_of_two(texture.width);

    for (i = 0; i < 2; i++ )
    {
        int xpos = 0;
        texture_t *pieces;
        if (i == 0)
            pieces = white_pieces;
        else
            pieces = black_pieces;

        for (j = 0; j < 7; j++ )
        {
            texture_t c;
            c.width = texture.width / 7;
            c.height = texture.height / 2;
            c.u1 = xpos / (float) tex_width;
            c.v1 = ypos / (float) tex_height;
            xpos += c.width;
            c.u2 = xpos / (float) tex_width;
            c.v2 = (ypos + c.height) / (float) tex_height;
            c.id = texture.id;
            c.width = 64;
            c.height = 64;
            switch (j)
            {
            case 0:
                pieces[GUI_PIECE_KING] = c;
                break;
            case 1:
                pieces[GUI_PIECE_QUEEN] = c;
                break;
            case 2:
                pieces[GUI_PIECE_ROOK] = c;
                break;
            case 3:
                pieces[GUI_PIECE_KNIGHT] = c;
                break;
            case 4:
                pieces[GUI_PIECE_BISHOP] = c;
                break;
            case 5:
                pieces[GUI_PIECE_PAWN] = c;
                break;
            case 6:
                pieces[GUI_PIECE_AVATAR] = c;
            }
        }
        ypos += texture.height / 2;
    }
}

/** @brief Loads a theme.
 *
 *  @param name The name of the subdirectory of the theme to load.
 */
static void load_theme(char* name, char* pieces, char *board)
{
    printf( "Loading theme.\n" );

    ch_datadir();
    chdir("themes");
    chdir(name);

    /* Theme! */
    load_texture_png( &backdrop, "backdrop.png", 0 );
    load_border(border, "border.png");
    load_pieces();

    ch_datadir();
    chdir("pieces");
    chdir(pieces);
    loadmodels("set.cfg");

    ch_datadir();
    chdir("boards");
    chdir(board);
    load_board("board.dcm", "board.png");

    ch_datadir();
    printf( "Loaded theme.\n" );
}

/** @brief Sets the OpenGL rendering options. */
static void init_gl()
{
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

/** @brief Resizes the OpenGL window.
 *
 *  @param width Desired width in pixels.
 *  @param height Desired height in pixels.
 */
static void resize_window( int width, int height )
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    /*gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);*/
    glOrtho(0, 640, 0, 480, -1, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

/** @brief Renders the in-game backdrop. */
static void draw_backdrop()
{
    draw_texture( &backdrop, 0, 0, 640, 480, 1.0f, &col_white );
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
static void draw_move_list( w_colour_t *col_normal, w_colour_t *col_high )
{
    char **list;
    int entries, view, i;
    int y;
    int start;
    float x_white = 30;
    float y_white = 350;
    float x_black = 610;
    float y_black = 350;
    w_colour_t col_normal2=*col_normal;
    w_colour_t col_high2=*col_normal;

    game_get_move_list(&list, &entries, &view);

    if (!(view % 2))
        start = (view - 8 < 0 ? 0 : view - 8);
    else
        start = (view - 9 < 0 ? 0 : view - 9);

    y = y_white;
    for (i = view -1; i >= start; i -= 2)
    {
        char s[11];
        if (snprintf(s, 11, "%i.%s", (i >> 1) + 1, list[i]) >= 11)
            exit(1);
        if (i != view)
            text_draw_string( x_white+5, y-5, s, 1, &col_normal2, 999 );
        else
            text_draw_string( x_white+5, y-5, s, 1, &col_high2, 999 );
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
    col_normal2=*col_normal;
    col_high2=*col_normal;
    y = y_black;
    for (i = view; i >= start; i -= 2)
    {
        if (i != view)
            text_draw_string_right( x_black-5, y-5, list[i], 1, &col_normal2, 999 );
        else
            text_draw_string_right( x_black-5, y-5, list[i], 1, &col_high2, 999 );
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
}

/* Draw .. health bars? */
static void draw_health_bars()
{
    float white_health_percent;
    float black_health_percent;
    int black_health;
    int white_health;

    /* This function really stinks, and will be fixed ;) .... eventually */
    /* Full health = 39 */
    /* pawn  1, knight 3, bishop 3, rook 5, queen 9 */

    /* Get da new healths? */
    white_health = 39 -((board.captured[WHITE_PAWN])+
                        (board.captured[WHITE_ROOK]*5)+(board.captured[WHITE_BISHOP]*3)+
                        (board.captured[WHITE_KNIGHT]*3)+(board.captured[WHITE_QUEEN]*9));
    black_health = 39 -((board.captured[BLACK_PAWN])+
                        (board.captured[BLACK_ROOK]*5)+(board.captured[BLACK_BISHOP]*3)+
                        (board.captured[BLACK_KNIGHT]*3)+(board.captured[BLACK_QUEEN]*9));

    white_health_percent=(float)white_health/39;
    black_health_percent=(float)black_health/39;

    /* Draw da bar? */
    draw_rect_fill( 20, 375, 75, 10, &col_yellow );
    draw_rect_fill( 640-20-75, 375, 75, 10, &col_yellow );

    draw_rect_fill( 20, 375, 75*white_health_percent, 10, &col_red );
    draw_rect_fill( 640-20-(75*black_health_percent), 375, 75*black_health_percent,
                    10, &col_red );

    draw_rect( 20, 375, 75, 10, &col_black );
    draw_rect( 640-75-20, 375, 75, 10, &col_black );
}

/** @brief Renders the list of captured pieces for both sides.
 *
 *  @param col The text colour to use.
 */
static void draw_capture_list(w_colour_t *col)
{
    float x_white = 70;
    float y_white = 180;
    float x_black = 570;
    float y_black = 180;
    int i;

    for (i = 9; i > 0; i -= 2)
    {
        char s[4];
        if (board.captured[i] != 0)
        {
            if (snprintf(s, 4, "%i", board.captured[i]) >= 4)
                exit(1);
            text_draw_string( x_white, y_white, s, 1, col, 999);
            draw_texture( &black_pieces[i/2], x_white-24, y_white, 24,
                          24, 1.0f, &col_white );
        }
        y_white -= text_characters['a'].height;
        if (board.captured[i - 1] != 0)
        {
            if (snprintf(s, 4, "%i", board.captured[i - 1]) >= 4)
                exit(1);
            text_draw_string_right( x_black, y_black, s, 1, col, 999);
            draw_texture( &white_pieces[(i-1)/2], x_black, y_black, 24,
                          24, 1.0f, &col_white );
        }
        y_black -= text_characters['a'].height;
    }
}

/** Implements ui_driver::update. */
static void update(board_t *b, move_t *move)
{
    while ( piece_moving_done == 0 )
        poll_move();

    board = *b;

    if ( move != NULL )
        start_piece_move( move->source, move->destination );

    if ( board.state == BOARD_CHECK )
    {
        if (IS_WHITE(board.turn))
            white_in_check=TRUE;
        else
            black_in_check=TRUE;
    }
    else
    {
        black_in_check=FALSE;
        white_in_check=FALSE;
    }

}

/** Implements ui_driver::show_result. */
static void show_result(result_t *res)
{
    dialog_open(dialog_victory_create(res));
}

/** @brief Main in-game rendering routine.
 *
 *  @param b Board configuration to render.
 */
static void draw_scene( board_t *b )
{
    float square_size = 48;
    char temp[80];
    int clock_seconds=0;
    int clock_minutes=0;

    dialog_cleanup();
    update_string_type_length();
    //printf( "Piece is moving from %i to %i..\n\r", piece_moving_source, piece_moving_dest );

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);

    draw_backdrop();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    go_3d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glDepthMask(GL_TRUE);

    render_scene_3d(b);

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);

    /* draw_captured_pieces( 480, 70 ); */
    glPushMatrix();

    draw_border(style_ingame.border.textured.image, (w_rect_t)
                {
                    20, 375, 75, 10
                }
                , 8
               );
    draw_border(style_ingame.border.textured.image, (w_rect_t)
                {
                    20, 440, 170, 20
                }
                , 8
               );

    draw_border(style_ingame.border.textured.image, (w_rect_t)
                {
                    545, 375, 75, 10
                }
                , 8
               );
    draw_border(style_ingame.border.textured.image, (w_rect_t)
                {
                    455, 440, 170, 20
                }
                , 8
               );

    /* Da clocken */

    draw_border(style_ingame.border.textured.image, (w_rect_t)
                {
                    290, 440, 60, 20
                }
                , 8
               );

    glPopMatrix();

    glPushMatrix();
    draw_health_bars();
    draw_name_dialog( 50, 430, "White", TRUE, 1 );
    draw_name_dialog( 490, 430, "Black", FALSE, 0 );
    draw_move_list(&col_white, &col_yellow);
    draw_capture_list(&col_white);

    clock_minutes=(((SDL_GetTicks()-turn_counter_start)/1000)/60);
    clock_seconds=((SDL_GetTicks()-turn_counter_start)/1000)-(clock_minutes*60);
    sprintf( temp, "%i:%02i", clock_minutes, clock_seconds );
    text_draw_string( 303, 440, temp, 1, &col_black, 999 );
    glPopMatrix();

    if ( white_in_check == TRUE )
        text_draw_string_bouncy( 180, 420, "White is in check!", 2, &col_white,
                                 string_type_pos );
    else if ( black_in_check == TRUE )
        text_draw_string_bouncy( 180, 420, "Black is in check!", 2, &col_white,
                                 string_type_pos );

    if (dialog_current())
    {
        w_dialog_t *dialog = dialog_current();
        w_dialog_render(dialog);
    }

    /* Draw it to the screen */
    gl_swap();
}

/** Implements ui_driver::init. */
static int sdlgl_init()
{
    init_gui();
    return 0;
}

/** Implements ui_driver::exit. */
static int sdlgl_exit()
{
    glDeleteTextures(1, &menu_title_tex.id);
    SDL_Quit();
    return 0;
}

/** @brief Frees all textures of the currently loaded theme. */
static void unload_theme()
{
    glDeleteTextures(1, &white_pieces[GUI_PIECE_KING].id);
    glDeleteTextures(1, &backdrop.id);
    unload_border(border);
    freemodels();
}

/** @brief Renders a latin1 character.
 *
 *  @param xpos Leftmost x-coordinate to render the character at.
 *  @param ypos Lowermost y-coordinate to render the character at.
 *  @param scale Size scale factor.
 *  @param character To character to render.
 *  @param col The colour to render with.
 *  @return The width of the textured quad in pixels.
 */
int text_draw_char( float xpos, float ypos, float scale, int character, w_colour_t *col )
{
    int index, offset;

    offset=0;
    index=character;
    draw_texture( &text_characters[index], xpos, ypos, text_characters[index].width*scale,
                  text_characters[index].height*scale, 1.0f, col );

    return text_characters[index].width*scale;
}

/** @brief Renders a latin1 string.
 *
 *  @param xpos Leftmost x-coordinate to render the string at.
 *  @param ypos Lowermost y-coordinate to render the string at.
 *  @param text The string to render.
 *  @param scale Size scale factor.
 *  @param col The colour to render with.
 */
void text_draw_string( float xpos, float ypos, unsigned char *text, float scale, w_colour_t *col, int length )
{
    int i;
    int xposition=xpos;

    if ( length > strlen(text) )
        length=strlen(text);

    for ( i=0; i<length; i++ )
    {
        xposition+=text_draw_char( xposition, ypos, scale, text[i], col );
    }
}

static int text_width_n(unsigned char *text, int n)
{
    int retval, i;

    retval = 0;
    for (i = 0; i < n; i++)
        retval += text_characters[(int) text[i]].width;
    return retval;
}

/** @brief Returns the width of a string.
 *
 *  @param text String to compute width of.
 *  @return Width of string in pixels.
 */
static int text_width(unsigned char *text)
{
    return text_width_n(text, strlen(text));
}

/** @brief Returns the font height.
 *
 *  @return Font height in pixels.
 */
static int text_height()
{
    return text_characters['a'].height;
}

static int text_max_width()
{
    static int width = -1;
    int i;

    if (width >= 0)
        return width;

    for (i = 0; i < 256; i++)
    {
        int cur_width = text_characters[i].width;
        if (cur_width > width)
            width = cur_width;
    }

    return width;
}

/** @brief Renders a latin1 string with right-alignment.
 *
 *  @param xpos Rightmost x-coordinate to render the string at.
 *  @param ypos Lowermost y-coordinate to render the string at.
 *  @param text The string to render.
 *  @param scale Size scale factor.
 *  @param col The colour to render with.
 */
void text_draw_string_right( float xpos, float ypos, unsigned char *text, float scale, w_colour_t *col, int length )
{
    text_draw_string(xpos - text_width(text), ypos, text, scale, col, length);
}

/** @brief Renders a bouncy latin1 string.
 *
 *  The bouncing effect is based on BOUNCE_AMP, BOUNCE_LEN and BOUNCE_SPEED.
 *  The current time is used to determine the current animation frame.
 *
 *  @param xpos Leftmost x-coordinate to render the string at.
 *  @param ypos Lowermost y-coordinate to render the string at.
 *  @param text The string to render.
 *  @param scale Size scale factor.
 *  @param col The colour to render with.
 */
void text_draw_string_bouncy( float xpos, float ypos, unsigned char *text, float scale, w_colour_t *col, int length )
{
    int i;
    int xposition=xpos;
    int yposition=ypos;
    Uint32 ticks = SDL_GetTicks();

    if ( length > strlen(text) )
        length=strlen(text);

    for ( i=0; i<length; i++ )
    {
        float temp_off;
        float phase = ((ticks % (1000 / BOUNCE_SPEED)) / (float) (1000 / BOUNCE_SPEED));

        if (phase < 0.5)
            temp_off = phase * 2 * (BOUNCE_AMP + 1);
        else
            temp_off = ((1.0 - phase) * 2) * (BOUNCE_AMP + 1);

        yposition=ypos+temp_off;
        xposition+=text_draw_char( xposition, yposition, scale, text[i], col );

        ticks += 1000 / BOUNCE_SPEED / BOUNCE_LEN;
    }
}

/** @brief Generates textures for the latin1 character set. */
void generate_text_chars()
{
    int i, j;
    texture_t texture;
    char width[256];
    FILE *f;
    int ypos = 0;
    int tex_height, tex_width;

    load_texture_png(&texture, "font.png", 1);

    tex_height = power_of_two(texture.height);
    tex_width = power_of_two(texture.width);

    f = fopen("font.wid", "rb");

    if (!f)
    {
        fprintf(stderr, "Couldn't open font width file\n");
        exit(1);
    }

    if (fread(width, 1, 256, f) < 256)
    {
        fprintf(stderr, "Error reading font width file\n");
        exit(1);
    }

    for (i = 0; i < 16; i++ )
    {
        int xpos = 0;
        for (j = 0; j < 16; j++ )
        {
            texture_t c;
            c.width = width[i*16+j];
            c.height = texture.height / 16;
            c.u1 = xpos / (float) tex_width;
            c.v1 = ypos / (float) tex_height;
            xpos += width[i*16+j];
            c.u2 = xpos / (float) tex_width;
            c.v2 = (ypos + c.height) / (float) tex_height;
            c.id = texture.id;
            text_characters[i*16+j] = c;
        }
        ypos += texture.height / 16;
    }
}

/** Implements ui_driver::show_message. */
static void show_message (char *msg)
{
    dialog_open(dialog_message_create(msg));
}

/** Implements ui_driver::poll. */
static void poll_move()
{
    static int source = -1, dest = -1, needprom = 0;
    /* board_t *board = history->play->board; */
    move_t *move;
    int input;

    draw_scene(&board);

    if (quit_to_menu)
    {
        quit_to_menu = 0;
        needprom = 0;
        source = -1;
        dest = -1;
        unload_theme();
        game_quit();
        return;
    }

    input = GetMove();

    if (!game_want_move())
    {
        source = -1;
        dest = -1;
        needprom = 0;
    }
    /* else
        flip_board =  board.turn; */

    if (source == -1)
    {
        source = input;
        if ((source >= 0) && flip_board)
            source = 63 - source;
        /* Only allow piece of current player to be moved. */
        if ((source >= 0) && ((PIECE(board.square[source]) == NONE) || (COLOUR(board.square[source]) != board.turn)))
            source = -1;
        return;
    }

    if (dest == -1)
    {
        dest = input;
        if (dest >= 0)
        {
            if (flip_board)
                dest = 63 - dest;
            /* Destination square must not contain piece of current player. */
            if ((PIECE(board.square[dest]) != NONE) && (COLOUR(board.square[dest]) == board.turn))
            {
                dest = -1;
                /* We use currently selected square as source. */
                source = input;
            }
            else
                select_piece(-1);
        }
        return;
    }

    if (needprom == 1)
    {
        if (dialog_promote_piece != NONE)
            needprom = 2;
        return;
    }

    if ((needprom  == 0) && (((board.square[source] == WHITE_PAWN) && (dest >= 56)) ||
                             ((board.square[source] == BLACK_PAWN) && (dest <= 7))))
    {
        dialog_open(dialog_promote_create(COLOUR(board.square[source])));
        needprom = 1;
        return;
    }

    move = (move_t *) malloc(sizeof(move_t));
    move->source = source;
    move->destination = dest;
    if (needprom == 2)
        move->promotion_piece = dialog_promote_piece;
    else
        move->promotion_piece = NONE;
    needprom = 0;

    //start_piece_move( source, dest );

    source = -1;
    dest = -1;
    game_make_move(move);
    reset_turn_counter();
    return;
}

#define MOVE_SPEED (60 / fps)

/** @brief Main input routine.
 *
 *  Handles keyboard commands. When the user selects a chess piece
 *  selected_piece is updated.
 *
 *  @return If the user selected a chess piece a value between 0 (A1) and 63
 *          (H8) is returned. -1 if no chess piece was selected.
 */
static int GetMove()
{
    int retval = -1;
    static Sint16 rotx, roty;
    SDL_Event event;
    Uint8 *keystate = SDL_GetKeyState(NULL);

    if (keystate[SDLK_LCTRL])
    {
        if (keystate[SDLK_DOWN])
            move_camera(-0.6f * MOVE_SPEED, 0.0f);
        if (keystate[SDLK_LEFT])
            move_camera(0.0f, -0.6f * MOVE_SPEED);
        if (keystate[SDLK_RIGHT])
            move_camera(0.0f, 0.6f * MOVE_SPEED);
        if (keystate[SDLK_UP])
            move_camera(0.6f * MOVE_SPEED, 0.0f);

        while (SDL_PollEvent( &event ))
            ;
    }

    if ((roty < -3000) || (roty > 3000))
       move_camera(-roty / (float) 32768 * 0.6f * MOVE_SPEED, 0.0f);

    if ((rotx < -3000) || (rotx > 3000))
        move_camera(0.0f, -rotx / (float) 32768 * 0.6f * MOVE_SPEED);

    while ( SDL_PollEvent( &event ) )
    {
        ui_event_t ui_event;

        if (event.type == SDL_MOUSEMOTION)
        {
            w_dialog_t *dialog = dialog_current();
            if (dialog)
                w_dialog_mouse_movement(dialog, event.motion.x, 479 - event.motion.y);

            continue;
        }

        if ((event.type == SDL_JOYAXISMOTION) && (event.jaxis.axis == AXIS_VIEW_X))
        {
            rotx = event.jaxis.value;
            continue;
        }

        if ((event.type == SDL_JOYAXISMOTION) && (event.jaxis.axis == AXIS_VIEW_Y))
        {
            roty = event.jaxis.value;
            continue;
        }

        ui_event = convert_event(&event);

        if (ui_event == UI_EVENT_NONE)
            continue;

        if (dialog_current())
            dialog_input(ui_event);
        /* In the promote dialog */
        else
            switch (ui_event)
            {
            case UI_EVENT_LEFT:
                move_selector(SELECTOR_LEFT);
                break;
            case UI_EVENT_RIGHT:
                move_selector(SELECTOR_RIGHT);
                break;
            case UI_EVENT_UP:
                move_selector(SELECTOR_UP);
                break;
            case UI_EVENT_DOWN:
                move_selector(SELECTOR_DOWN);
                break;
            case UI_EVENT_ACTION:
                retval = get_selector();
                select_piece(retval);
                break;
            case UI_EVENT_ESCAPE:
                dialog_open(dialog_system_create());
                break;
            case 'g':
            case UI_EVENT_EXTRA3:
                dialog_open(dialog_ingame_create());
                break;
            case 'p':
                game_view_prev();
                break;
            case 'n':
                game_view_next();
                break;
            case 'u':
                game_undo();
                break;
            case 0x06:
                fps_enabled = 1 - fps_enabled;
                break;
            default:
                break;
            }
        break;
#if 0

    case SDL_MOUSEMOTION:
        mouse_x_pos=((event.motion.x-10))/(460/8);
        mouse_y_pos=(470-(event.motion.y))/(460/8);
        break;
    case SDL_MOUSEBUTTONDOWN:
        if ( event.button.button == SDL_BUTTON_LEFT )
        {
            retval = (mouse_y_pos*8)+mouse_x_pos;
            select_piece(retval);
        }
        break;
#endif

    }
    return retval;
}

/** SDL + OpenGL driver. */
ui_driver_t ui_sdlgl =
    {
        "sdlgl",
        sdlgl_init,
        sdlgl_exit,
        do_menu,
        update,
        poll_move,
        show_message,
        show_result
    };

#endif /* WITH_UI_SDLGL */
