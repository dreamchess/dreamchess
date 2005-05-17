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

/* Define our booleans */
#define TRUE  1
#define FALSE 0

/** Desired frames per second. */
#define FPS 9999

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

/** Colour description. */
typedef struct colour
{
    /** Red channel. Ranges from 0.0f to 1.0f. */
    float r;

    /** Green channel. Ranges from 0.0f to 1.0f. */
    float g;

    /** Blue channel. Ranges from 0.0f to 1.0f. */
    float b;

    /** Alpha channel. Ranges from 0.0f (transparent) to 1.0f (opaque). */
    float a;
}
colour_t;

/* Some predefined colours. */

static colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static colour_t col_grey =
    {
        0.5f, 0.5f, 0.5f, 1.0f
    };

static colour_t col_red =
    {
        1.0f, 0.0f, 0.0f, 1.0f
    };

static colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

static colour_t col_yellow =
    {
        1.0f, 1.0f, 0.0f, 1.0f
    };

void draw_rect(int x, int y, int w, int h, colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + h + 0.5f, 1.0f);
    glVertex3f(x + 0.5f, y + h + 0.5f, 1.0f);
    glEnd();
}

void draw_rect_fill(int x, int y, int w, int h, colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_QUADS);
    glVertex3f(x + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + h + 0.5f, 1.0f);
    glVertex3f(x + 0.5f, y + h + 0.5f, 1.0f);
    glEnd();
}

/*
   Key table?
*/
ui_event_t keys[94];


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

static ui_event_t convert_event(SDL_Event *event)
{
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
            return UI_EVENT_EXTRA1;
        case 3:
            return UI_EVENT_EXTRA2;
        case 4:
            return UI_EVENT_EXTRA3;
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
static void draw_texture( texture_t *texture, float xpos,
                          float ypos, float width, float height, float zpos,
                          colour_t *col )
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

void text_draw_string( float xpos, float ypos, unsigned char *text, float scale, colour_t *col );
void text_draw_string_right( float xpos, float ypos, unsigned char *text, float scale, colour_t *col );
void text_draw_string_bouncy( float xpos, float ypos, unsigned char *text, float scale, colour_t *col );
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
#define GUI_PIECE_ROOK     1
#define GUI_PIECE_BISHOP   2
#define GUI_PIECE_KNIGHT   3
#define GUI_PIECE_QUEEN    4
#define GUI_PIECE_KING     5
#define GUI_PIECE_AVATAR   6


static texture_t white_pieces[7];
static texture_t black_pieces[7];

static texture_t text_characters[256];

static config_t *config;

#define FOCUS_NONE 0
#define FOCUS_ONE 1
#define FOCUS_ALL 2

typedef int w_class_id;
#define CLASS_ID_NONE -1

#define CAST_ERROR(C) ((C *) cast_error(__FILE__, __LINE__, #C))

#define CHECK_CAST(W, I, C) ((w_check_cast((w_widget_t *) W, I)) ? (C *) W \
    : CAST_ERROR(C))

#define CHILD(C) \
    static w_class_id class_id = CLASS_ID_NONE;\
    if (class_id == CLASS_ID_NONE) \
        class_id = w_register_class(C); \
    return class_id;

#define W_WIDGET(W) CHECK_CAST(W, w_widget_get_class_id(), w_widget_t)

#define W_WIDGET_DATA \
    void (* render) (struct widget *widget, int x, int y, int focus); \
    int (* input) (struct widget *widget, ui_event_t event); \
    void (* set_size) (struct widget *widget, int width, int height); \
    void (* get_requested_size) (struct widget *widget, int *width, int *height); \
    void (* get_focus_pos) (struct widget *widget, int *x, int *y); \
    void (* set_focus_pos) (struct widget *widget, int x, int y); \
    void (* destroy) (struct widget *widget); \
    w_class_id id; \
    void *data; \
    int enabled; \
    int width; \
    int height; \
    int width_f; \
    int height_f; \
    int width_a; \
    int height_a;

typedef struct widget
{
    W_WIDGET_DATA
}
widget_t;

typedef widget_t w_widget_t;

static int classes = 0;
static w_class_id *parent_class = NULL;

w_widget_t *cast_error(char *file, int line, char *type)
{
    fprintf(stderr, "Fatal error (%s:L%d): Widget is not of type %s.\n", file, line, type);
    exit(1);
    return NULL;
}

w_class_id w_register_class(w_class_id parent)
{
    parent_class = realloc(parent_class, (classes + 1) * sizeof(w_class_id));

    parent_class[classes] = parent;

    return classes++;
}

w_class_id w_widget_get_class_id()
{
    CHILD(CLASS_ID_NONE)
}

int w_check_cast(w_widget_t *widget, w_class_id id)
{
    w_class_id parent = parent_class[widget->id];

    if (widget->id == id)
        return 1;

    while ((parent != CLASS_ID_NONE) && (parent != id))
        parent = parent_class[parent];

    return parent != CLASS_ID_NONE;
}

void w_widget_destroy(widget_t *widget)
{
    free(widget);
}

static void w_widget_get_requested_size(widget_t *widget, int *width, int *height)
{
    if (width)
    {
        if (widget->width_f > widget->width)
            *width = widget->width_f;
        else
            *width = widget->width;
    }

    if (height)
    {
        if (widget->height_f > widget->height)
            *height = widget->height_f;
        else
            *height = widget->height;
    }
}

static void w_set_size(widget_t *widget, int width, int height)
{
    widget->width_a = width;
    widget->height_a = height;
}

static void w_get_focus_pos(widget_t *widget, int *x, int *y)
{
    *x = widget->width_a / 2;
    *y = widget->height_a / 2;
}

static void w_set_focus_pos(widget_t *widget, int x, int y)
{}

static void w_widget_init(widget_t *widget)
{
    widget->render = NULL;
    widget->input = NULL;
    widget->destroy = w_widget_destroy;
    widget->get_requested_size = w_widget_get_requested_size;
    widget->set_size = w_set_size;
    widget->get_focus_pos = w_get_focus_pos;
    widget->set_focus_pos = w_set_focus_pos;
    widget->id = w_widget_get_class_id();
    widget->enabled = 0;
    widget->width = widget->height = 0;
    widget->width_f = widget->height_f = -1;
    widget->width_a = widget->height_a = 0;
}

typedef struct list
{
    /** Total number of items in the list. */
    int items;

    /** The items in the list. */
    void **item;
}
list_t;

static list_t *list_create()
{
    list_t *list = malloc(sizeof(list_t));

    list->items = 0;
    list->item = NULL;

	return list;
}

static void list_append_item(list_t *list, void *item)
{
    list->item = realloc(list->item, (list->items + 1) * sizeof(void *));
    list->item[list->items++] = item;
}

static int list_get_size(list_t *list)
{
    return list->items;
}

static void *list_get_item(list_t *list, int index)
{
    if (index >= list->items)
        return NULL;

    return list->item[index];
}

static void list_destroy(list_t *list)
{
    free(list);
}

typedef struct widget_list
{
    /** Total number of widgets in the dialog box. */
    int nr;

    /** Currently selected widget. -1 = no widget selected. */
    int sel;

    /** The widgets in the list. */
    widget_t **item;
}
widget_list_t;

/** @brief Moves the box selector backwards to the next enabled widget.
 *
 *  If no enabled widget is found, the selector is not moved.
 *
 *  @param box The box to update.
 */
static int widget_list_select_prev(widget_list_t *list, int input, int enabled)
{
    int sel = list->sel - 1;

    while ((sel >= 0) && ((enabled && !list->item[sel]->enabled) || (input && !list->item[sel]->input)))
        sel--;

    if (sel >= 0)
    {
        list->sel = sel;
        return 1;
    }

    return 0;
}

