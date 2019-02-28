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

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "playlist.h"
#include "xml.h"

typedef struct {
	playlist_t *playlist;
	char *dir;
	playlist_entry_t *entry;
} state;

playlist_t *playlist_create(void)
{
	playlist_t *playlist = malloc(sizeof(playlist_t));

	TAILQ_INIT(playlist);

	return playlist;
}

void playlist_destroy(playlist_t *playlist)
{
	playlist_entry_t *entry;

	while ((entry = TAILQ_FIRST(playlist))) {
		free(entry->title);
		free(entry->album);
		free(entry->artist);
		free(entry->filename);
		TAILQ_REMOVE(playlist, entry, entries);
		free(entry);
	}
}

static void track_open_cb(void *user_data)
{
	state *s = (state *)user_data;
	s->entry = malloc(sizeof(playlist_entry_t));
	memset(s->entry, 0, sizeof(playlist_entry_t));
	s->entry->title = strdup("Unknown title");
	s->entry->album = strdup("Unknown album");
	s->entry->artist = strdup("Unknown artist");
}

static void track_close_cb(void *user_data)
{
	state *s = (state *)user_data;

	if (s->entry->filename) {
		TAILQ_INSERT_TAIL(s->playlist, s->entry, entries);
	} else {
		DBG_WARN("skipping playlist item without filename");
		free(s->entry->title);
		free(s->entry->album);
		free(s->entry->artist);
		free(s->entry);
	}

	s->entry = NULL;
}

static void set_string(char **s, const char *t)
{
	free(*s);
	*s = strdup(t);
}

static void track_data_cb(void *user_data, const char *element, char *const *attrs, const char *text)
{
	state *s = (state *)user_data;

	if (!strcmp(element, "title"))
		set_string(&s->entry->title, text);
	else if (!strcmp(element, "album"))
		set_string(&s->entry->album, text);
	else if (!strcmp(element, "artist"))
		set_string(&s->entry->artist, text);
	else if (!strcmp(element, "filename")) {
		free(s->entry->filename);
		s->entry->filename = malloc(strlen(s->dir) + strlen(text) + 2);
		strcpy(s->entry->filename, s->dir);
		strcat(s->entry->filename, "/");
		strcat(s->entry->filename, text);
	} else
		DBG_WARN("skipping invalid playlist item property '%s'", element);
}

void playlist_add_tracks(playlist_t *playlist, char *dir)
{
	state s;
	char *filename = malloc(strlen(dir) + strlen("/tracks.xml") + 1);

	strcpy(filename, dir);
	strcat(filename, "/tracks.xml");

	s.playlist = playlist;
	s.dir = dir;
	s.entry = NULL;

	if (xml_parse(filename, "track", track_data_cb, track_open_cb, track_close_cb, &s))
		DBG_WARN("failed to load '%s'", filename);

	free(filename);

	if (s.entry) {
		free(s.entry->title);
		free(s.entry->album);
		free(s.entry->artist);
		free(s.entry);
	}
}
