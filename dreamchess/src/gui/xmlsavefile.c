/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
**  with this source distribution.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ui_sdlgl.h"

static int slots;
static char time_save[SAVEGAME_SLOTS][80];
static config_t config_save[SAVEGAME_SLOTS];
static board_t saved_board[SAVEGAME_SLOTS];

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

int get_slots(void)
{
    return slots;
}

static int load_opaque(mxml_node_t *top, char *name, char *dest)

{

    mxml_node_t *node = mxmlFindElement(top, top, name, NULL, NULL, MXML_DESCEND);

    if (node)

    {

        node = mxmlWalkNext(node, node, MXML_DESCEND);

        if (node && node->type == MXML_OPAQUE)

        {

            strcpy(dest, node->value.opaque);

            return 0;

        }

    }

    return 1;

}

int write_save_xml(int slot)
{
    FILE *fp;
    char temp[80];
    char *fen;
    int retval = 0;

    snprintf( temp, sizeof(temp), "save%i.xml", slot );

    if (ch_userdir())
    {
        DBG_WARN("could not enter user directory");
        return -1;
    }

    fp = fopen(temp, "w");

    if (!fp)
        return -1;

    DBG_LOG("writing save xml: %s", temp );

    fputs("<?xml version=\"1.0\"?>\n<save>\n", fp);
    fprintf(fp, "<time>%i</time>\n<white>", (int)time(NULL));

    if (get_config()->player[WHITE] == PLAYER_UI)
        fputs("ui", fp);
    else
        fputs("engine", fp);

    fputs("</white>\n<black>", fp);

    if (get_config()->player[BLACK] == PLAYER_UI)
        fputs("ui", fp);
    else
        fputs("engine", fp);

    fputs("</black>\n", fp);

    fprintf(fp, "<level>%i</level>\n", get_config()->cpu_level);
    fprintf(fp, "<difficulty>%i</difficulty>\n", get_config()->difficulty);

    fen = fen_encode(get_board());
    if (!fen)
    {
        DBG_ERROR("error encoding FEN");
        retval = -1;
    }
    else
    {
        fprintf(fp, "<fen>%s</fen>\n", fen);
        free(fen);
    }

    fputs("</save>\n", fp);

    if (ferror(fp)) {
        DBG_ERROR("error writing XML file for savegame in slot %d", slot);
        retval = -1;
    }

    fclose(fp);

    return retval;
}

/*static void load_opaque(mxml_node_t *top, char *name, char *dest);*/
void load_save_xml( int slot )
{
    FILE *fp;
    char temp[256];
    mxml_node_t *tree, *save;

    snprintf( temp, sizeof(temp), "save%i.xml", slot );

    if (ch_userdir())
    {
        DBG_WARN("could not enter user directory");
        return;
    }

    fp = fopen(temp, "r");
    if (fp)
    {
        DBG_LOG("reading save xml: %s", temp );
        tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
        slots |= (1 << slot);
    }
    else
    {
        /*printf( "Error opening theme file.\n" );*/
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
        snprintf(time_save[slot], sizeof(time_save[slot]), "%02i/%02i at %02i:%02i.", tm->tm_mday, tm->tm_mon+1,
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

        if (!load_opaque(save, "difficulty", temp))
            config_save[slot].difficulty = atoi(temp);
        else
            config_save[slot].difficulty = 1;

        load_opaque(save, "fen", temp);
        board = fen_decode(temp);
        if (board)
        {
            saved_board[slot] = *board;
            free(board);
        }
    }

    mxmlDelete(tree);
}
