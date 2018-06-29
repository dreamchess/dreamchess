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

static unsigned int utf8_to_utf32(const char *utf8) {
	unsigned int char_utf32 = 0;
	char *utf32 = SDL_iconv_string("UTF-32LE", "UTF-8", utf8, strlen(utf8) + 1);

	if (utf32) {
		char_utf32 = ((unsigned char)utf32[3] << 24) | ((unsigned char)utf32[2] << 16) |
		             ((unsigned char)utf32[1] << 8) | ((unsigned char)utf32[0]);
		SDL_free(utf32);
	}

	return char_utf32;
}

extern SDL_Window *sdl_window;

gg_event_t convert_event(SDL_Event *event) {
	gg_event_t gg_event;

	gg_event.type = GG_EVENT_NONE;

	switch (event->type) {
	case SDL_KEYDOWN:
		gg_event.type = GG_EVENT_KEY;
		switch (event->key.keysym.sym) {
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
		case SDLK_ESCAPE:
			gg_event.key = GG_KEY_ESCAPE;
			break;
		default:
			if (event->key.keysym.sym >= 32 || event->key.keysym.sym < 255)
				gg_event.key = event->key.keysym.sym;
			else
				gg_event.type = GG_EVENT_NONE;
			return gg_event;
		}
		break;

	case SDL_TEXTINPUT: {
		unsigned int utf32 = utf8_to_utf32(event->text.text);

		if (utf32 > 0 && utf32 <= 0xff) {
			gg_event.type = GG_EVENT_CHAR;
			gg_event.key = utf32 & 0xff;
			return gg_event;
		}
	} break;

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP: {
		int width, height;
		int mouse_x, mouse_y;
		SDL_GetWindowSize(sdl_window, &width, &height);
		mouse_x = event->motion.x * get_screen_width() / width;
		mouse_y = event->motion.y * get_screen_height() / height;
		set_mouse_pos(mouse_x, mouse_y);
		gg_event.type = GG_EVENT_MOUSE;
		gg_event.mouse.type = (event->type == SDL_MOUSEBUTTONDOWN ? GG_MOUSE_BUTTON_DOWN : GG_MOUSE_BUTTON_UP);
		gg_event.mouse.button = event->button.button - 1;
		gg_event.mouse.x = (int)(((float)mouse_x / (float)get_screen_width()) * 640);
		gg_event.mouse.y = SCREEN_HEIGHT - 1 - (int)(((float)mouse_y / (float)get_screen_height()) * 480);
	} break;

	case SDL_MOUSEMOTION: {
		int width, height;
		int mouse_x, mouse_y;
		SDL_GetWindowSize(sdl_window, &width, &height);
		mouse_x = event->motion.x * get_screen_width() / width;
		mouse_y = event->motion.y * get_screen_height() / height;
		set_mouse_pos(mouse_x, mouse_y);
		gg_event.type = GG_EVENT_MOUSE;
		gg_event.mouse.type = GG_MOUSE_MOVE;
		gg_event.mouse.x = (int)(((float)mouse_x / (float)get_screen_width()) * 640);
		gg_event.mouse.y = SCREEN_HEIGHT - 1 - (int)(((float)mouse_y / (float)get_screen_height()) * 480);
	}
	}

	return gg_event;
}

static void draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour) {
	texture_t *texture = (texture_t *)image;
	float hsize = texture->u2 - texture->u1;
	float vsize = texture->v2 - texture->v1;
	float tex_h = texture->width / hsize;
	float tex_v = texture->height / vsize;
	float xsrc = texture->u1 + source.x / tex_h;
	float ysrc = texture->v1 + source.y / tex_v;
	float width, height;
	GLenum en_h, en_v;

	if (mode_h == GG_MODE_TILE) {
		en_h = GL_REPEAT;
		width = dest.width / tex_h;
	} else {
		en_h = GL_CLAMP;
		width = source.width / tex_h;
	}

	if (mode_v == GG_MODE_TILE) {
		en_v = GL_REPEAT;
		height = dest.height / tex_v;
	} else {
		en_v = GL_CLAMP;
		height = source.height / tex_v;
	}

	draw_texture_uv(texture, (float)dest.x, (float)dest.y, (float)dest.width, (float)dest.height, 1.0f, colour, xsrc,
	                ysrc, xsrc + width, ysrc + height, en_h, en_v);
}

static void *get_char_image(int c) {
	if (c < 0)
		c += 256;

	return get_text_character(c);
	/*    return &text_characters[c];*/
}

static void draw_char(int c, int x, int y, gg_colour_t *colour) { text_draw_char((float)x, (float)y, 1.0f, c, colour); }

static void get_image_size(void *image, int *width, int *height) {
	texture_t *texture = (texture_t *)image;

	if (width)
		*width = texture->width;

	if (height)
		*height = texture->height;
}

static void get_char_size(int c, int *width, int *height) {
	if (c < 0)
		c += 256;

	if (width)
		*width = get_text_character(c)->width;

	if (height)
		*height = get_text_character(c)->height;
}

