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

#include <stdlib.h>

#include <gamegui/entry.h>

#define CURSOR_WIDTH 1

static gg_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

gg_class_id gg_entry_get_class_id()
{
    GG_CHILD(gg_widget_get_class_id())
}

static int string_width(unsigned char *s, int n)
{
    int retval;
    unsigned char c = s[n];

    s[n] = 0;
    gg_system_get_string_size(s, &retval, NULL);
    s[n] = c;
    return retval;
}

/** Implements widget::render for text entry widgets. */
void gg_entry_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_entry_t *entry = GG_ENTRY(widget);
    gg_rect_t rect;
    int len = string_width(entry->text, entry->cursor_pos);

    if (focus != GG_FOCUS_NONE)
        gg_system_draw_rect(x, y, entry->width_a, entry->height_a, &col_dark_red);
    else
        gg_system_draw_rect(x, y, entry->width_a, entry->height_a, &col_black);

    x += ENTRY_SPACING;
    y += ENTRY_SPACING;

    rect.x = x;
    rect.y = y;
    rect.width = entry->width_a - 2 * ENTRY_SPACING;
    rect.height = entry->height_a - 2 * ENTRY_SPACING;
    gg_clipping_adjust(&rect);

    if (focus != GG_FOCUS_NONE)
    {
        gg_system_draw_string(entry->text, x - entry->display_pos, y, &col_dark_red, 0, 0);
        if (gg_system_get_ticks() % 400 < 200)
            gg_system_draw_filled_rect(x + len - entry->display_pos, y, CURSOR_WIDTH,
                                       entry->height_a - 2 * ENTRY_SPACING, &col_dark_red);
    }
    else
        gg_system_draw_string(entry->text, x - entry->display_pos, y, &col_black, 0, 0);

    gg_clipping_undo();
}

/** Implements widget::input for text entry widgets. */
int gg_entry_input(gg_widget_t *widget, gg_event_t event)
{
    gg_entry_t *entry = GG_ENTRY(widget);
    int c = -1;
    int len = strlen(entry->text);
    int width, max_width;

    if (event.type == GG_EVENT_KEY)
    {

        if (event.key == GG_KEY_LEFT)
        {
            if (entry->cursor_pos > 0)
                entry->cursor_pos--;
        }
        else if (event.key == GG_KEY_RIGHT)
        {
            if (entry->cursor_pos < len)
                entry->cursor_pos++;
        }
        else if (event.key == GG_KEY_HOME)
            entry->cursor_pos = 0;
        else if (event.key == GG_KEY_END)
            entry->cursor_pos = len;
        else if (event.key == GG_KEY_BACKSPACE)
        {
            if (entry->cursor_pos > 0)
            {
                int i;
                for (i = entry->cursor_pos; i <= len; i++)
                    entry->text[i - 1] = entry->text[i];
                entry->cursor_pos--;
            }
        }
        else if (event.key == GG_KEY_DELETE)
        {
            if (entry->cursor_pos < len)
            {
                int i;
                for (i = entry->cursor_pos + 1; i <= len; i++)
                    entry->text[i - 1] = entry->text[i];
            }
        }
        else
        {
            if ((event.key > 0) && (event.key <= 255))
            {
                int i;

                if (len >= entry->max_len)
                    return 1;

                for (i = len; i >= entry->cursor_pos; i--)
                    entry->text[i + 1] = entry->text[i];
                entry->text[entry->cursor_pos++] = event.key;
            }
            else
                return 0;
        }
    }

    if (event.type == GG_EVENT_MOUSE
        && event.mouse.type == GG_MOUSE_BUTTON_DOWN
        && event.mouse.button == 0)
    {
        int total_width = 0;
        int i;

        for (i = 0; i < strlen(entry->text); i++)
        {
            int width;

            gg_system_get_char_size(entry->text[i], &width, NULL);
            total_width += width;
            if (total_width > entry->display_pos + event.mouse.x)
            {
                if (total_width - width / 2 < entry->display_pos + event.mouse.x)
                    i++;
                break;
            }
        }

        entry->cursor_pos = i;
    }

    width = string_width(entry->text, entry->cursor_pos);
    max_width = entry->width_a - 2 * ENTRY_SPACING - CURSOR_WIDTH;

    if (width - entry->display_pos > max_width)
        entry->display_pos = width - max_width;

    if (width - entry->display_pos < 0)
        entry->display_pos = width;

    return 1;
}

void gg_entry_init(gg_entry_t *entry)
{
    gg_widget_init((gg_widget_t *) entry);

    entry->render = gg_entry_render;
    entry->input = gg_entry_input;
    entry->id = gg_entry_get_class_id();
    entry->max_len = ENTRY_MAX_LEN;
    entry->cursor_pos = 0;
    entry->text[0] = '\0';
    entry->enabled = 1;
    gg_system_get_string_size("Visible text", &entry->width, &entry->height);
    entry->width += ENTRY_SPACING * 2;
    entry->height += ENTRY_SPACING * 2;
    entry->display_pos = 0;
}

/** @brief Creates a text entry widget.
 *
 *  A text entry widget for a single line of text.
 *
 *  @return The created widget.
 */
gg_widget_t *gg_entry_create()
{
    gg_entry_t *entry = malloc(sizeof(gg_entry_t));

    gg_entry_init(entry);

    return GG_WIDGET(entry);
}
