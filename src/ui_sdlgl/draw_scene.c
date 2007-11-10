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

static float mouse_square;

float get_mouse_square()
{
    return mouse_square;
}

/** @brief Main in-game rendering routine.
 *
 *  @param b Board configuration to render.
 */
void draw_scene( board_t *b, int reflections )
{
    char temp[80];
    int clock_seconds=0;
    int clock_minutes=0;

    gg_dialog_cleanup();

    glDisable(GL_BLEND);
    glDepthFunc(GL_ALWAYS);

    draw_backdrop();

    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);

    go_3d(get_screen_width(), get_screen_height());

    render_scene_3d(b, reflections);
    mouse_square=find_square(get_true_mouse_x(), get_true_mouse_y(), get_zerodepth());

    resize_window(get_screen_width(), get_screen_height());

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

    /*if ( get_white_in_check() == TRUE )
        text_draw_string_bouncy( 180, 420, "White is in check!", 1, get_col(COL_WHITE));
    else if ( get_black_in_check() == TRUE )
        text_draw_string_bouncy( 180, 420, "Black is in check!", 1, get_col(COL_WHITE));*/

    gg_dialog_render_all();

    if ( get_fading_out() )
    {
        if ( !draw_fade( FADE_OUT ) )
            set_switch_to_menu(TRUE);
    }
    else
    {
        if ( get_show_egg() ) 
            draw_sonic_fade( FADE_IN );
        else
            draw_fade( FADE_IN );
    }

    /* Draw mouse cursor.. */
    #ifndef _arch_dreamcast
    #ifndef __BEOS__
    draw_texture( get_mouse_cursor(), get_mouse_x(), (479-get_mouse_y()-32), 32, 32, 1.0f, get_col(COL_WHITE));
	#endif /* __BEOS__ */
    #endif /* _arch_dreamcast */

    /* Draw it to the screen */
    gl_swap();
}
