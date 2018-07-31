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

#ifndef DREAMCHESS_BACKEND
#define DREAMCHESS_BACKEND

#include <string>

struct SDL_Window;
typedef void (*audio_music_callback_t)(char *title, char *artist, char *album);

class Backend {
public:
	Backend() { }
	~Backend() { }

	// Video
	void init();
	void init_gl(void);	
	void init_screen_fbo(int ms);
	void init_screen_fbo_ms(int ms);	
	void init_fbo(void);
	void deinit_fbo(void);
	int create_window(int width, int height, int fullscreen, int ms);	
	int resize(int width, int height, int fullscreen, int ms);
	void blit_fbo();
	void gl_swap(void);
	void go_3d(int width, int height);	

	// Audio
	void init_audio();
	void audio_exit(void);
	void audio_play_music(void);
	void audio_poll(int title);
	void audio_set_music_callback(audio_music_callback_t callback);
	void audio_play_sound(int id);
	void audio_set_sound_volume(int vol);
	void audio_set_music_volume(int vol);

	// Getter setters!
	int get_screen_width(void) { return screen_width; }
	int get_screen_height(void)	{ return screen_height; }

	int ch_datadir(void);
	int ch_userdir(void);	

	int ch_dir(std::string name);
	int set_fullscreen(int fullscreen);
private:
	SDL_Window *sdl_window;
	int screen_width;
	int screen_height;
	int screen_fs;
	int screen_ms;	
	unsigned int fb, colourpicking_tex, depth_rb;

	float zerodepth = 1.0f;

	int fps_enabled = 0;
 	int frames = 0;
 	unsigned int fps_time = 0;
 	float fps;

	int max_width, max_height;

	unsigned int screen_fb, screen_temp_fb, screen_tex, screen_color_rb, screen_temp_color_rb, screen_depth_stencil_rb;

	int mode_set_failed;
	int max_samples;			
};

extern Backend *g_Backend;

#endif
