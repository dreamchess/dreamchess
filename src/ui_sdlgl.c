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
#include <time.h>

#include "mxml.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_thread.h"
#include "SDL_opengl.h"
#include "SDL_joystick.h"

#include "dreamchess.h"
#include "history.h"
#include "ui.h"
#include "dir.h"
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

static struct
{
    int x;
    int y;
}
mouse_pos;

static int mouse_square;
static void load_theme(char* style, char* pieces, char *board);
static void load_theme_xml( char *xmlfile );
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

int text_draw_char( float xpos, float ypos, float scale, int character, gg_colour_t *col );

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

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_red =
    {
        1.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

static gg_colour_t col_yellow =
    {
        1.0f, 1.0f, 0.0f, 1.0f
    };

void draw_rect(int x, int y, int w, int h, gg_colour_t *col)
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
void draw_rect_fill(int x, int y, int w, int h, gg_colour_t *col)
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
void draw_rect_fill(int x, int y, int w, int h, gg_colour_t *col)
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

int string_type_pos=-1;

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

static gg_event_t convert_event(SDL_Event *event)
{
    static unsigned int pressed;
    gg_event_t gg_event;

    gg_event.type=GG_EVENT_NONE;	

    switch (event->type)
    {
    case SDL_KEYDOWN:
        gg_event.type = GG_EVENT_KEY;
        switch (event->key.keysym.sym)
        {
        case SDLK_RIGHT:
            gg_event.key = GG_KEY_RIGHT;
            break;
        case SDLK_LEFT:
            gg_event.key = GG_KEY_LEFT;
            break;
        case SDLK_UP:
            gg_event.key = GG_KEY_UP;
            break;
        case SDLK_DOWN:
            gg_event.key = GG_KEY_DOWN;
            break;
        case SDLK_HOME:
            gg_event.key = GG_KEY_HOME;
            break;
        case SDLK_END:
            gg_event.key = GG_KEY_END;
            break;
        case SDLK_RETURN:
            gg_event.key = GG_KEY_ACTION;
            break;
        default:
            if (event->key.keysym.unicode <= 0xff)
	    {		
                gg_event.key = event->key.keysym.unicode;
	    }
            else
            {
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
        }
        break;

    case SDL_JOYHATMOTION:
        gg_event.type = GG_EVENT_KEY;
        switch (event->jhat.value)
        {
        case SDL_HAT_RIGHT:
            gg_event.key = GG_KEY_RIGHT;
            break;
        case SDL_HAT_LEFT:
            gg_event.key = GG_KEY_LEFT;
            break;
        case SDL_HAT_UP:
            gg_event.key = GG_KEY_UP;
            break;
        case SDL_HAT_DOWN:
            gg_event.key = GG_KEY_DOWN;
            break;
        default:
            gg_event.type = GG_EVENT_NONE;
            return gg_event;
        }
        break;

    case SDL_JOYBUTTONDOWN:
        gg_event.type = GG_EVENT_KEY;
        switch (event->jbutton.button)
        {
        case 0:
            gg_event.key = GG_KEY_ACTION;
            break;
        case 1:
            gg_event.key = GG_KEY_ESCAPE;
            break;
        case 2:
            gg_event.key = GG_KEY_EXTRA3;
            break;
        case 3:
            gg_event.key = GG_KEY_EXTRA2;
            break;
        case 4:
            gg_event.key = GG_KEY_ESCAPE;
            break;
        default:
            gg_event.type = GG_EVENT_NONE;
            return gg_event;
        }
        break;

#ifndef AXIS_CURSOR_DISABLED
    case SDL_JOYAXISMOTION:
        gg_event.type = GG_EVENT_KEY;
        switch (event->jaxis.axis)
        {
        case AXIS_CURSOR_X:
            if (event->jaxis.value >= -15000 && event->jaxis.value <= 15000)
            {
                pressed &= ~(LEFT | RIGHT);
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }

            if (!(pressed & LEFT) && (event->jaxis.value < 0))
            {
                pressed |= LEFT;
                gg_event.key = GG_KEY_LEFT;
            }
            else if (!(pressed & RIGHT) && (event->jaxis.value > 0))
            {
                pressed |= RIGHT;
                gg_event.key = GG_KEY_RIGHT;
            }
            else
            {
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
            break;
        case AXIS_CURSOR_Y:
            if (event->jaxis.value >= -15000 && event->jaxis.value <= 15000)
            {
                pressed &= ~(UP | DOWN);
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
            if (!(pressed & UP) && (event->jaxis.value < 0))
            {
                pressed |= UP;
                gg_event.key = GG_KEY_UP;
            }
            else if (!(pressed & DOWN) && (event->jaxis.value > 0))
            {
                pressed |= DOWN;
                gg_event.key = GG_KEY_DOWN;
            }
            else
            {
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
        }
#endif

    case SDL_MOUSEBUTTONDOWN:
        gg_event.type = GG_EVENT_MOUSE;
        gg_event.mouse.type = GG_MOUSE_BUTTON_DOWN;
        gg_event.mouse.button = event->button.button - 1;
        gg_event.mouse.x = event->button.x;
        gg_event.mouse.y = SCREEN_HEIGHT - 1 - event->button.y;
    }

/*    if ((event->type == SDL_KEYDOWN) && (event->key.keysym.unicode <= 0xff))
        gg_event.key = event->key.keysym.unicode;*/

    return gg_event;
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
                          gg_colour_t *col )
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
                          gg_colour_t *col )
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
                             gg_colour_t *col, float u1, float v1, float u2, float v2, GLenum mode_h, GLenum mode_v)
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
                             gg_colour_t *col, float u1, float v1, float u2, float v2, GLenum mode_h, GLenum mode_v)
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

void text_draw_string( float xpos, float ypos, char *text, float scale, gg_colour_t *col, int length );
void text_draw_string_right( float xpos, float ypos, char *text, float scale, gg_colour_t *col, int length );
void text_draw_string_bouncy( float xpos, float ypos, char *text, float scale, gg_colour_t *col, int length );
static int text_width(char *text);
static int text_height();
static int quit_to_menu;
static int title_process_retval;
static int set_loading=FALSE;

static int fps_enabled = 0;
static int vkeyboard_enabled = 0;
static int frames = 0;
static Uint32 fps_time = 0;
static float fps;

typedef struct theme
{
    char name[25];
    char style[25];
    char pieces[25];
    char board[25];
    char white_name[25];
    char black_name[25];
    char piece_tex_spin;
    int piece_tex_spin_speed;
    char lighting;
}theme;

theme themes[25];
int theme_count=0;
int selected_theme=0;
int selected_player_layout=0;
int selected_difficulty=0;
int swapping_custom=FALSE;
int selected_custom_board=0;
int selected_custom_style=0;
int selected_custom_pieces=0;

/* xml theme options */
int use_lighting()
{
    if (selected_theme==theme_count)
        return TRUE;
    else
        return themes[selected_theme].lighting;
}

int use_tex_spin()
{
    if (selected_theme==theme_count)
        return FALSE;
    else
        return themes[selected_theme].piece_tex_spin;
}

int get_tex_spin_speed()
{
    return themes[selected_theme].piece_tex_spin_speed;
}

static char* themelist[25];
static char* stylelist[25];
static int num_style;
static int cur_style;
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

static config_t config;
static config_t config_save[10];
static char time_save[10][80];
static int save_valid[10];
static char save_squares[10][80];
static int pgn_slot;
static int slots;

static gg_dialog_style_t style_ingame, style_menu;

/* In-game dialog. */

/** The in-game dialog. Provides a set of gameplay-related actions to the
 *  user.
 */

static void retract_move(gg_widget_t *widget, void *data)
{
    game_retract_move();
}

static void move_now(gg_widget_t *widget, void *data)
{
    game_move_now();
}

static void view_prev(gg_widget_t *widget, void *data)
{
    game_view_prev();
}

static void view_next(gg_widget_t *widget, void *data)
{
    game_view_next();
}

/** @brief Creates the in-game dialog.
 *
 *  @return The created dialog.
 */
static gg_dialog_t *dialog_ingame_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);

    gg_widget_t *widget = gg_action_create_with_label("Retract Move", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), retract_move, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Move Now", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), move_now, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("View Previous Move", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), view_prev, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("View Next Move", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), view_next, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_ingame);
    return GG_DIALOG(dialog);
}


