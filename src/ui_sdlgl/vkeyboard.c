
#include "ui_sdlgl.h"

static ui_event_t keys[94];
static int vkeyboard_enabled = 0;

void populate_key_table()
{
    int i;

    for ( i=0; i<94; i++ )
    {
        keys[i]=i+33;
    }
}

void toggle_vkeyboard_enabled()
{
    vkeyboard_enabled = 1 - vkeyboard_enabled;
}

int get_vkeyboard_enabled()
{
    return vkeyboard_enabled;
}

static void dialog_vkeyboard_key(gg_widget_t *widget, void *data)
{
    if (gg_dialog_get_active())
    {
        gg_event_t event;
        event.type = GG_EVENT_KEY;
        event.key = *((int *) data);
        gg_dialog_input_current(event);
    }

    printf( "Pressed a keyyy... it was uh.. '%c' .. right?\n\r", *(ui_event_t *)data);
}

gg_dialog_t *dialog_vkeyboard_create()
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
                gg_action_set_callback(GG_ACTION(action), dialog_vkeyboard_key, &keys[k]);

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
