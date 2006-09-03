/*  DreamChess
 *  Copyright (C) 2003-2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ui_sdlgl.h"

static int selected_player_layout=0;
static int selected_difficulty=0;
static int swapping_custom=FALSE;
static int selected_custom_board=0;
static int selected_custom_style=0;
static int selected_custom_pieces=0;
static int flip_board;
static int cur_style;
static int pieces_list_cur;
static int board_list_cur;

static void dialog_close_cb(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
}

void set_selected_player_layout( int set )
{
    selected_player_layout=set;
}

void set_selected_difficulty( int set )
{
    selected_difficulty=set;
}

int get_pieces_list_cur()
{
    return pieces_list_cur;
}

int get_board_list_cur()
{
    return board_list_cur;
}

int get_cur_style()
{
    return cur_style;
}

/** @brief Triggers gameplay start based on currently selected options. */
static void menu_title_start(gg_widget_t *widget, void *data)
{
    set_set_loading(TRUE);

    gg_dialog_close();
    gg_dialog_close();
}

/** @brief Triggers DreamChess exit. */
static void menu_title_quit(gg_widget_t *widget, void *data)
{
    set_title_process_retval(1);
    gg_dialog_close();
}

void dialog_title_players(gg_widget_t *widget, void *data)
{
    selected_player_layout=gg_option_get_selected(GG_OPTION(widget));

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
}

static void dialog_title_root_new(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_title_newgame_create(gg_widget_find_dialog(widget)));
}

static void dialog_title_root_load(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_saveload_create(gg_widget_find_dialog(widget), FALSE));
}

static void dialog_title_root_select_theme(gg_widget_t *widget, void *data)
{
    /* If created, and theme set to custom.. open custom.. */
    if ( get_selected_theme() == get_theme_count() )
         gg_dialog_open(dialog_title_custom_create(gg_widget_find_dialog(widget)));
    else
        gg_dialog_open(dialog_title_select_theme_create(gg_widget_find_dialog(widget)));
}

static void dialog_title_level(gg_widget_t *widget, void *data)
{
    get_config()->cpu_level = gg_option_get_selected(GG_OPTION(widget)) + 1;
    selected_difficulty=get_config()->cpu_level-1;
}

static void dialog_title_custom_theme(gg_widget_t *widget, void *data)
{
    set_selected_theme( gg_option_get_selected(GG_OPTION(widget)) );
    if ( gg_option_get_selected(GG_OPTION(widget)) != get_theme_count() )
    {
        /* printf( "Theme changed from Custom!\n" ); */
        gg_dialog_close();
        swapping_custom=TRUE;
        gg_dialog_open(dialog_title_select_theme_create(gg_widget_find_dialog(widget)->parent_dialog));
        swapping_custom=FALSE;
    }
}

static void dialog_title_theme(gg_widget_t *widget, void *data)
{
    set_selected_theme( gg_option_get_selected(GG_OPTION(widget)) );
    if ( gg_option_get_selected(GG_OPTION(widget)) == get_theme_count() )
    {
        /* printf( "Theme changed to Custom!\n" ); */
        gg_dialog_close();
        swapping_custom=TRUE;
        gg_dialog_open(dialog_title_custom_create(gg_widget_find_dialog(widget)->parent_dialog));
        swapping_custom=FALSE;
        gg_option_set_selected(GG_OPTION(widget),get_theme_count()-1);
        gg_dialog_cleanup();
    }
}

static void dialog_title_style(gg_widget_t *widget, void *data)
{
    cur_style = gg_option_get_selected(GG_OPTION(widget));
    selected_custom_style=cur_style;
}

static void dialog_title_pieces(gg_widget_t *widget, void *data)
{
    pieces_list_cur = gg_option_get_selected(GG_OPTION(widget));
    selected_custom_pieces=pieces_list_cur;
}

static void dialog_title_board(gg_widget_t *widget, void *data)
{
    board_list_cur = gg_option_get_selected(GG_OPTION(widget));
    selected_custom_board=board_list_cur;
}

