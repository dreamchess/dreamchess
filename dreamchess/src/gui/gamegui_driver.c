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

static unsigned int utf8_to_utf32(const char *utf8) {
    unsigned int char_utf32 = 0;
    char *utf32 = SDL_iconv_string("UTF-32LE", "UTF-8", utf8, strlen(utf8) + 1);

    if (utf32) {
        char_utf32 = ((unsigned char)utf32[3] << 24) | ((unsigned char)utf32[2] << 16)
                     | ((unsigned char)utf32[1] << 8) | ((unsigned char)utf32[0]);
        SDL_free(utf32);
    }

    return char_utf32;
}

extern SDL_Window *sdl_window;

gg_event_t convert_event(SDL_Event *event)
{
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
        case SDLK_BACKSPACE:
            gg_event.key = GG_KEY_BACKSPACE;
            break;
        case SDLK_DELETE:
            gg_event.key = GG_KEY_DELETE;
            break;
        default:
            gg_event.type = GG_EVENT_NONE;
            return gg_event;
        }
        break;

    case SDL_TEXTINPUT:
        {
            unsigned int utf32 = utf8_to_utf32(event->text.text);

            if (utf32 > 0 && utf32 <= 0xff) {
                gg_event.type = GG_EVENT_CHAR;
                gg_event.key = utf32 & 0xff;
                return gg_event;
            }
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        {
            int width, height;
            int mouse_x, mouse_y;
            SDL_GetWindowSize(sdl_window, &width, &height);
            mouse_x = event->motion.x * get_screen_width() / width;
            mouse_y = event->motion.y * get_screen_height() / height;
            set_mouse_pos( mouse_x, mouse_y );
            gg_event.type = GG_EVENT_MOUSE;
            gg_event.mouse.type = (event->type == SDL_MOUSEBUTTONDOWN
                                ? GG_MOUSE_BUTTON_DOWN : GG_MOUSE_BUTTON_UP);
            gg_event.mouse.button = event->button.button - 1;
            gg_event.mouse.x = ((float)mouse_x/(float)get_screen_width())*640;
            gg_event.mouse.y = SCREEN_HEIGHT - 1 - ((float)mouse_y/(float)get_screen_height())*480;
        }
        break;

    case SDL_MOUSEMOTION:
        {
            int width, height;
            int mouse_x, mouse_y;
            SDL_GetWindowSize(sdl_window, &width, &height);
            mouse_x = event->motion.x * get_screen_width() / width;
            mouse_y = event->motion.y * get_screen_height() / height;
            set_mouse_pos( mouse_x, mouse_y );
            gg_event.type = GG_EVENT_MOUSE;
            gg_event.mouse.type = GG_MOUSE_MOVE;
            gg_event.mouse.x = ((float)mouse_x/(float)get_screen_width())*640;
            gg_event.mouse.y = SCREEN_HEIGHT - 1 - ((float)mouse_y/(float)get_screen_height())*480;
        }
    }

    return gg_event;
}


static void draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour)
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

static void *get_char_image(int c)
{
    if (c < 0)
        c += 256;

    return get_text_character(c);
/*    return &text_characters[c];*/
}

static void draw_char(int c, int x, int y, gg_colour_t *colour)
{
    text_draw_char(x, y, 1.0f, c, colour);
}

static void get_image_size(void *image, int *width, int *height)
{
    texture_t *texture = image;

    if (width)
        *width = texture->width;

    if (height)
        *height = texture->height;
}

static void get_char_size(int c, int *width, int *height)
{
    if (c < 0)
        c += 256;

    if (width)
        *width = get_text_character(c)->width;

    if (height)
        *height = get_text_character(c)->height;
}

static unsigned int get_ticks(void)
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

gg_driver_t *get_gg_driver_sdlgl(void)
{
    return &gg_driver_sdlgl;
}
