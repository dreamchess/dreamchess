
#include "ui_sdlgl.h"

/** @brief Quits the current game.
 *
 *  Closes the dialog and causes the game to go back to the title menu.
 */
static void dialog_quit_ok(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
    gg_dialog_close();
    DBG_LOG("entering title menu");
    set_quit_to_menu(1);
}

static void dialog_close_cb(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
}

/** The quit dialog. Asks the user to confirm that he wants to quit the game.
 */

/** @brief Creates the quit confirmation dialog.
 *
 *  @return The created dialog.
 */
gg_dialog_t *dialog_quit_create(gg_dialog_t *parent)
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);

    gg_widget_t *widget = gg_label_create("Quit to Title?");
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_label_create("");
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Yes", 0.5f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_quit_ok, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("No", 0.5f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
    return GG_DIALOG(dialog);
}

