
#include "ui_sdlgl.h"

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

gg_dialog_t *dialog_systemopts_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox,*hbox;
    gg_widget_t *widget;

    vbox = gg_vbox_create(0);

    hbox = gg_hbox_create(20);
    widget = gg_label_create("Music:  ");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(hbox), widget);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "On", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "Off", 0.5f, 0.0f);
    //gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", NULL, NULL);
    gg_container_append(GG_CONTAINER(hbox), widget);
    gg_container_append(GG_CONTAINER(vbox), hbox);
    //gg_option_set_selected(GG_OPTION(widget),selected_level);

    hbox = gg_hbox_create(20);
    widget = gg_label_create("Volume:");
    gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
    gg_container_append(GG_CONTAINER(hbox), widget);

    widget = gg_option_create();
    gg_option_append_label(GG_OPTION(widget), "0", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "1", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "2", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "3", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "4", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "5", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "6", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "7", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "8", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "9", 0.5f, 0.0f);
    gg_option_append_label(GG_OPTION(widget), "10", 0.5f, 0.0f);
    //gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", NULL, NULL);
    gg_container_append(GG_CONTAINER(hbox), widget);
    gg_container_append(GG_CONTAINER(vbox), hbox);
    //gg_option_set_selected(GG_OPTION(widget),selected_level);

    #ifdef __unix__
    widget = gg_action_create_with_label("Toggle Fullscreen", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_title_toggle_fullscreen, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);
    #endif

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