/* Quit dialog. */

/** @brief Quits the current game.
 *
 *  Closes the dialog and causes the game to go back to the title menu.
 */
static void dialog_quit_ok(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
    gg_dialog_close();
    quit_to_menu = 1;
}

static void dialog_close_cb(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
}

/** The quit dialog. Asks the user to confirm that he wants to quit the game.
 */

/** @brief Creates the quit confirmation dialog.
 *
 *  @return The created dialog.
 */
static gg_dialog_t *dialog_quit_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);

    gg_widget_t *widget = gg_label_create("You don't really want to quit do ya?");
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_label_create("");
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Yeah.. I suck..", 0.5f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_quit_ok, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Of course not!", 0.5f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_ingame);
    return GG_DIALOG(dialog);
}


/* System dialog. */

/** The system dialog. Provides a set of system-related actions to the user.
 *  Currently this dialog only contains an item to quit the game. In the
 *  future this will be extended with load/save game items and possibly
 *  other items as well.
 */

/** @brief Opens the quit dialog. */
static void dialog_quit_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_quit_create());
}

/** @brief Opens the ingame dialog. */
static void dialog_ingame_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_ingame_create());
}

static gg_dialog_t *dialog_saveload_create( int saving );
static void dialog_savegame_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_saveload_create(TRUE));
}

static void dialog_loadgame_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_saveload_create(FALSE));
}

const char *whitespace_cb(mxml_node_t *node, int where)
{
    const char *name;
    name = node->value.element.name;

    if (!strcmp(name, "save"))
    {
        if (where == MXML_WS_AFTER_OPEN)
            return "\n";
    }
    else
    {
        if (where == MXML_WS_BEFORE_OPEN)
            return "\t";
        else if (where == MXML_WS_AFTER_CLOSE)
            return "\n";
    }

    return (NULL);
}

static void save_opaque(mxml_node_t *parent, char *name, char *value)
{
    mxml_node_t *node = mxmlNewElement(parent, name);
    mxmlNewOpaque(node, value);
}

static board_t board;
void write_save_xml( int slot, char *desc )
{
    int i=0;
    FILE *fp;
    char temp[80];
    char boardtemp[80];
    mxml_node_t *tree,*node,*node2;

    sprintf( temp, "save%i.xml", slot );

    if (ch_userdir())
    {
        printf("Could not enter user directory.\n");
        return;
    }

    fp = fopen(temp, "w");

    fprintf( fp, "<?xml version=\"1.0\"?>\n" );
    tree = mxmlNewElement( MXML_NO_PARENT, "save" );

    sprintf(temp, "%i", (int) time(NULL));
    save_opaque(tree, "time", temp);

    if (config.player[WHITE] == PLAYER_UI)
        save_opaque(tree, "white", "ui");
    else
        save_opaque(tree, "white", "engine");

    if (config.player[BLACK] == PLAYER_UI)
        save_opaque(tree, "black", "ui");
    else
        save_opaque(tree, "black", "engine");

    sprintf(temp, "%i", config.cpu_level);
    save_opaque(tree, "level", temp);

    for ( i=0; i<(8*8); i++ )
    {
        /*printf ( "Uber: %i\n", PIECE(board.square[i]) );*/
        switch ( PIECE(board.square[i]) )
        {
            case 0: /* pawn */
                boardtemp[i]='1';
                break;
            case 2: /* Horsey */
                boardtemp[i]='2';
                break;
            case 4: /* bishop */
                boardtemp[i]='3';
                break;
            case 6: /* Rook */
                boardtemp[i]='4';
                break;
            case 8: /* Queen */
                boardtemp[i]='5';
                break;
            case 10: /* King */
                boardtemp[i]='6';
                break;
            case 12: /* Blank */
                boardtemp[i]='0';
                break;
        }
    }
    boardtemp[i]='\0';

    save_opaque(tree, "squares", boardtemp);

    /*printf( "%s\n", boardtemp );*/

    mxmlSaveFile(tree, fp, whitespace_cb);
    fclose(fp);
}

static void load_opaque(mxml_node_t *top, char *name, char *dest);
void load_save_xml( int slot, char *desc, int *player_layout, int *difficulty )
{
    FILE *fp;
    char temp[80];
    mxml_node_t *tree, *save;

    sprintf( temp, "save%i.xml", slot );
    /*printf( "Loading %s\n", temp );*/

    if (ch_userdir())
    {
        printf("Could not enter user directory.\n");
        return;
    }

    fp = fopen(temp, "r");
    if (fp)
    {
        tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
        slots |= (1 << slot);
    }
    else
    {
        /*printf( "Error opening theme file.\n" );*/
        sprintf( desc, "Empty." );
        slots &= ~(1 << slot);
        return;
    }

    fclose(fp);

    save = tree;

    while ((save = mxmlFindElement(save, tree, "save", NULL, NULL, MXML_DESCEND)))
    {
        mxml_node_t *node;
        time_t time;
        struct tm *tm;

        load_opaque(save, "time", temp);

        time = atoi(temp);
        tm = localtime(&time);
        sprintf(time_save[slot], "%02i/%02i at %02i:%02i.", tm->tm_mday, tm->tm_mon,
                tm->tm_hour, tm->tm_min);

        load_opaque(save, "white", temp);
        if (!strcmp(temp, "ui"))
            config_save[slot].player[WHITE] = PLAYER_UI;
        else
            config_save[slot].player[WHITE] = PLAYER_ENGINE;

        load_opaque(save, "black", temp);
        if (!strcmp(temp, "ui"))
            config_save[slot].player[BLACK] = PLAYER_UI;
        else
            config_save[slot].player[BLACK] = PLAYER_ENGINE;

        load_opaque(save, "level", temp);
        config_save[slot].cpu_level = atoi(temp);

        load_opaque(save, "squares", temp);
        sprintf( save_squares[slot], "%s", temp );
    }
}

