
#include "ui_sdlgl.h"

/* System dialog. */

static void dialog_close_cb(gg_widget_t *widget, void *data)
{
    gg_dialog_close();
}

/** The system dialog. Provides a set of system-related actions to the user.
 *  Currently this dialog only contains an item to quit the game. In the
 *  future this will be extended with load/save game items and possibly
 *  other items as well.
 */

/** @brief Opens the quit dialog. */
static void dialog_quit_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_quit_create(gg_widget_find_dialog(widget)));
}

/** @brief Opens the ingame dialog. */
static void dialog_ingame_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_ingame_create(gg_widget_find_dialog(widget)));
}

static void dialog_savegame_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_saveload_create(gg_widget_find_dialog(widget), TRUE));
}

/*static void dialog_loadgame_open(gg_widget_t *widget, void *data)
{
    gg_dialog_open(dialog_saveload_create(gg_widget_find_dialog(widget), FALSE));
}*/

/** @brief Creates the system dialog.
 *
 *  @return The created dialog.
 */
gg_dialog_t *dialog_system_create()
{
    gg_widget_t *dialog;
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *widget;

    widget = gg_action_create_with_label("Return To Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_close_cb, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("  Save game..", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_savegame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    /*widget = gg_action_create_with_label("  Load game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_loadgame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);*/

    widget = gg_action_create_with_label("  Move Options..", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_ingame_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    widget = gg_action_create_with_label("Quit Game", 0.0f, 0.0f);
    gg_action_set_callback(GG_ACTION(widget), dialog_quit_open, NULL);
    gg_container_append(GG_CONTAINER(vbox), widget);

    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
    return GG_DIALOG(dialog);
}
