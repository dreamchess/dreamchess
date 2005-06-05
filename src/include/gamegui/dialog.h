/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
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

#ifndef GAMEGUI_DIALOG_H
#define GAMEGUI_DIALOG_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

#define W_DIALOG(W) CHECK_CAST(W, w_dialog_get_class_id(), w_dialog_t)

#define W_DIALOG_DATA \
    W_BIN_DATA \
    int modal; \
    position_t pos; \
    dialog_style_t style;

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ALIGN_CENTER 2
#define ALIGN_TOP 3
#define ALIGN_BOTTOM 4

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

typedef struct dialog_border_plain
{
    /** Border size in pixels. */
    int border;

    /** Border colour. */
    w_colour_t border_col;

    /** Background colour inside the dialog. */
    w_colour_t bg_col;
}
dialog_border_plain_t;

typedef struct dialog_border_textured
{
    void *image[9];
}
dialog_border_textured_t;

typedef union dialog_border
{
    dialog_border_plain_t plain;
    dialog_border_textured_t textured;
} dialog_border_t;

/** Dialog box style. */
typedef struct dialog_style
{
    char textured;

    dialog_border_t border;

    /** Colour of the quad that will be drawn the size of the whole screen.
     */
    w_colour_t fade_col;

    /** Horizontal padding in pixels. This is the area between the border
     *  and the widgets.
     */
    int hor_pad;

    /** Vertical padding in pixels. This is the area between the border
     *  and the widgets.
     */
    int vert_pad;
}
dialog_style_t;

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

/** Dialog state. */
typedef struct w_dialog
{
    W_DIALOG_DATA
}
w_dialog_t;

void dialog_cleanup();

void dialog_open(w_dialog_t *menu);

void dialog_close();

w_dialog_t *dialog_current();

void w_dialog_get_screen_pos(w_dialog_t *dialog, int *x, int *y);

void w_dialog_render(w_dialog_t *dialog);

void w_dialog_mouse_movement(w_dialog_t *dialog, int x, int y);

int w_dialog_input(w_widget_t *widget, ui_event_t event);

void dialog_input(ui_event_t event);

void w_dialog_set_modal(w_dialog_t *dialog, int modal);

void w_dialog_set_position(w_dialog_t *dialog, int x, int y, int x_align, int y_align);

void w_dialog_init(w_dialog_t *dialog, w_widget_t *child);

void w_dialog_set_style(w_dialog_t *dialog, dialog_style_t *style);

w_widget_t *w_dialog_create(w_widget_t *child);

#endif /* GAMEGUI_DIALOG_H */
