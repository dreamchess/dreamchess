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

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

int saveload_selected=0;
int changing_slot=FALSE;
int change_saving=FALSE;

char xmlsquaretofont( int square )
{

    switch(PIECE(square))
    {
    case KING:
        return CHAR_KING;
    case QUEEN:
        return CHAR_QUEEN;
    case ROOK:
        return CHAR_ROOK;
    case KNIGHT:
        return CHAR_KNIGHT;
    case BISHOP:
        return CHAR_BISHOP;
    case PAWN:
        return CHAR_PAWN;
    }

    return ' ';
}

static int dialog_loadgame_load(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    int slot = saveload_selected;

    if (get_slots() & (1 << slot))
    {
        set_pgn_slot(slot);
        set_set_loading(TRUE);
        gg_dialog_close();
        gg_dialog_close();
    }
    return 1;
}

static int dialog_saveload_change(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_widget_t *select = GG_WIDGET(extra_data);
    saveload_selected=GG_OPTION(select)->sel;

    gg_dialog_close();
    changing_slot=TRUE;
    gg_dialog_open(dialog_saveload_create(gg_widget_find_dialog(widget)->parent_dialog, change_saving));
    changing_slot=FALSE;
    return 1;
}

static int dialog_savegame_save(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    char temp[80];
    int save_good=TRUE;

    /* Close the dialogs.. */
    gg_dialog_close();
    gg_dialog_close();

    if (!game_save( saveload_selected ))
    {
        write_save_xml( saveload_selected );
    }
    else
        save_good=FALSE;

#ifdef _arch_dreamcast
    if (dc_store_savegames())
        save_good=FALSE;
#endif

    if ( save_good )
        show_message_dialog( "Save successful" );
    else
        show_message_dialog( "Save failed" );

    return 1;
}