int saveload_selected=0;

static int load_game( int slot );
static void dialog_loadgame_load(gg_widget_t *widget, void *data)
{
    gg_widget_t *vbox = widget->parent;
    int slot = saveload_selected;

    if (slots & (1 << slot))
    {
        pgn_slot = slot;
        set_loading = TRUE;
        gg_dialog_close();
        gg_dialog_close();
    }
}

int changing_slot=FALSE;
int change_saving=FALSE;
static void dialog_saveload_change(gg_widget_t *widget, void *data)
{
    gg_widget_t *vbox = widget->parent;
    saveload_selected=GG_SELECT(vbox)->sel;

    /*printf( "Selected save: %i\n", saveload_selected );*/

    gg_dialog_close();
    changing_slot=TRUE;
    gg_dialog_open(dialog_saveload_create(change_saving));
    changing_slot=FALSE;
}

static void dialog_savegame_save(gg_widget_t *widget, void *data)
{
    char temp[80];
    time_t timething;
    struct tm *current_time;

    gg_widget_t *vbox = widget->parent;

    time( &timething );
    current_time = localtime( &timething );

    /*printf( "Saving slot %i\n", GG_SELECT(vbox)->sel );*/
    sprintf( temp, "Saved on %02i/%02i at %02i:%02i.", current_time->tm_mday, current_time->tm_mon,
        current_time->tm_hour, current_time->tm_min );

    write_save_xml( saveload_selected, temp );
    game_save( saveload_selected );
    gg_dialog_close();
    gg_dialog_close();
}

/** @brief Creates the system dialog.
 *
 *  @return The created dialog.
 */
static gg_dialog_t *dialog_system_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *widget;

    widget = gg_action_create_with_label("Return To Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("  Save game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_savegame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    /*widget = gg_action_create_with_label("  Load game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_loadgame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);*/

    widget = gg_action_create_with_label("  Move Options..", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_ingame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Quit Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_quit_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_ingame);
    return GG_DIALOG(dialog);
}

char xmlsquaretofont( char c )
{
    char retval;

    switch ( c )
    {
        case '0': /* blank */
            retval=32;
            break;
        case '1': /* pawn */
            retval=21;
            break;
        case '2': /* Horsey */
            retval=19;
            break;
        case '3': /* bishop */
            retval=20;
            break;
        case '4': /* Rook */
            retval=18;
            break;
        case '5': /* Queen */
            retval=17;
            break;
        case '6': /* King */
            retval=16;
            break;
    }

    /*printf( "retval: %c:%i\n", retval, retval );*/
    /*printf( "c: %c:%i\n", c, c );*/

    return retval;
}

static void menu_title_back(gg_widget_t *widget, void *data);
static gg_dialog_t *dialog_saveload_create( int saving )
{
    gg_widget_t *dialog;
    gg_widget_t *rootvbox = gg_vbox_create(0);
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *hbox = gg_hbox_create(0);
    gg_widget_t *board_box = gg_vbox_create(0);
    gg_widget_t *hboxtemp, *hboxtemp2;
    gg_widget_t *widget;
    int max_saveslots=10;
    char desc[80];
    int player_layout=0;
    int difficulty=0;
    char temp[80];
    int i=0,j=0;

    change_saving=saving;

    /* top part */
    if ( saving )
    {
        widget = gg_action_create_with_label("Save Game..                              ", 0.0f, 0.0f);
        gg_action_set_callback(GG_ACTION(widget), dialog_savegame_save, vbox);
    }
    else
    {
        widget = gg_action_create_with_label("Load Game..                              ", 0.0f, 0.0f);
        selected_player_layout=player_layout;
        selected_difficulty=difficulty;
        gg_action_set_callback(GG_ACTION(widget), dialog_loadgame_load, vbox);
    }
    gg_container_append(GG_CONTAINER(rootvbox), widget);

    /* left side */
    for ( i=0; i<max_saveslots; i++ )
    {
        load_save_xml( i, desc, &player_layout, &difficulty );

        sprintf( temp, "%i:  ", i );
        widget = gg_action_create_with_label(temp, 0.0f, 0.0f);

        gg_action_set_callback(GG_ACTION(widget), dialog_saveload_change, vbox);

        gg_container_append(GG_CONTAINER(vbox), widget);
    }

    if ( changing_slot )
        gg_vbox_set_selected(vbox, saveload_selected );

    gg_container_append(GG_CONTAINER(hbox), vbox );

    widget = gg_seperatorv_create();
    gg_container_append(GG_CONTAINER(hbox), widget );

    /* Right side.. */
    vbox = gg_vbox_create(0);

    sprintf( temp, "   Save slot %i.", saveload_selected );
    widget = gg_label_create(temp);
    gg_container_append(GG_CONTAINER(vbox), widget);

    if ( slots & (1 << saveload_selected) )
    {
        sprintf( temp, "   Saved: %s", time_save[saveload_selected] );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        switch ( config_save[saveload_selected].player[WHITE] )
        {
            case PLAYER_ENGINE:
                sprintf( temp, "   White: CPU" );
                break;
            case PLAYER_UI:
                sprintf( temp, "   White: Human" );
                break;
            default:
                /* Whoops */
                sprintf( temp, "   White: Oh no.." );
                break;
        }
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        switch ( config_save[saveload_selected].player[BLACK] )
        {
            case PLAYER_ENGINE:
                sprintf( temp, "   Black: CPU" );
                break;
            case PLAYER_UI:
                sprintf( temp, "   Black: Human" );
                break;
            default:
                /* Whoops */
                sprintf( temp, "   Black: Oh no.." );
                break;
        }
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        sprintf( temp, "   Difficulty: %i", config_save[saveload_selected].cpu_level );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        /* create board.. */

        for ( i=0; i<8; i++ )
        {
            hboxtemp = gg_hbox_create(0);
            for ( j=0; j<8; j++ )
            {
                if ( j==0 )
                {
                    hboxtemp2 = gg_hbox_create(0);
                    gg_set_requested_size(hboxtemp2, 35, 20);
                    gg_container_append(GG_CONTAINER(hboxtemp), hboxtemp2);
                }

                hboxtemp2 = gg_hbox_create(0);
                gg_set_requested_size(hboxtemp2, 20, 20);

                sprintf( temp, "%c", xmlsquaretofont(save_squares[saveload_selected][j+(i*8)]) );
                widget = gg_label_create( temp );
                gg_container_append(GG_CONTAINER(hboxtemp2), widget);
                gg_container_append(GG_CONTAINER(hboxtemp), hboxtemp2);
            }
            gg_container_append(GG_CONTAINER(board_box), hboxtemp);
        }
        gg_container_append(GG_CONTAINER(vbox), board_box);
    }
    else
    {
        sprintf( temp, "   Empty slot" );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        for ( i=0; i<11; i++ )
        {
            widget = gg_label_create(" ");
            gg_container_append(GG_CONTAINER(vbox), widget);
        }
    }

    gg_container_append(GG_CONTAINER(hbox), vbox);
    gg_container_append(GG_CONTAINER(rootvbox), hbox);

    /* bottom */
    widget = gg_action_create_with_label("Back..", 0.0f, 0.0f);

    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);

    gg_container_append(GG_CONTAINER(rootvbox), widget);

    /* Dialog stuff */
    dialog = gg_dialog_create(rootvbox);

    if ( saving )
        gg_dialog_set_style(GG_DIALOG(dialog), &style_ingame);
    else
        gg_dialog_set_style(GG_DIALOG(dialog), &style_menu);

    if ( changing_slot )
        gg_vbox_set_selected(rootvbox, 1 );


    return GG_DIALOG(dialog);
}


