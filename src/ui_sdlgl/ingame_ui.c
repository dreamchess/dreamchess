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

void draw_move_lists( coord3_t offset, gg_colour_t *col_normal, gg_colour_t *col_high );
void draw_capture_list( coord3_t offset, gg_colour_t *col);

coord3_t transition;
float transition_speed=50;
float transition_start_pos;
float trans_amount=-300;
int trans_in;

void reset_transition( int in )
{
    if ( in )
    {
        transition_start_pos=SDL_GetTicks();
        transition.x=trans_amount;
        trans_in=TRUE;
    }
    else
    {
        transition_start_pos=SDL_GetTicks();
        transition.x=0;
        trans_in=FALSE;
    }
}

void transition_update()
{
    float ticks=SDL_GetTicks();

    if ( trans_in  )
    {
        if ( transition.x < 0 )
        {
            if (transition.x+((ticks-transition_start_pos)/transition_speed) > 0 )
                transition.x=0;
            else
                transition.x+=((ticks-transition_start_pos)/transition_speed);
        }
        else 
            transition.x=0;
    }
    else
    {
        if ( transition.x > trans_amount )
            transition.x-=((ticks-transition_start_pos)/transition_speed);
        else 
            transition.x=trans_amount;
    }

    printf( "F: %f\n", transition.x );
}

void draw_health_bar( coord3_t position, coord3_t size, int white )
{
    int health;
    float bar_len;

    if ( white )
    {
        health = 36-((get_board()->captured[WHITE_PAWN])+
            (get_board()->captured[WHITE_ROOK]*5)+(get_board()->captured[WHITE_BISHOP]*3)+
            (get_board()->captured[WHITE_KNIGHT]*3)+(get_board()->captured[WHITE_QUEEN]*9));
    }
    else
    {
        health = 36-((get_board()->captured[BLACK_PAWN])+
            (get_board()->captured[BLACK_ROOK]*5)+(get_board()->captured[BLACK_BISHOP]*3)+
            (get_board()->captured[BLACK_KNIGHT]*3)+(get_board()->captured[BLACK_QUEEN]*9));
    }

    if ( health > 36 )
        health=36;
    if ( health < 2 )
        health=2;

    gg_colour_t top_left_col={0.59f,0.60f,0.86f,1.0f};
    gg_colour_t bottom_left_col={0.19f,0.20f,0.46f,1.0f};
    gg_colour_t top_right_col={0.79f,0.80f,1.0f,1.0f};
    gg_colour_t bottom_right_col={0.59f,0.60f,0.86f,1.0f};

    bar_len=(size.x)*((float)health/36.0f);

    if ( white )
    {
        draw_rect_fill( position.x, position.y, bar_len, size.y, get_col(COL_WHITE));
        draw_rect_fill_gradient( position.x+1, position.y+1, bar_len-2, size.y-2,
            &bottom_left_col, &bottom_right_col, &top_left_col, &top_right_col);
    }
    else
    {
        draw_rect_fill( position.x+(size.x-bar_len), position.y, bar_len, size.y, get_col(COL_WHITE));
        draw_rect_fill_gradient( position.x+1+(size.x-bar_len), position.y+1, bar_len-2, size.y-2,
            &bottom_right_col, &bottom_left_col, &top_right_col, &top_left_col);
    }
}

void draw_player_status( coord3_t offset, int white )
{
    if (white) /* UGLY */
    {
        if ( get_game_stalemate() == TRUE )
            text_draw_string_bouncy( offset.x, offset.y, "Tied!", 1, get_col(COL_WHITE));
        else if ( get_white_in_checkmate() == TRUE )
            text_draw_string_bouncy( offset.x, offset.y, "Checkmate!", 1, get_col(COL_RED));
        else if ( get_white_in_check() == TRUE )
            text_draw_string_bouncy( offset.x, offset.y, "Check!", 1, get_col(COL_RED));


        if ( get_black_in_checkmate() == TRUE )
            text_draw_string_bouncy( offset.x, offset.y, "Victory!", 1, get_col(COL_WHITE));
    }
    else
    {
        int namew, nameh;

        if ( get_game_stalemate() == TRUE )
        {
            gg_system_get_string_size("Tied!", &namew, &nameh );
            text_draw_string_bouncy( 640-offset.x-namew, offset.y, "Tied!", 1, get_col(COL_WHITE));
        }
        else if ( get_black_in_checkmate() == TRUE )
        {
            gg_system_get_string_size("Checkmate!", &namew, &nameh );
            text_draw_string_bouncy( 640-offset.x-namew, offset.y, "Checkmate!", 1, get_col(COL_RED));
        }
        else if ( get_black_in_check() == TRUE )
        {
            gg_system_get_string_size("Check!", &namew, &nameh );
            text_draw_string_bouncy( 640-offset.x-namew, offset.y, "Check!", 1, get_col(COL_RED));
        }
        if ( get_white_in_checkmate() == TRUE )
        {
            gg_system_get_string_size("Victory!", &namew, &nameh );
            text_draw_string_bouncy( 640-offset.x-namew, offset.y, "Victory!", 1, get_col(COL_WHITE));
        }
    }
}

