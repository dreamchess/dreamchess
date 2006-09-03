
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

  /*  printf( "!text: %f\n", xpos-10 );
    printf( "text: %f\n", xpos+10+width-(namew) );*/

    /* Draw the text stuff */
    if (!left) /* UGLY */
    {
        text_draw_string( xpos-10, ypos-12, name, 1, get_col(COL_WHITE));

        if ( get_black_in_check() == TRUE )
            text_draw_string_bouncy( xpos-10-80, ypos-12, "Check!", 1, get_col(COL_RED));
    }
    else
    {
        text_draw_string( xpos+10+width-(namew), ypos-12, name, 1, get_col(COL_WHITE));

        if ( get_white_in_check() == TRUE )
            text_draw_string_bouncy( xpos+10+width+5, ypos-12, "Check!", 1, get_col(COL_RED));
    }


   /* else if ( get_black_in_check() == TRUE )
        text_draw_string_bouncy( xpos+10+width-(namew), ypos-12, "Check!", 1, get_col(COL_RED));*/

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
    int leftx, rightx, bary, barw, barh;
    gg_colour_t left_col, right_col;
    int white_max=0, black_max=0;
    int i;

    /* This function really stinks, and will be fixed ;) .... eventually */
    /* Full health = 39 */
    /* pawn  1, knight 3, bishop 3, rook 5, queen 9 */

    for ( i=0; i<64; i++ )
    {
        switch ( get_board()->square[i] )
        {
            case WHITE_PAWN:
                white_max+=1; break;
            case WHITE_BISHOP:
                white_max+=3; break;
            case WHITE_KNIGHT:
                white_max+=3; break;
            case WHITE_ROOK:
                white_max+=5; break;
            case WHITE_QUEEN:
                white_max+=9; break;

            case BLACK_PAWN:
                black_max+=1; break;
            case BLACK_BISHOP:
                black_max+=3; break;
            case BLACK_KNIGHT:
                black_max+=3; break;
            case BLACK_ROOK:
                black_max+=5; break;
            case BLACK_QUEEN:
                black_max+=9; break;
        }
    }

    white_health = ((get_board()->captured[WHITE_PAWN])+
                        (get_board()->captured[WHITE_ROOK]*5)+(get_board()->captured[WHITE_BISHOP]*3)+
                        (get_board()->captured[WHITE_KNIGHT]*3)+(get_board()->captured[WHITE_QUEEN]*9));
    black_health = ((get_board()->captured[BLACK_PAWN])+
                        (get_board()->captured[BLACK_ROOK]*5)+(get_board()->captured[BLACK_BISHOP]*3)+
                        (get_board()->captured[BLACK_KNIGHT]*3)+(get_board()->captured[BLACK_QUEEN]*9));

    white_max+=white_health;
    black_max+=black_health;

    white_health = white_max - white_health;
    black_health = black_max - black_health;

    /*printf( "White health is %i\n", white_max );
    printf( "Black health is %i\n", black_max );*/

    white_health_percent=(float)white_health/white_max;
    black_health_percent=(float)black_health/black_max;

    /* Draw da bar? */
    bary=440; barw=192; barh=15;
    leftx=100; rightx=639-100-barw;

    /* Set bar colours.. */
    left_col.b=0.0f; left_col.a=1.0f;
    right_col=left_col;

    /* Draw white.. */
    if ( white_health_percent > 0.80 )
    {
        right_col.r=0.59f;
        right_col.g=0.60f;
        right_col.b=0.86f;
    }
    else if ( white_health_percent > 0.60 )
    {
        right_col.r=1.0f;
        right_col.g=1.0f;
        right_col.b=0.0f;
    }
    else if ( white_health_percent > 0.40 )
    {
        right_col.r=1.0f;
        right_col.g=0.5f;
        right_col.b=0.0f;
    }
    else if ( white_health_percent > 0.20 )
    {
        right_col.r=1.0f;
        right_col.g=0.0f;
        right_col.b=0.0f;
    }

    left_col.r=right_col.r-0.4f;
    left_col.g=right_col.g-0.4f;
    left_col.b=right_col.b-0.4f;

    draw_rect_fill( leftx-2, bary-2, barw*white_health_percent+4, barh+4, get_col(COL_WHITE));
    draw_rect_fill_gradient( leftx, bary, barw*white_health_percent, barh,
        &left_col, &right_col, &left_col, &right_col);

    /* Draw black.. */
    if ( black_health_percent > 0.80 )
    {
        right_col.r=0.59f;
        right_col.g=0.60f;
        right_col.b=0.86f;
    }
    else if ( black_health_percent > 0.60 )
    {
        right_col.r=1.0f;
        right_col.g=1.0f;
        right_col.b=0.0f;
    }
    else if ( black_health_percent > 0.40 )
    {
        right_col.r=1.0f;
        right_col.g=0.5f;
        right_col.b=0.0f;
    }
    else if ( black_health_percent > 0.20 )
    {
        right_col.r=1.0f;
        right_col.g=0.0f;
        right_col.b=0.0f;
    }

    left_col.r=right_col.r-0.4f;
    left_col.g=right_col.g-0.4f;
    left_col.b=right_col.b-0.4f;

    draw_rect_fill( 639-100-2-(int)(barw*black_health_percent), bary-2, barw*black_health_percent+4, 
        barh+4, get_col(COL_WHITE));
    draw_rect_fill_gradient( 639-100-(int)(barw*black_health_percent), bary, 
        barw*black_health_percent, barh, &right_col, &left_col, &right_col, &left_col);
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
            draw_texture( get_black_piece(i/2), x_white-24, y_white, 24,
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