#ifdef CREEPYMUPPET
static gg_dialog_t *dialog_saveload_create( int saving )
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *widget;
    int max_saveslots=10;
    char desc[80];
    int player_layout=0;
    int difficulty=0;
    char temp[80];
    int i=0;

    if ( saving )
        widget = gg_label_create("Save game..");
    else
        widget = gg_label_create("Load game..");
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_label_create(" ");
    gg_container_append(GG_CONTAINER(vbox), widget);

    for ( i=0; i<max_saveslots; i++ )
    {
        load_save_xml( i, desc, &player_layout, &difficulty );

        sprintf( temp, "  %i: %s", i, desc );
        widget = gg_action_create_with_label(temp, 0.0f, 0.0f);

        if ( saving )
            gg_action_set_callback(GG_ACTION(widget), dialog_savegame_save, vbox);
        else
        {
            /* loading.. which means no board up yet.. */
            selected_player_layout=player_layout;
            selected_difficulty=difficulty;
            gg_action_set_callback(GG_ACTION(widget), dialog_loadgame_load, vbox);
        }

        gg_container_append(GG_CONTAINER(vbox), widget);
    }

    widget = gg_label_create(" ");
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Back..", 0.0f, 0.0f);

    if ( saving )
        gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    else
        gg_action_set_callback(GG_ACTION(widget), menu_title_back, NULL);

    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);

    if ( saving )
        gg_dialog_set_style(GG_DIALOG(dialog), &style_ingame);
    else
        gg_dialog_set_style(GG_DIALOG(dialog), &style_menu);

    return GG_DIALOG(dialog);
}
#endif
/* Victory dialog. */

static gg_dialog_t *dialog_victory_create(result_t *result)
{
    gg_widget_t *dialog;
    gg_widget_t *hbox = gg_hbox_create(20);
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *image_l, *image_r;
    gg_widget_t *action;
    gg_widget_t *text;

    switch (result->code)
    {
    case RESULT_WHITE_WINS:
        image_l = gg_image_create(&white_pieces[GUI_PIECE_KING]);
        image_r = gg_image_create(&white_pieces[GUI_PIECE_QUEEN]);
        text = gg_label_create("White won the match!");
        break;

    case RESULT_BLACK_WINS:
        image_l = gg_image_create(&black_pieces[GUI_PIECE_KING]);
        image_r = gg_image_create(&black_pieces[GUI_PIECE_QUEEN]);
        text = gg_label_create("Black won the match!");
        break;

    default:
        image_l = gg_image_create(&white_pieces[GUI_PIECE_KING]);
        image_r = gg_image_create(&black_pieces[GUI_PIECE_KING]);
        text = gg_label_create("The game ended in a draw!");
    }

    gg_container_append(GG_CONTAINER(vbox), text);
    text = gg_label_create(result->reason);
    gg_container_append(GG_CONTAINER(vbox), text);
    text = gg_label_create("");
    gg_container_append(GG_CONTAINER(vbox), text);
    action = gg_action_create_with_label("Ok", 0.5f, 0.5f);
    gg_action_set_callback(GG_ACTION(action), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), action);
    gg_container_append(GG_CONTAINER(hbox), image_l);
    gg_container_append(GG_CONTAINER(hbox), vbox);
    gg_container_append(GG_CONTAINER(hbox), image_r);
    dialog = gg_dialog_create(hbox);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_ingame);
    return GG_DIALOG(dialog);
}

/* Title dialog. */

#define GAME_TYPE_HUMAN_VS_CPU 0
#define GAME_TYPE_CPU_VS_HUMAN 1
#define GAME_TYPE_HUMAN_VS_HUMAN 2

/** @brief Triggers gameplay start based on currently selected options. */
static void menu_title_start(gg_widget_t *widget, void *data)
{
    set_loading=TRUE;

    gg_dialog_close();
    gg_dialog_close();
}

/** @brief Triggers DreamChess exit. */
static void menu_title_quit(gg_widget_t *widget, void *data)
{
    title_process_retval = 1;
    gg_dialog_close();
}

static gg_dialog_t *dialog_title_root_create();
static gg_dialog_t *dialog_title_create();

void dialog_title_players(gg_widget_t *widget, void *data)
{
    selected_player_layout=gg_option_get_selected(GG_OPTION(widget));

    switch (selected_player_layout)
    {
    case GAME_TYPE_HUMAN_VS_CPU:
        config.player[WHITE] = PLAYER_UI;
        config.player[BLACK] = PLAYER_ENGINE;
        flip_board = 0;
        break;
    case GAME_TYPE_CPU_VS_HUMAN:
        config.player[WHITE] = PLAYER_ENGINE;
        config.player[BLACK] = PLAYER_UI;
        flip_board = 1;
        break;
    case GAME_TYPE_HUMAN_VS_HUMAN:
        config.player[WHITE] = PLAYER_UI;
        config.player[BLACK] = PLAYER_UI;
        flip_board = 0;
    }
}

static void dialog_title_root_new(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_title_create());
}

static void dialog_title_root_load(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_saveload_create(FALSE));
}

static void dialog_title_level(gg_widget_t *widget, void *data)
{
    config.cpu_level = gg_option_get_selected(GG_OPTION(widget)) + 1;
    selected_difficulty=config.cpu_level-1;
}

static void dialog_title_custom_theme(gg_widget_t *widget, void *data)
{
    selected_theme = gg_option_get_selected(GG_OPTION(widget));
    if ( gg_option_get_selected(GG_OPTION(widget)) != theme_count )
    {
        /* printf( "Theme changed from Custom!\n" ); */
        gg_dialog_close();
        swapping_custom=TRUE;
        gg_dialog_open(dialog_title_create());
        swapping_custom=FALSE;
    }
}

static gg_dialog_t *dialog_title_custom_create();
static void dialog_title_theme(gg_widget_t *widget, void *data)
{
    selected_theme = gg_option_get_selected(GG_OPTION(widget));
    if ( gg_option_get_selected(GG_OPTION(widget)) == theme_count )
    {
        /* printf( "Theme changed to Custom!\n" ); */
        gg_dialog_close();
        swapping_custom=TRUE;
        gg_dialog_open(dialog_title_custom_create());
        swapping_custom=FALSE;
        gg_option_set_selected(GG_OPTION(widget),theme_count-1);
        gg_dialog_cleanup();
    }
}

