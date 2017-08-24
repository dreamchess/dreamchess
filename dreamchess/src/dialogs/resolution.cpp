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
#include <errno.h>

#include "dialogs.h"
#include "system_config.h"

static gg_widget_t *entry1, *entry2, *label1, *label2, *container;

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

gg_dialog_t *dialog_error_create(gg_dialog_t *parent, const char *message1, const char *message2)
{
    gg_widget_t *dialog;
    gg_widget_t *widget;

    gg_widget_t *vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(message1));
    if (message2)
        gg_container_append(GG_CONTAINER(vbox), gg_label_create(message2));
    widget = gg_action_create_with_label("Ok", 0.5f, 0.5f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 60, 0.5f, 0.0f);

    return GG_DIALOG(dialog);
}

static void create_option_values(gg_option_t *widget, option_t *option)
{
    option_value_t *value;

    OPTION_VALUE_FOREACH(value, option) {
        gg_option_append_label(widget, value->name, 0.5f, 0.0f);
    }

    gg_option_set_selected(widget, option->selected->index);
}

static int dialog_cancel_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data )
{
	char *old_config = (char *)extra_data;

	config_restore(old_config);
	free(old_config);

	gg_dialog_close();
	return 1;
}

static int dialog_ok_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data )
{
	int width;
	int height;
	option_t *option;
	char *old_config = (char *)extra_data;

	errno = 0;
	width = strtol(gg_entry_get_text(GG_ENTRY(entry1)), NULL, 10);
	if (!errno) {
		option = config_get_option("custom_resolution_width");
		option->value = width;
	}

	errno = 0;
	height = strtol(gg_entry_get_text(GG_ENTRY(entry2)), NULL, 10);
        if (!errno) {
		option = config_get_option("custom_resolution_height");
		option->value = height;
	}

	gg_dialog_close();

	if (set_resolution(0)) {
		config_restore(old_config);

		gg_dialog_open(dialog_error_create(gg_dialog_get_active(), "Error: failed to change video mode", NULL));
	}

	free(old_config);

	return 1;
}

static int resolution_changed(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
	int nr = gg_option_get_selected(GG_OPTION(widget));
	int size = gg_container_get_size(GG_CONTAINER(widget));
        option_t *option = config_get_option("resolution");

        option_select_value_by_index(option, nr);

	if (nr == size - 1) {
		container->enabled = 1;
		entry1->enabled = 1;
		entry2->enabled = 1;
		label1->enabled = 1;
		label2->enabled = 1;
	} else {
		container->enabled = 0;
		entry1->enabled = 0;
		entry2->enabled = 0;
		label1->enabled = 0;
		label2->enabled = 0;
	}

        return 1;
}

static int fs_changed(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
	int nr = gg_option_get_selected(GG_OPTION(widget));
        option_t *option = config_get_option("full_screen");

        option_select_value_by_index(option, nr);
        return 1;
}

static int multisampling_changed(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
	int nr = gg_option_get_selected(GG_OPTION(widget));
        option_t *option = config_get_option("multisampling");

        option_select_value_by_index(option, nr);
        return 1;
}

gg_dialog_t *dialog_resolution_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox, *vbox2, *hbox;
    gg_widget_t *widget;
    gg_widget_t *resolution;
    option_t *option;
    char val[5];
    char *old_config;

    old_config = config_backup();

    vbox = gg_vbox_create(0);
    vbox2 = gg_vbox_create(0);

    widget = gg_label_create("Resolution:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    label2 = gg_label_create("Custom:");
    gg_align_set_alignment(GG_ALIGN(label2), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label2);

    widget = gg_label_create("Full screen:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    widget = gg_label_create("Multisampling:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    hbox = gg_hbox_create(20);
    gg_container_append(GG_CONTAINER(hbox), vbox2);
    vbox2 = gg_vbox_create(0);

    option = config_get_option("resolution");
    resolution = gg_option_create();
    create_option_values(GG_OPTION(resolution), option);
    gg_widget_subscribe_signal_name(resolution, resolution->id, "option_changed", resolution_changed, NULL);
    gg_container_append(GG_CONTAINER(vbox2), resolution);

    option = config_get_option("custom_resolution_width");
    val[0] = 0;
    snprintf(val, 5, "%d", option->value);
    container = gg_hbox_create(0);
    entry1 = gg_entry_create(60);
    gg_entry_set_text(GG_ENTRY(entry1), val);
    gg_container_append(GG_CONTAINER(container), entry1);
    label1 = gg_label_create(" x ");
    gg_container_append(GG_CONTAINER(container), label1);

    option = config_get_option("custom_resolution_height");
    val[0] = 0;
    snprintf(val, 5, "%d", option->value);
    entry2 = gg_entry_create(60);
    gg_entry_set_text(GG_ENTRY(entry2), val);
    gg_container_append(GG_CONTAINER(container), entry2);
    gg_container_append(GG_CONTAINER(vbox2), container);

    option = config_get_option("full_screen");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", fs_changed, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    option = config_get_option("multisampling");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", multisampling_changed, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

	widget = gg_action_create_with_label("OK", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_ok_cb, old_config);
    gg_container_append(GG_CONTAINER(vbox), widget);
	widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_cancel_cb, old_config);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 23, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    resolution_changed(resolution, NULL, NULL, NULL);

    return GG_DIALOG(dialog);
}