/** @brief Moves the box selector forwards to the next enabled widget.
 *
 *  If no enabled widget is found, the selector is not moved.
 *
 *  @param box The box to update.
 */
static int widget_list_select_next(widget_list_t *list, int input, int enabled)
{
    int sel = list->sel + 1;

    while ((sel < list->nr) && ((enabled && !list->item[sel]->enabled) || (input && !list->item[sel]->input)))
        sel++;

    if (sel < list->nr)
    {
        list->sel = sel;
        return 1;
    }

    return 0;
}

static void widget_list_append(widget_list_t *list, widget_t *widget)
{
    list->item = realloc(list->item, (list->nr + 1) * sizeof(widget_t *));
    list->item[list->nr++] = widget;
}

static void widget_list_destroy(widget_list_t *list)
{
    int i;

    for (i = 0; i < list->nr; i++)
        list->item[i]->destroy(list->item[i]);

    if (list->item)
        free(list->item);
}

static void widget_list_set_size(widget_list_t *list, int width, int height)
{
    int i;

    for (i = 0; i < list->nr; i++)
        list->item[i]->set_size(list->item[i], width, height);
}

/** Dialog box. */
typedef struct dialog
{
    /** Width in pixels. */
    int width;

    /** Height in pixels. */
    int height;

    /** Whether or not dialog is escapable. 0 = yes, 1 = no. */
    int modal;

    /** The widget. */
    widget_t *widget;
}
dialog_t;

void dialog_destroy(dialog_t *dialog);

/** The maximum amount of open dialogs that the dialog system can handle. */
#define DIALOG_MAX 10

/** The dialog stack. */
dialog_t *dialog_stack[DIALOG_MAX];

/** The amount of dialogs that are currently open. */
int dialog_nr = 0;

/** To-be-destroyed dialogs. */
dialog_t *dialog_closed[DIALOG_MAX];

/** The amount of dialogs that need to be destroyed. */
int dialog_closed_nr = 0;

static void dialog_cleanup()
{
    int i;

    for (i = 0; i < dialog_closed_nr; i++)
    {
        dialog_t *dialog = dialog_closed[i];
        dialog_destroy(dialog);
    }

    dialog_closed_nr = 0;
}

/** @brief Adds a dialog to the top of the dialog stack.
 *  @param menu The dialog to add.
 */
static void dialog_open(dialog_t *menu)
{
    if (dialog_nr == DIALOG_MAX)
    {
        printf("Too many open dialogs.\n");
        return;
    }

    dialog_stack[dialog_nr++] = menu;
}

/** @brief Closes the dialog that's on top of the dialog stack. */
static void dialog_close()
{
    dialog_t *menu;

    if (dialog_nr == 0)
    {
        printf("No open dialogs.\n");
        return;
    }

    menu = dialog_stack[dialog_nr-- - 1];

    if (dialog_closed_nr == DIALOG_MAX)
    {
        printf("Too many to-be-destroyed dialogs.\n");
        return;
    }

    dialog_closed[dialog_closed_nr++] = menu;
}

/** @brief Returns the dialog that's on top of the stack.
 *
 *  @return The dialog that's on top of the stack, or NULL if the stack is
 *          empty.
 */
static dialog_t *dialog_current()
{
    if (dialog_nr == 0)
        return NULL;

    return dialog_stack[dialog_nr - 1];
}

/** Amount of pixels that widgets containing a label will add to the
 *  x-coordinate to determine the horizontal position of the actual widget.
 */
#define TAB 100

/** Dialog box style. */
typedef struct style
{
    char border_textured;

    /** Border size in pixels. */
    int border;

    /** Horizontal padding in pixels. This is the area between the border
     *  and the widgets.
     */
    int hor_pad;

    /** Vertical padding in pixels. This is the area between the border
     *  and the widgets.
     */
    int vert_pad;

    /** Colour of the quad that will be drawn the size of the whole screen.
     */
    colour_t fade_col;

    /** Border colour. */
    colour_t border_col;

    /** Background colour inside the dialog. */
    colour_t bg_col;
}
style_t;

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ALIGN_CENTER 2
#define ALIGN_TOP 3
#define ALIGN_BOTTOM 4

/** Dialog box position. */
typedef struct position
{
    /** x-coordinate in pixels. */
    int x;

    /** y-coordinate in pixels. */
    int y;

    /** Alignment of dialog in relation to x-coordinate. */
    int x_align;

    /** Alignment of dialog in relation to y-coordinate. */
    int y_align;
}
position_t;

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

/** Position used for the title menu. */
static position_t pos_title =
    {
        320, 100,
        ALIGN_CENTER, ALIGN_CENTER
    };

static void w_set_requested_size(widget_t *widget, int width, int height)
{
    widget->width_f = width;
    widget->height_f = height;
}

#define W_ALIGNABLE(W) CHECK_CAST(W, w_alignable_get_class_id(), w_alignable_t)

#define W_ALIGNABLE_DATA \
    W_WIDGET_DATA \
    float xalign; \
    float yalign;

typedef struct w_alignable
{
    W_ALIGNABLE_DATA
}
w_alignable_t;

w_class_id w_alignable_get_class_id()
{
    CHILD(w_widget_get_class_id())
}

void w_alignable_init(w_alignable_t *alignable)
{
    w_widget_init((w_widget_t *) alignable);

    alignable->id = w_alignable_get_class_id();
    alignable->xalign = 0.0f;
    alignable->yalign = 0.0f;
}

static void w_alignable_set_alignment(w_alignable_t *alignable, float xalign, float yalign)
{
    alignable->xalign = xalign;
    alignable->yalign = yalign;
}

/* Text widget. */

#define W_LABEL(W) CHECK_CAST(W, w_label_get_class_id(), w_label_t)

#define W_LABEL_DATA \
    W_ALIGNABLE_DATA \
    char *label; \
    int bouncy;

/** Text widget state. */
typedef struct w_label
{
    W_LABEL_DATA
}
w_label_t;

w_class_id w_label_get_class_id()
{
    CHILD(w_alignable_get_class_id())
}

/** Implements widget::render for text widgets. */
static void w_label_render(widget_t *widget, int x, int y, int focus)
{
    w_label_t *label = W_LABEL(widget);

    x += label->xalign * (label->width_a - label->width);
    y += (1.0f - label->yalign) * (label->height_a - label->height);

    if (focus != FOCUS_NONE)
    {
        if (label->bouncy)
            text_draw_string_bouncy(x, y, label->label, 1, &col_dark_red);
        else
            text_draw_string(x, y, label->label, 1, &col_dark_red);
    }
    else
        text_draw_string(x, y, label->label, 1, &col_black);
}

static void w_label_set_bouncy(w_label_t *label, int bouncy)
{
    label->bouncy = bouncy;
}

/** @brief Destroys a text widget.
 *
 *  @param widget The text widget.
 */
void w_label_destroy(widget_t *widget)
{
    w_label_t *label = W_LABEL(widget);

    if (label->label)
        free(label->label);

    w_widget_destroy(widget);
}

void w_label_init(w_label_t *label, char *text)
{
    w_alignable_init((w_alignable_t *) label);

    label->render = w_label_render;
    label->destroy = w_label_destroy;
    label->id = w_label_get_class_id();
    label->label = strdup(text);
    label->bouncy = 0;
    label->width = text_width(text);
    label->height = text_height() + BOUNCE_AMP;
}

/** @brief Creates a text widget.
 *
 *  A text widget consists of a single label. This widget has no input
 *  functionality.
 *
 *  @param string The text for the widget.
 *  @return The created text widget.
 */
