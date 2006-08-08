
#include "ui_sdlgl.h"

static void poll_move();
static gg_dialog_style_t style_ingame, style_menu;
static int turn_counter_start=0;
static texture_t menu_title_tex;
static int white_in_check;
static int black_in_check;
static board_t board;
static int can_load=FALSE;
static int pgn_slot;
static SDL_Surface *surface;
static int quit_to_menu=FALSE;
static int title_process_retval;
static int set_loading=FALSE;
static int dialog_promote_piece;
static int wait_menu = 1;
static SDL_Joystick *joy;
static float board_xpos, board_ypos;
static int game_difficulty;
static int game_type;
static char* themelist[25];
static char* stylelist[25];
static int num_style;
static int pieces_list_total;
static char** pieces_list;
static char** board_list;
static int board_list_total;

int get_pieces_list_total()
{
    return pieces_list_total;
}

int get_board_list_total()
{
    return board_list_total;
}

int get_num_style()
{
    return num_style;
}

char *get_board_list( int index )
{
    return board_list[index];
}

char *get_pieces_list( int index )
{
    return pieces_list[index];
}

char *get_stylelist( int index )
{
    return stylelist[index];
}

char *get_themelist( int index )
{
    return themelist[index];
}

int get_dialog_promote_piece()
{
    return dialog_promote_piece;
}

void set_dialog_promote_piece( int piece )
{
    dialog_promote_piece=piece;
}

void set_title_process_retval( int ret )
{
    title_process_retval=ret;
}

void set_set_loading( int set )
{
    set_loading=set;
}

void set_quit_to_menu( int menu )
{
    quit_to_menu=menu;
}

void set_pgn_slot( int slot )
{
    pgn_slot=slot;
}

board_t *get_board()
{
    return &board;
}

config_t config;

config_t *get_config()
{
    return &config;
}

static int switch_to_menu=FALSE;
static int fading_out=FALSE;

void set_switch_to_menu(int sw)
{
    switch_to_menu=sw;
}

int get_fading_out()
{
    return fading_out;
}

void set_fading_out(int fade)
{
    fading_out=fade;
}

int get_white_in_check()
{
    return white_in_check;
}

int get_black_in_check()
{
    return black_in_check;
}

gg_dialog_style_t *get_ingame_style()
{
    return &style_ingame;
}

gg_dialog_style_t *get_menu_style()
{
    return &style_menu;
}

int get_turn_counter()
{
    return turn_counter_start;
}

void reset_turn_counter()
{
    turn_counter_start=SDL_GetTicks();
}

