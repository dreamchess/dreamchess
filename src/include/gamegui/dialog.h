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

/** @file
 *  @brief Dialog class interface.
 */

#ifndef GAMEGUI_DIALOG_H
#define GAMEGUI_DIALOG_H

#include <gamegui/system.h>
#include <gamegui/bin.h>
#include <gamegui/queue.h>

/** Typecast to dialog. */
#define GG_DIALOG(W) GG_CHECK_CAST(W, gg_dialog_get_class_id(), gg_dialog_t)

/** Dialog methods and properties. */
#define GG_DIALOG_DATA                                                       \
    /** Inherit from bin class. */                                           \
    GG_BIN_DATA                                                              \
                                                                             \
    /** Dialog list entry */                                                 \
    TAILQ_ENTRY(gg_dialog) entries;                                          \
                                                                             \
    /** Parent dialog */                                                     \
    struct gg_dialog *parent_dialog;                                         \
                                                                             \
    /** Flags */                                                             \
    int flags;                                                               \
                                                                             \
    /** Dialog state */                                                      \
    int dialog_state;                                                        \
                                                                             \
    /** Dialog movement original x-coordinate */                             \
    int movement_org_x;                                                      \
                                                                             \
    /** Dialog movement original x-coordinate */                             \
    int movement_org_y;                                                      \
                                                                             \
    /** Modal flag. 1 = modal (dialog cannot be escaped), 0 = not modal. */  \
    int modal;                                                               \
                                                                             \
    /** Position of dialog on the screen. */                                 \
    gg_dialog_position_t pos;                                                \
                                                                             \
    /** Visual dialog style. */                                              \
    gg_dialog_style_t style;                                                 \
                                                                             \
    /** Title bar text, or NULL for no title bar. */                         \
    char *title;

/** Dialog flags */
#define GG_DIALOG_MODAL (1 << 0)
#define GG_DIALOG_HIDDEN (1 << 1)
#define GG_DIALOG_AUTOHIDE_PARENT (1 << 2)

/** Dialog state */
#define GG_DIALOG_MOVING (1 << 0)
#define GG_DIALOG_LEFT_BUTTON (1 << 1)

/* FIXME */
/** Screen width in pixels. */
#define SCREEN_WIDTH  640
/** Screen height in pixels. */
#define SCREEN_HEIGHT 480
/** Screen colour depth in bits per pixel. */
#define SCREEN_BPP     16

/** Textured dialog border style. */
typedef struct gg_dialog_border
{
    /** Border images.
     *  0: Top-left corner.
     *  1: Top edge (to be tiled horizontally).
     *  2: Top-right corner.
     *  3: Left edge (to be tiled vertically).
     *  4: Center (to be tiled horizontally and vertically).
     *  5: Right edge (to be tiled vertically).
     *  6: Bottom-left corner.
     *  7: Bottom edge (to be tiled horizontally).
     *  8: Bottom-right corner.
     */
    void *image[9];
}
gg_dialog_border_t;

/** Dialog style. */
typedef struct gg_dialog_style
{
    /** Textured flag. 1 = textured, 0 = plain. */
    char textured;

    /** Dialog border style. */
    gg_dialog_border_t border;

    /** Colour of the quad that will be drawn the size of the whole screen.
     */
    gg_colour_t fade_col;

    /** Horizontal padding in pixels. This is the area between the border
     *  and the widget.
     */
    int hor_pad;

    /** Vertical padding in pixels. This is the area between the border
     *  and the widget.
     */
    int vert_pad;
}
gg_dialog_style_t;

/** Dialog position. */
typedef struct gg_dialog_position
{
    /** x-coordinate in pixels. */
    int x;

    /** y-coordinate in pixels. */
    int y;

    /** Horizontal alignment relative to x. Ranges from 0 (left aligned) to 1
     *  (right aligned).
     */
    float x_align;

    /** Vertical alignment relative to y. Ranges from 0 (bottom aligned) to 1
     *  (top aligned).
     */
    float y_align;
}
gg_dialog_position_t;

/** Dialog class. */
typedef struct gg_dialog
{
    GG_DIALOG_DATA
}
gg_dialog_t;

/** @brief Destroys all closed dialogs on the dialog stack and removes them
 *         from the stack.
 */
void gg_dialog_cleanup();

/** @brief Opens a dialog by placing it on top of the dialog stack.
 *
 *  @param dialog The dialog to open.
 */
void gg_dialog_open(gg_dialog_t *dialog);

/** @brief Closes the dialog that's on top of the dialog stack (if any). */
void gg_dialog_close();

void gg_dialog_close_all();

/** @brief Returns the dialog that's on top of the dialog stack.
 *
 *  @return Dialog on top of stack, or NULL if stack is empty.
 */
gg_dialog_t *gg_dialog_get_active();

/** @brief Determines the screen position of a dialog.
 *
 *  @param dialog The dialog.
 *  @param x Returns the x-coordinate (in pixels) of the dialog.
 *  @param y Returns the y-coordinate (in pixels) of the dialog.
 */
void gg_dialog_get_screen_pos(gg_dialog_t *dialog, int *x, int *y);

/** Implements gg_widget_t::render. */
void gg_dialog_render(gg_dialog_t *dialog);

/** @brief Notifies a dialog of mouse movement. Used for shifting focus on
 *         mouse movement.
 *
 *  @param x Screen x-coordinate (in pixels) of current mouse position.
 *  @param y Screen y-coordinate (in pixels) of current mouse position.
 */
void gg_dialog_mouse_movement(gg_dialog_t *dialog, int x, int y);

/** @brief Sends an input event to the currently open dialog (if any).
 *
 *  @param event The input event.
 */
void gg_dialog_input_current(gg_event_t event);

/** @brief Sets dialog modal setting.
 *
 *  @param dialog The dialog.
 *  @param modal 1 = modal, 0 = not modal.
 */
void gg_dialog_set_modal(gg_dialog_t *dialog, int modal);

void gg_dialog_show(gg_dialog_t *dialog);

void gg_dialog_hide(gg_dialog_t *dialog);

void gg_dialog_set_position(gg_dialog_t *dialog, int x, int y, float x_align, float y_align);

void gg_dialog_init(gg_dialog_t *dialog, gg_widget_t *child, char *title,
                    gg_dialog_t *parent, int flags);

void gg_dialog_set_style(gg_dialog_t *dialog, gg_dialog_style_t *style);

gg_widget_t *gg_dialog_create(gg_widget_t *child, char *title,
                              gg_dialog_t *parent, int flags);

void gg_dialog_destroy(gg_widget_t *widget);

gg_class_id gg_dialog_get_class_id();

void draw_border(void *image[9], char *title, int active, gg_rect_t area, int size);

void gg_dialog_render_all();

void gg_dialog_set_active(gg_dialog_t *dialog);

#endif /* GAMEGUI_DIALOG_H */
