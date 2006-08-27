
#include "ui_sdlgl.h"

static int slots;
static char time_save[10][80];
static config_t config_save[10];
static board_t saved_board[10];

board_t *get_saved_board( int index )
{
    return &saved_board[index];
}

config_t *get_config_save( int index )
{
    return &config_save[index];
}

char *get_time_save( int index )
{
    return time_save[index];
}

void set_slots( int slots )
{
    slots=slots;
}

int get_slots()
{
    return slots;
}

int load_game( int slot )
{
    int retval=game_load( slot );

    /*if (!retval)
        show_message("PGN file loading failed.");*/

    return retval;
}

const char *whitespace_cb(mxml_node_t *node, int where)
{
    const char *name;
    name = node->value.element.name;

    if (!strcmp(name, "save"))
    {
        if (where == MXML_WS_AFTER_OPEN)
            return "\n";
    }
    else
    {
        if (where == MXML_WS_BEFORE_OPEN)
            return "\t";
        else if (where == MXML_WS_AFTER_CLOSE)
            return "\n";
    }

    return (NULL);
}

static void save_opaque(mxml_node_t *parent, char *name, char *value)
{
    mxml_node_t *node = mxmlNewElement(parent, name);
    mxmlNewOpaque(node, value);
}

void write_save_xml( int slot, char *desc )
{
    FILE *fp;
    char temp[80];
    mxml_node_t *tree;
    char *fen;

    sprintf( temp, "save%i.xml", slot );

    if (ch_userdir())
    {
        printf("Could not enter user directory.\n");
        return;
    }

    fp = fopen(temp, "w");

    fprintf( fp, "<?xml version=\"1.0\"?>\n" );
    tree = mxmlNewElement( MXML_NO_PARENT, "save" );

    sprintf(temp, "%i", (int) time(NULL));
    save_opaque(tree, "time", temp);

    if (get_config()->player[WHITE] == PLAYER_UI)
        save_opaque(tree, "white", "ui");
    else
        save_opaque(tree, "white", "engine");

    if (get_config()->player[BLACK] == PLAYER_UI)
        save_opaque(tree, "black", "ui");
    else
        save_opaque(tree, "black", "engine");

    sprintf(temp, "%i", get_config()->cpu_level);
    save_opaque(tree, "level", temp);

    fen = fen_encode(get_board());
    if (!fen)
    {
        fprintf(stderr, "Error encoding FEN\n");
    }
    else
    {
        save_opaque(tree, "fen", fen);
        free(fen);
    }

    mxmlSaveFile(tree, fp, whitespace_cb);
    fclose(fp);
}

/*static void load_opaque(mxml_node_t *top, char *name, char *dest);*/
void load_save_xml( int slot, char *desc, int *player_layout, int *difficulty )
{
    FILE *fp;
    char temp[256];
    mxml_node_t *tree, *save;

    sprintf( temp, "save%i.xml", slot );
    /*printf( "Loading %s\n", temp );*/

    if (ch_userdir())
    {
        printf("Could not enter user directory.\n");
        return;
    }

    fp = fopen(temp, "r");
    if (fp)
    {
        tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
        slots |= (1 << slot);
    }
    else
    {
        /*printf( "Error opening theme file.\n" );*/
        sprintf( desc, "Empty." );
        slots &= ~(1 << slot);
        return;
    }

    fclose(fp);

    save = tree;

    while ((save = mxmlFindElement(save, tree, "save", NULL, NULL, MXML_DESCEND)))
    {
        time_t time;
        struct tm *tm;
        board_t *board;

        load_opaque(save, "time", temp);

        time = atoi(temp);
        tm = localtime(&time);
        sprintf(time_save[slot], "%02i/%02i at %02i:%02i.", tm->tm_mday, tm->tm_mon,
                tm->tm_hour, tm->tm_min);

        load_opaque(save, "white", temp);
        if (!strcmp(temp, "ui"))
            config_save[slot].player[WHITE] = PLAYER_UI;
        else
            config_save[slot].player[WHITE] = PLAYER_ENGINE;

        load_opaque(save, "black", temp);
        if (!strcmp(temp, "ui"))
            config_save[slot].player[BLACK] = PLAYER_UI;
        else
            config_save[slot].player[BLACK] = PLAYER_ENGINE;

        load_opaque(save, "level", temp);
        config_save[slot].cpu_level = atoi(temp);

        load_opaque(save, "fen", temp);
        board = fen_decode(temp);
        if (board)
        {
            saved_board[slot] = *board;
            free(board);
        }
    }
}