static void dialog_title_style(gg_widget_t *widget, void *data)
{
    cur_style = gg_option_get_selected(GG_OPTION(widget));
    selected_custom_style=cur_style;
}

static void dialog_title_pieces(gg_widget_t *widget, void *data)
{
    pieces_list_cur = gg_option_get_selected(GG_OPTION(widget));
    selected_custom_pieces=pieces_list_cur;
}

static void dialog_title_board(gg_widget_t *widget, void *data)
{
    board_list_cur = gg_option_get_selected(GG_OPTION(widget));
    selected_custom_board=board_list_cur;
}

static gg_dialog_t *dialog_title_custom_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;
    int i;

    config.player[WHITE] = PLAYER_UI;
    config.player[BLACK] = PLAYER_ENGINE;
    config.cpu_level = selected_difficulty;
    cur_style = selected_custom_style;
    pieces_list_cur = selected_custom_pieces;
    board_list_cur = selected_custom_board;
    flip_board = 0;
    pgn_slot = -1;

    widget = gg_action_create_with_label("Start Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), menu_title_start, NULL);
    vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), widget);

    label = gg_label_create("  Players:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Difficulty:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Theme:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("    Style:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("    Pieces:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("    Board:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    /*   label = gg_label_create("Name:");
       gg_alignable_set_alignment(GG_ALIGNABLE(label), 0.0f, 0.0f);
       gg_container_append(GG_CONTAINER(vbox2), label);*/

    hbox = gg_hbox_create(20);
    gg_container_append(GG_CONTAINER(hbox), vbox2);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Human vs. CPU", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "CPU vs. Human", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Human vs. Human", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_players, NULL);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_player_layout);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Level 1", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 2", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 3", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 4", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_level, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_difficulty);

    /* Themelist list.. */
    widget = gg_option_create();
    for (i = 0; i < theme_count+1; i++)
        gg_option_append_label(GG_OPTION(widget), themelist[i], 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_custom_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),theme_count);

    widget = gg_option_create();
    for (i = 0; i < num_style; i++)
        gg_option_append_label(GG_OPTION(widget), stylelist[i], 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_style, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_custom_style);

    widget = gg_option_create();
    for (i = 0; i < pieces_list_total; i++)
        gg_option_append_label(GG_OPTION(widget), pieces_list[i], 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_pieces, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_custom_pieces);

    widget = gg_option_create();
    for (i = 0; i < board_list_total; i++)
        gg_option_append_label(GG_OPTION(widget), board_list[i], 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_board, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_custom_board);

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Back", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 0, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_menu);

    if ( swapping_custom )
    {
        gg_vbox_set_selected(vbox, 1 );
        gg_vbox_set_selected(vbox2, 2 );
    }

    return GG_DIALOG(dialog);
}

static gg_dialog_t *dialog_title_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;
    int i;

    config.player[WHITE] = PLAYER_UI;
    config.player[BLACK] = PLAYER_ENGINE;
    config.cpu_level = 1;
    cur_style = 0;
    pieces_list_cur = 0;
    board_list_cur = 0;
    flip_board = 0;
    pgn_slot = -1;

    widget = gg_action_create_with_label("Start Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), menu_title_start, NULL);
    vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), widget);

    label = gg_label_create("  Players:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Difficulty:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Theme:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    hbox = gg_hbox_create(20);
    gg_container_append(GG_CONTAINER(hbox), vbox2);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Human vs. CPU", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "CPU vs. Human", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Human vs. Human", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_players, NULL);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_player_layout);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Level 1", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 2", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 3", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 4", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_level, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_difficulty);

    /* Themelist list.. */
    widget = gg_option_create();
    for (i = 0; i < theme_count+1; i++)
        gg_option_append_label(GG_OPTION(widget), themelist[i], 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_theme);

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Back", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 63, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_menu);

    if ( swapping_custom )
    {
        gg_vbox_set_selected(vbox, 1 );
        gg_vbox_set_selected(vbox2, 2 );
    }

    return GG_DIALOG(dialog);
}

static gg_dialog_t *dialog_title_root_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;
    int i;

    config.player[WHITE] = PLAYER_UI;
    config.player[BLACK] = PLAYER_ENGINE;
    config.cpu_level = 1;
    cur_style = 0;
    pieces_list_cur = 0;
    board_list_cur = 0;
    flip_board = 0;

    vbox = gg_vbox_create(0);
    widget = gg_action_create_with_label("New Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_title_root_new, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Load Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_title_root_load, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Quit", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), menu_title_quit, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 63, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_menu);

    return GG_DIALOG(dialog);
}

static void dialog_vkeyboard_key(gg_widget_t *widget, void *data)
{
    if (gg_dialog_current())
    {
        gg_event_t event;
        event.type = GG_EVENT_KEY;
        event.key = *((int *) data);
        gg_dialog_input_current(event);
    }

    printf( "Pressed a keyyy... it was uh.. '%c' .. right?\n\r", *(ui_event_t *)data);
}

static gg_dialog_t *dialog_vkeyboard_create()
{
    gg_widget_t *dialog;
    gg_widget_t *label;
    gg_widget_t *action;
    gg_widget_t *hbox;
    gg_widget_t *vbox2;
    static ui_event_t key;
    int i,j,k;
    int max_width = 0;

    hbox=gg_hbox_create(0);
    label=gg_label_create("Type stuff, k?" );
    gg_container_append(GG_CONTAINER(hbox), label);
    vbox2=gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), hbox );

    for (i = 0; i < 256; i++)
    {
        int cur_width = text_characters[i].width;
        if (cur_width > max_width)
            max_width = cur_width;
    }

    k=0;
    for ( j=0; j<6; j++ )
    {
        hbox = gg_hbox_create(2);
        for ( i=0; i<16; i++ )
        {
            if ( k < 127-33 )
            {
                char key_str[2];
                key=k+33;
                key_str[0] = key;
                key_str[1] = '\0';
                action = gg_action_create_with_label(key_str, 0.5f, 0.5f);
                gg_set_requested_size(action, max_width, 0);
                gg_action_set_callback(GG_ACTION(action), dialog_vkeyboard_key, &keys[k]);

                gg_container_append(GG_CONTAINER(hbox), action);
                k++;
            }
        }
        gg_container_append(GG_CONTAINER(vbox2), hbox);
    }

    dialog = gg_dialog_create(vbox2);
    return GG_DIALOG(dialog);
}

/* This is our SDL surface */
static SDL_Surface *surface;

static void generate_text_chars();
static void draw_backdrop();

static void init_gl();
static void resize_window( int width, int height );
static int GetMove();
void load_texture_png( texture_t *texture, char *filename, int alpha );
static void draw_name_dialog( float xpos, float ypos, char* name, int left, int white );

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
    gg_colour_t col_cap = {0.55f, 0.75f, 0.95f, 0.0f};
    gg_colour_t col_item = {1.0f, 1.0f, 1.0f, 0.0f};

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

