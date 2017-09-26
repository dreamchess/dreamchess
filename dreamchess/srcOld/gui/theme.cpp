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

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

static texture_t backdrop;
static texture_t border[9];
static texture_t menu_border[9];
static texture_t mouse_cursor, menu_mouse_cursor;
static texture_t white_pieces[7];
static texture_t black_pieces[7];
static texture_t selector_tex;

static char white_name[80];
static char black_name[80];

texture_t *get_menu_border(void)
{
    return menu_border;
}

texture_t *get_border(void)
{
    return border;
}

texture_t *get_mouse_cursor(void)
{
    return &mouse_cursor;
}

texture_t *get_menu_mouse_cursor(void)
{
    return &menu_mouse_cursor;
}

char *get_white_name(void)
{
    return white_name;
}

char *get_black_name(void)
{
    return black_name;
}

texture_t *get_backdrop(void)
{
    return &backdrop;
}

texture_t *get_white_pieces(void)
{
    return white_pieces;
}

texture_t *get_black_pieces(void)
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

static int ld_style(char *name)
{
    if (chdir("styles"))
        return 1;

    if (chdir(name))
        return 1;

    /* Theme! */
    load_texture_png( &backdrop, "backdrop.png", 0, 1 );
    load_texture_png( &mouse_cursor, "mouse_cursor.png", 1, 1 );

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
void load_theme(struct theme_struct *theme)
{
    ch_userdir();
    if (ld_style(theme->style)) {
        ch_datadir();
        if (ld_style(theme->style)) {
            DBG_ERROR("failed to find style '%s'", theme->style);
            exit(1);
        }
    }

    ch_userdir();
    if (ld_pieces(theme->pieces)) {
        ch_datadir();
        if (ld_pieces(theme->pieces)) {
            DBG_ERROR("failed to find pieces '%s'", theme->pieces);
            exit(1);
        }
    }

    ch_userdir();
    if (ld_board(theme->board)) {
        ch_datadir();
        if (ld_board(theme->board)) {
            DBG_ERROR("failed to find board '%s'", theme->board);
            exit(1);
        }
    }

    set_theme(theme, selector_tex);

    ch_datadir();
}

/** @brief Frees all textures of the currently loaded theme. */
void unload_theme(void)
{
    glDeleteTextures(1, &white_pieces[GUI_PIECE_KING].id);
    glDeleteTextures(1, &backdrop.id);
    glDeleteTextures(1, &selector_tex.id);
    glDeleteTextures(1, &mouse_cursor.id);
    unload_border(border);
    freemodels();
}
