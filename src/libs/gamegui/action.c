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

#include <gamegui/action.h>
#include <gamegui/label.h>

w_class_id w_action_get_class_id()
{
    CHILD(w_bin_get_class_id())
}

/** Implements widget::render for action widgets. */
void w_action_render(w_widget_t *widget, int x, int y, int focus)
{
    w_action_t *action = W_ACTION(widget);
    w_widget_t *child = w_bin_get_child(W_BIN(widget));

    if (focus != FOCUS_NONE)
        focus = FOCUS_ALL;

    child->render(child, x, y, focus);
}

/** Implements widget::input for action widgets. */
int w_action_input(w_widget_t *widget, ui_event_t event)
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

void w_action_init(w_action_t *action, w_widget_t *widget)
{
    w_bin_init((w_bin_t *) action, widget);

    action->render = w_action_render;
    action->input = w_action_input;
    action->id = w_action_get_class_id();
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
    w_align_set_alignment(W_ALIGN(label), xalign, yalign);
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
