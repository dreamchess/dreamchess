/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
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

/** @file
 *  @brief Widget class implementation.
 */

#include <stdlib.h>

#include <gamegui/system.h>
#include <gamegui/widget.h>
#include <gamegui/dialog.h>

gg_class_id gg_widget_get_class_id()
{
    GG_CHILD(GG_CLASS_ID_NONE)
}

void gg_widget_destroy(gg_widget_t *widget)
{
    while (!TAILQ_EMPTY(&widget->callbacks)) {
        gg_widget_cb_list_t *cb = TAILQ_FIRST(&widget->callbacks);
        TAILQ_REMOVE(&widget->callbacks, cb, entries);
        free(cb);
    }

    free(widget);
}

void gg_set_requested_size(gg_widget_t *widget, int width, int height)
{
    widget->width_f = width;
    widget->height_f = height;
}

void gg_widget_get_requested_size(gg_widget_t *widget, int *width,
                                  int *height)
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

void gg_set_size(gg_widget_t *widget, int width, int height)
{
    widget->width_a = width;
    widget->height_a = height;
}

gg_rect_t gg_get_focus_pos(gg_widget_t *widget)
{
    gg_rect_t rect;

    rect.x = 0;
    rect.y = 0;
    rect.width = widget->width_a;
    rect.height = widget->height_a;

    return rect;
}

int gg_set_focus_pos(gg_widget_t *widget, int x, int y)
{
    return 1;
}

gg_dialog_t *gg_widget_find_dialog(gg_widget_t *widget)
{
    if (widget->parent == NULL)
        return NULL;

    while (widget->parent != NULL)
        widget = widget->parent;

    return GG_DIALOG(widget);
}

void gg_widget_init(gg_widget_t *widget)
{
    widget->render = NULL;
    widget->input = NULL;
    widget->destroy = gg_widget_destroy;
    widget->get_requested_size = gg_widget_get_requested_size;
    widget->set_size = gg_set_size;
    widget->get_focus_pos = gg_get_focus_pos;
    widget->set_focus_pos = gg_set_focus_pos;
    widget->id = gg_widget_get_class_id();
    widget->enabled = 0;
    widget->width = widget->height = 0;
    widget->width_f = widget->height_f = -1;
    widget->width_a = widget->height_a = 0;
    widget->parent = NULL;
    TAILQ_INIT(&widget->callbacks);
}

void gg_widget_subscribe_signal(gg_widget_t *widget, gg_signal_t signal, gg_widget_cb_t callback, void *extra_data)
{
    gg_widget_cb_list_t *cb = malloc(sizeof(gg_widget_cb_list_t));
    cb->signal = signal;
    cb->callback = callback;
    cb->extra_data = extra_data;
    TAILQ_INSERT_HEAD(&widget->callbacks, cb, entries);
}

int gg_widget_subscribe_signal_name(gg_widget_t *widget, gg_class_id id, char *name, gg_widget_cb_t callback, void *extra_data)
{
    gg_signal_t signal = gg_signal_lookup(id, name);

    if (signal == -1)
        return -1;

    gg_widget_subscribe_signal(widget, signal, callback, extra_data);
}

void gg_widget_emit_signal(gg_widget_t *widget, gg_widget_t *emitter, gg_signal_t signal, void *data)
{
    gg_widget_cb_list_t *cb_list;

    TAILQ_FOREACH(cb_list, &widget->callbacks, entries) {
        if (cb_list->signal == signal) {
            if (cb_list->callback(widget, emitter, data, cb_list->extra_data))
                return;
        }
    }

    if (widget->parent)
        gg_widget_emit_signal(widget->parent, emitter, signal, data);
}
