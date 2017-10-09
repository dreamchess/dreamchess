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

#include "dialogs.h"

/* System dialog. */

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

/** The system dialog. Provides a set of system-related actions to the user.
 *  Currently this dialog only contains an item to quit the game. In the
 *  future this will be extended with load/save game items and possibly
 *  other items as well.
 */

/** @brief Opens the quit dialog. */
static int dialog_quit_open(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_quit_create(gg_widget_find_dialog(widget)));
    return 1;
}

/** @brief Opens the ingame dialog. */
static int dialog_ingame_open(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_ingame_create(gg_widget_find_dialog(widget)));
    return 1;
}

static int dialog_savegame_open(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_saveload_create(gg_widget_find_dialog(widget), true));
    return 1;
}

/** @brief Creates the system dialog.
 *
 *  @return The created dialog.
 */
gg_dialog_t *dialog_system_create(void)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *widget;

    widget = gg_action_create_with_label("Save game..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_savegame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Move Options..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_ingame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Quit Game..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_quit_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    gg_vbox_set_selected(vbox, 3);

    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
    return GG_DIALOG(dialog);
}