w_widget_t *w_label_create(char *string)
{
    w_label_t *label = malloc(sizeof(w_label_t));

    w_label_init(label, string);

    return W_WIDGET(label);
}


/* Image widget. */

#define W_IMAGE(W) CHECK_CAST(W, w_image_get_class_id(), w_image_t)

#define W_IMAGE_DATA \
    W_ALIGNABLE_DATA \
    texture_t *image;

/** Image widget state. */
typedef struct w_image
{
    W_IMAGE_DATA
}
w_image_t;

w_class_id w_image_get_class_id()
{
    CHILD(w_alignable_get_class_id())
}

/** Implements widget::render for image widgets. */
static void w_image_render(w_widget_t *widget, int x, int y, int focus)
{
    w_image_t *image = W_IMAGE(widget);
    int w = image->width;
    int h = image->height;
    Uint32 ticks = SDL_GetTicks();
    float phase = ((ticks % (int) (1000 / IMAGE_SPEED)) / (float) (1000 / IMAGE_SPEED));
    float factor;

    if (phase < 0.5f)
        factor = 1.0f + IMAGE_SCALE * phase * 2;
    else
        factor = 1.0f + IMAGE_SCALE * ((1.0f - phase) * 2);

    x += image->xalign * (image->width_a - image->width);
    y += (1.0f - image->yalign) * (image->height_a - image->height);


    if (focus != FOCUS_NONE)
    {
        w *= factor;
        h *= factor;
    }

    draw_texture(image->image, x - (w - image->width)/2, y - (h - image->height)/2, w, h, 1.0f, &col_white);
}

void w_image_init(w_image_t *image, texture_t *texture)
{
    w_alignable_init((w_alignable_t *) image);

    image->render = w_image_render;
    image->id = w_image_get_class_id();
    image->image = texture;
    image->enabled = 1;
    image->width = texture->width;
    image->height = texture->height;
}

/** @brief Creates an image widget. */
w_widget_t *w_image_create(texture_t *texture)
{
    w_image_t *image = malloc(sizeof(w_image_t));

    w_image_init(image, texture);

    return W_WIDGET(image);
}


/* Action widget. */

#define W_ACTION(W) CHECK_CAST(W, w_action_get_class_id(), w_action_t)

#define W_ACTION_DATA \
    W_WIDGET_DATA \
    widget_t *child; \
    void (* func) (widget_t *widget, void *data); \
    void *func_data;

/** Action widget state. */
typedef struct w_action
{
    W_ACTION_DATA
}
w_action_t;

w_class_id w_action_get_class_id()
{
    CHILD(w_widget_get_class_id())
}

/** Implements widget::render for action widgets. */
static void w_action_render(w_widget_t *widget, int x, int y, int focus)
{
    w_action_t *action = W_ACTION(widget);

    if (focus != FOCUS_NONE)
        focus = FOCUS_ALL;

    action->child->render(action->child, x, y, focus);
}

/** Implements widget::input for action widgets. */
static int w_action_input(w_widget_t *widget, ui_event_t event)
{
    w_action_t *action = W_ACTION(widget);

    if (event == UI_EVENT_ACTION)
    {
        if (action->func)
            action->func(widget, action->func_data);
        return 1;
    }

    return 0;
}

void w_action_set_size(w_widget_t *widget, int width, int height)
{
    w_action_t *action = W_ACTION(widget);

    if (action->child)
        action->child->set_size(action->child, width, height);

    w_set_size(widget, width, height);
}

/** @brief Destroys an action widget.
 *
 *  @param widget The action widget.
 */
void w_action_destroy(w_widget_t *widget)
{
    w_action_t *action = W_ACTION(widget);

    if (action->child)
        action->child->destroy(action->child);

    w_widget_destroy(widget);
}

void w_action_init(w_action_t *action, w_widget_t *widget)
{
    w_widget_init((w_widget_t *) action);

    action->render = w_action_render;
    action->input = w_action_input;
    action->destroy = w_action_destroy;
    action->set_size = w_action_set_size;
    action->id = w_action_get_class_id();
    action->child = widget;
    action->func = NULL;
    action->func_data = NULL;
    action->enabled = 1;
    action->width = widget->width; /* FIXME */
    action->height = widget->height; /* FIXME */
}

/** @brief Creates an action widget.
 *
 *  An action widget consists of a single label. When the widget is activated
 *  a function is executed.
 *
 *  @param string The text for the widget.
 *  @return The created action widget.
 */
w_widget_t *w_action_create(w_widget_t *widget)
{
    w_action_t *action = malloc(sizeof(w_action_t));

    w_action_init(action, widget);

    return W_WIDGET(action);
}

w_widget_t *w_action_create_with_label(char *text, float xalign, float yalign)
{
    w_widget_t *label = w_label_create(text);
    w_widget_t *action;

    w_label_set_bouncy(W_LABEL(label), 1);
    w_alignable_set_alignment(W_ALIGNABLE(label), xalign, yalign);
    action = w_action_create(label);
    return action;
}

/** @brief Sets action widget callback.
 *
 *  @param widget The action widget.
 *  @param callback Function that should be called when widget is activated.
 */
void w_action_set_callback(w_action_t *action, void (* callback) (w_widget_t *, void *), void *func_data)
{
    action->func = callback;
    action->func_data = func_data;
}


/* Option widget. */

#define W_OPTION(W) CHECK_CAST(W, w_option_get_class_id(), w_option_t)

#define W_OPTION_DATA \
    W_WIDGET_DATA \
    widget_list_t list; \
    void (* func) (widget_t *widget, void *data); \
    void *func_data;

/** Option widget state. */
typedef struct w_option
{
    W_OPTION_DATA
}
w_option_t;

w_class_id w_option_get_class_id()
{
    CHILD(w_widget_get_class_id())
}

#define OPTION_ARROW_LEFT "\253 "
#define OPTION_ARROW_RIGHT " \273"

/** Implements widget::render for option widgets. */
static void w_option_render(w_widget_t *widget, int x, int y, int focus)
{
    w_option_t *option = W_OPTION(widget);
    widget_list_t *list = &option->list;
    w_widget_t *child;
    int xx, yy;
    int border_l = text_width(OPTION_ARROW_LEFT);
    int border_r = text_width(OPTION_ARROW_RIGHT);
    yy = y + option->height_a / 2 - text_height() / 2;

    if (list->sel == -1)
        return;
    if (list->sel > 0)
    {
        if (focus != FOCUS_NONE)
            text_draw_string_bouncy(x, yy, OPTION_ARROW_LEFT, 1, &col_dark_red);
        else
            text_draw_string(x, yy, OPTION_ARROW_LEFT, 1, &col_black);
    }
    else
        text_draw_string(x, yy, OPTION_ARROW_LEFT, 1, &col_grey);
    xx = x + border_l;

    child = list->item[list->sel];
    child->render(child, xx, y, focus);
    xx = x + option->width_a - border_r;
    if (list->sel < list->nr - 1)
    {
        if (focus != FOCUS_NONE)
            text_draw_string_bouncy(xx, yy, OPTION_ARROW_RIGHT, 1, &col_dark_red);
        else
            text_draw_string(xx, yy, OPTION_ARROW_RIGHT, 1, &col_black);
    }
    else
        text_draw_string(xx, yy, OPTION_ARROW_RIGHT, 1, &col_grey);
}

/** Implements widget::input for option widgets. */
static int w_option_input(w_widget_t *widget, ui_event_t event)
{
    w_option_t *option = W_OPTION(widget);
    widget_list_t *list = &option->list;

    if (list->sel == -1)
        return 0;

    if (event == UI_EVENT_RIGHT)
    {
        if (widget_list_select_next(list, 0, 0))
        {
            if (option->func)
                option->func(widget, option->func_data);
        }

        return 1;
    }
    if (event == UI_EVENT_LEFT)
    {
        if (widget_list_select_prev(list, 0, 0))
        {
            if (option->func)
                option->func(widget, option->func_data);
        }

        return 1;
    }

    return 0;
}

