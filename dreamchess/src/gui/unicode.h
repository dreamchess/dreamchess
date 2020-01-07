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

#ifndef GUI_UNICODE_H
#define GUI_UNICODE_H

#define UNICODE_FLAG_BOUNCY    (1 << 0)
#define UNICODE_FLAG_NO_SHADOW (1 << 1)

int unicode_init(float pt_size);
int unicode_resize(float pt_size);
void unicode_exit(void);
void unicode_string_render(const char *text, float x, float y, float align, float scale, unsigned int flags, gg_colour_t colour);
void unicode_render_atlas(void);
float unicode_get_font_height(void);
float unicode_get_string_width(const char *text);

#endif
