
#include "gamegui_dialogs.h"

/** The in-game dialog. Provides a set of gameplay-related actions to the
 *  user.
 */

static int retract_move(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("retracting move");
    game_retract_move();
    return 1;
}

static int move_now(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("forcing dreamer to move");
    game_move_now();
    return 1;
}

static int view_prev(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("viewing previous move");
    game_view_prev();
    return 1;
}

static int view_next(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    DBG_LOG("viewing next move");
    game_view_next();
    return 1;
}

static int dialog_return_to_game(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();

    /* Close parent too..*/
    gg_dialog_close();
    return 1;
}

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
    gg_dialog_close();
    return 1;
}

/** @brief Creates the in-game dialog.
 *
 *  @return The created dialog.
 */
gg_dialog_t *dialog_ingame_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);

    gg_widget_t *widget = gg_action_create_with_label("Return To Game", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_return_to_game, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("  Retract Move", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        retract_move, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("  Move Now", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        move_now, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("  View Previous Move", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        view_prev, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("  View Next Move", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        view_next, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Back..", 0.0f, 0.0f);
    gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed",
        dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
    return GG_DIALOG(dialog);
}
