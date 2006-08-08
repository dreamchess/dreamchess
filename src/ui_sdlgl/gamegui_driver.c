
#include "ui_sdlgl.h"

gg_event_t convert_event(SDL_Event *event)
{
    static unsigned int pressed;
    gg_event_t gg_event;

    gg_event.type=GG_EVENT_NONE;

    switch (event->type)
    {
    case SDL_KEYDOWN:
        gg_event.type = GG_EVENT_KEY;
        switch (event->key.keysym.sym)
        {
        case SDLK_RIGHT:
            gg_event.key = GG_KEY_RIGHT;
            break;
        case SDLK_LEFT:
            gg_event.key = GG_KEY_LEFT;
            break;
        case SDLK_UP:
            gg_event.key = GG_KEY_UP;
            break;
        case SDLK_DOWN:
            gg_event.key = GG_KEY_DOWN;
            break;
        case SDLK_HOME:
            gg_event.key = GG_KEY_HOME;
            break;
        case SDLK_END:
            gg_event.key = GG_KEY_END;
            break;
        case SDLK_RETURN:
            gg_event.key = GG_KEY_ACTION;
            break;
        default:
            if (event->key.keysym.unicode <= 0xff)
            {
                gg_event.key = event->key.keysym.unicode;
            }
            else
            {
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
        }
        break;

    case SDL_JOYHATMOTION:
        gg_event.type = GG_EVENT_KEY;
        switch (event->jhat.value)
        {
        case SDL_HAT_RIGHT:
            gg_event.key = GG_KEY_RIGHT;
            break;
        case SDL_HAT_LEFT:
            gg_event.key = GG_KEY_LEFT;
            break;
        case SDL_HAT_UP:
            gg_event.key = GG_KEY_UP;
            break;
        case SDL_HAT_DOWN:
            gg_event.key = GG_KEY_DOWN;
            break;
        default:
            gg_event.type = GG_EVENT_NONE;
            return gg_event;
        }
        break;

    case SDL_JOYBUTTONDOWN:
        gg_event.type = GG_EVENT_KEY;
        switch (event->jbutton.button)
        {
        case 0:
            gg_event.key = GG_KEY_ACTION;
            break;
        case 1:
            gg_event.key = GG_KEY_ESCAPE;
            break;
        case 2:
            gg_event.key = GG_KEY_EXTRA3;
            break;
        case 3:
            gg_event.key = GG_KEY_EXTRA2;
            break;
        case 4:
            gg_event.key = GG_KEY_ESCAPE;
            break;
        default:
            gg_event.type = GG_EVENT_NONE;
            return gg_event;
        }
        break;

#ifndef AXIS_CURSOR_DISABLED

    case SDL_JOYAXISMOTION:
        gg_event.type = GG_EVENT_KEY;
        switch (event->jaxis.axis)
        {
        case AXIS_CURSOR_X:
            if (event->jaxis.value >= -15000 && event->jaxis.value <= 15000)
            {
                pressed &= ~(LEFT | RIGHT);
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }

            if (!(pressed & LEFT) && (event->jaxis.value < 0))
            {
                pressed |= LEFT;
                gg_event.key = GG_KEY_LEFT;
            }
            else if (!(pressed & RIGHT) && (event->jaxis.value > 0))
            {
                pressed |= RIGHT;
                gg_event.key = GG_KEY_RIGHT;
            }
            else
            {
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
            break;
        case AXIS_CURSOR_Y:
            if (event->jaxis.value >= -15000 && event->jaxis.value <= 15000)
            {
                pressed &= ~(UP | DOWN);
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
            if (!(pressed & UP) && (event->jaxis.value < 0))
            {
                pressed |= UP;
                gg_event.key = GG_KEY_UP;
            }
            else if (!(pressed & DOWN) && (event->jaxis.value > 0))
            {
                pressed |= DOWN;
                gg_event.key = GG_KEY_DOWN;
            }
            else
            {
                gg_event.type = GG_EVENT_NONE;
                return gg_event;
            }
        }
#endif

    case SDL_MOUSEBUTTONDOWN:
        gg_event.type = GG_EVENT_MOUSE;
        gg_event.mouse.type = GG_MOUSE_BUTTON_DOWN;
        gg_event.mouse.button = event->button.button - 1;
        gg_event.mouse.x = event->button.x;
        gg_event.mouse.y = SCREEN_HEIGHT - 1 - event->button.y;

    case SDL_MOUSEMOTION:
        gg_event.type = GG_EVENT_MOUSE;
        gg_event.mouse.type = GG_MOUSE_MOVE;
        gg_event.mouse.x = event->motion.x;
        gg_event.mouse.y = SCREEN_HEIGHT - 1 - event->motion.y;
    }

    /*    if ((event->type == SDL_KEYDOWN) && (event->key.keysym.unicode <= 0xff))
            gg_event.key = event->key.keysym.unicode;*/

    return gg_event;
}


void draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour)
{
    texture_t *texture = image;
    float hsize = texture->u2 - texture->u1;
    float vsize = texture->v2 - texture->v1;
    float tex_h = texture->width / hsize;
    float tex_v = texture->height / vsize;
    float xsrc = texture->u1 + source.x / tex_h;
    float ysrc = texture->v1 + source.y / tex_v;
    float width, height;
    GLenum en_h, en_v;

    if (mode_h == GG_MODE_TILE)
    {
        en_h = GL_REPEAT;
        width = dest.width / tex_h;
    }
    else
    {
        en_h = GL_CLAMP;
        width = source.width / tex_h;
    }

    if (mode_v == GG_MODE_TILE)
    {
        en_v = GL_REPEAT;
        height = dest.height / tex_v;
    }
    else
    {
        en_v = GL_CLAMP;
        height = source.height / tex_v;
    }

    draw_texture_uv(texture, dest.x,
                    dest.y, dest.width, dest.height, 1.0f,
                    colour, xsrc,
                    ysrc,
                    xsrc + width, ysrc + height, en_h, en_v);
}

void *get_char_image(int c)
{
    if (c < 0)
        c += 256;

    return get_text_character(c);
/*    return &text_characters[c];*/
}

void draw_char(int c, int x, int y, gg_colour_t *colour)
{
    text_draw_char(x, y, 1.0f, c, colour);
}

void get_image_size(void *image, int *width, int *height)
{
    texture_t *texture = image;

    if (width)
        *width = texture->width;

    if (height)
        *height = texture->height;
}

void get_char_size(int c, int *width, int *height)
{
    if (c < 0)
        c += 256;

    if (width)
        *width = get_text_character(c)->width;

    if (height)
        *height = get_text_character(c)->height;
}

unsigned int get_ticks()
{
    return SDL_GetTicks();
}

gg_driver_t gg_driver_sdlgl =
    {
        draw_rect,
        draw_rect_fill,
        draw_rect_fill_gradient,
        draw_image,
        get_char_image,
        draw_char,
        get_image_size,
        get_char_size,
        get_ticks
    };

gg_driver_t *get_gg_driver_sdlgl()
{
    return &gg_driver_sdlgl;
}
