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

static void tile_line(w_rect_t line, void *image, int size)
{
    w_rect_t source, dest;

    source.x = 0;
    source.y = size - line.height;
    source.height = line.height;
    source.width = size;

    dest = line;

    while (dest.x - line.x < line.width)
    {
        int width = line.x + line.width - dest.x;

        if (width > size)
            width = size;

        source.width = width;
        dest.width = width;
        w_system_draw_image(image, source, dest);
        dest.x += width;
    }
}

static void bg_line(w_rect_t line, void *image[3], int size)
{
    w_rect_t source, dest;

    source.x = 0;
    source.y = 0;
    source.height = line.height;
    source.width = size;

    dest = line;
    dest.width = size;

    w_system_draw_image(image[0], source, dest);
    dest.x = line.x + line.width - size;
    w_system_draw_image(image[2], source, dest);
    dest.x = line.x + size;
    dest.width = line.width - 2 * size;
    tile_line(dest, image[1], size);
}

static void bg_area(w_rect_t area, void *image[3], int size)
{
    w_rect_t line = area;

    while (line.y - area.y < area.height)
    {
        line.height = area.y + area.height - line.y;

        if (line.height > size)
            line.height = size;

        bg_line(line, image, size);

        line.y += line.height;
    }
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
    w_colour_t col;
    /* float tile_width=border.u2/3; */

    w_dialog_get_screen_pos(dialog, &xmin, &ymin);

    xmax = xmin + dialog->width;
    ymax = ymin + dialog->height;

    /* Draw the 'fade' */
    col = style->fade_col;
    glColor4f(col.r, col.b, col.g, col.a); /* 0.0f 0.0f 0.0f 0.5f */

    glBegin( GL_QUADS );
    glVertex3f( 640, 0, 0.9f );
    glVertex3f( 640, 480, 0.9f );
    glVertex3f( 0, 480, 0.9f );
    glVertex3f( 0, 0, 0.9f );
    glEnd( );

    if (style->textured)
    {
        int size;
        w_rect_t line;

        w_system_get_image_size(style->border.textured.image[0], &size, NULL);

        line.x = xmin;
        line.y = ymin;
        line.width = xmax - xmin;
        line.height = size;

        bg_line(line, &style->border.textured.image[6], size);
        line.y = ymax - size;
        bg_line(line, &style->border.textured.image[0], size);
        line.y = ymin + size;
        line.height = ymax - ymin - 2 * size;
        bg_area(line, &style->border.textured.image[3], size);

        xmin += size;
        xmax -= size;
        ymin += size;
        ymax -= size;
    }
    else
    {
        /* Draw the border. */
        col = style->border.plain.border_col;
        glColor4f(col.r, col.g, col.b, col.a); /* 0.0f 0.0f 0.0f 1.0f */

        glBegin( GL_QUADS );
        glVertex3f(xmax, ymin, 0.9f);
        glVertex3f(xmax, ymax, 0.9f);
        glVertex3f(xmin, ymax, 0.9f);
        glVertex3f(xmin, ymin, 0.9f);
        glEnd();

        xmin += style->border.plain.border;
        xmax -= style->border.plain.border;
        ymin += style->border.plain.border;
        ymax -= style->border.plain.border;

        /* Draw the backdrop. */
        col = style->border.plain.bg_col;
        glColor4f(col.r, col.g, col.b, col.a); /* 0.8f 0.8f 0.8f 1.0f */

        glBegin( GL_QUADS );
        glVertex3f(xmax, ymin, 0.95f);
        glVertex3f(xmax, ymax, 0.95f);
        glVertex3f(xmin, ymax, 0.95f);
        glVertex3f(xmin, ymin, 0.95f);
        glEnd();
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
    style.border.plain.border_col = (w_colour_t) {0.0f, 0.0f, 0.0f, 1.0f};
    style.border.plain.bg_col = (w_colour_t) {0.8f, 0.8f, 0.8f, 1.0f};
    style.fade_col = (w_colour_t) {0.0f, 0.0f, 0.0f, 0.5f};
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

#if 0
void dialog_render_border_section( int section, float xmin, float ymin, float xmax, float ymax )
{
    float tile_width=border.u2/3;
    int i,j;
    float width2, height2;
    colour_t back = {1.0f, 1.0f, 1.0f, 0.75f};

    //printf( "Border U/V: %f %f %f %f\n\r", border.u1, border.v1, border.u2, border.v2  );

    switch ( section )
    {
    case 0: /* Top */
        for ( i=0; i<((xmax-xmin)/16); i++ )
        {
            if ( i > (int)(((xmax-xmin)/16)-1) )
                draw_texture_uv( &border, xmin+(i*16), ymax,
                                 (int)(xmax-xmin)-(int)(i*16), 16, 0.95f, &col_white,
                                 tile_width, 0.0f, tile_width*2, tile_width );
            else
                draw_texture_uv( &border, xmin+(i*16), ymax, 16, 16, 0.95f, &col_white,
                                 tile_width, 0.0f, tile_width*2, tile_width );
        }
        break;
    case 1: /* Bottom */
        for ( i=0; i<((xmax-xmin)/16); i++ )
        {
            if ( i > (int)(((xmax-xmin)/16)-1) )
                draw_texture_uv( &border, xmin+(i*16), ymin-16, (int)(xmax-xmin)-(int)(i*16),
                                 16, 0.95f, &col_white, tile_width, border.v2-tile_width, tile_width*2,
                                 border.v2 );
            else
                draw_texture_uv( &border, xmin+(i*16), ymin-16, 16, 16, 0.95f, &col_white,
                                 tile_width, border.v2-tile_width, tile_width*2, border.v2 );
        }
        break;
    case 2: /* Left */
        for ( i=0; i<((ymax-ymin)/16); i++ )
        {
            if ( i > (int)(((ymax-ymin)/16)-1) )
                draw_texture_uv( &border, xmin-16, ymin+(i*16), 16,
                                 (int)(ymax-ymin)-(int)(i*16), 0.95f, &col_white, 0.0f, tile_width,
                                 tile_width, tile_width*2 );
            else
                draw_texture_uv( &border, xmin-16, ymin+(i*16), 16, 16, 0.95f, &col_white,
                                 0.0f, tile_width, tile_width, tile_width*2 );
        }
        break;
    case 3: /* Right */
        for ( i=0; i<((ymax-ymin)/16); i++ )
        {
            if ( i > (int)(((ymax-ymin)/16)-1) )
                draw_texture_uv( &border, xmax, ymin+(i*16), 16, (int)(ymax-ymin)-(int)(i*16),
                                 0.95f, &col_white, border.u2-tile_width, tile_width, border.u2,
                                 tile_width*2 );
            else
                draw_texture_uv( &border, xmax, ymin+(i*16), 16, 16, 0.95f, &col_white,
                                 border.u2-tile_width, tile_width, border.u2, tile_width*2 );
        }
        break;
    case 4: /* Top left. */
        draw_texture_uv( &border, xmin-16, ymax, 16, 16, 0.95f, &col_white,
                         0.0f, 0.0f, tile_width, tile_width );
        break;
    case 5: /* Top right. */
        draw_texture_uv( &border, xmax, ymax, 16, 16, 0.95f, &col_white,
                         border.u2-tile_width, 0.0f, border.u2, tile_width );
        break;
    case 6: /* Bottom left.*/
        draw_texture_uv( &border, xmin-16, ymin-16, 16, 16, 0.95f, &col_white,
                         0.0f, border.v2-tile_width, tile_width, border.v2 );
        break;
    case 7: /* Bottom right. */
        draw_texture_uv( &border, xmax, ymin-16, 16, 16, 0.95f, &col_white,
                         border.u2-tile_width, border.v2-tile_width, border.u2, border.v2 );
        break;
    case 8: /* Middle. */
        /* Draw backdrop.. */
        for ( j=0; j<=((ymax-ymin)/16); j++ )
            for ( i=0; i<=((xmax-xmin)/16); i++ )
            {
                if ( i > (int)(((xmax-xmin)/16)-1) )
                    width2=(int)(xmax-xmin)-(int)(i*16);
                else
                    width2=16;

                if ( j > (int)(((ymax-ymin)/16)-1) )
                    height2=(int)(ymax-ymin)-(int)(j*16);
                else
                    height2=16;

                draw_texture_uv( &border, xmin+(i*16), ymin+(j*16), width2, height2,
                                 0.95f, &back, tile_width, tile_width, tile_width*2,
                                 tile_width*2 );
            }
        break;
    }
}

void dialog_render_border( float xmin, float ymin, float xmax, float ymax )
{
    int i=0;

    for ( i; i<9; i++ )
        dialog_render_border_section( i, xmin, ymin, xmax, ymax );
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
#endif