/** Implements ui_driver::menu */
static config_t *do_menu(int *pgn)
{
    gg_dialog_t *keyboard = dialog_vkeyboard_create();
    SDL_Event event;
    int mouse_x=0, mouse_y=0;
    int switch_to_game=FALSE;
    game_difficulty=1;
    game_type=GAME_TYPE_HUMAN_VS_CPU;
    title_process_retval=2;

    board_xpos=128;
    board_ypos=30;
    can_load=FALSE;
    set_loading=FALSE;

    white_in_check=FALSE;
    black_in_check=FALSE;

    draw_credits(1);
    gg_dialog_open(dialog_title_root_create());
    /* If created, and theme set to custom.. open custom.. */
    /* if ( selected_theme == theme_count )
         gg_dialog_open(dialog_title_custom_create());
     else
         gg_dialog_open(dialog_title_create());*/

    resize_window(SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    set_fade_start(gg_system_get_ticks());

    while ( 1 )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gg_dialog_cleanup();

        /* Precess input */
        while ( SDL_PollEvent( &event ) )
        {
            gg_event_t gg_event;

            if (event.type == SDL_QUIT)
                /* FIXME */
                exit(0);

            if (event.type == SDL_MOUSEMOTION)
            {
                gg_dialog_t *dialog = gg_dialog_current();
                if (dialog)
                    gg_dialog_mouse_movement(dialog, event.motion.x, 479 - event.motion.y);

                continue;
            }

            gg_event = convert_event(&event);

            if (wait_menu)
            {
                if (gg_event.type != GG_EVENT_NONE)
                    wait_menu = 0;
                continue;
            }

            if (gg_event.type == GG_EVENT_KEY && gg_event.key == 0x06)
            {
                toggle_show_fps();
                continue;
            }

            if (gg_event.type == GG_EVENT_KEY && gg_event.key == 0x0b)
            {
                toggle_vkeyboard_enabled();
                continue;
            }

            if (get_vkeyboard_enabled())
                keyboard->input(GG_WIDGET(keyboard), gg_event);
            else
                gg_dialog_input_current(gg_event);

            if (title_process_retval == 1)
                return NULL;
        }

        /* Draw the menu.. */
        draw_texture( &menu_title_tex, 0, 0, 640, 480, 1.0f, get_col(COL_WHITE) );

        if ( switch_to_game == TRUE )
        {
            set_fading_out(FALSE);
            set_fade_start(gg_system_get_ticks());
            return &config;
        }
        else if ( fading_out == TRUE )
        {
            /* Draw fade... */
            if ( !draw_fade( FADE_OUT ) )
                switch_to_game=TRUE;
        }
        else if ( can_load == TRUE )
        {
            /* We using custom? */
            if ( get_selected_theme()==get_theme_count() )
            {
                sprintf(get_white_name(),"White"); 
                sprintf(get_black_name(),"Black");
                load_theme(get_stylelist(get_cur_style()),
                    get_pieces_list(get_pieces_list_cur()),
                    get_board_list(get_board_list_cur()));
            }
            else
            {
                /* printf( "Loading theme %i\n", selected_theme ); */
                load_theme(get_theme(get_selected_theme())->style, get_theme(get_selected_theme())->pieces,
                           get_theme(get_selected_theme())->board);
                sprintf(get_white_name(),"%s",get_theme(get_selected_theme())->white_name); 
                sprintf(get_black_name(),"%s",get_theme(get_selected_theme())->black_name);
            }

            reset_3d();
            *pgn = pgn_slot;
            if (pgn_slot >= 0)
                config = *get_config_save(pgn_slot);

            set_fade_start(gg_system_get_ticks());
            set_fading_out(TRUE);
        }

        if ( set_loading == FALSE )
        {
            char msg[] = "Press any key or button to start";
            if (wait_menu)
                text_draw_string_bouncy( SCREEN_WIDTH / 2 -
                                         text_width(msg) * 0.75, 30, msg,
                                         1.5f, get_col(COL_WHITE));
            else
                gg_dialog_render_all();

            if (get_vkeyboard_enabled())
                gg_dialog_render(keyboard);

            draw_credits(0);
        }
        else
        {
            text_draw_string( 390, 30, "Loading...", 3, get_col(COL_WHITE));
            can_load = TRUE;
        }

        /* Draw fade... */
        if ( !fading_out )
            draw_fade( FADE_IN );

        /* Draw mouse cursor.. */
        #ifndef _arch_dreamcast
        SDL_GetMouseState(&mouse_x, &mouse_y);
        draw_texture( get_mouse_cursor(), mouse_x, (479-mouse_y-32), 32, 32, 1.0f, 
            get_col(COL_WHITE) );
        #endif /* _arch_dreamcast */

        gl_swap();
    }
}

