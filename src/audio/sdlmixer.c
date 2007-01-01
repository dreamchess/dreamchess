/*  DreamChess
**  Copyright (C) 2006  The DreamChess project
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

#include <SDL.h>
#include <SDL_mixer.h>

#include "debug.h"
#include "playlist.h"
#include "audio.h"
#include "theme.h"

static audio_music_callback music_callback;
static Mix_Music *music = NULL;
static playlist_t *playlist;
static int next_song;
static playlist_entry_t *current_song;

static void music_finished()
{
	next_song = 1;
}

void audio_init()
{
        music_packs_t *music_packs;
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16;
	int audio_channels = 2;
	int audio_buffers = 4096;
	music_pack_t *music_pack;

	music_packs = theme_get_music_packs();

	SDL_Init(SDL_INIT_AUDIO);

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		DBG_ERROR("unable to open audio");
	}

	Mix_HookMusicFinished(music_finished);

	playlist = playlist_create();
	TAILQ_FOREACH(music_pack, music_packs, entries)
		playlist_add_tracks(playlist, music_pack->dir);

	if (!TAILQ_EMPTY(playlist))
	{
		current_song = TAILQ_LAST(playlist, playlist);
		next_song = 1;
	}
	else
		next_song = 0;
}

void audio_exit()
{
	Mix_CloseAudio();
	playlist_destroy(playlist);
}

void audio_poll()
{
	if (next_song == 1)
	{
		current_song = TAILQ_NEXT(current_song, entries);
		if (!current_song)
			current_song = TAILQ_FIRST(playlist);

		next_song = 0;

		if (music)
			Mix_FreeMusic(music);

		music = Mix_LoadMUS(current_song->filename);

		printf("Playing %s\n", current_song->filename);
		Mix_PlayMusic(music, 0);
	}
}

void audio_set_music_callback(audio_music_callback callback)
{
	music_callback = callback;
}
