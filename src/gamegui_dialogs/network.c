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

static gg_widget_t *entry1, *entry2;

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	gg_dialog_close();
	return 1;
}

static int dialog_start_join_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option = config_get_option("join_ip");
	free(option->string);
	option->string = strdup(gg_entry_get_text(GG_ENTRY(entry1)));
	option = config_get_option("join_port");
	free(option->string);
	option->string = strdup(gg_entry_get_text(GG_ENTRY(entry2)));
	config_save();
	gg_dialog_close();
	return 1;
}

static int dialog_start_host_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option = config_get_option("host_port");
	option->string = strdup(gg_entry_get_text(GG_ENTRY(extra_data)));
	config_save();
	gg_dialog_close();
	return 1;
}

static gg_dialog_t *dialog_network_join_create(gg_dialog_t *parent) {
	gg_widget_t *dialog;
	gg_widget_t *vbox, *vbox2, *hbox;
	gg_widget_t *widget;
	option_t *option;

	vbox = gg_vbox_create(0);
	vbox2 = gg_vbox_create(2);

	widget = gg_label_create("IP address:");
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	widget = gg_label_create("Port:");
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	hbox = gg_hbox_create(20);
	gg_container_append(GG_CONTAINER(hbox), vbox2);
	vbox2 = gg_vbox_create(2);

	option = config_get_option("join_ip");
	entry1 = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry1), option->string);
	gg_container_append(GG_CONTAINER(vbox2), entry1);

	option = config_get_option("join_port");
	entry2 = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry2), option->string);
	gg_container_append(GG_CONTAINER(vbox2), entry2);

	gg_container_append(GG_CONTAINER(hbox), vbox2);
	gg_container_append(GG_CONTAINER(vbox), hbox);

	hbox = gg_hbox_create(20);
	widget = gg_action_create_with_label("Connect", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_start_join_cb, NULL);
	gg_container_append(GG_CONTAINER(hbox), widget);

	widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_close_cb, NULL);
	gg_container_append(GG_CONTAINER(hbox), widget);
	gg_box_set_alignment(GG_BOX(hbox), 0.5f);
	gg_container_append(GG_CONTAINER(vbox), hbox);

	dialog = gg_dialog_create(vbox, "Join Game", parent, GG_DIALOG_AUTOHIDE_PARENT);
	gg_dialog_set_position(GG_DIALOG(dialog), 320, 23, 0.5f, 0.0f);
	gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

	return GG_DIALOG(dialog);
}

static gg_dialog_t *dialog_network_host_create(gg_dialog_t *parent) {
	gg_widget_t *dialog;
	gg_widget_t *vbox, *vbox2, *hbox;
	gg_widget_t *widget;
	option_t *option;

	vbox = gg_vbox_create(0);
	vbox2 = gg_vbox_create(2);

	widget = gg_label_create("Port:");
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	hbox = gg_hbox_create(20);
	gg_container_append(GG_CONTAINER(hbox), vbox2);
	vbox2 = gg_vbox_create(2);

	option = config_get_option("host_port");
	gg_widget_t *entry = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry), option->string);
	gg_container_append(GG_CONTAINER(vbox2), entry);

	gg_container_append(GG_CONTAINER(hbox), vbox2);
	gg_container_append(GG_CONTAINER(vbox), hbox);

	hbox = gg_hbox_create(20);
	widget = gg_action_create_with_label("Connect", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_start_host_cb, entry);
	gg_container_append(GG_CONTAINER(hbox), widget);

	widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_close_cb, NULL);
	gg_container_append(GG_CONTAINER(hbox), widget);
	gg_box_set_alignment(GG_BOX(hbox), 0.5f);
	gg_container_append(GG_CONTAINER(vbox), hbox);

	dialog = gg_dialog_create(vbox, "Host Game", parent, GG_DIALOG_AUTOHIDE_PARENT);
	gg_dialog_set_position(GG_DIALOG(dialog), 320, 23, 0.5f, 0.0f);
	gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

	return GG_DIALOG(dialog);
}

static int dialog_host_game_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	gg_dialog_open(dialog_network_host_create(gg_widget_find_dialog(widget)));
	return 1;
}

static int dialog_join_game_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	gg_dialog_open(dialog_network_join_create(gg_widget_find_dialog(widget)));
	return 1;
}

gg_dialog_t *dialog_network_create(gg_dialog_t *parent) {
	gg_widget_t *vbox = gg_vbox_create(0);

	gg_widget_t *widget = gg_action_create_with_label("Host Game", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_host_game_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);

	widget = gg_action_create_with_label("Join Game", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_join_game_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);

	widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
		dialog_close_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);

	widget = gg_dialog_create(vbox, "Network Game", parent, GG_DIALOG_AUTOHIDE_PARENT);
	gg_dialog_set_position(GG_DIALOG(widget), 320, 23, 0.5f, 0.0f);
	gg_dialog_set_style(GG_DIALOG(widget), get_menu_style());

	return GG_DIALOG(widget);
}
