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

/* SDL2 system.. */

#include <SDL.h>
#include <GL/glew.h>
#include "System.h"

Shader::Shader(System *s, std::string vert, std::string frag) {
    _system = s;
    _vertexShaderID = load(vert, SHADER_VERTEX);
    _fragmentShaderID = load(frag, SHADER_FRAGMENT);
   
    _programID = glCreateProgram();
    glAttachShader(_programID, _vertexShaderID);
    glAttachShader(_programID, _fragmentShaderID);

    // Set up the attribute locations..
    glBindAttribLocation(_programID, VERTLOCATION, "aPosition");
    glBindAttribLocation(_programID, NORMLOCATION, "aNormal");
    glBindAttribLocation(_programID, TEXLOCATION, "aTexCoord");
    glBindAttribLocation(_programID, COLLOCATION, "aColour");
    glBindAttribLocation(_programID, COLMODLOCATION, "aColourMod");
    
    glLinkProgram(_programID);

    //_projectionMatrixLocation = glGetUniformLocation(_program, "uProjectionMatrix");    
}

Shader::~Shader() {
    glDeleteShader(_programID);
}

void Shader::activate() {
    glUseProgram(_programID);
}

int Shader::load(std::string filename, int type) {
    unsigned int shaderID = 0;

    _system->chDataDir();

    std::ifstream fileStream(filename);
    if (!fileStream.is_open()) {
        printf("Error opening shader file. %s\n", filename.c_str());
        exit(1);        
    }

    std::string str{std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()};
    const char *shaderText = str.c_str();

    if (type == SHADER_VERTEX) 
        shaderID = glCreateShader(GL_VERTEX_SHADER);
    else 
        shaderID = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shaderID, 1, &shaderText, nullptr);
    glCompileShader(shaderID);

    int stat;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &stat);
    if (stat == GL_FALSE) {
        printf("Shader %s failed to compile.\n", filename.c_str());

        int length;
        char log[255];
        glGetShaderInfoLog(shaderID, 255, &length, (char *)log);
        printf("LOG: %s\n", log);        
    }

    return shaderID;
}

void System::finishFrame() {
    SDL_GL_SwapWindow(_window);
}

int System::pollEvents() {
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event)) {

        if (sdl_event.type == SDL_QUIT)
            /* FIXME */
            exit(0);

        if ( (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.mod & KMOD_ALT &&
            sdl_event.key.keysym.sym == SDLK_RETURN) || (sdl_event.type == SDL_KEYDOWN &&
            sdl_event.key.keysym.sym == SDLK_F11) )
        {
            //DBG_LOG( "toggled fullscreen" );
            //g_Dreamchess->toggleFullscreen();
            continue;
        }

        if ( (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.mod & KMOD_CTRL &&
            sdl_event.key.keysym.sym == SDLK_f) )
        {
            //DBG_LOG( "toggled fps counter" );
            //toggle_show_fps();
            continue;
        }

        //gg_event = convert_event(&sdl_event);

        //*event = gg_event;
        return 1;
    }

    return 0;
}

void System::go2D() {
    glDisable(GL_DEPTH_TEST);
    glViewport( 0, 0, _width, _height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0, _width, 0, _height, -1, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


void System::go3D() {
    glEnable(GL_DEPTH_TEST); 
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(90.0f, (float)_width/(float)_height, 1.0f, 1000.0f);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void System::initGL() {
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


int System::initVideo() {
    int width = 1280, height = 720, fullscreen = false, ms = 0;

    int i, err;

    _width = width;
    _height = height;
    _fullscreen = fullscreen;
    _ms = ms;

    int video_flags = SDL_WINDOW_OPENGL;

    //DBG_LOG("setting video mode to %ix%i; fullscreen %s; %ix multisampling",
    //        width, height, fullscreen ? "on" : "off", ms);

    if (_fullscreen)
        video_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

    _window = SDL_CreateWindow("DreamChess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        _width, _height, video_flags);

    if (!_window) {
        //DBG_ERROR("failed to set video mode: %ix%i; fullscreen %s; %ix multisampling: %s",
        //            _width, _height, _fullscreen ? "on" : "off", _ms, SDL_GetError());
        //mode_set_failed = 1;
        return 1;
    }

    if (!SDL_GL_CreateContext(_window)) {
        //DBG_ERROR("failed to create GL context: %s", SDL_GetError());
        SDL_DestroyWindow(_window);
        //mode_set_failed = 1;
        return 1;
    }

    err = glewInit();
    if (err != GLEW_OK) {
        printf("Failed to init GLEW\n");
        //DBG_ERROR("failed to initialize GLEW: %s", glewGetErrorString(err));
        exit(1);
    }

    if (!glewIsSupported("GL_ARB_framebuffer_object")) {
        //DBG_ERROR("OpenGL extension GL_ARB_framebuffer_object not supported");
        exit(1);
    }

    if (!glewIsSupported("GL_ARB_texture_non_power_of_two")) {
        //DBG_ERROR("OpenGL extension GL_ARB_texture_non_power_of_two not supported");
        exit(1);
    }

    _currentShader = new Shader(this, "shader/vertex.glsl", "shader/fragment.glsl");
    //_currentShader->activate();

    /*glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
    //init_fbo();

    if (_ms > max_samples) {
        SDL_DestroyWindow(_window);
        mode_set_failed = 1;
        return 1;
    }*/

    //init_screen_fbo(ms);

    initGL();

    //SDL_ShowCursor(SDL_DISABLE);

    //SDL_WM_SetCaption( "DreamChess", NULL );

    /*gg_system_init(get_gg_driver_sdlgl());
    get_ingame_style()->fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.5f);
    get_ingame_style()->hor_pad = 20;
    get_ingame_style()->vert_pad = 10;

    for (i = 0; i < 9; i++)
        get_ingame_style()->border.image[i] = &get_border()[i];

    get_menu_style()->textured = 1;
    get_menu_style()->fade_col = gg_colour(0.0f, 0.0f, 0.0f, 0.0f);
    get_menu_style()->hor_pad = 20;
    get_menu_style()->vert_pad = 10;

    for (i = 0; i < 9; i++)
        get_menu_style()->border.image[i] = &get_menu_border()[i];

    ch_datadir();
    audio_init();*/

    /* Make virtual keyboard table? */
    //populate_key_table();

    //update_fps_time();

    /* Register music callback */
    //audio_set_music_callback(music_callback);

    return 0;    
}

#ifdef _WIN32 /* Windows specific stuff... need to add the rest back later..*/

#define USERDIR "DreamChess"

#include <windows.h>
#include <io.h>
#include <direct.h>
#include "shlwapi.h"
#include "shlobj.h"

int System::chDataDir(void)
{
    char filename[MAX_PATH + 6];

    GetModuleFileName(NULL, filename, MAX_PATH);
    filename[MAX_PATH] = '\0';
    PathRemoveFileSpec(filename);
    strcat(filename, "/data");
    return chdir(filename);
}

int System::chUserDir(void)
{
    char appdir[MAX_PATH];

    if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdir))
        return -1;

    if (chdir(appdir))
        return -1;

    if (chdir(USERDIR))
    {
        if (mkdir(USERDIR))
            return -1;

        return chdir(USERDIR);
    }

    return 0;
}

#endif