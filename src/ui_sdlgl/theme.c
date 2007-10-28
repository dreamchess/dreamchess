/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
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

static texture_t backdrop;
static texture_t border[9];
static texture_t menu_border[9];
static texture_t mouse_cursor;
static texture_t white_pieces[7];
static texture_t black_pieces[7];
static texture_t selector_tex;
static theme themes[25];
static int selected_theme=0;
static int theme_count=0;

static char white_name[80];
static char black_name[80];

void set_theme_count( int count )
{
    theme_count=count;
}

int get_theme_count()
{
    return theme_count;
}

int get_selected_theme()
{
    return selected_theme;
}

void set_selected_theme( int index )
{
    selected_theme=index;
}

texture_t *get_menu_border()
{
    return menu_border;
}

texture_t *get_border()
{
    return border;
}

theme *get_theme( int index )
{
    return &themes[index];
}

texture_t *get_mouse_cursor()
{
    return &mouse_cursor;
}

char *get_white_name()
{
    return white_name;
}

char *get_black_name()
{
    return black_name;
}

texture_t *get_backdrop()
{
    return &backdrop;
}

texture_t *get_white_pieces()
{
    return white_pieces;
}

texture_t *get_black_pieces()
{
    return black_pieces;
}

texture_t *get_white_piece( int index )
{
    return &white_pieces[index];
}

texture_t *get_black_piece( int index )
{
    return &black_pieces[index];
}

texture_t *get_selector_tex()
{
    return &selector_tex;
}

/* xml theme options */
int use_lighting()
{
    if (selected_theme==theme_count)
        return TRUE;
    else
        return themes[selected_theme].lighting;
}

int use_tex_spin()
{
    if (selected_theme==theme_count)
        return FALSE;
    else
        return themes[selected_theme].piece_tex_spin;
}

int get_tex_spin_speed()
{
    return themes[selected_theme].piece_tex_spin_speed;
}

int load_opaque(mxml_node_t *top, char *name, char *dest)
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

static int ld_style(char *name)
{
    if (chdir("styles"))
        return 1;

    if (chdir(name))
        return 1;

    /* Theme! */
    load_texture_png( &backdrop, "backdrop.png", 0, 1 );

    #ifndef _arch_dreamcast
    #ifndef __BEOS__
    load_texture_png( &mouse_cursor, "mouse_cursor.png", 1, 1 );
	#endif /* __BEOS__ */
    #endif /* _arch_dreamcast */

    load_border(border, "border.png");
    load_pieces();

    return 0;
}

static int ld_pieces(char *name)
{
    if (chdir("pieces"))
        return 1;

    if (chdir(name))
        return 1;

    loadmodels("set.cfg");
    texture_t seltex;
    load_texture_png(&selector_tex, "selector.png", 1,1);

    return 0;
}

static int ld_board(char *name)
{
    if (chdir("boards"))
        return 1;

    if (chdir(name))
        return 1;

    load_board("board.dcm", "board.png");

    return 0;
}

/** @brief Loads a style.
 *
 *  @param name The name of the subdirectory of the theme to load.
 */
void load_theme(char* style, char* pieces, char *board)
{
    ch_userdir();
    if (ld_style(style)) {
        ch_datadir();
        if (ld_style(style)) {
            DBG_ERROR("failed to find style '%'", style);
            exit(1);
        }
    }

    ch_userdir();
    if (ld_pieces(pieces)) {
        ch_datadir();
        if (ld_pieces(pieces)) {
            DBG_ERROR("failed to find pieces '%'", pieces);
            exit(1);
        }
    }

    ch_userdir();
    if (ld_board(board)) {
        ch_datadir();
        if (ld_board(board)) {
            DBG_ERROR("failed to find board '%'", board);
            exit(1);
        }
    }

    ch_datadir();
}

/** @brief Frees all textures of the currently loaded theme. */
void unload_theme()
{
    glDeleteTextures(1, &white_pieces[GUI_PIECE_KING].id);
    glDeleteTextures(1, &backdrop.id);
    glDeleteTextures(1, &selector_tex.id);
    unload_border(border);
    freemodels();
}
