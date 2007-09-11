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

#include "gamegui/queue.h"

/* Define our booleans */
#define TRUE  1
#define FALSE 0

struct theme_struct
{
    char *name;
    char *style,*pieces,*board;
    char *white_name, *black_name;

    int lighting;
    int piece_tex_spin, piece_tex_spin_speed;
    float selector_colour[3];

    TAILQ_ENTRY(theme_struct) entries;
};

typedef TAILQ_HEAD(, music_pack) music_packs_t;

typedef struct
{
    char *name;

}theme_style_t;

typedef struct
{
    char *name;

}theme_pieces_t;

typedef struct
{
    char *name;

}theme_board_t;

typedef struct music_pack
{
    char *dir;
    TAILQ_ENTRY(music_pack) entries;
} music_pack_t;

void theme_read_theme_dir( char *datadir );
void theme_find_music_packs();
music_packs_t *theme_get_music_packs();