/** Implements ui_driver::init. */
static void init_gui()
{
    int video_flags;
    SDL_Surface *icon;
    const SDL_VideoInfo *video_info;
    int i;
    DIR* styledir;
    struct dirent* styledir_entry;
    char temp[80];

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_EnableUNICODE(1);

    ch_datadir();
    icon = IMG_Load("icon.png");

    if (!icon)
    {
        fprintf(stderr, "Could not load icon: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WM_SetIcon(icon, NULL);
    SDL_FreeSurface(icon);

    video_info = SDL_GetVideoInfo( );

    if ( !video_info )
    {
        fprintf( stderr, "Video query failed: %s\n",
                 SDL_GetError( ) );
        exit(1);
    }

    video_flags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    video_flags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    video_flags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    /* video_flags |= SDL_RESIZABLE; */      /* Enable window resizing */
    /* video_flags |= SDL_FULLSCREEN; */

    if ( video_info->hw_available )
        video_flags |= SDL_HWSURFACE;
    else
        video_flags |= SDL_SWSURFACE;

    if ( video_info->blit_hw )
        video_flags |= SDL_HWACCEL;

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
                                video_flags );
    if ( !surface )
    {
        fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
        exit(1);
    }

    SDL_ShowCursor(SDL_DISABLE);

    if( SDL_NumJoysticks()>0 )
        joy=SDL_JoystickOpen(0);

    SDL_WM_SetCaption( "DreamChess", NULL );
    init_gl();

    gg_system_init(get_gg_driver_sdlgl());

    /* New text stuff. */
    generate_text_chars();

    /* For the menu.. */
    load_texture_png( &menu_title_tex, "menu_title.png" , 0);

    /* Load themes xml */

    ch_datadir();
    if ( (styledir=opendir("themes")) != NULL )
    {
        set_theme_count(0);
        styledir_entry=readdir(styledir);
        while ( styledir_entry != NULL )
        {
            if ( styledir_entry->d_name[0] != '.' )
            {
                sprintf( temp, "themes/%s", styledir_entry->d_name );
                load_theme_xml( temp );
            }
            styledir_entry=readdir(styledir);
        }
    }

    /* Fill theme list. */
    if ( get_theme_count() > 0 )
    {
        for ( i=0; i<get_theme_count(); i++ )
            themelist[i]=strdup( get_theme(i)->name );
    }
    themelist[get_theme_count()]=strdup( "Custom" );

    /* Fill style list. */
    if ( (styledir=opendir("styles")) != NULL )
    {
        num_style = 0;
        styledir_entry=readdir(styledir);
        while ( styledir_entry != NULL )
        {
            if ( styledir_entry->d_name[0] != '.' )
                stylelist[num_style++]=strdup( styledir_entry->d_name );
            styledir_entry=readdir(styledir);
        }
    }

    chdir("styles");
    chdir("default");
    load_border(get_menu_border(), "border.png");
    #ifndef _arch_dreamcast
    load_texture_png( get_mouse_cursor(), "mouse_cursor.png", 1 );
    #endif /* _arch_dreamcast */

    /* Fill pieces list. */
    ch_datadir();

    if ((styledir=opendir("pieces")) != NULL )
    {
        pieces_list_total = 0;
        while ((styledir_entry = readdir(styledir)) != NULL)
        {
            if (styledir_entry->d_name[0] != '.')
            {
                pieces_list = realloc(pieces_list, (pieces_list_total + 1) *
                                      sizeof(char *));
                pieces_list[pieces_list_total++] =
                    strdup(styledir_entry->d_name);
            }
        }
    }

    style_ingame.textured = 1;
    style_ingame.fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.5f);
    style_ingame.hor_pad = 20;
    style_ingame.vert_pad = 10;

    for (i = 0; i < 9; i++)
        style_ingame.border.textured.image[i] = &get_border()[i];

    style_menu.textured = 1;
    style_menu.fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.0f);
    style_menu.hor_pad = 20;
    style_menu.vert_pad = 10;

    for (i = 0; i < 9; i++)
        style_menu.border.textured.image[i] = &get_menu_border()[i];

    /* Fill board list. */
    ch_datadir();

    /* Make virtual keyboard table? */
    populate_key_table();

    if ((styledir=opendir("boards")) != NULL )
    {
        board_list_total = 0;
        while ((styledir_entry = readdir(styledir)) != NULL)
        {
            if (styledir_entry->d_name[0] != '.')
            {
                board_list = realloc(board_list, (board_list_total + 1) *
                                     sizeof(char *));
                board_list[board_list_total++] =
                    strdup(styledir_entry->d_name);
            }
        }
    }
    update_fps_time();
}

