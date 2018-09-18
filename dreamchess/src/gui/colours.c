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

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_red =
    {
        1.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

static gg_colour_t col_yellow =
    {
        1.0f, 1.0f, 0.0f, 1.0f
    };

/* Return the specified colour, returning white if unknown value */
gg_colour_t *get_col( int colour )
{
    switch ( colour )
    {
        case COL_BLACK:
            return &col_black;
            break;
        case COL_RED:
            return &col_red;
            break;
        case COL_YELLOW:
            return &col_yellow;
            break;
        case COL_WHITE:
        default:
            return &col_white;
            break;
    }
}
