/*  DreamChess
**  Copyright (C) 2004  The DreamChess project
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef WITH_UI_SDL

#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_thread.h"
#include "SDL_ttf.h"

#include "history.h"
#include "ui.h"
#include "datadir.h"

/* dialogs */
#define DIALOG_DIFFICULTY  0
#define DIALOG_NONE        1
#define DIALOG_SUREQUIT    2
#define DIALOG_SYSTEM      3
#define DIALOG_PROMOTE_WHITE     4
#define DIALOG_PROMOTE_BLACK     5

#define GUI_PIECE_PAWN     0
#define GUI_PIECE_ROOK     1
#define GUI_PIECE_BISHOP   2
#define GUI_PIECE_KNIGHT   3
#define GUI_PIECE_QUEEN    4
#define GUI_PIECE_KING     5

/* Text message defines */
#define GUI_TEXT_MAX_MESSAGES       27
#define GUI_TEXT_PIECES_CAPTURED    0
#define GUI_TEXT_BLACK_TURN      1
#define GUI_TEXT_WHITE_TURN                2
#define GUI_TEXT_BAD_MOVE                 3
#define GUI_TEXT_VERSION                     4
#define GUI_TEXT_YES                              5
#define GUI_TEXT_NO                                6
#define GUI_TEXT_SUREQUIT                   7
#define GUI_TEXT_DIFFICULTY                8
#define GUI_TEXT_QUITGAME   9
#define GUI_TEXT_SAVEGAME   10
#define GUI_TEXT_LOADGAME   11
#define GUI_TEXT_EATGAME   12
#define MENU_TEXT_START_GAME           13
#define MENU_TEXT_AI_DIFFICULTY        14
#define MENU_TEXT_AI_DIFFICULTY_EASY   15
#define MENU_TEXT_AI_DIFFICULTY_HARD   16
#define MENU_TEXT_WHITE_METHOD         17
#define MENU_TEXT_BLACK_METHOD         18
#define MENU_TEXT_METHOD_AI            19
#define MENU_TEXT_METHOD_HUMAN         20
#define MENU_TEXT_METHOD_REMOTE        21
#define MENU_TEXT_CURSOR               22
#define MENU_TEXT_QUIT_GAME            23
#define GUI_TEXT_BURP                  24
#define GUI_TEXT_CHOOSEPROMOTE         25
#define GUI_TEXT_THEME                 26


/* screen width, height, and bit depth */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

/* Define our booleans */
#define TRUE  1
#define FALSE 0

#define INPUT_NONE -1
#define INPUT_VIEW_PREV -2
#define INPUT_VIEW_NEXT -3
#define INPUT_UNDO -4

typedef struct
{
    int width, height;
    SDL_Surface message;
}
text_message;

typedef struct
{
    int difficulty;
    int white_method, black_method;
}
game_settings;

/* This is our SDL surface */
SDL_Surface *surface;


extern board_t board;

static void init_gl();
static void kill_gui();
static void resize_window( int width, int height );
static void draw_textured_piece( float xpos, float ypos, int piece, float board_x, float board_y, float square_size, float alpha );
static void draw_text_message( text_message *message, float xpos, float ypos );
static void draw_texture( SDL_Surface *texture, float xpos, float ypos );
static void create_text_texture( text_message textures[], int texture_id, char *text );
static void load_theme( char* name );
static int GetMove(history_t *history);

static unsigned int board_squares[2];
static unsigned int white_pieces[6];
static unsigned int black_pieces[6];
static unsigned int num_of_captured_black;
static unsigned int captured_black_pieces[16];
static unsigned int num_of_captured_white;
static unsigned int captured_white_pieces[16];
static text_message text_messages[GUI_TEXT_MAX_MESSAGES];
static int selected_x, selected_y;
static int mouse_x_pos, mouse_y_pos;

/* Menu stuff */
static SDL_Surface *menu_title;

static board_t lboard;
static volatile int event_flag;
static volatile int gui_init_complete=0;
static float board_xpos, board_ypos;
static float fade;
extern int quit_game;
extern int game_finished;
static int selected_piece;
static int your_turn;
static int in_dialog=DIALOG_NONE;
static int in_burp=FALSE;
static int prev_dialog=DIALOG_NONE;
static int dialog_selection;
static char* themelist[25];
static text_message theme_names[25];
static int themelistindex;
static int num_theme;

static game_settings settings;
static SDL_Thread *thread;
static board_t *new_board;
static int status;

