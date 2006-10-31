
#include "gamegui_dialogs.h"

static gg_dialog_style_t style_ingame, style_menu;

gg_dialog_style_t *get_ingame_style()
{
    return &style_ingame;
}

gg_dialog_style_t *get_menu_style()
{
    return &style_menu;
}