static unsigned int get_ticks(void) { return SDL_GetTicks(); }

gg_driver_t gg_driver_sdlgl = {draw_rect, draw_rect_fill, draw_rect_fill_gradient, draw_image, get_char_image,
                               draw_char, get_image_size, get_char_size,           get_ticks};

gg_driver_t *get_gg_driver_sdlgl(void) { return &gg_driver_sdlgl; }

#include <array>
#include <memory>

#include "gamegui++/gamegui++.h"

static gg_colour_t make_colour(const GameGUI::Colour &col) { return {col.r, col.g, col.b, col.a}; }

class MyImage : public GameGUI::Image {
public:
	MyImage(const texture &texture) : _texture(texture) {}
	GameGUI::Size getSize() const { return {_texture.width, _texture.height}; }

	const texture &getTexture() const { return _texture; }

private:
	const texture &_texture;
};

class MyDriver : public GameGUI::Driver {
public:
	MyDriver() {
		for (unsigned int i = 0; i < 256; ++i)
			_charImages.push_back(MyImage(*get_text_character(i)));
	}

	void drawFilledRect(const GameGUI::Rect &rect, const GameGUI::Colour &colour) const {
		gg_colour_t c(make_colour(colour));
		draw_rect_fill(rect.x, rect.y, rect.width, rect.height, &c);
	}

	void drawGradientRect(const GameGUI::Rect &rect, const GameGUI::Colour &colTopLeft,
	                      const GameGUI::Colour &colTopRight, const GameGUI::Colour &colBottomLeft,
	                      const GameGUI::Colour &colBottomRight) const {
		gg_colour_t cTopLeft(make_colour(colTopLeft));
		gg_colour_t cTopRight(make_colour(colTopRight));
		gg_colour_t cBottomRight(make_colour(colBottomRight));
		gg_colour_t cBottomLeft(make_colour(colBottomLeft));
		draw_rect_fill_gradient(rect.x, rect.y, rect.width, rect.height, &cTopLeft, &cTopRight, &cBottomLeft,
		                        &cBottomRight);
	}

	void drawImage(const GameGUI::Image &image, const GameGUI::Rect &rectSource, const GameGUI::Rect &rectDest,
	               GameGUI::FillMode fillH, GameGUI::FillMode fillV, const GameGUI::Colour &colour) const {
		const texture &tex = static_cast<const MyImage &>(image).getTexture();
		float hsize = tex.u2 - tex.u1;
		float vsize = tex.v2 - tex.v1;
		float tex_h = tex.width / hsize;
		float tex_v = tex.height / vsize;
		float xsrc = tex.u1 + rectSource.x / tex_h;
		float ysrc = tex.v1 + rectSource.y / tex_v;
		float width, height;
		GLenum en_h, en_v;

		if (fillH == GameGUI::FillMode::Tile) {
			en_h = GL_REPEAT;
			width = rectDest.width / tex_h;
		} else {
			en_h = GL_CLAMP;
			width = rectSource.width / tex_h;
		}

		if (fillV == GameGUI::FillMode::Tile) {
			en_v = GL_REPEAT;
			height = rectDest.height / tex_v;
		} else {
			en_v = GL_CLAMP;
			height = rectSource.height / tex_v;
		}

		gg_colour_t col(make_colour(colour));

		draw_texture_uv(&tex, (float)rectDest.x, (float)rectDest.y, (float)rectDest.width, (float)rectDest.height, 1.0f,
		                &col, xsrc, ysrc, xsrc + width, ysrc + height, en_h, en_v);
	}

	const MyImage &getCharImage(char c) const { return _charImages[(unsigned char)c]; }

	GameGUI::Size getCharSize(char c) const {
		const texture *tex = get_text_character((unsigned char)c);
		return {tex->width, tex->height};
	}

	void drawChar(char c, const GameGUI::Point &p, const GameGUI::Colour &colour) const {
		gg_colour_t col = make_colour(colour);
		text_draw_char((float)p.x, (float)p.y, 1.0f, c, &col);
	}

private:
	std::vector<MyImage> _charImages;
};

class MyDialog : public GameGUI::Dialog {
public:
	MyDialog(const GameGUI::DialogStyle *style) : GameGUI::Dialog(style) { _minSize = {100, 100}; }
};

static GameGUI::WindowManager *gameGUI;

extern void fooCreate() {
	gameGUI = new GameGUI::WindowManager(std::unique_ptr<MyDriver>(new MyDriver()));

	GameGUI::DialogStyle *style = new GameGUI::DialogStyle;
	style->applyTexture = true;
	style->fadeColour = {0.0f, 0.0f, 0.0f, 0.0f};
	style->horPadding = 20;
	style->vertPadding = 10;

	for (int i = 0; i < 9; i++)
		style->border[i] = new MyImage(get_menu_border()[i]);

	gameGUI->open<MyDialog>(style);
}

extern void fooRender() {
	gameGUI->render();
}