/** Implements ui_driver::update. */
static void update(board_t *b, move_t *move)
{
    board = *b;

    if ( move != NULL )
        start_piece_move( move->source, move->destination );

    if ( board.state == BOARD_CHECK )
    {
        if (IS_WHITE(board.turn))
            white_in_check=TRUE;
        else
            black_in_check=TRUE;
    }
    else
    {
        black_in_check=FALSE;
        white_in_check=FALSE;
    }

    /* FIXME */
    while ( get_piece_moving_done() == 0 )
        poll_move();
}

/** Implements ui_driver::show_result. */
static void show_result(result_t *res)
{
    gg_dialog_open(dialog_victory_create(res));
}

/** Implements ui_driver::init. */
static int sdlgl_init()
{
    init_gui();
    return 0;
}

/** Implements ui_driver::exit. */
static int sdlgl_exit()
{
    glDeleteTextures(1, &menu_title_tex.id);
    SDL_Quit();
    return 0;
}

/** Implements ui_driver::show_message. */
static void show_message (char *msg)
{
    gg_dialog_open(dialog_message_create(msg));
}

/** Implements ui_driver::poll. */
static void poll_move()
{
    static int source = -1, dest = -1, needprom = 0;
    /* board_t *board = history->play->board; */
    move_t *move;
    int input;

    draw_scene(&board);

    if (switch_to_menu == TRUE)
    {
        quit_to_menu = 0;
        switch_to_menu=FALSE;
        fading_out=FALSE;
        needprom = 0;
        source = -1;
        dest = -1;
        unload_theme();
        game_quit();
        return;
    }
    else if (quit_to_menu)
    {
        set_fading_out(TRUE);
        set_fade_start(gg_system_get_ticks());
        quit_to_menu=FALSE;
    }

    input = get_move();

    if (!game_want_move())
    {
        source = -1;
        dest = -1;
        needprom = 0;
    }

    if (source == -1)
    {
        source = input;
        /* Only allow piece of current player to be moved. */
        if ((source >= 0) && ((PIECE(board.square[source]) == NONE) || (COLOUR(board.square[source]) != board.turn)))
            source = -1;
        return;
    }

    if (dest == -1)
    {
        dest = input;
        if (dest >= 0)
        {
            /* Destination square must not contain piece of current player. */
            if ((PIECE(board.square[dest]) != NONE) && (COLOUR(board.square[dest]) == board.turn))
            {
                dest = -1;
                /* We use currently selected square as source. */
                source = input;
            }
            else
                select_piece(-1);
        }
        return;
    }

    if (needprom == 1)
    {
        if (dialog_promote_piece != NONE)
            needprom = 2;
        return;
    }

    if ((needprom  == 0) && (((board.square[source] == WHITE_PAWN) && (dest >= 56)) ||
                             ((board.square[source] == BLACK_PAWN) && (dest <= 7))))
    {
        gg_dialog_open(dialog_promote_create(COLOUR(board.square[source])));
        needprom = 1;
        return;
    }

    move = (move_t *) malloc(sizeof(move_t));
    move->source = source;
    move->destination = dest;
    if (needprom == 2)
        move->promotion_piece = dialog_promote_piece;
    else
        move->promotion_piece = NONE;
    needprom = 0;

    /* start_piece_move( source, dest ); */

    source = -1;
    dest = -1;
    game_make_move(move, 1);
    reset_turn_counter();
    return;
}

/** SDL + OpenGL driver. */
ui_driver_t ui_sdlgl =
    {
        "sdlgl",
        sdlgl_init,
        sdlgl_exit,
        do_menu,
        update,
        poll_move,
        show_message,
        show_result
    };