void w_option_set_size(w_widget_t *widget, int width, int height)
{
    w_option_t *option = W_OPTION(widget);
    widget_list_t *list = &option->list;
    int border_l = text_width(OPTION_ARROW_LEFT);
    int border_r = text_width(OPTION_ARROW_RIGHT);
    int i;

    for (i = 0; i < list->nr; i++)
        list->item[i]->set_size(list->item[i], width - border_l - border_r,
                                height);

    w_set_size(widget, width, height);
}

/** @brief Destroys an option widget.
 *
 *  @param widget The option widget.
 */
void w_option_destroy(widget_t *widget)
{
    w_option_t *option = W_OPTION(widget);

    widget_list_destroy(&option->list);
    w_widget_destroy(widget);
}

void w_option_init(w_option_t *option)
{
    w_widget_init((w_widget_t *) option);

    option->render = w_option_render;
    option->input = w_option_input;
    option->destroy = w_option_destroy;
    option->set_size = w_option_set_size;
    option->id = w_option_get_class_id();
    option->list.item = NULL;
    option->list.nr = 0;
    option->list.sel = -1;
}

/** @brief Creates an option widget.
 *
 *  An option widget consists of a label and a set of options. The label is
 *  rendered to the left of the currently selected option. The input handler
 *  allows for cycling through the available options.
 *
 *  @return The created widget.
 */
widget_t *w_option_create()
{
    w_option_t *option = malloc(sizeof(w_option_t));

    w_option_init(option);

    return W_WIDGET(option);
}

/** @brief Appends an option to the option widget's option list.
 *
 *  @param widget The option widget.
 *  @param string The option to append.
 */
void w_option_append(w_option_t *option, widget_t *child)
{
    widget_list_t *list = &option->list;
    int width, child_height;
    int height = text_height();

    child->get_requested_size(child, &width, &child_height);

    width += text_width(OPTION_ARROW_LEFT) + text_width(OPTION_ARROW_RIGHT);

    if (child_height > height)
        height = child_height;

    widget_list_append(list, child);

    if (width > option->width)
        option->width = width;

    if (height > option->height)
        option->height = height;

    if (list->nr == 2)
        option->enabled = 1;

    if (list->sel == -1)
        list->sel = 0;
}

void w_option_append_label(w_option_t *option, char *text, float xalign, float yalign)
{
    w_widget_t *label = w_label_create(text);
    w_alignable_set_alignment(W_ALIGNABLE(label), xalign, yalign);
    w_option_append(option, label);
}

/** @brief Returns the index of the selected option of an option widget.
 *
 *  @param widget The option widget.
 *  @return Index of the selected option.
 */
int w_option_get_selected(w_option_t *option)
{
    return option->list.sel;
}

/** @brief Sets option widget callback.
 *
 *  @param widget The option widget.
 *  @param callback Function that should be called when an option is
 *                  selected.
 */
void w_option_set_callback(w_option_t *option, void (* callback) (w_widget_t *, void *), void *func_data)
{
    option->func = callback;
    option->func_data = func_data;
}


/* Container class. */

#define W_CONTAINER(W) CHECK_CAST(W, w_container_get_class_id(), w_container_t)

#define W_CONTAINER_DATA \
    W_WIDGET_DATA \
    list_t *widget_list;

typedef struct w_container
{
    W_CONTAINER_DATA
}
w_container_t;

static w_class_id w_container_get_class_id()
{
    CHILD(w_widget_get_class_id())
}

static void w_container_destroy(w_widget_t *widget)
{
    w_container_t *container = W_CONTAINER(widget);
    int i;

    for (i = 0; i < list_get_size(container->widget_list); i++)
    {
         w_widget_t *item = W_WIDGET(list_get_item(container->widget_list, i));
         item->destroy(item);
    }

    list_destroy(container->widget_list);
    w_widget_destroy(widget);
}

static void w_container_init(w_container_t *container)
{
    w_widget_init((w_widget_t *) container);

    container->destroy = w_container_destroy;
    container->id = w_container_get_class_id();
    container->widget_list = list_create();
}

static void w_container_append(w_container_t *container, widget_t *widget)
{
    list_append_item(container->widget_list, widget);
}

static int w_container_get_size(w_container_t *container)
{
    return list_get_size(container->widget_list);
}

static widget_t *w_container_get_child(w_container_t *container, int index)
{
    return list_get_item(container->widget_list, index);
}

/* Vertical box widget. */

#define W_BOX(W) CHECK_CAST(W, w_box_get_class_id(), w_box_t)

#define W_BOX_DATA \
    W_CONTAINER_DATA \
    int sel; \
    int spacing;

typedef struct w_box
{
    W_BOX_DATA
}
w_box_t;

w_class_id w_box_get_class_id()
{
    CHILD(w_container_get_class_id())
}

static void w_box_init(w_box_t *box, int spacing)
{
    w_container_init((w_container_t *) box);

    box->id = w_box_get_class_id();
    box->spacing = spacing;
    box->sel = -1;
}

static int w_box_select_prev(w_box_t *box, int input, int enabled)
{
    int sel = box->sel - 1;

    while (sel >= 0)
    {
        widget_t *child = w_container_get_child(W_CONTAINER(box), sel);

        if ((enabled && !child->enabled)
            || (input && !child->input))
            sel--;
        else
            break;
    }

    if (sel >= 0)
    {
        box->sel = sel;
        return 1;
    }

    return 0;
}

static int w_box_select_next(w_box_t *box, int input, int enabled)
{
    int sel = box->sel + 1;
    int size = w_container_get_size(W_CONTAINER(box));

    while (sel < size)
    {
        widget_t *child = w_container_get_child(W_CONTAINER(box), sel);

        if ((enabled && !child->enabled)
            || (input && !child->input))
            sel++;
        else
            break;
    }

    if (sel < size)
    {
        box->sel = sel;
        return 1;
    }

    return 0;
}

/* Vertical box widget. */

void w_vbox_render(w_widget_t *widget, int x, int y, int focus)
{
    w_box_t *box = W_BOX(widget);
    int nr = list_get_size(box->widget_list);

    y += box->height_a - box->height;

    while (--nr >= 0)
    {
        int focus_child;
        widget_t *child = w_container_get_child(W_CONTAINER(widget), nr);

        if (focus == FOCUS_ALL)
            focus_child = FOCUS_ALL;
        else if (focus == FOCUS_ONE)
            focus_child = (box->sel == nr ? FOCUS_ONE : FOCUS_NONE);
        else
            focus_child = 0;

        child->render(child, x, y, focus_child);
        y += child->height_a;
        y += box->spacing;
    }
}

static int w_vbox_input(w_widget_t *widget, ui_event_t event)
{
    w_box_t *box = W_BOX(widget);
    widget_t *child;
    int retval = 0, x, y;

    if (box->sel == -1)
        return 0;

    child = list_get_item(box->widget_list, box->sel);

    if (child->input(child, event))
        return 1;

    child->get_focus_pos(child, &x, &y);

    if (event == UI_EVENT_UP)
    {
        retval = w_box_select_prev(box, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), box->sel);
        y = 0;
    }

    if (event == UI_EVENT_DOWN)
    {
        retval = w_box_select_next(box, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), box->sel);
        y = child->height_a - 1;
    }

    if (retval)
    {
        child->set_focus_pos(child, x, y);
        return retval;
    }

    return 0;
}

