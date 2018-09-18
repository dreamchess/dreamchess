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

#include "ui_sdlgl.h"

static ui_event_t keys[94];
static int vkeyboard_enabled = 0;

void populate_key_table(void)
{
    int i;

    for ( i=0; i<94; i++ )
    {
        keys[i]=i+33;
    }
}

void toggle_vkeyboard_enabled(void)
{
    vkeyboard_enabled = 1 - vkeyboard_enabled;
}

int get_vkeyboard_enabled(void)
{
    return vkeyboard_enabled;
}

static int dialog_vkeyboard_key(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data )
{
    if (gg_dialog_get_active())
    {
        gg_event_t event;
        event.type = GG_EVENT_KEY;
        event.key = *((int *) extra_data);
        gg_dialog_input_current(event);
    }

    printf( "Pressed a keyyy... it was uh.. '%c' .. right?\n\r", *(ui_event_t *)data);

    return 1;
}

gg_dialog_t *dialog_vkeyboard_create(void)
{
    gg_widget_t *dialog;
    gg_widget_t *label;
    gg_widget_t *action;
    gg_widget_t *hbox;
    gg_widget_t *vbox2;
    static ui_event_t key;
    int i,j,k;
    int max_width = 0;

    hbox=gg_hbox_create(0);
    label=gg_label_create("Type stuff, k?" );
    gg_container_append(GG_CONTAINER(hbox), label);
    vbox2=gg_vbox_create(0);
    gg_container_append(GG_CONTAINER(vbox2), hbox );

    for (i = 0; i < 256; i++)
    {
        int cur_width = get_text_character(i)->width;
        if (cur_width > max_width)
            max_width = cur_width;
    }

    k=0;
    for ( j=0; j<6; j++ )
    {
        hbox = gg_hbox_create(2);
        for ( i=0; i<16; i++ )
        {
            if ( k < 127-33 )
            {
                char key_str[2];
                key=k+33;
                key_str[0] = key;
                key_str[1] = '\0';
                action = gg_action_create_with_label(key_str, 0.5f, 0.5f);
                gg_set_requested_size(action, max_width, 0);
                gg_widget_subscribe_signal_name(action, action->id, "action_pressed",
                    dialog_vkeyboard_key, &keys[k]);;

                gg_container_append(GG_CONTAINER(hbox), action);
                k++;
            }
        }
        gg_container_append(GG_CONTAINER(vbox2), hbox);
    }

    dialog = gg_dialog_create(vbox2, NULL, NULL, 0);
    gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());
    return GG_DIALOG(dialog);
}