/*  This function loads the image upside-down. Meaning that texture
 *  coordinate (0,0) corresponds to the top-left corner of the image.
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

void dialog_promote_cb(gg_widget_t *widget, void *data)
{
    dialog_promote_piece = *(int *)data;
    gg_dialog_close();
}

gg_dialog_t *dialog_promote_create(int colour)
{
    static int cb_pieces[4];

    texture_t *pieces;
    gg_widget_t *dialog;
    gg_widget_t *action;
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *hbox = gg_hbox_create(0);
    gg_widget_t *gg_image;
    gg_widget_t *text = gg_label_create("Promotion! Choose new piece!");

    dialog_promote_piece = NONE;
    cb_pieces[0] = QUEEN + colour;
    cb_pieces[1] = ROOK + colour;
    cb_pieces[2] = BISHOP + colour;
    cb_pieces[3] = KNIGHT + colour;

    gg_container_append(GG_CONTAINER(vbox), text);

    if (IS_WHITE(colour))
        pieces = white_pieces;
    else
        pieces = black_pieces;

    gg_image = gg_image_create(&pieces[GUI_PIECE_QUEEN]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[0]);
    gg_container_append(GG_CONTAINER(hbox), action);

    gg_image = gg_image_create(&pieces[GUI_PIECE_ROOK]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[1]);
    gg_container_append(GG_CONTAINER(hbox), action);

    gg_image = gg_image_create(&pieces[GUI_PIECE_BISHOP]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[2]);
    gg_container_append(GG_CONTAINER(hbox), action);

    gg_image = gg_image_create(&pieces[GUI_PIECE_KNIGHT]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[3]);
    gg_container_append(GG_CONTAINER(hbox), action);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    dialog = gg_dialog_create(vbox);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_message_create(char *message)
{
    gg_widget_t *dialog;
    gg_widget_t *widget;

    gg_widget_t *vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), gg_label_create("Important message from engine"));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(""));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(message));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(""));
    widget = gg_action_create_with_label("Ok", 0.5f, 0.5f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    dialog = gg_dialog_create(vbox);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);

    return GG_DIALOG(dialog);
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
static config_t *do_menu(int *pgn)
{
    gg_dialog_t *keyboard = dialog_vkeyboard_create();
    SDL_Event event;
    game_difficulty=1;
    game_type=GAME_TYPE_HUMAN_VS_CPU;
    title_process_retval=2;

    board_xpos=128;
    board_ypos=30;
    can_load=FALSE;
    set_loading=FALSE;

    white_in_check=FALSE;
    black_in_check=FALSE;

    draw_credits(1);
    gg_dialog_open(dialog_title_root_create());
    /* If created, and theme set to custom.. open custom.. */
   /* if ( selected_theme == theme_count )
        gg_dialog_open(dialog_title_custom_create());
    else
        gg_dialog_open(dialog_title_create());*/

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while ( 1 )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gg_dialog_cleanup();

        /* Precess input */
        while ( SDL_PollEvent( &event ) )
        {
            gg_event_t gg_event;

            if (event.type == SDL_QUIT)
                /* FIXME */
                exit(0);

            if (event.type == SDL_MOUSEMOTION)
            {
                gg_dialog_t *dialog = gg_dialog_current();
                if (dialog)
                    gg_dialog_mouse_movement(dialog, event.motion.x, 479 - event.motion.y);

                continue;
            }

            gg_event = convert_event(&event);

            if (wait_menu)
            {
                if (gg_event.type != GG_EVENT_NONE)
                {
                    reset_string_type_length();
                    wait_menu = 0;
                }
                continue;
            }

            if (gg_event.type == GG_EVENT_KEY && gg_event.key == 0x06)
            {
                fps_enabled = 1 - fps_enabled;
                continue;
            }

            if (gg_event.type == GG_EVENT_KEY && gg_event.key == 0x0b)
            {
                vkeyboard_enabled = 1 - vkeyboard_enabled;
                continue;
            }

            if (vkeyboard_enabled)
                keyboard->input(GG_WIDGET(keyboard), gg_event);
            else
                gg_dialog_input_current(gg_event);

            if (title_process_retval == 1)
                return NULL;
        }

        /* Draw the menu.. */
        draw_texture( &menu_title_tex, 0, 0, 640, 480, 1.0f, &col_white );

        if ( can_load == TRUE )
        {
            /* We using custom? */
            if ( selected_theme==theme_count )
                load_theme(stylelist[cur_style], pieces_list[pieces_list_cur],
                    board_list[board_list_cur]);
            else
            {
                /* printf( "Loading theme %i\n", selected_theme ); */
                load_theme(themes[selected_theme].style, themes[selected_theme].pieces,
                    themes[selected_theme].board);
            }

            reset_3d();
            *pgn = pgn_slot;
            if (pgn_slot >= 0)
                config = config_save[pgn_slot];
            return &config;
        }

        if ( set_loading == FALSE )
        {
            if (wait_menu)
                text_draw_string_bouncy( 140, 30, "Press any key or button to start",
                                         1.5, &col_white, string_type_pos );
            else
            {
                gg_dialog_t *dialog = gg_dialog_current();
                gg_dialog_render(dialog);
            }

            if (vkeyboard_enabled)
                gg_dialog_render(keyboard);

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

void draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour)
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
                    colour, xsrc,
                    ysrc,
                    xsrc + width, ysrc + height, en_h, en_v);
}

void draw_char(int c, int x, int y, gg_colour_t *colour)
{
    text_draw_char(x, y, 1.0f, c, colour);
}

void *get_char_image(int c)
{
    if (c < 0)
        c += 256;

    return &text_characters[c];
}

unsigned int get_ticks()
{
    return SDL_GetTicks();
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
    if (c < 0)
        c += 256;

    if (width)
        *width = text_characters[c].width;

    if (height)
        *height = text_characters[c].height;
}