gg_dialog_t *dialog_title_custom_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;
    int i;

    get_config()->player[WHITE] = PLAYER_UI;
    get_config()->player[BLACK] = PLAYER_ENGINE;
    get_config()->cpu_level = selected_difficulty;
    cur_style = selected_custom_style;
    pieces_list_cur = selected_custom_pieces;
    board_list_cur = selected_custom_board;
    flip_board = 0;
    set_pgn_slot(-1);

    vbox = gg_vbox_create(0);
    vbox2 = gg_vbox_create(0);
    /*widget = gg_action_create_with_label("Start Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), menu_title_start, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    label = gg_label_create("  Players:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Difficulty:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);*/

    label = gg_label_create("  Theme:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("    Style:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("    Pieces:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("    Board:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    /*   label = gg_label_create("Name:");
       gg_alignable_set_alignment(GG_ALIGNABLE(label), 0.0f, 0.0f);
       gg_container_append(GG_CONTAINER(vbox2), label);*/

    hbox = gg_hbox_create(20);
    gg_set_requested_size(vbox2, 100, 0);
    gg_container_append(GG_CONTAINER(hbox), vbox2);
    vbox2 = gg_vbox_create(0);

   /* widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Human vs. CPU", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "CPU vs. Human", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Human vs. Human", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_players, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_player_layout);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Level 1", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 2", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 3", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 4", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_level, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_difficulty);*/

    /* Themelist list.. */
    widget = gg_option_create();
    for (i = 0; i < get_theme_count()+1; i++)
        gg_option_append_label(GG_OPTION(widget), get_themelist(i), 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_custom_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),get_theme_count());

    widget = gg_option_create();
    for (i = 0; i < get_num_style(); i++)
        gg_option_append_label(GG_OPTION(widget), get_stylelist(i), 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_style, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_custom_style);

    widget = gg_option_create();
    for (i = 0; i < get_pieces_list_total(); i++)
        gg_option_append_label(GG_OPTION(widget), get_pieces_list(i), 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_pieces, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_custom_pieces);

    widget = gg_option_create();
    for (i = 0; i < get_board_list_total(); i++)
        gg_option_append_label(GG_OPTION(widget), get_board_list(i), 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_board, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_custom_board);

    gg_set_requested_size(vbox2, 250, 0);
    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Back", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 0, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    /*if ( swapping_custom )
    {
        gg_vbox_set_selected(vbox, 0 );
        gg_vbox_set_selected(vbox2, 0 );
    }*/

    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_title_select_theme_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;
    int i;

    get_config()->player[WHITE] = PLAYER_UI;
    get_config()->player[BLACK] = PLAYER_ENGINE;
    get_config()->cpu_level = 1;
    cur_style = 0;
    pieces_list_cur = 0;
    board_list_cur = 0;
    flip_board = 0;
    set_pgn_slot(-1);

    vbox = gg_vbox_create(0);
    vbox2 = gg_vbox_create(0);

    label = gg_label_create("  Theme:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(vbox2), label);

    hbox = gg_hbox_create(20);
    gg_set_requested_size(vbox2, 100, 0);
    gg_container_append(GG_CONTAINER(hbox), vbox2);
    vbox2 = gg_vbox_create(0);

    /* Themelist list.. */
    widget = gg_option_create();
    for (i = 0; i < get_theme_count()+1; i++)
        gg_option_append_label(GG_OPTION(widget), get_themelist(i), 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),get_selected_theme());

    gg_set_requested_size(vbox2, 250, 0);
    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Back", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 63, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    /*if ( swapping_custom )
    {
        gg_vbox_set_selected(vbox, 1 );
        gg_vbox_set_selected(vbox2, 2 );
    }*/

    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_title_newgame_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;
    gg_widget_t *vbox2;
    gg_widget_t *hbox;
    gg_widget_t *label;

    get_config()->player[WHITE] = PLAYER_UI;
    get_config()->player[BLACK] = PLAYER_ENGINE;
    get_config()->cpu_level = 1;
    cur_style = 0;
    pieces_list_cur = 0;
    board_list_cur = 0;
    flip_board = 0;
    set_pgn_slot(-1);

    widget = gg_action_create_with_label("Start Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), menu_title_start, NULL);
    vbox = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox), widget);

    label = gg_label_create("  Players:");
    gg_align_set_alignment(GG_ALIGN(label), 0.0f, 0.0f);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), label);

    label = gg_label_create("  Difficulty:");
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
    gg_option_set_callback(GG_OPTION(widget), dialog_title_players, NULL);
    vbox2 = gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_player_layout);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "Level 1", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 2", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 3", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 4", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 5", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 6", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 7", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Level 8", 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_level, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),selected_difficulty);

    /* Themelist list.. */
   /* widget = gg_option_create();
    for (i = 0; i < get_theme_count()+1; i++)
        gg_option_append_label(GG_OPTION(widget), get_themelist(i), 0.5f, 0.0f);
    gg_option_set_callback(GG_OPTION(widget), dialog_title_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox2), widget);
    gg_option_set_selected(GG_OPTION(widget),get_selected_theme());*/

    gg_container_append(GG_CONTAINER(hbox), vbox2);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    widget = gg_action_create_with_label("Back", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_position(GG_DIALOG(dialog), 320, 63, 0.5f, 0.0f);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    if ( swapping_custom )
    {
        gg_vbox_set_selected(vbox, 1 );
        gg_vbox_set_selected(vbox2, 2 );
    }

    /* re-apply configs to match dialog.. */
    get_config()->cpu_level = selected_difficulty+1;
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

    return GG_DIALOG(dialog);
}

gg_dialog_t *dialog_title_root_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox;
    gg_widget_t *widget;

    get_config()->player[WHITE] = PLAYER_UI;
    get_config()->player[BLACK] = PLAYER_ENGINE;
    get_config()->cpu_level = 1;
    cur_style = 0;
    pieces_list_cur = 0;
    board_list_cur = 0;
    flip_board = 0;

    vbox = gg_vbox_create(0);
    widget = gg_action_create_with_label("New Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_title_root_new, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Load Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_title_root_load, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Select Theme", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_title_root_select_theme, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Quit", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), menu_title_quit, NULL);
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
