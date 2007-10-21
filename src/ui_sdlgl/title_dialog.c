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

static int selected_player_layout=0;
static int selected_difficulty=1;
static int selected_level=0;
static int flip_board;

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

void set_selected_player_layout( int set )
{
    selected_player_layout=set;
}

void set_selected_difficulty( int set )
{
    selected_difficulty=set;
}

void set_selected_level( int set )
{
    selected_level=set;
}

/** @brief Triggers gameplay start based on currently selected options. */
static int menu_title_start(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    set_set_loading(TRUE);
    DBG_LOG("starting a new game - difficulty: %i - level: %i - player scheme: %i", 
        selected_difficulty, selected_level, selected_player_layout);

    if ( get_egg_req() )
    {
        set_show_egg(TRUE);
        DBG_LOG("easter egg enabled");
    }

    gg_dialog_close();
    gg_dialog_close();

    switch (selected_player_layout)
    {
    case GAME_TYPE_HUMAN_VS_CPU:
        get_config()->player[WHITE] = PLAYER_UI;
        get_config()->player[BLACK] = PLAYER_ENGINE;
        flip_board = 0;
        break;
    case GAME_TYPE_CPU_VS_HUMAN:
        get_config()->player[WHITE] = PLAYER_ENGINE;
        get_config()->player[BLACK] = PLAYER_UI;
        flip_board = 1;
        break;
    case GAME_TYPE_HUMAN_VS_HUMAN:
        get_config()->player[WHITE] = PLAYER_UI;
        get_config()->player[BLACK] = PLAYER_UI;
        flip_board = 0;
    }

    get_config()->difficulty = selected_difficulty;
    get_config()->cpu_level = selected_level + 1;

    return 1;
}

/** @brief Triggers DreamChess exit. */
static int menu_title_quit(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    set_title_process_retval(1);
    gg_dialog_close();
    return 1;
}

static int dialog_title_players(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    selected_player_layout=gg_option_get_selected(GG_OPTION(widget));
    return 1;
}

static int dialog_title_root_new(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_title_newgame_create(gg_widget_find_dialog(widget)));
    return 1;
}

static int dialog_title_root_load(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_saveload_create(gg_widget_find_dialog(widget), FALSE));
    return 1;
}

static int dialog_title_difficulty(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    selected_difficulty = gg_option_get_selected(GG_OPTION(widget));
    return 1;
}

static int dialog_title_level(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    selected_level=gg_option_get_selected(GG_OPTION(widget));
    return 1;
}

static int dialog_title_open_systemopts(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_systemopts_create(gg_widget_find_dialog(widget)));
    return 1;
}

gg_dialog_t *dialog_title_newgame_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;

    set_pgn_slot(-1);

    widget = gg_action_create_with_label("Start Game", 0.0f, 0.0f);

    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", menu_title_start, NULL);
    vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), widget);

    label = gg_label_create("  Players:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Difficulty:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Level:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

   /* label = gg_label_create("  Theme:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);*/

    hbox = gg_hbox_create(20);
    gg_container_append(GG_CONTAINER(hbox), vbox2);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Human vs. CPU", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "CPU vs. Human", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Human vs. Human", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_players, NULL);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_player_layout);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Easy", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Normal", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_difficulty, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_difficulty);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "1", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "2", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "3", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "4", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "5", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "6", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "7", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "8", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_level, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_level);

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Back..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 63, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_title_root_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;

    vbox = gg_vbox_create(0);
    widget = gg_action_create_with_label("New Game..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_title_root_new, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Load Game..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_title_root_load, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Configuration..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_title_open_systemopts, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);


    widget = gg_action_create_with_label("Quit", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", menu_title_quit, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 23, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    return GG_DIALOG(dialog);
}

void open_title_root_dialog()
{
    gg_dialog_t *title=dialog_title_root_create();

    gg_dialog_open(title);
}
