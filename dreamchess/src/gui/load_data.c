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

void load_border(texture_t border[9], char *filename)
{
    /* Create storage space for the texture */
    SDL_Surface *surface;

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ((surface = IMG_Load(filename)))
    {
        int i;
        for (i = 0; i < 9; i++)
        {
            SDL_Rect rect;
            rect.x = (i % 3) * surface->w / 3;
            rect.y = (i / 3) * surface->h / 3;
            rect.w = surface->w / 3;
            rect.h = surface->h / 3;
            border[i] = SDL_GL_LoadTexture(surface, &rect, 1,1);

            // Render size
            border[i].width = 16;
            border[i].height = 16;

            glBindTexture(GL_TEXTURE_2D, border[i].id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        /* Free up any memory we may have used */
        SDL_FreeSurface(surface);
    }
    else
    {
        DBG_ERROR("could not load texture: %s", filename);
        exit(1);
    }
}

void unload_border(texture_t border[9])
{
    int i;

    for (i = 0; i < 9; i++)
    {
        glDeleteTextures(1, &border[i].id);
    }
}

/** @brief Loads the textures for the chess pieces. */
void load_pieces(void)
{
    int i, j;
    texture_t texture;
    int ypos = 0;
    int tex_height, tex_width;

    load_texture_png(&texture, "pieces.png", 1,1);

    tex_height = power_of_two(texture.height);
    tex_width = power_of_two(texture.width);

    for (i = 0; i < 2; i++ )
    {
        int xpos = 0;
        texture_t *pieces;
        if (i == 0)
            pieces = get_white_pieces();
        else
            pieces = get_black_pieces();

        for (j = 0; j < 7; j++ )
        {
            texture_t c;
            c.width = texture.width / 7;
            c.height = texture.height / 2;
            c.u1 = xpos / (float) tex_width;
            c.v1 = ypos / (float) tex_height;
            xpos += c.width;
            c.u2 = xpos / (float) tex_width;
            c.v2 = (ypos + c.height) / (float) tex_height;
            c.id = texture.id;
            c.width = 64;
            c.height = 64;
            switch (j)
            {
            case 0:
                pieces[GUI_PIECE_KING] = c;
                break;
            case 1:
                pieces[GUI_PIECE_QUEEN] = c;
                break;
            case 2:
                pieces[GUI_PIECE_ROOK] = c;
                break;
            case 3:
                pieces[GUI_PIECE_KNIGHT] = c;
                break;
            case 4:
                pieces[GUI_PIECE_BISHOP] = c;
                break;
            case 5:
                pieces[GUI_PIECE_PAWN] = c;
                break;
            case 6:
                pieces[GUI_PIECE_AVATAR] = c;
            }
        }
        ypos += texture.height / 2;
    }
}
