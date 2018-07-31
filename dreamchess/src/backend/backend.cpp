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

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_thread.h"
#include <SDL2/SDL_syswm.h>

#include <GL/glew.h>

//#define NO_SDL_GLEXT
//#include "SDL_opengl.h"
//#include "SDL_joystick.h"

#include "debug.h"

#include "backend.h"

static void music_callback(char *title, char *artist, char *album) { DBG_LOG("now playing: %s - %s", artist, title); }

void Backend::init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0) {
		DBG_ERROR("SDL initialization failed: %s", SDL_GetError());
		exit(1);
	}

	mode_set_failed = 0;

	ch_datadir();	

	// hmmmm?
	max_width = 1920; max_height = 1080;
}

int Backend::ch_dir(std::string name) {
	return chdir(name.c_str());
}

int Backend::set_fullscreen(int fullscreen) {
	if (SDL_SetWindowFullscreen(sdl_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) > 0) {
		DBG_ERROR("failed to set fullscreen to %s: %s", fullscreen ? "on" : "off", SDL_GetError());
		return 1;
	}

	return 0;
}

int Backend::create_window(int width, int height, int fullscreen, int ms) {
	g_Backend->create_window(width, height, fullscreen, ms);

	int err;
	
	screen_width = width;
	screen_height = height;
	screen_fs = fullscreen;
	screen_ms = ms;

	int video_flags = SDL_WINDOW_OPENGL;

	DBG_LOG("setting video mode to %ix%i; fullscreen %s; %ix multisampling", width, height, fullscreen ? "on" : "off",
			ms);

	if (fullscreen)
		video_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

	sdl_window =
		SDL_CreateWindow("DreamChess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, video_flags);

	if (!sdl_window) {
		DBG_ERROR("failed to set video mode: %ix%i; fullscreen %s; %ix multisampling: %s", width, height,
				  fullscreen ? "on" : "off", ms, SDL_GetError());
		mode_set_failed = 1;
		return 1;
	}

	if (!SDL_GL_CreateContext(sdl_window)) {
		DBG_ERROR("failed to create GL context: %s", SDL_GetError());
		SDL_DestroyWindow(sdl_window);
		mode_set_failed = 1;
		return 1;
	}

	err = glewInit();
	if (err != GLEW_OK) {
		DBG_ERROR("failed to initialize GLEW: %s", glewGetErrorString(err));
		exit(1);
	}

	if (!glewIsSupported("GL_ARB_framebuffer_object")) {
		DBG_ERROR("OpenGL extension GL_ARB_framebuffer_object not supported");
		exit(1);
	}

	if (!glewIsSupported("GL_ARB_texture_non_power_of_two")) {
		DBG_ERROR("OpenGL extension GL_ARB_texture_non_power_of_two not supported");
		exit(1);
	}

	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	init_fbo();

	if (ms > max_samples) {
		SDL_DestroyWindow(sdl_window);
		mode_set_failed = 1;
		return 1;
	}

	init_screen_fbo(ms);

	init_gl();
	// moved! load_menu_tex();

	SDL_ShowCursor(SDL_DISABLE);

	// SDL_WM_SetCaption( "DreamChess", NULL );

	/*
		MOVED!
	gg_system_init(get_gg_driver_sdlgl());
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
		get_menu_style()->border.image[i] = &get_menu_border()[i];*/

	ch_datadir();
	init_audio();

	/* Make virtual keyboard table? */
	// FIXMEEEEEE !!!! populate_key_table();

	//update_fps_time();
	fps_time = SDL_GetTicks();

	/* Register music callback */
	audio_set_music_callback(music_callback);

	return 0;
}

void Backend::init_screen_fbo_ms(int ms) {
	glBindRenderbuffer(GL_RENDERBUFFER, screen_color_rb);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, ms, GL_RGBA8, max_width, max_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, screen_color_rb);

	glBindRenderbuffer(GL_RENDERBUFFER, screen_depth_stencil_rb);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, ms, GL_DEPTH24_STENCIL8, max_width, max_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screen_depth_stencil_rb);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		DBG_ERROR("failed to set up screen FBO with %dx multisampling", ms);
		exit(1);
	}
}

void Backend::init_screen_fbo(int ms) {
	glGenFramebuffers(1, &screen_fb); // Our default-bound framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, screen_fb);

	glGenRenderbuffers(1, &screen_color_rb);
	glGenRenderbuffers(1, &screen_depth_stencil_rb);

	init_screen_fbo_ms(ms);

	glGenFramebuffers(1, &screen_temp_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_temp_fb);

	glGenRenderbuffers(1, &screen_temp_color_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, screen_temp_color_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, max_width, max_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, screen_temp_color_rb);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		DBG_ERROR("failed to set up temp FBO");
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, screen_fb);
}

void Backend::init_fbo(void) {
	const int width = 1920;
	const int height = 1080;

	glGenTextures(1, &colourpicking_tex);
	glBindTexture(GL_TEXTURE_2D, colourpicking_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourpicking_tex, 0);

	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		DBG_ERROR("failed to set up FBO");
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Backend::deinit_fbo(void) {
	glDeleteTextures(1, &colourpicking_tex);
	glDeleteRenderbuffers(1, &depth_rb);
	glDeleteFramebuffers(1, &fb);
}

void Backend::blit_fbo() {
	int width, height;
	SDL_GetWindowSize(sdl_window, &width, &height);

	/* 4:3 letterboxing for fullscreen */
	int start_x = 0, start_y = 0;
	int new_width = width;
	int new_height = height;

	if (screen_fs) {
		if (new_height * 4 < new_width * 3) {
			new_width = new_height * 4 / 3;
			start_x = (width - new_width) >> 1;
		} else if (new_width * 3 < new_height * 4) {
			new_height = new_width * 3 / 4;
			start_y = (height - new_height) >> 1;
		}
	}

	// Multisampled buffer requires a same-dimension blit to resolve
	if (screen_ms && (new_width != get_screen_width() || new_height != get_screen_height())) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen_temp_fb);
		glBlitFramebuffer(0, 0, get_screen_width(), get_screen_height(), 0, 0, get_screen_width(), get_screen_height(),
						  GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, screen_temp_fb);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, get_screen_width(), get_screen_height(), start_x, start_y, new_width + start_x,
					  new_height + start_y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, screen_fb);
}

void Backend::go_3d(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 640.0f / 480.0f, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int Backend::resize(int width, int height, int fullscreen, int ms) {
	DBG_LOG("resizing video mode to %ix%i; fullscreen %s; %ix multisampling", width, height, fullscreen ? "on" : "off",
			ms);

	if (ms > max_samples)
		return 1;

	if (fullscreen != screen_fs && set_fullscreen(fullscreen))
		return 1;

	if (screen_width != width || screen_height != height)
		SDL_SetWindowSize(sdl_window, width, height);

	if (ms != screen_ms)
		init_screen_fbo_ms(ms);

	screen_width = width;
	screen_height = height;
	screen_fs = fullscreen;
	screen_ms = ms;

	resize_window(screen_width, screen_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_temp_fb);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return 0;
}

/** @brief Sets the OpenGL rendering options. */
void Backend::init_gl(void) {
	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);

	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Depth buffer setup */
	glClearDepth(1.0f);

	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);

	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);

	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}