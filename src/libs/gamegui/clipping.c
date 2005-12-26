/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>

#include <gamegui/clipping.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

typedef struct clip_el
{
    struct clip_el *prev;
    gg_rect_t rect;
}
clip_el_t;

gg_rect_t gg_clipping_rect(gg_rect_t *rect1, gg_rect_t *rect2)
{
    gg_rect_t rect;

    rect.x = MAX(rect1->x, rect2->x);
    rect.y = MAX(rect1->y, rect2->y);
    rect.width = MIN(rect1->x + rect1->width, rect2->x + rect2->width) -
                 rect.x;
    rect.height = MIN(rect1->y + rect1->height, rect2->y + rect2->height) -
                  rect.y;

    if (rect.width < 0)
        rect.width = 0;
    if (rect.height < 0)
        rect.height = 0;

    return rect;
}

static clip_el_t *clip_stack;

gg_rect_t *gg_clipping_get()
{
    if (clip_stack)
        return &clip_stack->rect;
    else
        return NULL;
}

void gg_clipping_adjust(gg_rect_t *rect)
{
    clip_el_t *clip_new = malloc(sizeof(clip_el_t));

    if (clip_stack)
    {
        clip_new->prev = clip_stack;
        clip_new->rect = gg_clipping_rect(rect, &clip_stack->rect);
    }
    else
    {
        clip_new->prev = NULL;
        clip_new->rect = *rect;
    }

    clip_stack = clip_new;
}

void gg_clipping_undo()
{
    clip_el_t *clip_new;
    if (clip_stack)
    {
        clip_new = clip_stack->prev;
        free(clip_stack);
        clip_stack = clip_new;
    }
}
