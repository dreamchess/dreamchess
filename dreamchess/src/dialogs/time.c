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

#include <errno.h>
#include <stdlib.h>

#include "dialogs.h"
#include "system_config.h"

static gg_widget_t *entry1, *entry2, *entry3, *label1, *label2, *label3, *label4, *time_increment;

static void create_option_values(gg_option_t *widget, option_t *option) {
	option_value_t *value;

	OPTION_VALUE_FOREACH(value, option) {
		gg_option_append_label(widget, value->name, 0.5f, 0.0f);
	}

	gg_option_set_selected(widget, option->selected->index);
}

static int dialog_cancel_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	gg_dialog_close();
	return 1;
}

static int dialog_ok_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option;

	errno = 0;
	int moves = strtol(gg_entry_get_text(GG_ENTRY(entry2)), NULL, 10);
	if (!errno) {
		option = config_get_option("custom_time_moves");
		option->value = moves;
	}

	errno = 0;
	int time = strtol(gg_entry_get_text(GG_ENTRY(entry1)), NULL, 10);
	if (!errno) {
		option = config_get_option("custom_time_time");
		option->value = time;
	}

	errno = 0;
	int increments = strtol(gg_entry_get_text(GG_ENTRY(entry3)), NULL, 10);
	if (!errno) {
		option = config_get_option("custom_time_increment");
		option->value = increments;
	}

	config_save();

	gg_dialog_close();

	return 1;
}

static int time_time_changed(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	int nr = gg_option_get_selected(GG_OPTION(widget));
	int size = gg_container_get_size(GG_CONTAINER(widget));
	option_t *option = config_get_option("time_time");

	option_select_value_by_index(option, nr);

	if (nr == size - 1) {
		entry1->enabled = 1;
		label1->enabled = 1;
	} else {
		entry1->enabled = 0;
		label1->enabled = 0;
	}

	return 1;
}

static int time_increment_changed(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	if (time_increment->enabled == 0)
		return 1;

	int nr = gg_option_get_selected(GG_OPTION(widget));
	int size = gg_container_get_size(GG_CONTAINER(widget));
	option_t *option = config_get_option("time_increment");

	option_select_value_by_index(option, nr);

	if (nr == size - 1) {
		entry3->enabled = 1;
		label3->enabled = 1;
	} else {
		entry3->enabled = 0;
		label3->enabled = 0;
	}

	return 1;
}

static int time_moves_changed(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	int nr = gg_option_get_selected(GG_OPTION(widget));
	int size = gg_container_get_size(GG_CONTAINER(widget));
	option_t *option = config_get_option("time_moves");

	option_select_value_by_index(option, nr);

	if (nr == 0) {
		label3->enabled = 1;
		entry3->enabled = 1;
		time_increment->enabled = 1;
		label4->enabled = 1;

		time_increment_changed(time_increment, NULL, NULL, NULL);
	} else {
		label3->enabled = 0;
		entry3->enabled = 0;
		time_increment->enabled = 0;
		label4->enabled = 0;

		GG_OPTION(time_increment)->sel = 0;
	}

	if (nr == size - 1) {
		entry2->enabled = 1;
		label2->enabled = 1;
	} else {
		entry2->enabled = 0;
		label2->enabled = 0;
	}

	return 1;
}

gg_dialog_t *dialog_time_create(gg_dialog_t *parent) {
	gg_widget_t *dialog;
	gg_widget_t *vbox, *vbox2, *hbox;
	gg_widget_t *widget;
	option_t *option;
	gg_widget_t *time_moves, *time_time;
	char val[5];

	vbox = gg_vbox_create(0);
	vbox2 = gg_vbox_create(0);

	widget = gg_label_create("Moves:");
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	label2 = gg_label_create("Custom:");
	gg_align_set_alignment(GG_ALIGN(label2), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), label2);

	widget = gg_label_create("Time(mins):");
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	label1 = gg_label_create("Custom:");
	gg_align_set_alignment(GG_ALIGN(label1), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), label1);

	label4 = gg_label_create("Increments(secs):");
	gg_align_set_alignment(GG_ALIGN(label4), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), label4);

	label3 = gg_label_create("Custom:");
	gg_align_set_alignment(GG_ALIGN(label3), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), label3);

	hbox = gg_hbox_create(20);
	gg_container_append(GG_CONTAINER(hbox), vbox2);

	vbox2 = gg_vbox_create(0);

	option = config_get_option("time_moves");
	time_moves = gg_option_create();
	create_option_values(GG_OPTION(time_moves), option);
	gg_widget_subscribe_signal_name(time_moves, time_moves->id, "option_changed", time_moves_changed, NULL);
	gg_container_append(GG_CONTAINER(vbox2), time_moves);

	option = config_get_option("custom_time_moves");
	val[0] = 0;
	snprintf(val, 5, "%d", option->value);
	entry2 = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry2), val);
	gg_container_append(GG_CONTAINER(vbox2), entry2);

	option = config_get_option("time_time");
	time_time = gg_option_create();
	create_option_values(GG_OPTION(time_time), option);
	gg_widget_subscribe_signal_name(time_time, time_time->id, "option_changed", time_time_changed, NULL);
	gg_container_append(GG_CONTAINER(vbox2), time_time);

	option = config_get_option("custom_time_time");
	val[0] = 0;
	snprintf(val, 5, "%d", option->value);
	entry1 = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry1), val);
	gg_container_append(GG_CONTAINER(vbox2), entry1);

	option = config_get_option("time_increment");
	time_increment = gg_option_create();
	create_option_values(GG_OPTION(time_increment), option);
	gg_widget_subscribe_signal_name(time_increment, time_increment->id, "option_changed", time_increment_changed, NULL);
	gg_container_append(GG_CONTAINER(vbox2), time_increment);

	option = config_get_option("custom_time_increment");
	val[0] = 0;
	snprintf(val, 5, "%d", option->value);
	entry3 = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry3), val);
	gg_container_append(GG_CONTAINER(vbox2), entry3);

	gg_container_append(GG_CONTAINER(hbox), vbox2);
	gg_container_append(GG_CONTAINER(vbox), hbox);

	widget = gg_action_create_with_label("OK", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_ok_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);
	widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_cancel_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);

	dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
	gg_dialog_set_modal(GG_DIALOG(dialog), 1);
	gg_dialog_set_position(GG_DIALOG(dialog), gg_system_get_screen_width() / 2, 23, 0.5f, 0.0f);
	gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

	time_moves_changed(time_moves, NULL, NULL, NULL);
	time_time_changed(time_time, NULL, NULL, NULL);
	time_increment_changed(time_increment, NULL, NULL, NULL);

	return GG_DIALOG(dialog);
}
