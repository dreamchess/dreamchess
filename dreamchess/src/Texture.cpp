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

#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include "Texture.h"
#include "System.h"

#include <stdio.h>

int power_of_two(int input) {
    int value = 1;

    while ( value < input )
    {
        value <<= 1;
    }
    return value;
}

void Texture::load(const char *filename, int alpha, int clamp )
{
    /* Create storage space for the texture */
    SDL_Surface *texture_image;

    //DBG_LOG("loading texture: %s", filename );

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( ( texture_image = IMG_Load( filename ) ) ) {
        SDL_Rect area;
        area.x = 0;
        area.y = 0;
        area.w = texture_image->w;
        area.h = texture_image->h;
        loadFromSurface(texture_image, &area, alpha, clamp);
    }
    else {
    	printf("could not load texture: %s", filename);
        //DBG_ERROR("could not load texture: %s", filename);
        exit(1);
    }

    /* Free up any memory we may have used */
    if ( texture_image )
        SDL_FreeSurface( texture_image );
}

glm::vec4 Texture::getUVForRegion(int x, int y, int width, int height) {
    return glm::vec4((float)x / (float)_nativeSize.x, (float)y / (float)_nativeSize.y, (float)(width+x) / (float)_nativeSize.x, (float)(height+y) / (float)_nativeSize.y);
}

void Texture::loadFromSurface(SDL_Surface *surface, SDL_Rect *area, int alpha, int clamp) {
    int w, h;
    SDL_Surface *image;
    SDL_Rect dest;
    Uint32 saved_flags;
    Uint8  saved_alpha;

    /* Use the surface width and height expanded to powers of 2 */
    w = power_of_two(area->w);
    h = power_of_two(area->h);

    image = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                w, h,
                32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
                0x000000FF,
                0x0000FF00,
                0x00FF0000,
                0xFF000000
#else
                0xFF000000,
                0x00FF0000,
                0x0000FF00,
                0x000000FF
#endif
            );
    if ( image == NULL ) {
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
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (clamp) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 (alpha ? 4 : 3),
                 w, h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    _uv = glm::vec2(area->w / (float) w, area->h / (float) h);
    _size = glm::vec2(area->w, area->h);
    _nativeSize = glm::vec2(w, h);
}