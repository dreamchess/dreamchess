
#include "ui_sdlgl.h"

#ifdef _arch_dreamcast
static float dc_z;

float get_dc_z()
{
    dc_z += 0.00001f;
    return dc_z;
}

void reset_dc_z()
{
    dc_z = 1.0f;
}
#endif

static float zerodepth=1.0f;

static int fps_enabled = 0;
static int frames = 0;
static Uint32 fps_time = 0;
static float fps;

float get_fps()
{
    return fps;
}

void update_fps_time()
{
    fps_time=SDL_GetTicks();
}

void toggle_show_fps()
{
    fps_enabled = 1 - fps_enabled;
}

static struct
{
    int x;
    int y;
}
mouse_pos;

float get_zerodepth()
{
    return zerodepth;
}

void set_mouse_pos( int x, int y )
{
    mouse_pos.x=x;
    mouse_pos.y=y;
}

int get_true_mouse_x()
{
    return mouse_pos.x;
}

int get_true_mouse_y()
{
    return mouse_pos.y;
}

int get_mouse_x()
{
    return ((float)mouse_pos.x/(float)get_screen_width())*640;
}

int get_mouse_y()
{
    return ((float)mouse_pos.y/(float)get_screen_height())*480;
}

/** @brief Computes smallest power of two that's larger than the input value.
 *
 *  @param input Input value.
 *  @return Smallest power of two that's larger than input.
 */
int power_of_two(int input)
{
    int value = 1;

    while ( value < input )
    {
        value <<= 1;
    }
    return value;
}

void set_perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * DC_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void go_3d(int width, int height)
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    set_perspective(45.0f, 640.0f/480.0f, 0.1f, 100.0f);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

/** @brief Sets the OpenGL rendering options. */
void init_gl()
{
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

    #ifndef _arch_dreamcast 
    #ifndef __BEOS__
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glReadPixels(100, 100, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zerodepth);
    if ( zerodepth != 1.0f )
        DBG_WARN( "z depth should be 1.0f, but we got %f", zerodepth );
    #endif /* __BEOS__ */    
    #endif /* _arch_dreamcast */
}

/** @brief Resizes the OpenGL window.
 *
 *  @param width Desired width in pixels.
 *  @param height Desired height in pixels.
 */
void resize_window( int width, int height )
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    /*gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);*/
    glOrtho(0, 640, 0, 480, -1, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

/** @brief Swaps the OpenGL buffer.
 *
 *  Also maintains the frames-per-second counter.
 */
void gl_swap()
{
    static Uint32 last = 0;
    Uint32 now;
    Uint32 wait = 0;

    if (fps_enabled)
    {
        char fps_s[16];

        snprintf(fps_s, 16, "FPS: %.2f", fps);
        text_draw_string(10, 10, fps_s, 1, get_col(COL_RED));
    }

    SDL_GL_SwapBuffers();
    now = SDL_GetTicks();
    if (now - last < (frames + 1) * 1000 / FPS) {
        wait = (frames + 1) * 1000 / FPS - (now - last);
        SDL_Delay(wait);
    }

    frames++;
    if (frames == FPS)
    {
        Uint32 fps_now = SDL_GetTicks();
        last = now + wait;
        fps = 1000 * FPS / (float) (fps_now - fps_time);
        frames = 0;
        fps_time = fps_now;
    }

#ifdef _arch_dreamcast
    reset_dc_z();
#endif
}