static void draw_eatgame_dialog()
{
    /*    float r,g,b;
        r=0.01*(rand()%100);
        g=0.01*(rand()%100);
        b=0.01*(rand()%100);
        glEnable(GL_BLEND);
     
        glColor4f( r, g, b, 0.8f );
     
        glBegin( GL_QUADS );
        glVertex3f( 530, 250, 0.9f );
        glVertex3f( 530, 285, 0.9f );
        glVertex3f( 170, 285, 0.9f );
        glVertex3f( 170, 250, 0.9f );
        glEnd( );
     
        draw_text_message( text_messages, 180, 245, GUI_TEXT_BURP );
     
        glDisable(GL_BLEND);*/
}

static void draw_surequit_dialog()
{
    SDL_Rect rect;
    rect.x=170;
    rect.y=190;
    rect.w=360;
    rect.h=80;
    SDL_FillRect( surface, &rect, SDL_MapRGB(surface->format, 200, 200, 200) );

    draw_text_message( &text_messages[GUI_TEXT_SUREQUIT], 180, 200 );
    draw_text_message( &text_messages[GUI_TEXT_YES], 240, 225 );
    draw_text_message( &text_messages[GUI_TEXT_NO], 240, 245 );

    draw_text_message( &text_messages[MENU_TEXT_CURSOR], 210, 225+(dialog_selection*20) );
}

static void draw_system_dialog()
{
    SDL_Rect rect;
    rect.x=10;
    rect.y=10;
    rect.w=240;
    rect.h=100;
    SDL_FillRect( surface, &rect, SDL_MapRGB(surface->format, 200, 200, 200) );

    draw_text_message( &text_messages[GUI_TEXT_QUITGAME], 40, 80 );
    draw_text_message( &text_messages[GUI_TEXT_LOADGAME], 40, 60 );
    draw_text_message( &text_messages[GUI_TEXT_SAVEGAME], 40, 40 );
    draw_text_message( &text_messages[GUI_TEXT_EATGAME], 40, 20 );

    draw_text_message( &text_messages[MENU_TEXT_CURSOR], 20, 20+(dialog_selection*20) );
}

static void draw_promote_dialog()
{/*
            int colour = (in_dialog == DIALOG_PROMOTE_WHITE ? 0 : 1);
         
            glEnable(GL_BLEND);
         
            glColor4f( 0.0f, 0.0f, 0.0f, 0.5f );
         
            glBegin( GL_QUADS );
            glVertex3f( 640, 0, 0.9f );
            glVertex3f( 640, 480, 0.9f );
            glVertex3f( 0, 480, 0.9f );
            glVertex3f( 0, 0, 0.9f );
            glEnd( );
         
            glColor4f( 1.0f, 1.0f, 1.0f, 0.8f );
         
            glBegin( GL_QUADS );
            glVertex3f( 550, 200, 0.9f );
            glVertex3f( 550, 350, 0.9f );
            glVertex3f( 110, 350, 0.9f );
            glVertex3f( 110, 200, 0.9f );
            glEnd( );
         
            draw_text_message( text_messages, 130, 300, GUI_TEXT_CHOOSEPROMOTE );
            draw_textured_piece( 150, 250, KNIGHT + colour, 0, 0, 460/8, 1.0f );
            draw_textured_piece( 250, 250, BISHOP + colour, 0, 0, 460/8, 1.0f );
            draw_textured_piece( 350, 250, ROOK + colour, 0, 0, 460/8, 1.0f );
            draw_textured_piece( 450, 250, QUEEN + colour, 0, 0, 460/8, 1.0f );
         
            draw_text_message( text_messages, 140+(dialog_selection*100), 240, MENU_TEXT_CURSOR );
         
            glDisable(GL_BLEND);*/
}

static void draw_difficulty_dialog()
{
    /*
    glEnable(GL_BLEND);

    glColor4f( 0.0f, 0.0f, 0.0f, 0.5f );

    glBegin( GL_QUADS );
    glVertex3f( 640, 0, 0.9f );
    glVertex3f( 640, 480, 0.9f );
    glVertex3f( 0, 480, 0.9f );
    glVertex3f( 0, 0, 0.9f );
    glEnd( );

    glColor4f( 1.0f, 1.0f, 1.0f, 0.8f );

    glBegin( GL_QUADS );
    glVertex3f( 520, 200, 0.9f );
    glVertex3f( 520, 285, 0.9f );
    glVertex3f( 170, 285, 0.9f );
    glVertex3f( 170, 200, 0.9f );
    glEnd( );

    draw_text_message( text_messages, 180, 245, GUI_TEXT_DIFFICULTY );
    draw_text_message( text_messages, 240, 220, MENU_TEXT_AI_DIFFICULTY_EASY );
    draw_text_message( text_messages, 240, 200, MENU_TEXT_AI_DIFFICULTY_HARD );

    draw_text_message( text_messages, 210, 220-(dialog_selection*20), MENU_TEXT_CURSOR );

    glDisable(GL_BLEND);*/
}

