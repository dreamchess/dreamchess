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

#include <stdlib.h>
#include <string.h>

#include "dreamchess.h"
#include "gamegui_dialogs.h"
#include "options.h"
#include "system_config.h"
#include "audio.h"

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

static int add_text(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_edit_append((gg_edit_t *) extra_data, gg_entry_get_text(GG_ENTRY(widget)));
    gg_entry_set_text(GG_ENTRY(widget), "");
    return 1;
}

gg_dialog_t *dialog_chat_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *edit;

    vbox = gg_vbox_create(0);

    edit = gg_edit_create(500, 300);
    gg_container_append(GG_CONTAINER(vbox), edit);

    widget = gg_entry_create(500);
    gg_container_append(GG_CONTAINER(vbox), widget);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        add_text, edit);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    return GG_DIALOG(dialog);
}
