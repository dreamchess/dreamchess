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

#include <cstdlib>
#include <cstring>
#include <pugixml.hpp>
#include <string>

#include "debug.h"
#include "playlist.h"

playlist_t *playlist_create(void)
{
	playlist_t *playlist = (playlist_t *)malloc(sizeof(playlist_t));

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

static void playlist_add_track(playlist_t *playlist, const char *title, const char *album, const char *artist,
							   const char *filename)
{
	playlist_entry_t *entry = (playlist_entry_t *)malloc(sizeof(playlist_entry_t));

	entry->title = strdup(title);
	entry->album = strdup(album);
	entry->artist = strdup(artist);
	entry->filename = strdup(filename);

	TAILQ_INSERT_TAIL(playlist, entry, entries);
}

void playlist_add_tracks(playlist_t *playlist, char *dir)
{
	const std::string filename = std::string(dir) + "/tracks.xml";

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	if (!result) {
		DBG_ERROR("failed to load theme file '%s': %s at offset %d", filename.c_str(), result.description(),
				  result.offset);
		return;
	}

	for (pugi::xml_node track = doc.child("tracks").child("track"); track; track = track.next_sibling("track")) {
		const char *title = track.child("title").text().as_string("Unknown title");
		const char *album = track.child("album").text().as_string("Unknown album");
		const char *artist = track.child("artist").text().as_string("Unknown artist");

		if (!track.child("filename").text()) {
			DBG_ERROR("skipping music track without filename: '%s'", title);
			continue;
		}

		std::string trackfile = std::string(dir) + '/' + track.child("filename").text().get();
		playlist_add_track(playlist, title, album, artist, trackfile.c_str());
	}
}