static void w_vbox_get_requested_size(widget_t *widget, int *width, int *height)
{
    w_container_t *container = W_CONTAINER(widget);
    w_box_t *box = W_BOX(widget);
    int size = w_container_get_size(container);
    int i;

	int nr = w_container_get_size(container);

    widget->width = 0;
    widget->height = (size - 1) * box->spacing;
    widget->enabled = 0;

    for (i = 0; i < size; i++)
    {
        int child_width, child_height;
        widget_t *child = w_container_get_child(container, i);

        child->get_requested_size(child, &child_width, &child_height);

	    if (child_width > widget->width)
    	    widget->width = child_width;

	    widget->height += child_height;

	    if (child->enabled && child->input)
        {
   	        if (box->sel == -1)
   	            box->sel = i;
   	        widget->enabled = 1;
  	    }
    }

    w_widget_get_requested_size(widget, width, height);
}

static void w_vbox_set_size(w_widget_t *widget, int width, int height)
{
    w_box_t *box = W_BOX(widget);
    int i;

    for (i = 0; i < w_container_get_size(W_CONTAINER(widget)); i++)
    {
        widget_t *child = w_container_get_child(W_CONTAINER(widget), i);
        int item_height;

        child->get_requested_size(child, NULL, &item_height);
        child->set_size(child, width, item_height);
    }

    w_set_size(widget, width, height);
}

static void w_vbox_get_focus_pos(w_widget_t *widget, int *x , int *y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    int size = w_container_get_size(container);
    w_widget_t *child;
    int nr = w_container_get_size(container) - 1;

    assert(box->sel != -1);

    child = w_container_get_child(container, box->sel);
    child->get_focus_pos(child, x, y);

    while (nr > box->sel)
    {
        w_widget_t *sibling = w_container_get_child(container, nr);
        *y += sibling->height_a;
        nr--;
    }

    *y += (size - box->sel - 1) * box->spacing;
}

static void w_vbox_set_focus_pos(widget_t *widget, int x , int y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    int size = w_container_get_size(container);
    int cur_y = box->height_a - box->height;

    box->sel = size;

    while (w_box_select_prev(box, 1, 1))
    {
        widget_t *child = w_container_get_child(container, box->sel);

        cur_y += child->height_a;
        if (cur_y >= y)
            break;
        cur_y += box->spacing;
    }

    assert(box->sel != size);
}

void w_vbox_init(w_box_t *box, int spacing)
{
    w_box_init(box, spacing);

    box->render = w_vbox_render;
    box->input = w_vbox_input;
    box->get_requested_size = w_vbox_get_requested_size;
    box->set_size = w_vbox_set_size;
    box->get_focus_pos = w_vbox_get_focus_pos;
    box->set_focus_pos = w_vbox_set_focus_pos;
}

/** @brief Creates a vertical box widget.
 *
 *  A vertical box widget contains other widgets.
 *
 *  @return The created widget.
 */
w_widget_t *w_vbox_create(int spacing)
{
    w_box_t *box = malloc(sizeof(w_box_t));

    w_vbox_init(box, spacing);

    return W_WIDGET(box);
}

/* Horizontal box widget. */

void w_hbox_render(w_widget_t *widget, int x, int y, int focus)
{
    w_box_t *box = W_BOX(widget);
    int nr = 0;

    while (nr < list_get_size(box->widget_list))
    {
        int focus_child;
        widget_t *child = w_container_get_child(W_CONTAINER(widget), nr);

        if (focus == FOCUS_ALL)
            focus_child = FOCUS_ALL;
        else if (focus == FOCUS_ONE)
            focus_child = (box->sel == nr ? FOCUS_ONE : FOCUS_NONE);
        else
            focus_child = 0;

        child->render(child, x, y, focus_child);
        x += child->width_a;
        x += box->spacing;
        nr++;
    }
}

static int w_hbox_input(widget_t *widget, ui_event_t event)
{
    w_box_t *box = W_BOX(widget);
    widget_t *child;
    int retval = 0, x, y;

    if (box->sel == -1)
        return 0;

    child = w_container_get_child(W_CONTAINER(widget), box->sel);

    if (child->input(child, event))
        return 1;

    child->get_focus_pos(child, &x, &y);

    if (event == UI_EVENT_LEFT)
    {
        retval = w_box_select_prev(box, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), box->sel);
        x = child->width_a - 1;
    }

    if (event == UI_EVENT_RIGHT)
    {
        retval = w_box_select_next(box, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), box->sel);
        x = 0;
    }

    if (retval)
    {
        child->set_focus_pos(child, x, y);
        return retval;
    }

    return 0;
}

static void w_hbox_get_requested_size(widget_t *widget, int *width, int *height)
{
    w_container_t *container = W_CONTAINER(widget);
    w_box_t *box = W_BOX(widget);
    int size = w_container_get_size(container);
    int i;

    widget->width = (size - 1) * box->spacing;
    widget->height = 0;
    widget->enabled = 0;

    for (i = 0; i < size; i++)
    {
        int child_width, child_height;
        widget_t *child = w_container_get_child(container, i);

        child->get_requested_size(child, &child_width, &child_height);

	    if (child_height > widget->height)
    	    widget->height = child_height;

	    widget->width += child_width;

	    if (child->enabled && child->input)
        {
   	        if (box->sel == -1)
   	            box->sel = i;
   	        widget->enabled = 1;
  	    }
    }

    w_widget_get_requested_size(widget, width, height);
}

static void w_hbox_set_size(w_widget_t *widget, int width, int height)
{
    w_box_t *box = W_BOX(widget);
    int i;

    for (i = 0; i < w_container_get_size(W_CONTAINER(widget)); i++)
    {
        widget_t *child = w_container_get_child(W_CONTAINER(widget), i);
        int item_width;

        child->get_requested_size(child, &item_width, NULL);
        child->set_size(child, item_width, height);
    }

    w_set_size(widget, width, height);
}

static void w_hbox_get_focus_pos(w_widget_t *widget, int *x , int *y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    w_widget_t *child;
    int nr = 0;

    assert(box->sel != -1);

    child = w_container_get_child(container, box->sel);
    child->get_focus_pos(child, x, y);

    while (nr < box->sel)
    {
        w_widget_t *sibling = w_container_get_child(container, nr);

        *x += child->width_a;
        nr++;
    }

    *x += box->sel * box->spacing;
}

static void w_hbox_set_focus_pos(w_widget_t *widget, int x , int y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    int cur_x = 0;

    box->sel = -1;

    while (w_box_select_next(box, 1, 1))
    {
        w_widget_t *child = w_container_get_child(container, box->sel);

        cur_x += child->width_a;
        if (cur_x >= x)
            break;
        cur_x += box->spacing;
    }

    assert (box->sel != -1);
}

void w_hbox_init(w_box_t *box, int spacing)
{
    w_box_init(box, spacing);

    box->render = w_hbox_render;
    box->input = w_hbox_input;
    box->get_requested_size = w_hbox_get_requested_size;
    box->set_size = w_hbox_set_size;
    box->get_focus_pos = w_hbox_get_focus_pos;
    box->set_focus_pos = w_hbox_set_focus_pos;
}

/** @brief Creates a horizontal box widget.
 *
 *  A horizontal box widget contains other widgets.
 *
 *  @return The created widget.
 */
widget_t *w_hbox_create(int spacing)
{
    w_box_t *box = malloc(sizeof(w_box_t));

    w_hbox_init(box, spacing);

    return W_WIDGET(box);
}

/* Text entry widget. */

#define ENTRY_SPACING 2
#define ENTRY_MAX_LEN 255
#define ENTRY_CURSOR "|"

#define W_ENTRY(W) CHECK_CAST(W, w_entry_get_class_id(), w_entry_t)

