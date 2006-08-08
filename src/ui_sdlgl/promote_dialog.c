
#include "ui_sdlgl.h"

void dialog_promote_cb(gg_widget_t *widget, void *data)
{
    set_dialog_promote_piece(*(int *)data);
    gg_dialog_close();
}

gg_dialog_t *dialog_promote_create(int colour)
{
    static int cb_pieces[4];

    texture_t *pieces;
    gg_widget_t *dialog;
    gg_widget_t *action;
    gg_widget_t *vbox = gg_vbox_create(0);
    gg_widget_t *hbox = gg_hbox_create(0);
    gg_widget_t *gg_image;
    gg_widget_t *text = gg_label_create("Promotion! Choose new piece!");

    set_dialog_promote_piece(NONE);
    cb_pieces[0] = QUEEN + colour;
    cb_pieces[1] = ROOK + colour;
    cb_pieces[2] = BISHOP + colour;
    cb_pieces[3] = KNIGHT + colour;

    gg_container_append(GG_CONTAINER(vbox), text);

    if (IS_WHITE(colour))
        pieces = get_white_pieces();
    else
        pieces = get_black_pieces();

    gg_image = gg_image_create(&pieces[GUI_PIECE_QUEEN]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[0]);
    gg_container_append(GG_CONTAINER(hbox), action);

    gg_image = gg_image_create(&pieces[GUI_PIECE_ROOK]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[1]);
    gg_container_append(GG_CONTAINER(hbox), action);

    gg_image = gg_image_create(&pieces[GUI_PIECE_BISHOP]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[2]);
    gg_container_append(GG_CONTAINER(hbox), action);

    gg_image = gg_image_create(&pieces[GUI_PIECE_KNIGHT]);
    action = gg_action_create(gg_image);
    gg_action_set_callback(GG_ACTION(action), dialog_promote_cb, &cb_pieces[3]);
    gg_container_append(GG_CONTAINER(hbox), action);
    gg_container_append(GG_CONTAINER(vbox), hbox);

    dialog = gg_dialog_create(vbox, NULL, NULL, 0);
    gg_dialog_set_modal(GG_DIALOG(dialog), 1);
    return GG_DIALOG(dialog);
}
