
#include "ui_sdlgl.h"

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
