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

#include <gamegui/entry.h>

static w_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

w_class_id w_entry_get_class_id()
{
    CHILD(w_widget_get_class_id())
}

/** Implements widget::render for text entry widgets. */
void w_entry_render(w_widget_t *widget, int x, int y, int focus)
{
    w_entry_t *entry = W_ENTRY(widget);
    int len;
    int c;

    c = entry->text[entry->cursor_pos];
    entry->text[entry->cursor_pos] = '\0';
    w_system_get_string_size(entry->text, &len, NULL);
    entry->text[entry->cursor_pos] = c;

    if (focus != FOCUS_NONE)
        draw_rect(x, y, entry->width_a, entry->height_a, &col_dark_red);
    else
        draw_rect(x, y, entry->width_a, entry->height_a, &col_black);

    x += ENTRY_SPACING;
    y += ENTRY_SPACING;

    if (focus != FOCUS_NONE)
    {
        int cursor_width;

        w_system_get_string_size(ENTRY_CURSOR, &cursor_width, NULL);
        w_system_draw_string(entry->text, x, y, &col_dark_red, 0, 0);
        if (SDL_GetTicks() % 400 < 200)
            w_system_draw_string(ENTRY_CURSOR, x + len - cursor_width / 2, y,
                &col_dark_red, 0, 0);
    }
    else
        w_system_draw_string(entry->text, x, y, &col_black, 0, 0);
}

/** Implements widget::input for text entry widgets. */
int w_entry_input(w_widget_t *widget, ui_event_t event)
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
    w_system_get_string_size("Visible text", &entry->width, &entry->height);
    entry->width += ENTRY_SPACING * 2;
    entry->height += ENTRY_SPACING * 2;
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
