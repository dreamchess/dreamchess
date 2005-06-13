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

#include <gamegui/dialog.h>

w_class_id w_dialog_get_class_id()
{
    CHILD(w_bin_get_class_id())
}

/** The maximum amount of open dialogs that the dialog system can handle. */
#define DIALOG_MAX 10

/** The dialog stack. */
w_dialog_t *dialog_stack[DIALOG_MAX];

/** The amount of dialogs that are currently open. */
int dialog_nr = 0;

/** To-be-destroyed dialogs. */
w_dialog_t *dialog_closed[DIALOG_MAX];

/** The amount of dialogs that need to be destroyed. */
int dialog_closed_nr = 0;

void dialog_cleanup()
{
    int i;

    for (i = 0; i < dialog_closed_nr; i++)
    {
        w_dialog_t *dialog = dialog_closed[i];
        dialog->destroy(W_WIDGET(dialog));
    }

    dialog_closed_nr = 0;
}

/** @brief Adds a dialog to the top of the dialog stack.
 *  @param menu The dialog to add.
 */
void dialog_open(w_dialog_t *menu)
{
    if (dialog_nr == DIALOG_MAX)
    {
        printf("Too many open dialogs.\n");
        return;
    }

    reset_string_type_length();
    dialog_stack[dialog_nr++] = menu;
}

