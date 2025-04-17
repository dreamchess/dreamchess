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

/*  This function loads the image upside-down. Meaning that texture
 *  coordinate (0,0) corresponds to the top-left corner of the image.
 */
texture_t SDL_GL_LoadTexture(SDL_Surface *surface, SDL_Rect *area, int alpha, int clamp) {
	texture_t texture;
	int w, h;
	SDL_Surface *image;
	SDL_Rect dest;
	Uint32 saved_flags;
	Uint8 saved_alpha;

	/* Use the surface width and height expanded to powers of 2 */
	w = power_of_two(area->w);
	h = power_of_two(area->h);

	image = SDL_CreateSurface(w, h,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
	SDL_PIXELFORMAT_ABGR8888
#else
	SDL_PIXELFORMAT_RGBA8888
#endif
	);
	if (image == NULL) {
		exit(0);
	}

	/* Copy the surface into the GL texture image */
	dest.x = 0;
	dest.y = 0;
	dest.w = area->w;
	dest.h = area->h;
	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
	SDL_BlitSurface(surface, area, image, &dest);

	/* Create an OpenGL texture for the image */
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, (alpha ? 4 : 3), w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	SDL_DestroySurface(image); /* No longer needed */

	glEnable(GL_TEXTURE_2D);
	glGenerateMipmap(GL_TEXTURE_2D);

	texture.u1 = 0;
	texture.v1 = 0;
	texture.u2 = area->w / (float)w;
	texture.v2 = area->h / (float)h;
	texture.width = area->w;
	texture.height = area->h;

	return texture;
}

/** @brief Renders a textured quad.
 *
 *  @param texture The texture to use.
 *  @param xpos The leftmost x-coordinate.
 *  @param ypos The lowermost y-coordinate.
 *  @param width The width in pixels.
 *  @param height The height in pixels.
 *  @param zpos The z-coordinate.
 *  @param col The colour to render with.
 */
void draw_texture(texture_t *texture, float xpos, float ypos, float width, float height, float zpos, gg_colour_t *col) {
	glEnable(GL_TEXTURE_2D);

	glColor4f(col->r, col->g, col->b, col->a);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glBegin(GL_QUADS);
	glTexCoord2f(texture->u1, texture->v1);
	glVertex3f(xpos, ypos + height, zpos);
	glTexCoord2f(texture->u2, texture->v1);
	glVertex3f(xpos + width, ypos + height, zpos);
	glTexCoord2f(texture->u2, texture->v2);
	glVertex3f(xpos + width, ypos, zpos);
	glTexCoord2f(texture->u1, texture->v2);
	glVertex3f(xpos, ypos, zpos);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void draw_texture_uv(texture_t *texture, float xpos, float ypos, float width, float height, float zpos,
					 gg_colour_t *col, float u1, float v1, float u2, float v2, GLenum mode_h, GLenum mode_v) {
	glEnable(GL_TEXTURE_2D);

	glColor4f(col->r, col->g, col->b, col->a);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_v);

	glBegin(GL_QUADS);
	glTexCoord2f(u1, v1);
	glVertex3f(xpos, ypos + height, zpos);
	glTexCoord2f(u2, v1);
	glVertex3f(xpos + width, ypos + height, zpos);
	glTexCoord2f(u2, v2);
	glVertex3f(xpos + width, ypos, zpos);
	glTexCoord2f(u1, v2);
	glVertex3f(xpos, ypos, zpos);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void draw_texture_fullscreen(texture_t *texture, float zpos) {
	glEnable(GL_TEXTURE_2D);

	glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	float hor_visible = texture->height * get_screen_width() / (float)get_screen_height() / texture->width;

	// If the image isn't wide enough, we stretch it
	if (hor_visible >= 1.0f)
		hor_visible = 1.0f;

	float u_skip = (1.0f - hor_visible) / 2 * (texture->u2 - texture->u1);

	glBegin(GL_QUADS);
	glTexCoord2f(texture->u1 + u_skip, texture->v1);
	glVertex3f(0, get_gl_height(), zpos);
	glTexCoord2f(texture->u2 - u_skip, texture->v1);
	glVertex3f(get_gl_width(), get_gl_height(), zpos);
	glTexCoord2f(texture->u2 - u_skip, texture->v2);
	glVertex3f(get_gl_width(), 0, zpos);
	glTexCoord2f(texture->u1 + u_skip, texture->v2);
	glVertex3f(0, 0, zpos);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

/** @brief Loads a PNG file and turns it into a texture.
 *
 *  @param texture Texture to write to.
 *  @param filename The PNG file to load.
 *  @param alpha 1 = Create texture with alpha channel (taken from image),
 *               0 = Create texture without alpha channel.
 */
void load_texture_png(texture_t *texture, char *filename, int alpha, int clamp) {
	/* Create storage space for the texture */
	SDL_Surface *texture_image;

	DBG_LOG("Loading texture: %s", filename);

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if ((texture_image = IMG_Load(filename))) {
		SDL_Rect area;
		area.x = 0;
		area.y = 0;
		area.w = texture_image->w;
		area.h = texture_image->h;
		*texture = SDL_GL_LoadTexture(texture_image, &area, alpha, clamp);
	} else {
		DBG_ERROR("Could not load texture: %s", filename);
		exit(1);
	}

	/* Free up any memory we may have used */
	if (texture_image)
		SDL_DestroySurface(texture_image);
}