gg_dialog_t *dialog_saveload_create(gg_dialog_t *parent, int saving)
{
    gg_widget_t *dialog;
    gg_widget_t *rootvbox = gg_vbox_create(0);
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *hbox = gg_vbox_create(0);
    gg_widget_t *hboxtemp;
    gg_widget_t *widget;
    char temp[80];
    char whiteis[80], blackis[80];
    int i=0,j=0;
    int padding=0;

    change_saving=saving;

    if ( !changing_slot )
        saveload_selected=0;

    /*DBG_LOG( "dialog opened with saveselected of %i", saveload_selected );*/

    /* Right side.. */
    if (!changing_slot)
    {
#ifdef _arch_dreamcast
        dc_restore_savegames();
#endif
        for ( i=0; i<SAVEGAME_SLOTS; i++ )
            load_save_xml( i );
    }

    if ( get_slots() & (1 << saveload_selected) )
    {
        gg_widget_t *board_box = gg_vbox_create(0);

        sprintf( temp, "Saved: %s", get_time_save(saveload_selected) );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        switch ( get_config_save(saveload_selected)->player[WHITE] )
        {
        case PLAYER_ENGINE:
            sprintf( whiteis, "CPU" );
            break;
        case PLAYER_UI:
            sprintf( whiteis, "Human" );
            break;
        default:
            /* Whoops */
            sprintf( whiteis, "Oh no.." );
            break;
        }

        switch ( get_config_save(saveload_selected)->player[BLACK] )
        {
        case PLAYER_ENGINE:
            sprintf( blackis, "CPU" );
            break;
        case PLAYER_UI:
            sprintf( blackis, "Human" );
            break;
        default:
            /* Whoops */
            sprintf( blackis, "Oh no.." );
            break;
        }

        sprintf( temp, "%s vs %s", whiteis, blackis );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        sprintf( temp, "Difficulty: %s",
            get_config_save(saveload_selected)->difficulty ? "Normal" : "Easy" );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        sprintf( temp, "Level: %i",
            get_config_save(saveload_selected)->cpu_level );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        widget = gg_label_create(" ");
        gg_container_append(GG_CONTAINER(vbox), widget);

        /* create board.. */

        for ( i=7; i>=0; i-- )
        {
            gg_widget_t *hboxtemp2;
            gg_colour_t col_white =
                {
                    1.0f, 1.0f, 1.0f, 1.0f
                };
            /*gg_colour_t col_grey =
                {
                    0.3f, 0.3f, 0.3f, 1.0f
                };*/
            hboxtemp = gg_hbox_create(0);
            hboxtemp2 = gg_hbox_create(0);
            gg_set_requested_size(hboxtemp2, 20, 20);
            gg_container_append(GG_CONTAINER(hboxtemp), hboxtemp2);

            for ( j=0; j<8; j++ )
            {
                gg_colour_t col_green = {0.5, 0.6, 0.5, 1.0};
                gg_colour_t col_yellow = {0.8, 0.7, 0.4, 1.0};
                gg_colour_t *front, *back;
                int square = get_saved_board(saveload_selected)->square[i * 8 + j];

                sprintf(temp, "%c", xmlsquaretofont(square));
                widget = gg_label_create( temp );
                gg_set_requested_size(widget, 20, 20);
                gg_align_set_alignment(GG_ALIGN(widget), 0.5, 0.5);

                if (COLOUR(square) == WHITE)
                    front = &col_white;
                else
                    front = get_col(COL_BLACK);

                if ((i + j) % 2 == 0)
                    back = &col_green;
                else
                    back = &col_yellow;

                gg_label_set_colour(GG_LABEL(widget), front, back);
                gg_container_append(GG_CONTAINER(hboxtemp), widget);
            }
            gg_container_append(GG_CONTAINER(board_box), hboxtemp);
        }
        gg_container_append(GG_CONTAINER(vbox), board_box);
    }
    else
    {
        sprintf( temp, "Empty slot" );
        widget = gg_label_create(temp);
        gg_container_append(GG_CONTAINER(vbox), widget);

        for ( i=0; i<12; i++ )
        {
            widget = gg_label_create(" ");
            gg_container_append(GG_CONTAINER(vbox), widget);
        }
    }
    gg_set_requested_size(vbox, 201, 0);
    gg_container_append(GG_CONTAINER(hbox), gg_frame_create(vbox));

    /* left side */
    vbox = gg_vbox_create(0);
    /* padding.. */
    for ( i=0; i<padding; i++ )
    {
        widget = gg_label_create(" ");
        gg_container_append(GG_CONTAINER(vbox), widget);
    }

    if ( saving )
    {
        widget = gg_action_create_with_label("Save Game", 0.0f, 0.0f);
        gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", 
            dialog_savegame_save, vbox);
    }
    else
    {
        widget = gg_action_create_with_label("Load Game", 0.0f, 0.0f);
        gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", 
            dialog_loadgame_load, vbox);
    }
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_option_create();
    for ( i=0; i<SAVEGAME_SLOTS; i++ )
    {
        sprintf( temp, "Save slot: %i", i+1 );
        gg_option_append_label(GG_OPTION(widget), temp, 0.5f, 0.0f);
    }
    gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", 
        dialog_saveload_change, widget);
    gg_container_append(GG_CONTAINER(vbox), widget);

    if ( changing_slot )
        gg_option_set_selected(GG_OPTION(widget), saveload_selected);

    widget = gg_action_create_with_label("Back..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", 
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    /*for ( i=0; i<SAVEGAME_SLOTS; i++ )
    {
        sprintf( temp, "%i:  ", i );
        widget = gg_action_create_with_label(temp, 0.0f, 0.0f);

        gg_action_set_callback(GG_ACTION(widget), dialog_saveload_change, vbox);

        gg_container_append(GG_CONTAINER(vbox), widget);
    }*/

    gg_container_append(GG_CONTAINER(hbox), vbox );

    if ( changing_slot )
        gg_vbox_set_selected(vbox, padding+1 );

    /* Dialog stuff */
    gg_container_append(GG_CONTAINER(rootvbox), hbox);
    dialog = gg_dialog_create(rootvbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);

    if ( saving )
        gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
    else
        gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

    return GG_DIALOG(dialog);
}