/** @brief Closes the dialog that's on top of the dialog stack. */
void dialog_close()
{
    w_dialog_t *menu;

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
w_dialog_t *dialog_current()
{
    if (dialog_nr == 0)
        return NULL;

    return dialog_stack[dialog_nr - 1];
}

void w_dialog_get_screen_pos(w_dialog_t *dialog, int *x, int *y)
{
    if (dialog->pos.x_align == ALIGN_LEFT)
        *x = dialog->pos.x;
    else if (dialog->pos.x_align == ALIGN_RIGHT)
        *x = dialog->pos.x - dialog->width;
    else
        *x = dialog->pos.x - dialog->width / 2;

    if (dialog->pos.y_align == ALIGN_TOP)
        *y = dialog->pos.y - dialog->height;
    else if (dialog->pos.y_align == ALIGN_BOTTOM)
        *y = dialog->pos.y;
    else
        *y = dialog->pos.y - dialog->height / 2;
}

void draw_border(void *image[9], w_rect_t area)
{
    w_rect_t source, dest;
    int size;

    w_system_get_image_size(image[0], &size, NULL);

    area.x -= size;
    area.y -= size;

    area.width += 2 * size;
    area.height += 2 * size;

    source.x = 0;
    source.y = 0;
    source.width = size;
    source.height = size;

    dest = source;
    dest.x = area.x;
    dest.y = area.y;

    w_system_draw_image(image[6], source, dest, GG_MODE_SCALE, GG_MODE_SCALE);
    dest.y += area.height - size;
    w_system_draw_image(image[0], source, dest, GG_MODE_SCALE, GG_MODE_SCALE);
    dest.x += area.width - size;
    w_system_draw_image(image[2], source, dest, GG_MODE_SCALE, GG_MODE_SCALE);
    dest.y -= area.height - size;
    w_system_draw_image(image[8], source, dest, GG_MODE_SCALE, GG_MODE_SCALE);

    dest.x = area.x + size;
    dest.y = area.y;
    dest.width = area.width - 2 * size;
    dest.height = size;
    source.width = dest.width;
    w_system_draw_image(image[7], source, dest, GG_MODE_TILE, GG_MODE_SCALE);
    dest.y += area.height - size;
    w_system_draw_image(image[1], source, dest, GG_MODE_TILE, GG_MODE_SCALE);

    dest.x = area.x;
    dest.y = area.y + size;
    dest.width = size;
    dest.height = area.height - 2 * size;
    source.width = size;
    source.height = dest.height;
    w_system_draw_image(image[3], source, dest, GG_MODE_TILE, GG_MODE_SCALE);
    dest.x += area.width - size;
    w_system_draw_image(image[5], source, dest, GG_MODE_TILE, GG_MODE_SCALE);

    dest.x = area.x + size;
    dest.width = area.width - 2 * size;
    source.width = dest.width;
    w_system_draw_image(image[4], source, dest, GG_MODE_TILE, GG_MODE_TILE);
}

/** @brief Renders a dialog.
 *
 *  Renders a dialog in a specific style and at a specific position.
 *
 *  @param menu The dialog to render.
 *  @param style The style to render in.
 *  @param pos The position to render at.
 */
void w_dialog_render(w_dialog_t *dialog)
{
    dialog_style_t *style = &dialog->style;
    w_widget_t *child = w_bin_get_child(W_BIN(dialog));

    int xmin, xmax, ymin, ymax;

    w_dialog_get_screen_pos(dialog, &xmin, &ymin);

    xmax = xmin + dialog->width;
    ymax = ymin + dialog->height;

    /* Draw the 'fade' */
    w_system_draw_filled_rect(0, 0, 640, 480, &style->fade_col);

    if (style->textured)
    {
        int size;
        w_rect_t area;

        w_system_get_image_size(style->border.textured.image[0], &size, NULL);

        xmin += size;
        xmax -= size;
        ymin += size;
        ymax -= size;

        area.x = xmin;
        area.y = ymin;
        area.width = xmax - xmin;
        area.height = ymax - ymin;

        draw_border(style->border.textured.image, area);
    }
    else
    {
        /* Draw the border. */
        w_system_draw_filled_rect(xmin, ymin, xmax - xmin, ymax - ymin, &style->border.plain.border_col);

        xmin += style->border.plain.border;
        xmax -= style->border.plain.border;
        ymin += style->border.plain.border;
        ymax -= style->border.plain.border;

        /* Draw the backdrop. */
        w_system_draw_filled_rect(xmin, ymin, xmax - xmin, ymax - ymin, &style->border.plain.bg_col);
    }

    xmin += style->hor_pad;
    xmax -= style->hor_pad;
    ymin += style->vert_pad;
    ymax -= style->vert_pad;

    child->render(child, xmin, ymin, 1);
}

void w_dialog_mouse_movement(w_dialog_t *dialog, int x, int y)
{
    int xmin, xmax, ymin, ymax;

    w_dialog_get_screen_pos(dialog, &xmin, &ymin);

    xmax = xmin + dialog->width;
    ymax = ymin + dialog->height;

    if ((x < xmin) || (x >= xmax) || (y < ymin) || (y >= ymax))
        return;

    x -= xmin + dialog->style.hor_pad;
    y -= ymin + dialog->style.vert_pad;

    dialog->set_focus_pos(W_WIDGET(dialog), x, y);
}

/** @brief Processes an input event for a specific dialog.
 *
 *  @param event The event to process.
 */
int w_dialog_input(w_widget_t *widget, ui_event_t event)
{
    w_dialog_t *dialog = W_DIALOG(widget);
    w_widget_t *child = w_bin_get_child(W_BIN(widget));

    if (!dialog->modal && (event == UI_EVENT_ESCAPE))
        dialog_close();

    child->input(child, event);
}

void dialog_input(ui_event_t event)
{
    w_dialog_t *dialog = dialog_current();

    if (!dialog)
        return;

    w_dialog_input(W_WIDGET(dialog), event);
}

void w_dialog_set_modal(w_dialog_t *dialog, int modal)
{
    dialog->modal = modal;
}

void w_dialog_set_position(w_dialog_t *dialog, int x, int y, int x_align, int y_align)
{
    dialog->pos.x = x;
    dialog->pos.y = y;
    dialog->pos.x_align = x_align;
    dialog->pos.y_align = y_align;
}

void w_dialog_init(w_dialog_t *dialog, w_widget_t *child)
{
    dialog_style_t style;

    style.textured = 0;
    style.border.plain.border = 5;
    style.border.plain.border_col = (w_colour_t)
                                    {
                                        0.0f, 0.0f, 0.0f, 1.0f
                                    };
    style.border.plain.bg_col = (w_colour_t)
                                {
                                    0.8f, 0.8f, 0.8f, 1.0f
                                };
    style.fade_col = (w_colour_t)
                     {
                         0.0f, 0.0f, 0.0f, 0.5f
                     };
    style.hor_pad = 20;
    style.vert_pad = 10;

    w_bin_init((w_bin_t *) dialog, child);

    dialog->input = w_dialog_input;
    dialog->id = w_dialog_get_class_id();
    dialog->modal = 0;

    child->get_requested_size(child, &dialog->width, &dialog->height);
    child->set_size(child, dialog->width, dialog->height);
    w_dialog_set_style(dialog, &style);

    w_dialog_set_position(dialog, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALIGN_CENTER, ALIGN_CENTER);
}

/** @brief Creates a dialog.
 *
 *  @param widget The widget the dialog contains.
 *  @return The created dialog.
 */
w_widget_t *w_dialog_create(w_widget_t *child)
{
    w_dialog_t *dialog = malloc(sizeof(w_dialog_t));

    w_dialog_init(dialog, child);

    return W_WIDGET(dialog);
}

void w_dialog_set_style(w_dialog_t *dialog, dialog_style_t *style)
{
    w_widget_t *child = w_bin_get_child(W_BIN(dialog));
    int size;

    dialog->style = *style;
    dialog->width = child->width_a + 2 * dialog->style.hor_pad;
    dialog->height = child->height_a + 2 * dialog->style.vert_pad;

    if (style->textured)
    {
        int size;

        w_system_get_image_size(style->border.textured.image[0], &size, NULL);
        dialog->width += 2 * size;
        dialog->height += 2 * size;
    }
    else
    {
        dialog->width += 2 * dialog->style.border.plain.border;
        dialog->height += 2 * dialog->style.border.plain.border;
    }
}
