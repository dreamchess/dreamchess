#include "gamegui_dialogs.h"

static void dialog_close_cb(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
}

gg_dialog_t *dialog_message_create(char *message)
{
    gg_widget_t *dialog;
    gg_widget_t *widget;

    gg_widget_t *vbox = gg_vbox_create(0);
    /*gg_container_append(GG_CONTAINER(vbox), gg_label_create("Important message from engine"));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(""));*/
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(message));
    gg_container_append(GG_CONTAINER(vbox), gg_label_create(""));
    widget = gg_action_create_with_label("Ok", 0.5f, 0.5f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());

    return GG_DIALOG(dialog);
}

void show_message_dialog( char *message )
{
    gg_dialog_open(dialog_message_create(message));
}
