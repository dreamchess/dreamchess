
#include "dreamchess.h"
#include "gamegui.h"
#include "debug.h"
#include "ui.h"
#include "engine.h"

/* Define our booleans */
#define TRUE  1
#define FALSE 0

gg_dialog_style_t *get_ingame_style();
gg_dialog_style_t *get_menu_style();

gg_dialog_t *dialog_ingame_create(gg_dialog_t *parent);
gg_dialog_t *dialog_system_create();
gg_dialog_t *dialog_quit_create(gg_dialog_t *parent);
gg_dialog_t *dialog_message_create(char *message);
gg_dialog_t *dialog_promote_create(int colour);
gg_dialog_t *dialog_victory_create(result_t *result);
void show_message_dialog( char *message );