#define W_ENTRY_DATA \
    W_WIDGET_DATA \
    char text[ENTRY_MAX_LEN + 1]; \
    int max_len; \
    int cursor_pos; \
    int display_pos; \
    int display_len;

/** Text entry widget state. */
typedef struct w_entry
{
    W_ENTRY_DATA
}
w_entry_t;

w_class_id w_entry_get_class_id()
{
    CHILD(w_widget_get_class_id())
}

/** Implements widget::render for text entry widgets. */
void w_entry_render(w_widget_t *widget, int x, int y, int focus)
{
    w_entry_t *entry = W_ENTRY(widget);
    int len;

    len = text_width_n(entry->text, entry->cursor_pos);

    if (focus != FOCUS_NONE)
        draw_rect(x, y, entry->width_a, entry->height_a, &col_dark_red);
    else
        draw_rect(x, y, entry->width_a, entry->height_a, &col_black);

    x += ENTRY_SPACING;
    y += ENTRY_SPACING;

    if (focus != FOCUS_NONE)
    {
        int cursor_width = text_width(ENTRY_CURSOR);
        text_draw_string(x, y, entry->text, 1, &col_dark_red);
        if (SDL_GetTicks() % 400 < 200)
            text_draw_string(x + len - cursor_width / 2, y, ENTRY_CURSOR,1,
                             &col_dark_red);
    }
    else
        text_draw_string(x, y, entry->text, 1, &col_black);
}

/** Implements widget::input for text entry widgets. */
static int w_entry_input(widget_t *widget, ui_event_t event)
{
    w_entry_t *entry = W_ENTRY(widget);
    int c = -1;
    int len = strlen(entry->text);

    if (event == UI_EVENT_LEFT)
    {
        if (entry->cursor_pos > 0)
            entry->cursor_pos--;
        return 1;
    }

    if (event == UI_EVENT_RIGHT)
    {
        if (entry->cursor_pos < len)
            entry->cursor_pos++;
        return 1;
    }

    if (event == UI_EVENT_BACKSPACE)
    {
        if (entry->cursor_pos > 0)
        {
            int i;
            for (i = entry->cursor_pos; i <= len; i++)
                entry->text[i - 1] = entry->text[i];
            entry->cursor_pos--;
        }

        return 1;
    }

    if ((event > 0) && (event <= 255))
        c = event;
    else
        return 0;

    if (len < entry->max_len)
    {
        int i;
        for (i = len; i >= entry->cursor_pos; i--)
            entry->text[i + 1] = entry->text[i];
        entry->text[entry->cursor_pos++] = c;
    }

    return 1;
}

void w_entry_init(w_entry_t *entry)
{
    w_widget_init((w_widget_t *) entry);

    entry->render = w_entry_render;
    entry->input = w_entry_input;
    entry->id = w_entry_get_class_id();
    entry->max_len = ENTRY_MAX_LEN;
    entry->cursor_pos = 0;
    entry->text[0] = '\0';
    entry->enabled = 1;
    entry->width = text_width("Visible text") + ENTRY_SPACING * 2;
    entry->height = text_height() + ENTRY_SPACING * 2;
}

/** @brief Creates a text entry widget.
 *
 *  A text entry widget for a single line of text.
 *
 *  @return The created widget.
 */
w_widget_t *w_entry_create()
{
    w_entry_t *entry = malloc(sizeof(w_entry_t));

    w_entry_init(entry);

    return W_WIDGET(entry);
}

void dialog_set_modal(dialog_t *dialog, int modal)
{
    dialog->modal = modal;
}

/** @brief Creates a dialog.
 *
 *  @param widget The widget the dialog contains.
 *  @return The created dialog.
 */
dialog_t *dialog_create(widget_t *widget)
{
    dialog_t *dialog = malloc(sizeof(dialog_t));

    dialog->widget = widget;
    dialog->modal = 0;
    widget->get_requested_size(widget, &dialog->width, &dialog->height);

    widget->set_size(widget, dialog->width, dialog->height);

    return dialog;
}

void dialog_destroy(dialog_t *dialog)
{
    dialog->widget->destroy(dialog->widget);
    free(dialog);
}

void dialog_render_border_section( int section, float xmin, float ymin, float xmax, float ymax )
{
   switch ( section )
   {
      case 0: // Top left.
         draw_texture( &backdrop, xmin-16, ymax, 16, 16, 0.95f, &col_white );
         break;
      case 1: // Top right.
         draw_texture( &backdrop, xmax, ymax, 16, 16, 0.95f, &col_white );
         break;
      case 2: // Bottom left.
         draw_texture( &backdrop, xmin-16, ymin-16, 16, 16, 0.95f, &col_white );
         break;
      case 3: // Bottom right.
         draw_texture( &backdrop, xmax, ymin-16, 16, 16, 0.95f, &col_white );
         break;
      case 4: // Top
         /* Make tileable.. */
         draw_texture( &backdrop, xmin, ymax, xmax-xmin, 16, 0.95f, &col_white );
         break;
      case 5: // Bottom
         /* Make tileable.. */
         draw_texture( &backdrop, xmin, ymin-16, xmax-xmin, 16, 0.95f, &col_white );
         break;
      case 6: // Left
         /* Make tileable.. */
         draw_texture( &backdrop, xmin-16, ymin, 16, ymax-ymin, 0.95f, &col_white );
         break;
      case 7: // Right
         /* Make tileable.. */
         draw_texture( &backdrop, xmax, ymin, 16, ymax-ymin, 0.95f, &col_white );
         break;
   }
}

void dialog_render_border( float xmin, float ymin, float xmax, float ymax )
{
   int i=0;

   for ( i; i<8; i++ )
      dialog_render_border_section( i, xmin, ymin, xmax, ymax );   
}

/** @brief Renders a dialog.
 *
 *  Renders a dialog in a specific style and at a specific position.
 *
 *  @param menu The dialog to render.
 *  @param style The style to render in.
 *  @param pos The position to render at.
 */
static void dialog_render(dialog_t *menu, style_t *style, position_t *pos)
{
    int height, width;
    int total_height, total_width;
    int xmin, xmax, ymin, ymax;
    colour_t col;

    height = menu->height;
    width = menu->width;
    total_height = height + 2 * style->vert_pad + 2 * style->border;
    total_width = width + 2 * style->hor_pad + 2 * style->border;

    if (pos->x_align == ALIGN_LEFT)
        xmin = pos->x;
    else if (pos->x_align == ALIGN_RIGHT)
        xmin = pos->x - total_width;
    else
        xmin = pos->x - total_width / 2;

    if (pos->y_align == ALIGN_TOP)
        ymin = pos->y - total_height;
    else if (pos->y_align == ALIGN_BOTTOM)
        ymin = pos->y;
    else
        ymin = pos->y - total_height / 2;

    xmax = xmin + total_width;
    ymax = ymin + total_height;

    /* Draw the 'fade' */
    col = style->fade_col;
    glColor4f(col.r, col.b, col.g, col.a); /* 0.0f 0.0f 0.0f 0.5f */

    glBegin( GL_QUADS );
    glVertex3f( 640, 0, 0.9f );
    glVertex3f( 640, 480, 0.9f );
    glVertex3f( 0, 480, 0.9f );
    glVertex3f( 0, 0, 0.9f );
    glEnd( );

    if ( style->border_textured )
    {  
      dialog_render_border( xmin, ymin, xmax, ymax );
    }
    else
    {
      /* Draw the border. */
      col = style->border_col;
      glColor4f(col.r, col.g, col.b, col.a); /* 0.0f 0.0f 0.0f 1.0f */

      glBegin( GL_QUADS );
      glVertex3f(xmax, ymin, 0.9f);
      glVertex3f(xmax, ymax, 0.9f);
      glVertex3f(xmin, ymax, 0.9f);
      glVertex3f(xmin, ymin, 0.9f);
      glEnd();

      xmin += style->border;
      xmax -= style->border;
      ymin += style->border;
      ymax -= style->border;
    }  

    /* Draw the backdrop. */
    col = style->bg_col;
    glColor4f(col.r, col.g, col.b, col.a); /* 0.8f 0.8f 0.8f 1.0f */

    glBegin( GL_QUADS );
    glVertex3f(xmax, ymin, 0.95f);
    glVertex3f(xmax, ymax, 0.95f);
    glVertex3f(xmin, ymax, 0.95f);
    glVertex3f(xmin, ymin, 0.95f);
    glEnd();

    xmin += style->hor_pad;
    xmax -= style->hor_pad;
    ymin += style->vert_pad;
    ymax -= style->vert_pad;

    menu->widget->render(menu->widget, xmin, ymin, 1);
}