gg_driver_t gg_driver_sdlgl =
    {
        draw_rect,
        draw_rect_fill,
        draw_image,
        get_char_image,
        draw_char,
        get_image_size,
        get_char_size,
        get_ticks
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
    SDL_Surface *icon;
    const SDL_VideoInfo *video_info;
    int i;
    DIR* styledir;
    struct dirent* styledir_entry;
    char temp[80];

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_EnableUNICODE(1);

    ch_datadir();
    icon = IMG_Load("icon.png");

    if (!icon)
    {
        fprintf(stderr, "Could not load icon: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WM_SetIcon(icon, NULL);
    SDL_FreeSurface(icon);

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

    gg_system_init(&gg_driver_sdlgl);

    /* New text stuff. */
    generate_text_chars();

    /* For the menu.. */
    load_texture_png( &menu_title_tex, "menu_title.png" , 0);

    /* Load themes xml */

    ch_datadir();
    if ( (styledir=opendir("themes")) != NULL )
    {
        theme_count=0;
        styledir_entry=readdir(styledir);
        while ( styledir_entry != NULL )
        {
            if ( styledir_entry->d_name[0] != '.' )
            {
                sprintf( temp, "themes/%s", styledir_entry->d_name );
                load_theme_xml( temp );
            }
            styledir_entry=readdir(styledir);
        }
    }

    /* Fill theme list. */
    if ( theme_count > 0 )
    {
        for ( i=0; i<theme_count; i++ )
            themelist[i]=strdup( themes[i].name );
    }
    themelist[theme_count]=strdup( "Custom" );

    /* Fill style list. */
    if ( (styledir=opendir("styles")) != NULL )
    {
        num_style = 0;
        styledir_entry=readdir(styledir);
        while ( styledir_entry != NULL )
        {
            if ( styledir_entry->d_name[0] != '.' )
                stylelist[num_style++]=strdup( styledir_entry->d_name );
            styledir_entry=readdir(styledir);
        }
    }

    chdir("styles");
    chdir("default");
    load_border(menu_border, "border.png");

    /* Fill pieces list. */
    ch_datadir();

    if ((styledir=opendir("pieces")) != NULL )
    {
        pieces_list_total = 0;
        while ((styledir_entry = readdir(styledir)) != NULL)
        {
            if (styledir_entry->d_name[0] != '.')
            {
                pieces_list = realloc(pieces_list, (pieces_list_total + 1) *
                                      sizeof(char *));
                pieces_list[pieces_list_total++] =
                    strdup(styledir_entry->d_name);
            }
        }
    }

    style_ingame.textured = 1;
    style_ingame.fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.5f);
    style_ingame.hor_pad = 20;
    style_ingame.vert_pad = 10;

    for (i = 0; i < 9; i++)
        style_ingame.border.textured.image[i] = &border[i];

    style_menu.textured = 1;
    style_menu.fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.0f);
    style_menu.hor_pad = 20;
    style_menu.vert_pad = 10;

    for (i = 0; i < 9; i++)
        style_menu.border.textured.image[i] = &menu_border[i];

    /* Fill board list. */
    ch_datadir();

    /* Make key table? */
    PopulateKeyTable();

    if ((styledir=opendir("boards")) != NULL )
    {
        board_list_total = 0;
        while ((styledir_entry = readdir(styledir)) != NULL)
        {
            if (styledir_entry->d_name[0] != '.')
            {
                board_list = realloc(board_list, (board_list_total + 1) *
                                     sizeof(char *));
                board_list[board_list_total++] =
                    strdup(styledir_entry->d_name);
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

static void load_opaque(mxml_node_t *top, char *name, char *dest)
{
    mxml_node_t *node = mxmlFindElement(top, top, name, NULL, NULL, MXML_DESCEND);
    if (node)
    {
        node = mxmlWalkNext(node, node, MXML_DESCEND);
        if (node && node->type == MXML_OPAQUE)
            strcpy(dest, node->value.opaque);
    }
}

/** @brief Load the themes XML
 *
 */
static void load_theme_xml( char *xmlfile )
{
    FILE *fp;
    mxml_node_t *tree, *theme;

    fp = fopen(xmlfile, "r");
    if (fp)
        tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
    else
        printf( "Error opening theme file.\n" );

    fclose(fp);

    theme = tree;

    while ((theme = mxmlFindElement(theme, tree, "theme", NULL, NULL, MXML_DESCEND)))
    {
        mxml_node_t *node;

        /* Set theme to defaults.. incase we have missing bits..*/
        sprintf( themes[theme_count].name, "Un named" );
        sprintf( themes[theme_count].style, "default" );
        sprintf( themes[theme_count].pieces, "classiclow" );
        sprintf( themes[theme_count].board, "classic" );
        sprintf( themes[theme_count].white_name, "White" );
        sprintf( themes[theme_count].black_name, "Black" );
        themes[theme_count].lighting=TRUE;
        themes[theme_count].piece_tex_spin=FALSE;
        themes[theme_count].piece_tex_spin_speed=0;

        load_opaque(theme, "name", themes[theme_count].name);
        load_opaque(theme, "style", themes[theme_count].style);
        load_opaque(theme, "pieces", themes[theme_count].pieces);
        load_opaque(theme, "board", themes[theme_count].board);
        load_opaque(theme, "white_name", themes[theme_count].white_name);
        load_opaque(theme, "black_name", themes[theme_count].black_name);

        node = mxmlFindElement(theme, theme, "lighting", NULL, NULL, MXML_DESCEND);
        if (node)
        {
            node = mxmlWalkNext(node, node, MXML_DESCEND);

            if (node && node->type == MXML_OPAQUE)
                if ( !strcmp( node->value.opaque, "off" ) )
                    themes[theme_count].lighting=FALSE;
        }

        node = mxmlFindElement(theme, theme, "tex_spin", NULL, NULL, MXML_DESCEND);
        if (node)
        {
            node = mxmlWalkNext(node, node, MXML_DESCEND);

            themes[theme_count].piece_tex_spin=TRUE;
            themes[theme_count].piece_tex_spin_speed=atoi(node->value.opaque);
            /* printf( "Speed: %i\n", themes[theme_count].piece_tex_spin_speed );
            if (node && node->type == MXML_OPAQUE)
                if ( !strcmp( node->value.opaque, "off" ) )
                    themes[theme_count].lighting=FALSE; */
        }

        theme_count++;
    }
    /* printf( "We loaded %i themes\n", theme_count ); */
}

/** @brief Loads a style.
 *
 *  @param name The name of the subdirectory of the theme to load.
 */
static void load_theme(char* style, char* pieces, char *board)
{
    ch_datadir();
    chdir("styles");
    chdir(style);

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
    /* printf( "Loaded theme.\n" ); */
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
static void draw_move_list( gg_colour_t *col_normal, gg_colour_t *col_high )
{
    char **list;
    int entries, view, i;
    int y;
    int last_white, last_black;
    float x_white = 30;
    float y_white = 350;
    float x_black = 610;
    float y_black = 350;
    gg_colour_t col_normal2=*col_normal;
    gg_colour_t col_high2=*col_normal;

    game_get_move_list(&list, &entries, &view);

    if (IS_BLACK(board.turn))
    {
        last_white = view;
        last_black = view - 1;
    }
    else
    {
        last_black = view;
        last_white = view - 1;
    }

    y = y_white;
    for (i = last_white; i >= 0 && i >= last_white - 8; i -= 2)
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
    if (IS_BLACK(board.turn))
    {
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
    for (i = last_black; i >= 0 && i >= last_black - (IS_BLACK(board.turn) ? 6 : 8); i -= 2)
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
static void draw_capture_list(gg_colour_t *col)
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

    /* FIXME */
    while ( piece_moving_done == 0 )
        poll_move();
}

/** Implements ui_driver::show_result. */
static void show_result(result_t *res)
{
    gg_dialog_open(dialog_victory_create(res));
}

/** @brief Main in-game rendering routine.
 *
 *  @param b Board configuration to render.
 */
static void draw_scene( board_t *b )
{
    char temp[80];
    int clock_seconds=0;
    int clock_minutes=0;

    gg_dialog_cleanup();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);

    draw_backdrop();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    go_3d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glDepthMask(GL_TRUE);

    render_scene_3d(b);
    mouse_square = find_square(mouse_pos.x, mouse_pos.y);

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);

    /* draw_captured_pieces( 480, 70 ); */
    glPushMatrix();


    draw_border(style_ingame.border.textured.image, gg_rect(20, 440, 170, 20), 8);

    draw_border(style_ingame.border.textured.image, gg_rect(455, 440, 170, 20), 8);

    /* Da clocken */
    /*draw_border(style_ingame.border.textured.image, gg_rect(290, 440, 60, 20), 8);*/

    glPopMatrix();

    glPushMatrix();
    draw_name_dialog( 50, 430, themes[selected_theme].white_name, TRUE, 1 );
    draw_name_dialog( 490, 430, themes[selected_theme].black_name, FALSE, 0 );
    draw_border(style_ingame.border.textured.image, gg_rect(20, 375, 75, 10), 8);
    draw_border(style_ingame.border.textured.image, gg_rect(545, 375, 75, 10), 8);
    draw_health_bars();
    draw_move_list(&col_white, &col_yellow);
    draw_capture_list(&col_white);

    clock_minutes=(((SDL_GetTicks()-turn_counter_start)/1000)/60);
    clock_seconds=((SDL_GetTicks()-turn_counter_start)/1000)-(clock_minutes*60);
    sprintf( temp, "%i:%02i", clock_minutes, clock_seconds );
    /*text_draw_string( 303, 440, temp, 1, &col_black, 999 );*/
    glPopMatrix();

    if ( white_in_check == TRUE )
        text_draw_string_bouncy( 180, 420, "White is in check!", 2, &col_white,
                                 string_type_pos );
    else if ( black_in_check == TRUE )
        text_draw_string_bouncy( 180, 420, "Black is in check!", 2, &col_white,
                                 string_type_pos );

    if (gg_dialog_current())
    {
        gg_dialog_t *dialog = gg_dialog_current();
        gg_dialog_render(dialog);
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
int text_draw_char( float xpos, float ypos, float scale, int character, gg_colour_t *col )
{
    int index, offset;

    offset=0;
    index=character;

    if (index < 0)
        index += 256;

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
void text_draw_string( float xpos, float ypos, char *text, float scale, gg_colour_t *col, int length )
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

static int text_width_n(char *text, int n)
{
    int retval = 0, i;

    for (i = 0; i < n; i++)
    {
        int index = text[i];

        if (index < 0)
            index += 256;

        retval += text_characters[index].width;
    }

    return retval;
}

/** @brief Returns the width of a string.
 *
 *  @param text String to compute width of.
 *  @return Width of string in pixels.
 */
static int text_width(char *text)
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

/** @brief Renders a latin1 string with right-alignment.
 *
 *  @param xpos Rightmost x-coordinate to render the string at.
 *  @param ypos Lowermost y-coordinate to render the string at.
 *  @param text The string to render.
 *  @param scale Size scale factor.
 *  @param col The colour to render with.
 */
void text_draw_string_right( float xpos, float ypos, char *text, float scale, gg_colour_t *col, int length )
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
void text_draw_string_bouncy( float xpos, float ypos, char *text, float scale, gg_colour_t *col, int length )
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
    gg_dialog_open(dialog_message_create(msg));
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

    if (source == -1)
    {
        source = input;
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
        gg_dialog_open(dialog_promote_create(COLOUR(board.square[source])));
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

    /* start_piece_move( source, dest ); */

    source = -1;
    dest = -1;
    game_make_move(move, 1);
    reset_turn_counter();
    return;
}

#define MOVE_SPEED (60 / fps)

int load_game( int slot )
{
    int retval=game_load( slot );

    /*if (!retval)
        show_message("PGN file loading failed.");*/

    return retval;
}

/** @brief Main input routine.
 *
 *  Handles keyboard commands. When the user selects a chess piece
 *  selected_piece is updated.
 *
 *  @return If the user selected a chess piece a value between 0 (A1) and 63
 *          (H8) is returned. -1 if no chess piece was selected.
 */
int last_mousex=0;
int last_mousey=0;
static int GetMove()
{
    int retval = -1;
    int mousex, mousey;
    static Sint16 rotx, roty;
    SDL_Event event;
    Uint8 *keystate = SDL_GetKeyState(NULL);
    Uint8 mousestate = SDL_GetMouseState(&mousex, &mousey);

    if( mousestate & SDL_BUTTON_MIDDLE )
    {
        move_camera( -(mousey-last_mousey)*MOVE_SPEED, -(mousex-last_mousex)*MOVE_SPEED);
        last_mousex=mousex;
        last_mousey=mousey;
    }
    else
    {
        last_mousex=mousex;
        last_mousey=mousey;
    }

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
            if (event.type == SDL_QUIT)
                /* FIXME */
                exit(0);
    }

    if ((roty < -3000) || (roty > 3000))
        move_camera(-roty / (float) 32768 * 0.6f * MOVE_SPEED, 0.0f);

    if ((rotx < -3000) || (rotx > 3000))
        move_camera(0.0f, -rotx / (float) 32768 * 0.6f * MOVE_SPEED);

    while ( SDL_PollEvent( &event ) )
    {
        gg_event_t gg_event;

        if (event.type == SDL_QUIT)
            /* FIXME */
            exit(0);

        if (event.type == SDL_MOUSEMOTION)
        {
            gg_dialog_t *dialog = gg_dialog_current();
            mouse_pos.x = event.motion.x;
            mouse_pos.y = event.motion.y;

            if (dialog)
                gg_dialog_mouse_movement(dialog, event.motion.x, 479 - event.motion.y);

            continue;
        }

        if (!gg_dialog_current() && event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
        {
            retval = mouse_square;
            if (retval != -1)
                select_piece(retval);

            continue;
        }

        if (!gg_dialog_current() && event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_RIGHT)
        {
            gg_dialog_open(dialog_system_create());

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

        gg_event = convert_event(&event);

        if (gg_event.type == GG_EVENT_NONE)
            continue;

        if (gg_dialog_current())
            gg_dialog_input_current(gg_event);

        /* In the promote dialog */
        else if (gg_event.type == GG_EVENT_KEY)
            switch (gg_event.key)
            {
            case GG_KEY_LEFT:
                move_selector(SELECTOR_LEFT);
                break;
            case GG_KEY_RIGHT:
                move_selector(SELECTOR_RIGHT);
                break;
            case GG_KEY_UP:
                move_selector(SELECTOR_UP);
                break;
            case GG_KEY_DOWN:
                move_selector(SELECTOR_DOWN);
                break;
            case GG_KEY_ACTION:
                retval = get_selector();
                select_piece(retval);
                break;
            case GG_KEY_ESCAPE:
                gg_dialog_open(dialog_system_create());
                break;
            case 'g':
            case GG_KEY_EXTRA3:
                /* gg_dialog_open(dialog_ingame_create()); */
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
            case 's':
                /* game_save(); */
                break;
            case 'l':
                /* load_game(); */
                break;
            case 0x06:
                fps_enabled = 1 - fps_enabled;
                break;
            default:
                break;
            }
        break;
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
