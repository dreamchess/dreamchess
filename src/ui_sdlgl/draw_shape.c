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

#ifdef _arch_dreamcast
/* FIXME */
void draw_tri(int x1, int y1, int x2, int y2, int x3, int y3, gg_colour_t *col)
{
    float zpos = get_dc_z();
    y1 = SCREEN_HEIGHT - y1;
    y2 = SCREEN_HEIGHT - y2;
    y3 = SCREEN_HEIGHT - y3;
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_NT_QUADS);
    glVertex3f(x1, y1, zpos);
    glVertex3f(x1, y1, zpos);
    glVertex3f(x2, y2, zpos);
    glVertex3f(x3, y3, zpos);
    glEnd();
}
#else
void draw_tri(int x1, int y1, int x2, int y2, int x3, int y3, gg_colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y1, 1.0f);
    glVertex3f(x2, y2, 1.0f);
    glVertex3f(x3, y3, 1.0f);
    glEnd();
}
#endif

void draw_rect(int x, int y, int w, int h, gg_colour_t *col)
{
    glColor4f(col->r, col->g, col->b, col->a);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + 0.5f, 1.0f);
    glVertex3f(x + w + 0.5f, y + h + 0.5f, 1.0f);
    glVertex3f(x + 0.5f, y + h + 0.5f, 1.0f);
    glEnd();
}

#ifdef _arch_dreamcast
void draw_rect_fill_gradient(int x, int y, int w, int h,
                             gg_colour_t *top_left,
                             gg_colour_t *top_right,
                             gg_colour_t *bottom_left,
                             gg_colour_t *bottom_right)
{
    float zpos = get_dc_z();
    y = SCREEN_HEIGHT - y;
    glBegin(GL_NT_QUADS);
    glColor4f(top_left->r, top_left->g, top_left->b, top_left->a);
    glVertex3f(x, y, zpos);
    glColor4f(top_right->r, top_right->g, top_right->b, top_right->a);
    glVertex3f(x + w, y, zpos);
    glColor4f(bottom_right->r, bottom_right->g, bottom_right->b, bottom_right->a);
    glVertex3f(x + w, y - h, zpos);
    glColor4f(bottom_left->r, bottom_left->g, bottom_left->b, bottom_left->a);
    glVertex3f(x, y - h, zpos);
    glEnd();
}
#else
void draw_rect_fill_gradient(int x, int y, int w, int h,
                             gg_colour_t *top_left,
                             gg_colour_t *top_right,
                             gg_colour_t *bottom_left,
                             gg_colour_t *bottom_right)
{
    glBegin(GL_QUADS);
    glColor4f(top_left->r, top_left->g, top_left->b, top_left->a);
    glVertex3f(x, y, 1.0f);
    glColor4f(top_right->r, top_right->g, top_right->b, top_right->a);
    glVertex3f(x + w, y, 1.0f);
    glColor4f(bottom_right->r, bottom_right->g, bottom_right->b, bottom_right->a);
    glVertex3f(x + w, y + h, 1.0f);
    glColor4f(bottom_left->r, bottom_left->g, bottom_left->b, bottom_left->a);
    glVertex3f(x, y + h, 1.0f);
    glEnd();
}
#endif

void draw_rect_fill(int x, int y, int w, int h, gg_colour_t *col)
{
    draw_rect_fill_gradient(x, y, w, h, col, col, col, col);
}
