
#include "ui_sdlgl.h"

void draw_name_dialog( float xpos, float ypos, char* name, int left, int white )
{
    float width, height;
    int namew, nameh;

    gg_system_get_string_size(name, &namew, &nameh );

    width=100;
    height=30;

    /* draw avatar */
    if ( white == 1 )
    {
        draw_texture( get_white_piece(GUI_PIECE_AVATAR), xpos-45+2, ypos-50-2, 100, 100, 1.0f, get_col(COL_BLACK));
        draw_texture( get_white_piece(GUI_PIECE_AVATAR), xpos-45, ypos-50, 100, 100, 1.0f, get_col(COL_WHITE));
    }
    else
    {
        draw_texture( get_black_piece(GUI_PIECE_AVATAR), xpos+45+2, ypos-50-2, 100, 100, 1.0f, get_col(COL_BLACK));
        draw_texture( get_black_piece(GUI_PIECE_AVATAR), xpos+45, ypos-50, 100, 100, 1.0f, get_col(COL_WHITE));
    }

    /* Draw the text stuff */
    if (!left) /* UGLY */
        text_draw_string( xpos-20, ypos+10, name, 1, get_col(COL_WHITE));
    else
        text_draw_string( xpos+20+width-(namew), ypos+10, name, 1, get_col(COL_WHITE));
}

/** @brief Renders the in-game backdrop. */
void draw_backdrop()
{
    draw_texture( get_backdrop(), 0, 0, 640, 480, 1.0f, get_col(COL_WHITE) );
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
void draw_move_list( gg_colour_t *col_normal, gg_colour_t *col_high )
{
    char **list;
    int entries, view, i;
    int y;
    int last_white, last_black;
    float x_white = 30;
    float y_white = 350;
    float x_black = 610;
    float y_black = 350;
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

    y = y_white;
    for (i = last_white; i >= 0 && i >= last_white - 8; i -= 2)
    {
        char s[11];
        if (snprintf(s, 11, "%i.%s", (i >> 1) + 1, list[i]) >= 11)
            exit(1);
        if (i != view)
            text_draw_string( x_white+5, y-5, s, 1, &col_normal2);
        else
            text_draw_string( x_white+5, y-5, s, 1, &col_high2);
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
    col_normal2=*col_normal;
    col_high2=*col_normal;
    y = y_black;
    if (IS_BLACK(get_board()->turn))
    {
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
    for (i = last_black; i >= 0 && i >= last_black - (IS_BLACK(get_board()->turn) ? 6 : 8); i -= 2)
    {
        if (i != view)
            text_draw_string_right( x_black-5, y-5, list[i], 1, &col_normal2);
        else
            text_draw_string_right( x_black-5, y-5, list[i], 1, &col_high2);
        y -= text_height();
        col_normal2.a-=0.15f;
        col_high2.a-=0.15f;
    }
}

/* Draw .. health bars? */
void draw_health_bars()
{
    float white_health_percent;
    float black_health_percent;
    int black_health;
    int white_health;

    /* This function really stinks, and will be fixed ;) .... eventually */
    /* Full health = 39 */
    /* pawn  1, knight 3, bishop 3, rook 5, queen 9 */

    /* Get da new healths? */
    white_health = 39 -((get_board()->captured[WHITE_PAWN])+
                        (get_board()->captured[WHITE_ROOK]*5)+(get_board()->captured[WHITE_BISHOP]*3)+
                        (get_board()->captured[WHITE_KNIGHT]*3)+(get_board()->captured[WHITE_QUEEN]*9));
    black_health = 39 -((get_board()->captured[BLACK_PAWN])+
                        (get_board()->captured[BLACK_ROOK]*5)+(get_board()->captured[BLACK_BISHOP]*3)+
                        (get_board()->captured[BLACK_KNIGHT]*3)+(get_board()->captured[BLACK_QUEEN]*9));

    white_health_percent=(float)white_health/39;
    black_health_percent=(float)black_health/39;

    /* Draw da bar? */
    draw_rect_fill( 20, 375, 75, 10, get_col(COL_YELLOW) );
    draw_rect_fill( 640-20-75, 375, 75, 10, get_col(COL_YELLOW) );

    draw_rect_fill( 20, 375, 75*white_health_percent, 10, get_col(COL_RED) );
    draw_rect_fill( 640-20-(75*black_health_percent), 375, 75*black_health_percent,
                    10, get_col(COL_RED) );

    draw_rect( 20, 375, 75, 10, get_col(COL_BLACK) );
    draw_rect( 640-75-20, 375, 75, 10, get_col(COL_BLACK) );
}

/** @brief Renders the list of captured pieces for both sides.
 *
 *  @param col The text colour to use.
 */
void draw_capture_list(gg_colour_t *col)
{
    float x_white = 70;
    float y_white = 180;
    float x_black = 570;
    float y_black = 180;
    int i;

    for (i = 9; i > 0; i -= 2)
    {
        char s[4];
        if (get_board()->captured[i] != 0)
        {
            if (snprintf(s, 4, "%i", get_board()->captured[i]) >= 4)
                exit(1);
            text_draw_string( x_white, y_white, s, 1, col);
            draw_texture( get_black_pieces(i/2), x_white-24, y_white, 24,
                          24, 1.0f, get_col(COL_WHITE) );
        }
        y_white -= get_text_character('a')->height;
        if (get_board()->captured[i - 1] != 0)
        {
            if (snprintf(s, 4, "%i", get_board()->captured[i - 1]) >= 4)
                exit(1);
            text_draw_string_right( x_black, y_black, s, 1, col);
            draw_texture( get_white_piece((i-1)/2), x_black, y_black, 24,
                          24, 1.0f, get_col(COL_WHITE) );
        }
        y_black -= get_text_character('a')->height;
    }
}