static void do_menu()
{
    SDL_Event event;
    int menu_selection=0;
    int game_difficulty=MENU_TEXT_AI_DIFFICULTY_EASY;
    themelistindex=0;
    DIR* themedir;
    struct dirent* themedir_entry;
    int i;

    /* Populate theme list.. */
    i=0;
    num_theme=0;
    if ( (themedir=opendir("themes")) != NULL )
    {
        themedir_entry=readdir(themedir);
        while ( themedir_entry != NULL )
        {
            if ( themedir_entry->d_name[0] != '.' )
            {
                themelist[i]=strdup( themedir_entry->d_name );
                create_text_texture( theme_names, i, themedir_entry->d_name );
                i++;
            }
            themedir_entry=readdir(themedir);
        }
        num_theme=i;
    }

    while ( 1 )
    {
        /* Precess input */
        while ( SDL_PollEvent( &event ) )
        {
            if ( in_dialog == DIALOG_DIFFICULTY )
            {
                switch ( event.type )
                {
                case SDL_KEYDOWN:
                    switch ( event.key.keysym.sym )
                    {
                    case SDLK_ESCAPE:
                        in_dialog=DIALOG_NONE;
                        break;
                    case SDLK_UP:
                        if ( dialog_selection > 0 )
                            dialog_selection--;
                        break;
                    case SDLK_DOWN:
                        if ( dialog_selection < 1 )
                            dialog_selection++;
                        break;
                    case SDLK_RETURN:
                        if ( dialog_selection == 0 )
                            game_difficulty = MENU_TEXT_AI_DIFFICULTY_EASY;
                        else if ( dialog_selection == 1 )
                            game_difficulty = MENU_TEXT_AI_DIFFICULTY_HARD;
                        in_dialog=DIALOG_NONE;
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
            else
                switch ( event.type )
                {
                case SDL_KEYDOWN:
                    switch( event.key.keysym.sym )
                    {
                    case SDLK_ESCAPE:
                        status = 1;
                        break;
                    case SDLK_LEFT:
                        if ( menu_selection == 1 )
                            if ( game_difficulty > MENU_TEXT_AI_DIFFICULTY_EASY )
                                game_difficulty--;

                        if ( menu_selection == 2 )
                            if ( themelistindex > 0 )
                                themelistindex--;

                        break;
                    case SDLK_RIGHT:
                        if ( menu_selection == 1 )
                            if ( game_difficulty < MENU_TEXT_AI_DIFFICULTY_HARD )
                                game_difficulty++;

                        if ( menu_selection == 2 )
                            if ( themelistindex < num_theme-1 )
                                themelistindex++;

                        break;
                    case SDLK_UP:
                        if ( menu_selection > 0 )
                            menu_selection--;
                        break;
                    case SDLK_DOWN:
                        if ( menu_selection < 3 )
                            menu_selection++;
                        break;
                    case SDLK_RETURN:
                        if ( menu_selection == 0 )
                        {
                            /* Load theme data.... */
                            load_theme( themelist[themelistindex] );

                            return;
                        }
                        else if ( menu_selection == 1 )
                        {
                            if ( game_difficulty == MENU_TEXT_AI_DIFFICULTY_EASY )
                                dialog_selection = 0;
                            else if ( game_difficulty == MENU_TEXT_AI_DIFFICULTY_HARD )
                                dialog_selection = 1;

                            in_dialog = DIALOG_DIFFICULTY;
                        }
                        else if ( menu_selection == 3 )
                        {
                            kill_gui();
                            exit(0);
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                }
        }

        /* Draw the menu.. */
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
        draw_texture( menu_title, 150, 50 );

        draw_text_message( &text_messages[MENU_TEXT_START_GAME], 200, 320 );
        draw_text_message( &text_messages[MENU_TEXT_AI_DIFFICULTY], 200, 340 );
        draw_text_message( &text_messages[GUI_TEXT_THEME], 200, 360 );
        draw_text_message( &text_messages[MENU_TEXT_QUIT_GAME], 200, 380 );

        draw_text_message( &text_messages[MENU_TEXT_CURSOR], 180, 320+(menu_selection*20));

        draw_text_message( &theme_names[themelistindex], 300, 360 );

        if ( game_difficulty == MENU_TEXT_AI_DIFFICULTY_EASY )
            draw_text_message( &text_messages[MENU_TEXT_AI_DIFFICULTY_EASY], 300, 340 );
        else if ( game_difficulty == MENU_TEXT_AI_DIFFICULTY_HARD )
            draw_text_message( &text_messages[MENU_TEXT_AI_DIFFICULTY_HARD], 300, 340 );

        draw_text_message( &text_messages[GUI_TEXT_VERSION], 480, 460 );

        if ( in_dialog == DIALOG_DIFFICULTY )
            draw_difficulty_dialog();

        SDL_Flip(surface);
        SDL_Delay(1000/30);
    }
}

static void create_text_texture( text_message textures[], int texture_id, char *text )
{
    SDL_Surface *text_texture;
    SDL_Color black = {0,0,0};
    TTF_Font *font;
    font=TTF_OpenFont("text.ttf", 16);

    if(!font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }

    if(!(text_texture=TTF_RenderText_Blended(font, text, black )))
    {
        printf( "TTF_GetError" );
    }
    TTF_CloseFont(font);

    textures[texture_id].message=text_texture[0];
}

static void load_texture_png( unsigned int textures[], int texture_id, char *filename )
{}

static void load_texture_bmp( unsigned int textures[], int texture_id, char *filename )
{}

static void init_gui()
{
    int video_flags;
    const SDL_VideoInfo *video_info;

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    video_info = SDL_GetVideoInfo( );

    if ( !video_info )
    {
        fprintf( stderr, "Video query failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    video_flags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    /* video_flags |= SDL_FULLSCREEN; */

    if ( video_info->hw_available )
        video_flags |= SDL_HWSURFACE;
    else
        video_flags |= SDL_SWSURFACE;

    if ( video_info->blit_hw )
        video_flags |= SDL_HWACCEL;

    surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
                                video_flags );
    if ( !surface )
    {
        fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
        exit(1);
    }

    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
    }

    /* For Menu.. */
    create_text_texture( text_messages, MENU_TEXT_START_GAME, "Start Game! (press enter/esc)" );
    create_text_texture( text_messages, MENU_TEXT_AI_DIFFICULTY, "Difficulty:" );
    create_text_texture( text_messages, MENU_TEXT_AI_DIFFICULTY_EASY, "Easy as pie." );
    create_text_texture( text_messages, MENU_TEXT_AI_DIFFICULTY_HARD, "Pretty damn hard." );

    create_text_texture( text_messages, MENU_TEXT_WHITE_METHOD, "White:" );
    create_text_texture( text_messages, MENU_TEXT_BLACK_METHOD, "Black:" );
    create_text_texture( text_messages, MENU_TEXT_METHOD_AI, "Computer" );
    create_text_texture( text_messages, MENU_TEXT_METHOD_HUMAN, "Local Player" );
    create_text_texture( text_messages, MENU_TEXT_METHOD_REMOTE, "Remote Player" );
    create_text_texture( text_messages, MENU_TEXT_CURSOR, "->" );
    create_text_texture( text_messages, MENU_TEXT_QUIT_GAME, "Quit game" );
    create_text_texture( text_messages, GUI_TEXT_THEME, "Theme:" );

    create_text_texture( text_messages, GUI_TEXT_YES, "Yup!" );
    create_text_texture( text_messages, GUI_TEXT_NO, "Of course not!" );
    create_text_texture( text_messages, GUI_TEXT_SUREQUIT,
                         "Ye sure you want to quit the current game?" );
    create_text_texture( text_messages, GUI_TEXT_DIFFICULTY, "How smart would you like the AI to be, eh?" );
    create_text_texture( text_messages, GUI_TEXT_QUITGAME, "Quit the current game.." );
    create_text_texture( text_messages, GUI_TEXT_SAVEGAME, "Save the current game" );
    create_text_texture( text_messages, GUI_TEXT_LOADGAME, "Load the current game" );
    create_text_texture( text_messages, GUI_TEXT_EATGAME, "Eat the current game" );
    create_text_texture( text_messages, GUI_TEXT_BURP, "Buuuuuuuuuuuuuuuuuuuuuuuuuuuuuurp!" );
    create_text_texture( text_messages, GUI_TEXT_CHOOSEPROMOTE,
                         "A pawn has been promoted! Select your new piece!" );


    create_text_texture( text_messages, GUI_TEXT_PIECES_CAPTURED, "Pieces captured:" );
    create_text_texture( text_messages, GUI_TEXT_WHITE_TURN, "It's White's turn." );
    create_text_texture( text_messages, GUI_TEXT_BLACK_TURN, "It's Black's turn." );
    create_text_texture( text_messages, GUI_TEXT_VERSION, "ChessDC 0.1 beta3" );

    /* For the menu.. */
    menu_title = IMG_Load( "menu_title.png" );

    board_xpos=10;
    board_ypos=10;
    selected_piece=-1;
    selected_x=0;
    selected_y=0;
    gui_init_complete = 1;
}

static void load_theme( char* name )
{
    chdir("themes");
    chdir(name);

    /* Theme! */
    /*    sprintf( temp, "themes/%s/blacksquare.bmp", name );
        load_texture_bmp( board_squares, 0, temp );
        sprintf( temp, "themes/%s/whitesquare.bmp", name );
        load_texture_bmp( board_squares, 1, temp );*/

    white_pieces[GUI_PIECE_PAWN] = IMG_Load( "whitepawn.png" );
    white_pieces[GUI_PIECE_ROOK] = IMG_Load( "whiterook.png" );
    white_pieces[GUI_PIECE_KNIGHT] = IMG_Load( "whiteknight.png" );
    white_pieces[GUI_PIECE_BISHOP] = IMG_Load( "whitebishop.png" );
    white_pieces[GUI_PIECE_QUEEN] = IMG_Load( "whitequeen.png" );
    white_pieces[GUI_PIECE_KING] = IMG_Load( "whiteking.png" );
    black_pieces[GUI_PIECE_PAWN] = IMG_Load( "blackpawn.png" );
    black_pieces[GUI_PIECE_ROOK] = IMG_Load( "blackrook.png" );
    black_pieces[GUI_PIECE_KNIGHT] = IMG_Load( "blackknight.png" );
    black_pieces[GUI_PIECE_BISHOP] = IMG_Load( "blackbishop.png" );
    black_pieces[GUI_PIECE_QUEEN] = IMG_Load( "blackqueen.png" );
    black_pieces[GUI_PIECE_KING] = IMG_Load( "blackking.png" );

    ch_datadir();
}

static void kill_gui()
{
    SDL_Quit( );
}

static void init_gl()
{}

static void resize_window( int width, int height )
{}

static void draw_board( float xpos, float ypos, float square_size )
{
    int i,j,offset;
    SDL_Rect rect;
    rect.w=square_size;
    rect.h=square_size;

    for(j=0;j<8;j++)
    {
        if (j%2 == 0)
            offset=0;
        else
            offset=1;

        for (i=offset;i<8;i+=2)
        {
            rect.x=(i*square_size)+xpos;
            rect.y=(j*square_size)+ypos;
            SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));

        }
    }

    for(j=0;j<8;j++)
    {
        if (j%2 == 0)
            offset=1;
        else
            offset=0;

        for (i=offset;i<8;i+=2)
        {
            rect.x=(i*square_size)+xpos;
            rect.y=(j*square_size)+ypos;
            SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0, 0, 0));

        }
    }
}

static void draw_captured_pieces( float xpos, float ypos )
{
    /*   int i;
       draw_text_message( text_messages, xpos, ypos, GUI_TEXT_PIECES_CAPTURED );

       for ( i=0; i < num_of_captured_black; i++ )
           draw_textured_piece( xpos+(i*20), ypos-32, captured_black_pieces[i], 0, 0, 30, 1.0f );

       for ( i=0; i < num_of_captured_white; i++ )
           draw_textured_piece( xpos+(i*20), ypos-30-32, captured_white_pieces[i], 0, 0, 30, 1.0f );*/
}

static void draw_selector( int board_xpos, int board_ypos, float square_size,
                           int selector_x, int selector_y )
{
    SDL_Rect rect;
    rect.x=(selector_x*square_size)+board_xpos;
    rect.y=((selector_y*square_size)+board_ypos);
    rect.w=square_size;
    rect.h=square_size;
    SDL_FillRect( surface, &rect, SDL_MapRGB(surface->format, 255, 0, 0) );

    // Redo in software....
    /*   static float fadeamount;

       glEnable(GL_BLEND);
       glBlendFunc( GL_SRC_ALPHA, GL_ONE );

       if ( fade >= 1.0f )
           fadeamount=-0.05f;
       if ( fade <= 0.0f )
           fadeamount=0.05f;

       fade+=fadeamount;

       glColor4f( 1.0f, 0.5f, 0.5f, fade );

       glBegin( GL_QUADS );
       glVertex3f( (selector_x*square_size)+board_xpos,
                   (selector_y*square_size)+board_ypos, 0.9f );
       glVertex3f(  ((selector_x*square_size)+square_size)+board_xpos,
                    (selector_y*square_size)+board_ypos, 0.9f );
       glVertex3f(  ((selector_x*square_size)+square_size)+board_xpos,
                    ((selector_y*square_size)+square_size)+board_ypos, 0.9f );
       glVertex3f( (selector_x*square_size)+board_xpos,
                   ((selector_y*square_size)+square_size)+board_ypos, 0.9f );
       glEnd( );

       glDisable(GL_BLEND);*/
}

static void draw_pieces( board_t *board, float board_x, float board_y, float
                         square_size )
{
    int i,j,k;


    for (i = 7; i >= 0; i--)
        for (j = 0; j < 8; j++)
        {
            if ((k = board->square[i*8+j]) != NONE)
            {
                if ( i*8+j == selected_piece && selected_piece >= 0 )
                    draw_textured_piece( board_x, board_y, k, j, i,
                                         square_size, 0.5f );
                else
                    draw_textured_piece( board_x, board_y, k, j, i, square_size, 1.0f );
            }
        }
}

static void draw_scene( board_t *b )
{
    float square_size = 460/8;

    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
    draw_board( board_xpos, board_ypos, square_size );
    draw_selector( board_xpos, board_ypos, square_size, mouse_x_pos, mouse_y_pos );
    draw_selector( board_xpos, board_ypos, square_size, selected_x, selected_y );
    draw_pieces( b, board_xpos, board_ypos, square_size );

    draw_texture( white_pieces[0], 0, 0 );

    if ( in_burp == TRUE )
        draw_eatgame_dialog();

    if ( (in_dialog == DIALOG_PROMOTE_WHITE) || (in_dialog == DIALOG_PROMOTE_BLACK) )
        draw_promote_dialog();

    if ( in_dialog == DIALOG_SYSTEM )
        draw_system_dialog();
    else if ( in_dialog == DIALOG_SUREQUIT )
        draw_surequit_dialog();

    if ( b->turn == WHITE )
        draw_text_message( &text_messages[GUI_TEXT_WHITE_TURN], 500, 300 );
    else
        draw_text_message( &text_messages[GUI_TEXT_BLACK_TURN], 500, 300 );

    //draw_text_message( &text_messages[MENU_TEXT_START_GAME], 200, 320 );
    // Redo in software....
    /*
        draw_board( board_xpos, board_ypos, square_size );
        draw_selector( board_xpos, board_ypos, square_size, mouse_x_pos, mouse_y_pos );
        draw_selector( board_xpos, board_ypos, square_size, selected_x, selected_y );
        draw_pieces( b, board_xpos, board_ypos, square_size );
        draw_captured_pieces( 480, 70 );
     
        if ( b->turn == WHITE )
            draw_text_message( text_messages, 500, 300, GUI_TEXT_WHITE_TURN );
        else
            draw_text_message( text_messages, 500, 300, GUI_TEXT_BLACK_TURN );
     
        if ( in_burp == TRUE )
            draw_eatgame_dialog();
     
        if ( (in_dialog == DIALOG_PROMOTE_WHITE) || (in_dialog == DIALOG_PROMOTE_BLACK) )
            draw_promote_dialog();
     
        if ( in_dialog == DIALOG_SYSTEM )
            draw_system_dialog();
        else if ( in_dialog == DIALOG_SUREQUIT )
            draw_surequit_dialog();*/

    SDL_Delay( 1000/30 );
    SDL_Flip( surface );
}

static void draw_texture( SDL_Surface *texture, float xpos,
                          float ypos )
{
    SDL_Rect rect;
    rect.x=xpos;
    rect.y=ypos;
    SDL_BlitSurface(texture, NULL, surface, &rect);

}

static void draw_text_message( text_message *message, float xpos, float ypos )
{
    SDL_Rect rect;
    rect.x=xpos;
    rect.y=ypos;
    SDL_BlitSurface(&message->message, NULL, surface, &rect);
}

static void draw_textured_piece( float xpos, float ypos, int piece, float board_x, float board_y, float square_size, float alpha )
{
    // Redo in software....
    SDL_Surface **list;
    int index=0;

    if (IS_WHITE(piece))
        list=white_pieces;
    else
        list=black_pieces;

    switch( piece )
    {
    case WHITE_PAWN:
    case BLACK_PAWN:
        index = GUI_PIECE_PAWN;
        break;
    case WHITE_ROOK:
    case BLACK_ROOK:
        index = GUI_PIECE_ROOK;
        break;
    case WHITE_KNIGHT:
    case BLACK_KNIGHT:
        index = GUI_PIECE_KNIGHT;
        break;
    case WHITE_BISHOP:
    case BLACK_BISHOP:
        index = GUI_PIECE_BISHOP;
        break;
    case WHITE_QUEEN:
    case BLACK_QUEEN:
        index = GUI_PIECE_QUEEN;
        break;
    case WHITE_KING:
    case BLACK_KING:
        index = GUI_PIECE_KING;
        break;
    }

    draw_texture( list[index], (board_x*square_size)+xpos,  (board_y*square_size)+ypos );

    /*    int *test;
        unsigned int texture=0;
     
        glEnable( GL_TEXTURE_2D );
        glEnable(GL_BLEND);
        glColor4f( 1.0f, 1.0f, 1.0f, alpha );
     
        if (IS_WHITE(piece))
            test=white_pieces;
        else
            test=black_pieces;
     
        switch( piece )
        {
        case WHITE_PAWN:
        case BLACK_PAWN:
            texture = GUI_PIECE_PAWN;
            break;
        case WHITE_ROOK:
        case BLACK_ROOK:
            texture = GUI_PIECE_ROOK;
            break;
        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            texture = GUI_PIECE_KNIGHT;
            break;
        case WHITE_BISHOP:
        case BLACK_BISHOP:
            texture = GUI_PIECE_BISHOP;
            break;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            texture = GUI_PIECE_QUEEN;
            break;
        case WHITE_KING:
        case BLACK_KING:
            texture = GUI_PIECE_KING;
            break;
        }
     
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glBindTexture(GL_TEXTURE_2D, test[texture]);
     
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f( (board_x*square_size)+xpos,  (board_y*square_size)+ypos, 1.0f );
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( (board_x*square_size+square_size)+xpos,  (board_y*square_size)+ypos, 1.0f );
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f( (board_x*square_size+square_size)+xpos,  (board_y*square_size+(square_size))+ypos, 1.0f );
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f( (board_x*square_size)+xpos,  (board_y*square_size+(square_size))+ypos, 1.0f );
        glEnd();
     
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);*/
}

static int sdlgl_init()
{
    init_gui();
    return 0;
}

static int sdlgl_exit()
{
    SDL_WaitThread( thread, NULL );
    return 0;
}

static int poll_move(history_t *history, void **data)
{
    static int source = -1, dest = -1, prom, needprom = 0;
    board_t *board = history->play->board;
    move_t *move;
    int input;

    input = GetMove(history);

    switch (input)
    {
    case INPUT_VIEW_NEXT:
        return EVENT_VIEW_NEXT;
    case INPUT_VIEW_PREV:
        return EVENT_VIEW_PREV;
    case INPUT_UNDO:
        return EVENT_UNDO;
    }

    if (history->play != history->view)
    {
        source = -1;
        dest = -1;
        needprom = 0;
    }

    if (source == -1)
    {
        source = input;
        /* Only allow piece of current player to be moved. */
        if ((source > 0) && ((PIECE(board->square[source]) == NONE) || (COLOUR(board->square[source]) != board->turn)))
            source = -1;
        return EVENT_NONE;
    }

    if (dest == -1)
    {
        dest = input;
        /* Destination square must not contain piece of current player. */
        if ((dest > 0) && (PIECE(board->square[dest]) != NONE) && (COLOUR(board->square[dest]) == board->turn))
        {
            dest = -1;
            source = -1;
        }
        return EVENT_NONE;
    }

    if (needprom == 1)
    {
        prom = input;
        if (prom > 0)
        {
            needprom = 2;
            in_dialog = DIALOG_NONE;
        }
        return EVENT_NONE;
    }

    if ((needprom  == 0) && (((board->square[source] == WHITE_PAWN) && (dest >= 56)) ||
                             ((board->square[source] == BLACK_PAWN) && (dest <= 7))))
    {
        dialog_selection = 0;
        in_dialog = DIALOG_PROMOTE_WHITE + (IS_BLACK(board->square[source])? 1 : 0);
        needprom = 1;
        return EVENT_NONE;
    }

    move = (move_t *) malloc(sizeof(move_t));
    move->source = source;
    move->destination = dest;
    move->promotion_piece = prom * 2 + 2 + board->turn;
    needprom = 0;
    source = -1;
    dest = -1;
    *data = move;
    return EVENT_MOVE;
}


static int GetMove(history_t *history)
{
    int retval = INPUT_NONE;
    int done = 0;
    SDL_Event event;

    while ( SDL_PollEvent( &event ) )
    {
        /* in the quit dialog */
        if ( in_dialog == DIALOG_SUREQUIT )
        {
            switch ( event.type )
            {
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym )
                {
                case SDLK_ESCAPE:
                    in_dialog=prev_dialog;
                    dialog_selection = 0;
                    break;
                case SDLK_UP:
                    if ( dialog_selection > 0 )
                        dialog_selection--;
                    break;
                case SDLK_DOWN:
                    if ( dialog_selection < 1 )
                        dialog_selection++;
                    break;
                case SDLK_RETURN:
                    if ( dialog_selection == 0 )
                    {
                        done=1;
                        in_dialog=DIALOG_NONE;
                        kill_gui();
                        exit(0);
                    }
                    else if ( dialog_selection == 1 )
                    {
                        dialog_selection = 0;
                        in_dialog=prev_dialog;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        }
        /* In the system menu */
        else if ( in_dialog == DIALOG_SYSTEM )
        {
            switch ( event.type )
            {
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym )
                {
                case SDLK_ESCAPE:
                    in_dialog=DIALOG_NONE;
                    break;
                case SDLK_UP:
                    if ( dialog_selection > 0 )
                        dialog_selection--;
                    break;
                case SDLK_DOWN:
                    if ( dialog_selection < 3 )
                        dialog_selection++;
                    break;
                case SDLK_RETURN:
                    if ( dialog_selection == 0 )
                    {
                        /* Save game.. */
                    }
                    else if ( dialog_selection == 1 )
                    {
                        /* load game.. */
                    }
                    else if ( dialog_selection == 2 )
                    {
                        in_burp = TRUE;
                    }
                    else if ( dialog_selection == 3 )
                    {
                        prev_dialog = DIALOG_SYSTEM;
                        dialog_selection = 0;
                        in_dialog = DIALOG_SUREQUIT;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        }
        /* In the promote dialog */
        else if ( (in_dialog == DIALOG_PROMOTE_WHITE) || (in_dialog == DIALOG_PROMOTE_BLACK) )
        {
            switch ( event.type )
            {
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym )
                {
                case SDLK_ESCAPE:
                    in_dialog=prev_dialog;
                    break;
                case SDLK_LEFT:
                    if ( dialog_selection > 0 )
                        dialog_selection--;
                    break;
                case SDLK_RIGHT:
                    if ( dialog_selection < 3 )
                        dialog_selection++;
                    break;
                case SDLK_RETURN:
                    retval = dialog_selection;
                    break;
                default:
                    break;
                }
                break;
            }
        }
        else
            switch ( event.type )
            {
            case SDL_MOUSEMOTION:
                mouse_x_pos=((event.motion.x-10))/(460/8);
                mouse_y_pos=(470-(event.motion.y))/(460/8);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if ( event.button.button == SDL_BUTTON_LEFT )
                {
                    retval = (mouse_y_pos*8)+mouse_x_pos;
                    selected_piece=retval;
                    done=1;
                }
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym )
                {
                case SDLK_ESCAPE:
                    prev_dialog = DIALOG_NONE;
                    in_dialog = DIALOG_SYSTEM;
                    break;
                case SDLK_r:
                    dialog_selection=0;
                    in_dialog = DIALOG_PROMOTE_WHITE;
                    break;
                case SDLK_d:
                    board_xpos+=5;
                    break;
                case SDLK_a:
                    board_xpos-=5;
                    break;
                case SDLK_w:
                    board_ypos+=5;
                    break;
                case SDLK_s:
                    board_ypos-=5;
                    break;
                case SDLK_p:
                    retval = INPUT_VIEW_PREV;
                    break;
                case SDLK_n:
                    retval = INPUT_VIEW_NEXT;
                    break;
                case SDLK_u:
                    retval = INPUT_UNDO;
                    break;
                case SDLK_LEFT:
                    if ( selected_x > 0 )
                        selected_x--;
                    break;
                case SDLK_RIGHT:
                    if ( selected_x < 7 )
                        selected_x++;
                    break;
                case SDLK_UP:
                    if ( selected_y < 7 )
                        selected_y++;
                    break;
                case SDLK_DOWN:
                    if ( selected_y > 0 )
                        selected_y--;
                    break;
                case SDLK_RETURN:
                    retval = (selected_y*8)+selected_x;
                    selected_piece=retval;
                    done=1;
                    break;
                default:
                    break;
                }
                break;
            }
    }
    return retval;
}

ui_driver_t ui_sdl =
    {
        "sdl",
        sdlgl_init,
        sdlgl_exit,
        do_menu,
        draw_scene,
        poll_move
    };

#endif /* WITH_UI_SDL */
