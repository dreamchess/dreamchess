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

#include "ui_sdlgl.h"
#include <cstdio>
#include <chrono>
#include <pugixml.hpp>

static int slots;
static char time_save[SAVEGAME_SLOTS][80];
static config_t config_save[SAVEGAME_SLOTS];
static board_t saved_board[SAVEGAME_SLOTS];

board_t *get_saved_board( int index )
{
    return &saved_board[index];
}

config_t *get_config_save( int index )
{
    return &config_save[index];
}

char *get_time_save( int index )
{
    return time_save[index];
}

void set_slots( int slots )
{
    slots=slots;
}

int get_slots(void)
{
    return slots;
}

void write_save_xml( int slot )
{
    pugi::xml_document doc;
    std::string filename("save");
    filename += std::to_string(slot);
    filename += ".xml";

    DBG_LOG("writing save xml: %s", filename.c_str());

    pugi::xml_node save = doc.append_child("save");

    pugi::xml_node node = save.append_child("time");

    std::time_t curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#ifdef PUGIXML_HAS_LONG_LONG
    node.text().set(static_cast<long long>(curTime));
#else
    node.text().set(static_cast<int>(curTime));
#endif

    if (ch_userdir()) {
        DBG_WARN("could not enter user directory");
        return;
    }

    node = save.append_child("white");
    node.text().set((get_config()->player[WHITE] == PLAYER_UI ? "ui" : "engine"));

    node = save.append_child("black");
    node.text().set((get_config()->player[BLACK] == PLAYER_UI ? "ui" : "engine"));

    node = save.append_child("level");
    node.text().set(get_config()->cpu_level);

    node = save.append_child("difficulty");
    node.text().set(get_config()->difficulty);

    const char *fen = fen_encode(get_board());

    if (!fen) {
        DBG_ERROR("error encoding FEN");
        return;
    }

    node = save.append_child("fen");
    node.text().set(fen);

    if (!doc.save_file(filename.c_str()))
        DBG_ERROR("failed to save '%s'", filename.c_str());
}

void load_save_xml(int slot) {
    pugi::xml_document doc;
    std::string filename("save");
    filename += std::to_string(slot);
    filename += ".xml";

    if (ch_userdir()) {
        DBG_WARN("could not enter user directory");
        return;
    }

    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    if (!result) {
        if (result.status != pugi::status_file_not_found)
            DBG_ERROR("failed to parse '%s'", filename.c_str());
        return;
    }

    DBG_LOG("reading save xml: %s", filename.c_str());

    pugi::xml_node save = doc.child("save");

#ifdef PUGIXML_HAS_LONG_LONG
    time_t time = save.child("time").text().as_llong(0);
#else
    time_t time = save.child("time").text().as_int(0);
#endif

    tm *tm = localtime(&time);
    std::snprintf(time_save[slot], sizeof(time_save[slot]), "%02i/%02i at %02i:%02i.", tm->tm_mday, tm->tm_mon + 1, tm->tm_hour, tm->tm_min);

    std::string temp = save.child("white").text().get();
    if (temp == "ui")
        config_save[slot].player[WHITE] = PLAYER_UI;
    else
        config_save[slot].player[WHITE] = PLAYER_ENGINE;

    temp = save.child("black").text().get();
    if (temp == "ui")
        config_save[slot].player[BLACK] = PLAYER_UI;
    else
        config_save[slot].player[BLACK] = PLAYER_ENGINE;

    config_save[slot].cpu_level = save.child("level").text().as_int(1);
    config_save[slot].difficulty = save.child("difficult").text().as_int(1);
    board_t *board = fen_decode(save.child("fen").text().get());

    if (!board) {
        DBG_ERROR("no valid FEN board specified in save game");
        return;
    }

    saved_board[slot] = *board;
    free(board);
    slots |= (1 << slot);
}
