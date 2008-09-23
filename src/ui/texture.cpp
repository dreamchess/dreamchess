
#include "SDL_opengl.h"
#include "SDL.h"
#include "SDL_image.h"
#include "texture.h"
#include <string>

int power_of_two(int input)
{
    int value = 1;

    while ( value < input )
    {
        value <<= 1;
    }
    return value;
}

texture_t SDL_GL_LoadTexture(SDL_Surface *surface, SDL_Rect *area, int alpha, int clamp)
{
    texture_t texture;
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
    if ( image == NULL )
    {
        exit(0);
    }

    /* Save the alpha blending attributes */
    saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
    saved_alpha = surface->format->alpha;
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA )
    {
        SDL_SetAlpha(surface, 0, 0);
    }

    /* Copy the surface into the GL texture image */
    dest.x = 0;
    dest.y = 0;
    dest.w = area->w;
    dest.h = area->h;
    SDL_BlitSurface(surface, area, image, &dest);

    /* Restore the alpha blending attributes */
    if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA )
    {
        SDL_SetAlpha(surface, saved_flags, saved_alpha);
    }

    /* Create an OpenGL texture for the image */
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    #ifndef __BEOS__
    if (clamp)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    #endif /* __BEOS__ */
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 (alpha ? 4 : 3),
                 w, h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    texture.u1 = 0;
    texture.v1 = 0;
    texture.u2 = area->w / (float) w;
    texture.v2 = area->h / (float) h;
    texture.width = area->w;
    texture.height = area->h;

    return texture;
}

void load_texture_png( texture_t *texture, char *filename, int alpha, int clamp )
{
    /* Create storage space for the texture */
    SDL_Surface *texture_image;

    printf("loading texture: %s\n", filename );

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( ( texture_image = IMG_Load( filename ) ) )
    {
        SDL_Rect area;
        area.x = 0;
        area.y = 0;
        area.w = texture_image->w;
        area.h = texture_image->h;
        *texture = SDL_GL_LoadTexture(texture_image, &area, alpha, clamp);
    }
    else
    {
        printf("could not load texture: %s\n", filename);
        exit(1);
    }

    /* Free up any memory we may have used */
    if ( texture_image )
        SDL_FreeSurface( texture_image );
}

texture::texture(std::string filename)
{
    load_texture_png( &texture_data, (char*)filename.c_str(), 1, 0 );
}

texture::~texture()
{
    
}