/** @brief Processes an input event for a specific dialog.
 *
 *  @param event The event to process.
 */
static void dialog_input(ui_event_t event)
{
    dialog_t *dialog = dialog_current();

    if (!dialog)
        return;

    if (!dialog->modal && (event == UI_EVENT_ESCAPE))
        dialog_close();

    dialog->widget->input(dialog->widget, event);
}

/* In-game dialog. */

/** The in-game dialog. Provides a set of gameplay-related actions to the
 *  user.
 */

static void retract_move(widget_t *widget, void *data)
{
    game_retract_move();
}

static void move_now(widget_t *widget, void *data)
{
    game_move_now();
}

static void view_prev(widget_t *widget, void *data)
{
    game_view_prev();
}

static void view_next(widget_t *widget, void *data)
{
    game_view_next();
}

/** @brief Creates the in-game dialog.
 *
 *  @return The created dialog.
 */
static dialog_t *dialog_ingame_create()
{
    dialog_t *dialog;
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

    dialog = dialog_create(vbox);
    return dialog;
}


/* Quit dialog. */

/** @brief Quits the current game.
 *
 *  Closes the dialog and causes the game to go back to the title menu.
 */
static void dialog_quit_ok(widget_t *widget, void *data)
{
    dialog_close();
    dialog_close();
    quit_to_menu = 1;
}

static void dialog_close_cb(widget_t *widget, void *data)
{
    dialog_close();
}

/** The quit dialog. Asks the user to confirm that he wants to quit the game.
 */

/** @brief Creates the quit confirmation dialog.
 *
 *  @return The created dialog.
 */
static dialog_t *dialog_quit_create()
{
    dialog_t *dialog;
    widget_t *vbox = w_vbox_create(0);

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

    dialog = dialog_create(vbox);
    return dialog;
}


/* System dialog. */

/** The system dialog. Provides a set of system-related actions to the user.
 *  Currently this dialog only contains an item to quit the game. In the
 *  future this will be extended with load/save game items and possibly
 *  other items as well.
 */

/** @brief Opens the quit dialog. */
static void dialog_quit_open(widget_t *widget, void *data)
{
    dialog_open(dialog_quit_create());
}

/** @brief Creates the system dialog.
 *
 *  @return The created dialog.
 */
static dialog_t *dialog_system_create()
{
    dialog_t *dialog;
    widget_t *vbox = w_vbox_create(0);
    w_widget_t *widget;

    widget = w_action_create_with_label("Return To Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), dialog_close_cb, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    widget = w_action_create_with_label("Quit Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), dialog_quit_open, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    dialog = dialog_create(vbox);
    return dialog;
}


/* Victory dialog. */

dialog_t *dialog_victory_create(result_t *result)
{
    dialog_t *dialog;
    widget_t *hbox = w_hbox_create(20);
    widget_t *vbox = w_vbox_create(0);
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
    dialog = dialog_create(hbox);
    dialog_set_modal(dialog, 1);
    return dialog;
}

/* Title dialog. */

#define GAME_TYPE_HUMAN_VS_CPU 0
#define GAME_TYPE_CPU_VS_HUMAN 1
#define GAME_TYPE_HUMAN_VS_HUMAN 2

/** @brief Triggers gameplay start based on currently selected options. */
static void menu_title_start(widget_t *widget, void *data)
{
    set_loading=TRUE;
    dialog_close();
}

/** @brief Triggers DreamChess exit. */
static void menu_title_quit(widget_t *widget, void *data)
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

static dialog_t *dialog_title_create()
{
    dialog_t *dialog;
    widget_t *vbox;
    w_widget_t *widget;
    widget_t *vbox2;
    widget_t *hbox;
    w_widget_t *label;
    int i;

    config = malloc(sizeof(config_t));
    config->player[WHITE] = PLAYER_UI;
    config->player[BLACK] = PLAYER_ENGINE;
    config->cpu_level = 1;
    cur_theme = 0;
    flip_board = 0;

    widget = w_action_create_with_label("Start Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), menu_title_start, NULL);
    vbox = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox), widget);

    label = w_label_create("Players:");
    w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
    vbox2 = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Difficulty:");
    w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Theme:");
    w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Chess Set:");
    w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Board:");
    w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

    label = w_label_create("Name:");
    w_alignable_set_alignment(W_ALIGNABLE(label), 0.0f, 0.0f);
    w_container_append(W_CONTAINER(vbox2), label);

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

    widget = w_entry_create();
    w_container_append(W_CONTAINER(vbox2), widget);

    w_container_append(W_CONTAINER(hbox), vbox2);
    w_container_append(W_CONTAINER(vbox), hbox);

    widget = w_action_create_with_label("Quit Game", 0.0f, 0.0f);
    w_action_set_callback(W_ACTION(widget), menu_title_quit, NULL);
    w_container_append(W_CONTAINER(vbox), widget);

    dialog = dialog_create(vbox);
    dialog_set_modal(dialog, 1);
    return dialog;
}

static void dialog_vkeyboard_key(widget_t *widget, void *data)
{
    if (dialog_current())
        dialog_input(*(ui_event_t *) data);

    printf( "Pressed a keyyy... it was uh.. '%c' .. right?\n\r", *(ui_event_t *)data);
}

static dialog_t *dialog_vkeyboard_create()
{
    dialog_t *dialog;
    w_widget_t *label;
    w_widget_t *action;
    widget_t *hbox;
    widget_t *vbox2;
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

    dialog = dialog_create(vbox2);
    return dialog;
}

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

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
    colour_t col_cap = {0.55f, 0.75f, 0.95f, 0.0f};
    colour_t col_item = {1.0f, 1.0f, 1.0f, 0.0f};

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

        text_draw_string_right(x, y, credits[section][0], 1, &col_cap);

        break;

    case 1:
        col_cap.a = 1.0f;
        text_draw_string_right(x, y, credits[section][0], 1, &col_cap);

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

        text_draw_string_right(x, y - 40, credits[section][nr], 1, &col_item);

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

        text_draw_string_right(x, y, credits[section][0], 1, &col_cap);

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
static texture_t SDL_GL_LoadTexture(SDL_Surface *surface, int alpha)
{
    texture_t texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect area;
    Uint32 saved_flags;
    Uint8  saved_alpha;
    GLfloat texcoord[4];

    /* Use the surface width and height expanded to powers of 2 */
    w = power_of_two(surface->w);
    h = power_of_two(surface->h);
    texcoord[0] = 0.0f;			/* Min X */
    texcoord[1] = 0.0f;			/* Min Y */
    texcoord[2] = (GLfloat)surface->w / w;	/* Max X */
    texcoord[3] = (GLfloat)surface->h / h;	/* Max Y */

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
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

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
    texture.u2 = surface->w / (float) w;
    texture.v2 = surface->h / (float) h;
    texture.width = surface->w;
    texture.height = surface->h;

    return texture;
}