void draw_ui_elements()
{
    int namew, nameh;

    /* Enable/disable elements, set positions/sizes. */
    int avatars=TRUE;
    coord3_t avatar_size={100,100};
    coord3_t avatar_offset={0+transition.x,480-avatar_size.y};

    int shadows=TRUE;
    coord3_t shadow_offset={2,2};

    int names=TRUE;
    char *white_name, *black_name;
    coord3_t name_offset={100+transition.x,480-60};
    coord3_t white_name_size, black_name_size;

    int clocks=TRUE;
    char *white_clock, *black_clock;
    coord3_t clock_offset={300+transition.x,480-60};
    coord3_t white_clock_size, black_clock_size;

    int health_bars=TRUE;
    coord3_t health_bar_offset={100+transition.x,480-40};
    coord3_t health_bar_size={200,15};

    int move_lists=TRUE;
    coord3_t move_list_offset={30+transition.x,350};

    int capture_lists=TRUE;
    coord3_t capture_list_offset={60+transition.x,180};

    int player_status=TRUE;
    coord3_t player_status_offset={25+transition.x,480-80};


    /* Get name sizes, string */
    white_name=get_white_name();
    gg_system_get_string_size(white_name, &namew, &nameh );
    white_name_size.x=namew; white_name_size.y=nameh;
    black_name=get_black_name();
    gg_system_get_string_size(black_name, &namew, &nameh );
    black_name_size.x=namew; black_name_size.y=nameh;

    /* Get clock sizes, string */
    white_clock="00:00";
    gg_system_get_string_size(white_clock, &namew, &nameh );
    white_clock_size.x=namew; white_clock_size.y=nameh;

    black_clock="00:00";
    gg_system_get_string_size(black_clock, &namew, &nameh );
    black_clock_size.x=namew; black_clock_size.y=nameh;

    /* Draw the avatars. */
    if ( avatars )
    {
        if ( shadows )
        {
            draw_texture( get_white_piece(GUI_PIECE_AVATAR), avatar_offset.x+shadow_offset.x, 
                avatar_offset.y-shadow_offset.y, avatar_size.x, avatar_size.y, 1.0f, get_col(COL_BLACK));

            draw_texture( get_black_piece(GUI_PIECE_AVATAR), 640-avatar_size.x-avatar_offset.x+shadow_offset.x, 
                avatar_offset.y-shadow_offset.y, avatar_size.x, avatar_size.y, 1.0f, get_col(COL_BLACK));
        }
        draw_texture( get_white_piece(GUI_PIECE_AVATAR), avatar_offset.x, avatar_offset.y, avatar_size.x, avatar_size.y,
            1.0f, get_col(COL_WHITE));

        draw_texture( get_black_piece(GUI_PIECE_AVATAR), 640-avatar_size.x-avatar_offset.x, avatar_offset.y, 
            avatar_size.x, avatar_size.y, 1.0f, get_col(COL_WHITE));
    }

    /* Draw the names */
    if ( names )
    {
        text_draw_string( name_offset.x, name_offset.y, white_name, 1, get_col(COL_WHITE));
        text_draw_string( 640-black_name_size.x-name_offset.x, name_offset.y, black_name, 1, get_col(COL_WHITE));
    }

    /* Draw the clocks */
    if ( clocks )
    {
        text_draw_string( clock_offset.x-white_clock_size.x, clock_offset.y, white_clock, 1, get_col(COL_WHITE));
        text_draw_string( 640-clock_offset.x, clock_offset.y, black_clock, 1, get_col(COL_WHITE));
    }

    /* Draw the health bars. */
    if ( health_bars )
    {
        coord3_t black;
        draw_health_bar( health_bar_offset, health_bar_size, TRUE );

        black=health_bar_offset;
        black.x=640-black.x-health_bar_size.x;
        draw_health_bar( black, health_bar_size, FALSE );
    }   

    /* Draw the move lists. */
    if ( move_lists )
        draw_move_lists( move_list_offset, get_col(COL_WHITE), get_col(COL_YELLOW));

    if ( capture_lists )
        draw_capture_list( capture_list_offset, get_col(COL_WHITE));

    if ( player_status )
    {
        draw_player_status( player_status_offset, TRUE );
        draw_player_status( player_status_offset, FALSE );
    }
}

