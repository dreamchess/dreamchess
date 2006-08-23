/*  DreamChess
 *  Copyright (C) 2003-2005  The DreamChess project
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

#include "ui_sdlgl.h"

static float mouse_square;

float get_mouse_square()
{
    return mouse_square;
}

/** @brief Main in-game rendering routine.
 *
 *  @param b Board configuration to render.
 */
void draw_scene( board_t *b )
{
    char temp[80];
    int clock_seconds=0;
    int clock_minutes=0;
    int mouse_x=0, mouse_y=0;

    gg_dialog_cleanup();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 

    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);

    draw_backdrop();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    go_3d(SCREEN_WIDTH, SCREEN_HEIGHT);
    glDepthMask(GL_TRUE);

    render_scene_3d(b);
    mouse_square=find_square(get_mouse_x(), get_mouse_y(), get_zerodepth());

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);

    /* draw_captured_pieces( 480, 70 ); */
    glPushMatrix();

    /*draw_border(get_ingame_style()->border.image, NULL, 0, gg_rect(20, 440, 170, 20), 8);*/

    /*draw_border(get_ingame_style()->border.image, NULL, 0, gg_rect(455, 440, 170, 20), 8);*/

    /* Da clocken */
    /*draw_border(style_ingame.border.textured.image, NULL, 0, gg_rect(290, 440, 60, 20), 8);*/

    glPopMatrix();

    glPushMatrix();
    draw_name_dialog( 50, 430, get_white_name(), TRUE, 1 );
    draw_name_dialog( 490, 430, get_black_name(), FALSE, 0 );
    /*draw_border(get_ingame_style()->border.image, NULL, 0, gg_rect(20, 375, 75, 10), 8);
    draw_border(get_ingame_style()->border.image, NULL, 0, gg_rect(545, 375, 75, 10), 8);*/
    draw_health_bars();
    draw_move_list(get_col(COL_WHITE), get_col(COL_YELLOW));
    draw_capture_list(get_col(COL_WHITE));

    clock_minutes=(((SDL_GetTicks()-get_turn_counter())/1000)/60);
    clock_seconds=((SDL_GetTicks()-get_turn_counter())/1000)-(clock_minutes*60);
    sprintf( temp, "%i:%02i", clock_minutes, clock_seconds );
    /*text_draw_string( 303, 440, temp, 1, &col_black);*/
    glPopMatrix();

    if ( get_white_in_check() == TRUE )
        text_draw_string_bouncy( 180, 420, "White is in check!", 1.5, get_col(COL_WHITE));
    else if ( get_black_in_check() == TRUE )
        text_draw_string_bouncy( 180, 420, "Black is in check!", 1.5, get_col(COL_WHITE));

    gg_dialog_render_all();

    if ( get_fading_out() )
    {
        if ( !draw_fade( FADE_OUT ) )
            set_switch_to_menu(TRUE);
    }
    else
        draw_fade( FADE_IN );

    /* Draw mouse cursor.. */
    #ifndef _arch_dreamcast
    SDL_GetMouseState(&mouse_x, &mouse_y);
    draw_texture( get_mouse_cursor(), mouse_x, (479-mouse_y-32), 32, 32, 1.0f, get_col(COL_WHITE));
    #endif /* _arch_dreamcast */

    /* Draw it to the screen */
    gl_swap();
}