static void draw_name_dialog( float xpos, float ypos, char* name, int left, int white )
{
    float width, height;

    width=100;
    height=30;

    /* draw avatar */
    if ( white == 1 )
        draw_texture( &white_pieces[GUI_PIECE_AVATAR], xpos-45, ypos-50, 100, 100, 0.8f, &col_white);
    else
        draw_texture( &black_pieces[GUI_PIECE_AVATAR], xpos+45, ypos-50, 100, 100, 0.8f, &col_white);

    /* Draw the text stuff */
    if (!left) /* UGLY */
        text_draw_string( xpos+10, ypos+5, name, 1, &col_black );
    else
        text_draw_string( xpos+width-10-(strlen(name)*8), ypos+5, name, 1, &col_black );
}

void dialog_promote_cb(widget_t *widget, void *data)
{
    dialog_promote_piece = *(int *)data;
    dialog_close();
}

dialog_t *dialog_promote_create(int colour)
{
    static int cb_pieces[4];

    texture_t *pieces;
    dialog_t *dialog;
    w_widget_t *action;
    widget_t *vbox = w_vbox_create(0);
    widget_t *hbox = w_hbox_create(0);
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

    dialog = dialog_create(vbox);
    dialog_set_modal(dialog, 1);
    return dialog;
}

dialog_t *dialog_message_create(char *message)
{
    dialog_t *dialog;
    w_widget_t *widget;

    widget_t *vbox = w_vbox_create(0);
    w_container_append(W_CONTAINER(vbox), w_label_create("Important message from engine"));
    w_container_append(W_CONTAINER(vbox), w_label_create(""));
    w_container_append(W_CONTAINER(vbox), w_label_create(message));
    w_container_append(W_CONTAINER(vbox), w_label_create(""));
    widget = w_action_create_with_label("Ok", 0.5f, 0.5f);
    w_action_set_callback(W_ACTION(widget), dialog_close_cb, NULL);
    w_container_append(W_CONTAINER(vbox), widget);
    dialog = dialog_create(vbox);
    dialog_set_modal(dialog, 1);

    return dialog;
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
        text_draw_string(10, 10, fps_s, 1, &col_red);
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
}

/** Implements ui_driver::menu */
static config_t *do_menu()
{
    dialog_t *keyboard = dialog_vkeyboard_create();
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
            ui_event_t ui_event = convert_event(&event);

            if (wait_menu)
            {
                if (ui_event != UI_EVENT_NONE)
                    wait_menu = 0;

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
                keyboard->widget->input(keyboard->widget, ui_event);
            else
                dialog_input(ui_event);

            if (title_process_retval == 1)
            {
                free(config);
                return NULL;
            }
        }

        /* Draw the menu.. */

        draw_texture( &menu_title_tex, 0, 0, 640, 480, 0.95f, &col_white );

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
                text_draw_string_bouncy( 140, 30, "Press any key or button to start", 1.5, &col_white );
            else
                dialog_render(dialog_current(), &style_title, &pos_title);

            if (vkeyboard_enabled)
                dialog_render(keyboard, &style_title, &pos_vkeyboard);

            draw_credits(0);
        }
        else
        {
            text_draw_string( 390, 30, "Loading...", 3, &col_white);
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
        *texture = SDL_GL_LoadTexture(texture_image, alpha);
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

/** Implements ui_driver::init. */
static void init_gui()
{
    int video_flags;
    const SDL_VideoInfo *video_info;
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
    draw_texture( &backdrop, 0, 0, 640, 480, 0.95f, &col_white );
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
static void draw_move_list( colour_t *col_normal, colour_t *col_high )
{
    char **list;
    int entries, view, i;
    int y;
    int start;
    float x_white = 30;
    float y_white = 360;
    float x_black = 610;
    float y_black = 360;

    game_get_move_list(&list, &entries, &view);

    if (!(view % 2))
        start = (view - 8 < 0 ? 0 : view - 8);
    else
        start = (view - 9 < 0 ? 0 : view - 9);

    y = y_white;
    for (i = start; i <= view; i += 2)
    {
        char s[11];
        if (snprintf(s, 11, "%i.%s", (i >> 1) + 1, list[i]) >= 11)
            exit(1);
        if (i != view)
            text_draw_string( x_white+5, y-5, s, 1, col_normal );
        else
            text_draw_string( x_white+5, y-5, s, 1, col_high );
        y -= text_height();
    }
    y = y_black;
    for (i = start + 1; i <= view; i += 2)
    {
        if (i != view)
            text_draw_string_right( x_black-5, y-5, list[i], 1, col_normal );
        else
            text_draw_string_right( x_black-5, y-5, list[i], 1, col_high );
        y -= text_height();
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
    draw_rect_fill( 80, 400, 200, 20, &col_yellow );
    draw_rect_fill( 640-80-200, 400, 200, 20, &col_yellow );

    draw_rect_fill( 80, 400, 200*white_health_percent, 20, &col_red );
    draw_rect_fill( 640-80-(200*black_health_percent), 400, 200*black_health_percent, 20, &col_red );

    draw_rect( 80, 400, 200, 20, &col_black );
    draw_rect( 640-80-200, 400, 200, 20, &col_black );
}

/** @brief Renders the list of captured pieces for both sides.
 *
 *  @param col The text colour to use.
 */
static void draw_capture_list(colour_t *col)
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
            text_draw_string( x_white, y_white, s, 1, col);
        }
        y_white -= text_characters['a'].height;
        if (board.captured[i - 1] != 0)
        {
            if (snprintf(s, 4, "%i", board.captured[i - 1]) >= 4)
                exit(1);
            text_draw_string_right( x_black, y_black, s, 1, col);
        }
        y_black -= text_characters['a'].height;
    }
}

/** Implements ui_driver::update. */
static void update(board_t *b, move_t *move)
{
    board = *b;

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
    dialog_t *menu;

    dialog_cleanup();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);

    draw_backdrop();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_move_list(&col_white, &col_yellow);
    draw_capture_list(&col_white);
    /* draw_captured_pieces( 480, 70 ); */
    draw_health_bars();

    draw_name_dialog( 50, 430, "White", TRUE, 1 );
    draw_name_dialog( 490, 430, "Black", FALSE, 0 );

    if ( white_in_check == TRUE )
        text_draw_string_bouncy( 180, 420, "White is in check!", 2, &col_white );
    else if ( black_in_check == TRUE )
        text_draw_string_bouncy( 180, 420, "Black is in check!", 2, &col_white );

    go_3d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glDepthMask(GL_TRUE);

    render_scene_3d(b);

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (dialog_current())
        dialog_render(dialog_current(), &style_ingame, &pos_ingame);

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
int text_draw_char( float xpos, float ypos, float scale, int character, colour_t *col )
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
void text_draw_string( float xpos, float ypos, unsigned char *text, float scale, colour_t *col )
{
    int i;
    int xposition=xpos;
    for ( i=0; i<strlen(text); i++ )
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
void text_draw_string_right( float xpos, float ypos, unsigned char *text, float scale, colour_t *col )
{
    text_draw_string(xpos - text_width(text), ypos, text, scale, col);
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
void text_draw_string_bouncy( float xpos, float ypos, unsigned char *text, float scale, colour_t *col )
{
    int i;
    int xposition=xpos;
    int yposition=ypos;
    Uint32 ticks = SDL_GetTicks();

    for ( i=0; i<strlen(text); i++ )
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
    source = -1;
    dest = -1;
    game_make_move(move);
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

    while ( SDL_PollEvent( &event ) )
    {
        ui_event_t ui_event = convert_event(&event);

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
                printf("Ignoring event %i\n", ui_event);
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
