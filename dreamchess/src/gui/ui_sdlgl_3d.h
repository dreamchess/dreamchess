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

#ifndef GUI_UI_SDLGL_3D_H
#define GUI_UI_SDLGL_3D_H

#define SELECTOR_UP 0
#define SELECTOR_DOWN 1
#define SELECTOR_LEFT 2
#define SELECTOR_RIGHT 3

typedef struct coord3
{
    float x;
    float y;
    float z;
}
coord3_t;

void move_camera(float x, float z);
void render_scene_3d(Board *board, GLuint target_fb, int reflections);
void move_selector(int direction);
int get_selector(void);
void select_piece(int square);
void reset_3d(int flip);
void loadmodels(const char *filename);
void load_board(const char *dcm_name, const char *texture_name);
void init_fbo(void);
void deinit_fbo(void);
int find_square(int x, int y);
void freemodels(void);

#endif
