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

#include "gamegui_dialogs.h"

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

gg_dialog_t *dialog_message_create(char *message)
{
    gg_widget_t *dialog;
    gg_widget_t *widget;

    gg_widget_t *vbox = gg_vbox_create(0);
    /*gg_container_append(GG_CONTAINER(vbox), gg_label_create("Important message from engine"));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(""));*/
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(message));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(""));
    widget = gg_action_create_with_label("Ok", 0.5f, 0.5f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());

    return GG_DIALOG(dialog);
}

void show_message_dialog( char *message )
{
    gg_dialog_open(dialog_message_create(message));
}

gg_dialog_t *dialog_engine_error_create(void)
{
    gg_widget_t *dialog;
    gg_widget_t *widget;

    gg_widget_t *vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), gg_label_create("A fatal error has occured while"));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create("communicating with the chess engine."));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create("Please check your settings."));
    widget = gg_action_create_with_label("OK", 0.5f, 0.5f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());

    return GG_DIALOG(dialog);
}