/** @brief Renders the in-game backdrop. */
void draw_backdrop()
{
    draw_texture( get_backdrop(), 0, 0, 640, 480, -1.0f, get_col(COL_WHITE) );
}

/** @brief Renders the move list.
 *
 *  Only the last 5 moves (max) for each side are shown to prevent the screen
 *  from getting cluttered. The last move before the current board position
 *  is highlighted.
 *
 *  @param col_normal Text colour for move list.
 *  @param col_high Text colour for highlighting the last move.
 */
void draw_move_lists( coord3_t offset, gg_colour_t *col_normal, gg_colour_t *col_high )
{
    char **list;
    int entries, view, i;
    int y;
    int last_white, last_black;

    gg_colour_t col_normal2=*col_normal;
    gg_colour_t col_high2=*col_normal;

    game_get_move_list(&list, &entries, &view);

    if (IS_BLACK(get_board()->turn))
    {
        last_white = view;
        last_black = view - 1;
    }
    else
    {
        last_black = view;
        last_white = view - 1;
    }

    y = offset.y;
    for (i = last_white; i >= 0 && i >= last_white - 8; i -= 2)
    {
        char s[11];
        if (snprintf(s, 11, "%i.%s", (i >> 1) + 1, list[i]) >= 11)
            exit(1);
        if (i != view)
            text_draw_string( offset.x+5, y-5, s, 1, &col_normal2);
        else
            text_draw_string( offset.x+5, y-5, s, 1, &col_high2);
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
    col_normal2=*col_normal;
    col_high2=*col_normal;
    y = offset.y;
    if (IS_BLACK(get_board()->turn))
    {
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
    for (i = last_black; i >= 0 && i >= last_black - (IS_BLACK(get_board()->turn) ? 6 : 8); i -= 2)
    {
        if (i != view)
            text_draw_string_right( 640-offset.x-5, y-5, list[i], 1, &col_normal2);
        else
            text_draw_string_right( 640-offset.x-5, y-5, list[i], 1, &col_high2);
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
}

/** @brief Renders the list of captured pieces for both sides.
 *
 *  @param col The text colour to use.
 */
void draw_capture_list( coord3_t offset, gg_colour_t *col)
{
    /*float x_white = 60;
    float y_white = 180;
    float x_black = 580;
    float y_black = 180;*/
    int i;

    for (i = 9; i > 0; i -= 2)
    {
        char s[4];
        /*if (get_board()->captured[i] != 0)
        {*/
            if (snprintf(s, 4, "%i", get_board()->captured[i]) >= 4)
                exit(1);
            text_draw_string( offset.x, offset.y, s, 1, col);
            draw_texture( get_black_piece(i/2), offset.x-24, offset.y, 24,
                          24, 1.0f, get_col(COL_WHITE) );
       /* }*/
        //offset.y -= 28; /*get_text_character('a')->height;*/
        /*if (get_board()->captured[i - 1] != 0)
        {*/
            if (snprintf(s, 4, "%i", get_board()->captured[i - 1]) >= 4)
                exit(1);
            text_draw_string_right( 640-offset.x, offset.y, s, 1, col);
            draw_texture( get_white_piece((i-1)/2), 640-offset.x, offset.y, 24,
                          24, 1.0f, get_col(COL_WHITE) );
       /* }*/
        offset.y -= 28; /*get_text_character('a')->height;*/
    }
}
