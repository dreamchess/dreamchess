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

#include <GL/glew.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

#include "debug.h"
#include "ui_sdlgl.h"
#include "unicode.h"
#include "dir.h"

#include "freetype-gl/texture-font.h"
#include "freetype-gl/utf8-utils.h"

static texture_atlas_t *atlas;
static texture_font_t *font, *symbol_font;

typedef struct {
	float x, y, z;
	float s, t;
} vertex_t;

texture_font_t *load_font(const char *filename, texture_atlas_t *atlas, float pt_size) {
	ch_datadir();
	char *font_path = dir_get_real_path(filename);

	if (!font_path) {
		DBG_ERROR("Failed to get full path to font file '%s", filename);
		return NULL;
	}

	texture_font_t *font = texture_font_new_from_file(atlas, pt_size, font_path);

	free(font_path);

	if (!font)
		DBG_ERROR("Failed to load font file '%s'", filename);

	return font;
}

static int load_fonts(float pt_size) {
	const char *language = setlocale(LC_MESSAGES, NULL);
	const char *filename = "fonts/NotoSans-Regular.ttf";

	if (language)
		if (!strncmp(language, "zh_CN", 5))
			filename = "fonts/NotoSansSC-Regular.otf";

	font = load_font(filename, atlas, pt_size);
	symbol_font = load_font("fonts/NotoSansSymbols2-Regular.ttf", atlas, pt_size);

	if (!font || !symbol_font)
		return -1;

	return 0;
}

int unicode_init(float pt_size) {
	atlas  = texture_atlas_new(1024, 1024, 1);

	glGenTextures(1, &atlas->id);
	glBindTexture(GL_TEXTURE_2D, atlas->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, atlas->width, atlas->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, atlas->data);

	return load_fonts(pt_size);
}

int unicode_resize(float pt_size) {
	texture_atlas_clear(atlas);
	texture_font_delete(font);
	return load_fonts(pt_size);
}

void unicode_exit(void) {
	texture_font_delete(font);
	texture_font_delete(symbol_font);

	glDeleteTextures(1, &atlas->id);
	texture_atlas_delete(atlas);
}

static vertex_t *create_vertex_array(const char *text, size_t *array_size, float *width) {
	const size_t text_len = strlen(text);

	// Maximum size if every byte yields a glyph
	*array_size = text_len * 4;
	vertex_t *vertex_array = malloc(sizeof(vertex_t) * *array_size);
	size_t vertex_index = 0;

	float pen_x = 0.0f;
	const float pen_y = 0.0f;
	const char *prev_char = NULL;
	const texture_font_t *prev_font = NULL;

	for (size_t i = 0; i < text_len; ++i) {
		unsigned char cur_byte = text[i];
		// Skip bytes that are continuing a UTF-8 sequence
		if (cur_byte < 0x80 || cur_byte >= 0xc0) {
			texture_glyph_t *glyph;
			texture_font_t *cur_font = font;
			unsigned char next_byte = text[i + 1]; // Might be NUL

			// Use the symbol font for 'Miscellaneous Symbols' unicode block
			if (cur_byte == 0xe2 && next_byte >= 0x98 && next_byte < 0x9c)
				cur_font = symbol_font;

			glyph = texture_font_get_glyph(cur_font, text + i);

			if (glyph != NULL) {
				if (cur_font == prev_font)
					pen_x += texture_glyph_get_kerning(glyph, prev_char);

				prev_font = cur_font;
				prev_char = text + i;

				const int x0  = (int)(pen_x + glyph->offset_x);
				const int y0  = (int)(pen_y + glyph->offset_y);
				const int x1  = (int)(x0 + glyph->width);
				const int y1  = (int)(y0 - glyph->height);

				vertex_array[vertex_index++] = (vertex_t){ x0, y1, 1.0f, glyph->s0, glyph->t1 };
				vertex_array[vertex_index++] = (vertex_t){ x0, y0, 1.0f, glyph->s0, glyph->t0 };
				vertex_array[vertex_index++] = (vertex_t){ x1, y0, 1.0f, glyph->s1, glyph->t0 };
				vertex_array[vertex_index++] = (vertex_t){ x1, y1, 1.0f, glyph->s1, glyph->t1 };

				pen_x += glyph->advance_x;
			}
		}
	}

	if (atlas->modified) {
		glBindTexture(GL_TEXTURE_2D, atlas->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, atlas->width, atlas->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, atlas->data);
		atlas->modified = 0;
	}

	// Update to actual size
	*array_size = vertex_index;
	if (width)
		*width = pen_x;
	return vertex_array;
}

static void render_vertex_array(const vertex_t *vertex_array, size_t size, float x, float y, float xoffset, float scale, unsigned int flags, gg_colour_t colour) {
	if (size == 0)
		return;

	glEnable(GL_TEXTURE_2D);

	glColor4f(colour.r, colour.g, colour.b, colour.a);
	glBindTexture(GL_TEXTURE_2D, atlas->id);

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(scale, scale, 1.0f);

	glTranslatef(xoffset, -font->descender, 0.0f);

	glBegin(GL_QUADS);

	Uint32 ticks = SDL_GetTicks();
	float bounce_offset = 0.0f;

	for (size_t i = 0; i < size; ++i) {
		if (flags & UNICODE_FLAG_BOUNCY && i % 4 == 0) {
			float phase = ((ticks % (1000 / BOUNCE_SPEED)) / (float)(1000 / BOUNCE_SPEED));

			if (phase < 0.5)
				bounce_offset = phase * 2 * (BOUNCE_AMP + 1);
			else
				bounce_offset = ((1.0 - phase) * 2) * (BOUNCE_AMP + 1);

			bounce_offset /= scale;

			ticks += 1000 / BOUNCE_SPEED / BOUNCE_LEN;
		}

		glTexCoord2f(vertex_array[i].s, vertex_array[i].t);
		glVertex3f(vertex_array[i].x, vertex_array[i].y + bounce_offset, vertex_array[i].z);
	}

	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void unicode_string_render(const char *text, float x, float y, float align, float scale, unsigned int flags, gg_colour_t colour) {
	const float screen_scale = get_gl_height() / get_screen_height();
	size_t array_size;
	float width;
	vertex_t *vertex_array = create_vertex_array(text, &array_size, &width);
	float xoffset = -align * width;

	if (!(flags & UNICODE_FLAG_NO_SHADOW))
		render_vertex_array(vertex_array, array_size, x + 1.0f, y - 1.0f, xoffset, screen_scale * scale, flags, (gg_colour_t){0.0f, 0.0f, 0.0f, colour.a});

	render_vertex_array(vertex_array, array_size, x, y, xoffset, screen_scale * scale, flags, colour);

	free(vertex_array);
}

float unicode_get_font_height(void) {
	const float screen_scale = get_gl_height() / get_screen_height();
	return font->height * screen_scale;
}

float unicode_get_string_width(const char *text) {
	const float screen_scale = get_gl_height() / get_screen_height();
	float width;

	// FIXME: Do this without actually creating the array
	size_t array_size;
	vertex_t *vertex_array = create_vertex_array(text, &array_size, &width);
	free(vertex_array);

	return width * screen_scale;
}

void unicode_render_atlas(void) {
	glEnable(GL_TEXTURE_2D);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, atlas->id);

	glPushMatrix();
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(640.0f, 0.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(640.0f, 480.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 480.0f, 1.0f);

	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
