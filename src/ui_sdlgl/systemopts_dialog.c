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

#include "ui_sdlgl.h"
#include "options.h"
#include "system_config.h"

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

static int dialog_title_toggle_fullscreen(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG( "toggled fullscreen" );
    toggle_fullscreen();
    return 1;
}

static int dialog_title_save_options(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    config_save();
    return 1;
}

static int dialog_title_theme(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    option_t *option = config_get_option("theme");
    option_select_value_by_index(option, gg_option_get_selected(GG_OPTION(widget)));
    return 1;
}

static void create_option_values(gg_option_t *widget, option_t *option)
{
    option_value_t *value;

    OPTION_VALUE_FOREACH(value, option) {
        gg_option_append_label(widget, value->name, 0.5f, 0.0f);
    }

    gg_option_set_selected(widget, option->selected->index);
}

gg_dialog_t *dialog_systemopts_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox, *vbox2, *hbox;
    gg_widget_t *widget;
    gg_option_t *options;
    option_t *option;
    option_value_t *value;

    vbox = gg_vbox_create(0);
    vbox2 = gg_vbox_create(0);

    widget = gg_label_create("Theme:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    widget = gg_label_create("Music Volume:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    widget = gg_label_create("Sound Volume:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    hbox = gg_hbox_create(20);
    gg_container_append(GG_CONTAINER(hbox), vbox2);
    vbox2 = gg_vbox_create(0);

    option = config_get_option("theme");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    option = config_get_option("music_volume");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    //gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", NULL, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    //gg_option_set_selected(GG_OPTION(widget),selected_level);

    option = config_get_option("sound_volume");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    //gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", NULL, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    //gg_option_set_selected(GG_OPTION(widget),selected_level);

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    #ifdef __unix__
    widget = gg_action_create_with_label("Toggle Fullscreen", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_title_toggle_fullscreen, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    #endif

    widget = gg_action_create_with_label("Save options", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_title_save_options, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Back...", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", 
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 23, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    return GG_DIALOG(dialog);
}
