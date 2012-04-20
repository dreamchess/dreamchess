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
#include "gamegui_dialogs.h"
#include "news.h"
#include "browser.h"

static gg_dialog_style_t style_news;

static void create_option_values(gg_option_t *widget, option_t *option)
{
    option_value_t *value;

    OPTION_VALUE_FOREACH(value, option) {
        gg_option_append_label(widget, value->name, 0.5f, 0.0f);
    }

    gg_option_set_selected(widget, option->selected->index);
}

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

/** @brief Triggers gameplay start based on currently selected options. */
static int menu_title_start(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    int selected_difficulty, selected_level, selected_player_layout;
    option_t *option;

    set_set_loading(TRUE);

    option = config_get_option("game_mode");
    selected_player_layout = option->selected->index;
    option = config_get_option("difficulty");
    selected_difficulty = option->selected->index;
    option = config_get_option("level");
    selected_level = option->selected->index;

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
        break;
    case GAME_TYPE_CPU_VS_HUMAN:
        get_config()->player[WHITE] = PLAYER_ENGINE;
        get_config()->player[BLACK] = PLAYER_UI;
        break;
    case GAME_TYPE_HUMAN_VS_HUMAN:
        get_config()->player[WHITE] = PLAYER_UI;
        get_config()->player[BLACK] = PLAYER_UI;
    }

    get_config()->difficulty = selected_difficulty;
    get_config()->cpu_level = selected_level + 1;

    config_save();

    return 1;
}

/** @brief Triggers DreamChess exit. */
static int menu_title_quit(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    set_title_process_retval(1);
    gg_dialog_close();
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

static int dialog_title_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    option_t *option = config_get_option((char *) extra_data);
    option_select_value_by_index(option, gg_option_get_selected(GG_OPTION(widget)));
    return 1;
}

static int dialog_title_open_systemopts(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_systemopts_create(gg_widget_find_dialog(widget)));
    return 1;
}

static int dialog_title_time_load(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_time_create(gg_widget_find_dialog(widget)));
    return 1;
}

static int dialog_title_chat_load(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_chat_create(gg_widget_find_dialog(widget)));
    return 1;
}

static int dialog_title_network(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_open(dialog_network_create(gg_widget_find_dialog(widget)));
    return 1;
}

static int dialog_title_credits(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
	set_show_credits(1);
    return 1;
}

static int dialog_title_url_open(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
	browser_open((char *)extra_data);
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
    option_t *option;

    set_pgn_slot(-1);

    vbox = gg_vbox_create(0);

    label = gg_label_create("Players:");
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("Difficulty:");
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("Level:");
    gg_container_append(GG_CONTAINER(vbox2), label);

    hbox = gg_hbox_create(20);
    gg_container_append(GG_CONTAINER(hbox), vbox2);

    option = config_get_option("game_mode");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_cb, "game_mode");
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), widget);

    option = config_get_option("difficulty");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_cb, "difficulty");
    gg_container_append(GG_CONTAINER(vbox2), widget);

    option = config_get_option("level");
    widget = gg_option_create();
    create_option_values(GG_OPTION(widget), option);
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_cb, "level");
    gg_container_append(GG_CONTAINER(vbox2), widget);

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Time options..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_title_time_load, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Start Game", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", menu_title_start, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Cancel", 0.5f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    gg_vbox_set_selected(vbox, 2);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 63, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_title_news_create(news_item *news, int count)
{
	style_news.textured = 0;
	style_news.fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.0f);
	style_news.hor_pad = 0;
	style_news.vert_pad = 0;

	gg_widget_t *ticker = gg_ticker_create(640, 50);
	gg_widget_t *label = gg_label_create("DreamChess News");
	gg_colour_t colour = { 1.0f, 0.2f, 0.2f, 1.0f };
	gg_label_set_colour(label, &colour, NULL);
	gg_container_append(GG_CONTAINER(ticker), label);

	int i;
	for (i = 0; i < count; i++)
	{
		char timestr[20];
		if (!strftime(timestr, 20, "%d %b %Y - ", &news[i].time))
			timestr[0] = 0;

		char *str = malloc(strlen(news[i].title) + strlen(timestr) + 1);
		strcpy(str, timestr);
		strcat(str, news[i].title);

		label = gg_action_create_with_label(str, 0.0f, 0.0f);
		free(str);
		gg_widget_subscribe_signal_name(label, label->id, "action_pressed", dialog_title_url_open, news[i].link);
		gg_container_append(GG_CONTAINER(ticker), label);
	}

    gg_widget_t *dialog = gg_dialog_create(ticker, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_position(GG_DIALOG(dialog), 0, 2, 0.0f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_news);

    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_title_icons_create(texture_t *home, texture_t *fb, texture_t *twitter)
{
	style_news.textured = 0;
	style_news.fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.0f);
	style_news.hor_pad = 0;
	style_news.vert_pad = 0;

	gg_widget_t *vbox = gg_vbox_create(5);

	gg_widget_t *image = gg_action_create(gg_image_create(home));
	gg_widget_subscribe_signal_name(image, image->id, "action_pressed", dialog_title_url_open, "http://www.dreamchess.org");
    gg_container_append(GG_CONTAINER(vbox), image);

	image = gg_action_create(gg_image_create(fb));
	gg_widget_subscribe_signal_name(image, image->id, "action_pressed", dialog_title_url_open, "http://www.facebook.com");
    gg_container_append(GG_CONTAINER(vbox), image);

	image = gg_action_create(gg_image_create(twitter));
	gg_widget_subscribe_signal_name(image, image->id, "action_pressed", dialog_title_url_open, "http://www.twitter.com");
    gg_container_append(GG_CONTAINER(vbox), image);

    gg_widget_t *dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_position(GG_DIALOG(dialog), 8, 472, 0.0f, 1.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), &style_news);

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

    widget = gg_action_create_with_label("Network Game..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_title_network, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Configuration..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_title_open_systemopts, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Chat (test)", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_title_chat_load, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Credits", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_title_credits, NULL);
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
