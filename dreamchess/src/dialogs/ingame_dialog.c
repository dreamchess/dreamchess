/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
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

#include "dialogs.h"

/** The in-game dialog. Provides a set of gameplay-related actions to the
 *  user.
 */

static int retract_move(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("retracting move");
    game_retract_move();
    return 1;
}

static int move_now(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("forcing dreamer to move");
    game_move_now();
    return 1;
}

static int view_prev(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("viewing previous move");
    game_view_prev();
    return 1;
}

static int view_next(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("viewing next move");
    game_view_next();
    return 1;
}

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

/** @brief Creates the in-game dialog.
 *
 *  @return The created dialog.
 */
gg_dialog_t *dialog_ingame_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog, *widget;
    gg_widget_t *vbox = gg_vbox_create(0);

    widget = gg_action_create_with_label("Retract Move", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        retract_move, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Move Now", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        move_now, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("View Previous Move", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        view_prev, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("View Next Move", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        view_next, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
    return GG_DIALOG(dialog);
}
