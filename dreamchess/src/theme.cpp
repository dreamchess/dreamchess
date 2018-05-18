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

#include <pugixml.hpp>
#include <limits.h>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include "dreamchess.h"
#include "theme.h"
#include "debug.h"
#include "options.h"
#include "dir.h"

static music_packs_t music_packs;

static void theme_add_theme(char *xmlfile, option_t *option)
{
    DBG_LOG("loading %s", xmlfile);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(xmlfile);

    if (!result) {
        DBG_ERROR("failed to load theme file '%s': %s at offset %d", xmlfile, result.description(), result.offset);
        return;
    }

    for (pugi::xml_node theme = doc.child("themes").child("theme"); theme; theme = theme.next_sibling("theme")) {
        theme_struct *cur_theme = (theme_struct *)malloc(sizeof(theme_struct));

        cur_theme->name = strdup(theme.child("name").text().as_string("Untitled"));
        cur_theme->style = strdup(theme.child("style").text().as_string("default"));
        cur_theme->pieces = strdup(theme.child("pieces").text().as_string("classiclow"));
        cur_theme->board = strdup(theme.child("board").text().as_string("classic"));
        cur_theme->white_name = strdup(theme.child("white_name").text().as_string("White"));
        cur_theme->black_name = strdup(theme.child("black_name").text().as_string("Black"));
        cur_theme->lighting = true;

        pugi::xml_node selector = theme.child("selector");

        cur_theme->selector.spinspeed = selector.attribute("spinspeed").as_float(0);
        cur_theme->selector.size = selector.attribute("size").as_float(1);
        cur_theme->selector.bouncespeed = selector.attribute("bouncespeed").as_float(0);

        pugi::xml_node selectorColour = selector.child("colour");

        cur_theme->selector.colour[0] = selectorColour.child("red").text().as_float(1);
        cur_theme->selector.colour[1] = selectorColour.child("green").text().as_float(1);
        cur_theme->selector.colour[2] = selectorColour.child("blue").text().as_float(1);
        cur_theme->selector.colour[3] = selectorColour.child("alpha").text().as_float(1);

        DBG_LOG("added theme: %s %s", cur_theme->name, cur_theme->style);
        option_add_value(option, cur_theme->name, cur_theme);
    }
}

#ifdef _WIN32

static void find_themes(option_t *option)
{
    HANDLE hFind;
    WIN32_FIND_DATA ffd;

    hFind = FindFirstFile("themes\\*.xml", &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        char *filename = (char *)malloc(strlen(ffd.cFileName) + 7 + 1);
        strcpy(filename, "themes\\");
        strcat(filename, ffd.cFileName);
        theme_add_theme(filename, option);
        free(filename);
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

#else

static void find_themes(option_t *option)
{
    DIR* themedir;
    struct dirent* themedir_entry;

    if ( (themedir=opendir("themes")) != NULL )
    {
        while ((themedir_entry = readdir(themedir)) != NULL)
        {
            char temp[80];
            if ( themedir_entry->d_name[0] != '.' )
            {
                sprintf( temp, "themes/%s", themedir_entry->d_name );
                theme_add_theme(temp, option);
            }
        }
        closedir(themedir);
    }
}

#endif

void theme_find_themes(option_t *option)
{
    ch_datadir();
    find_themes(option);

    ch_userdir();
    find_themes(option);

    option_select_value_by_name(option, "Classic Wooden");
}

static void add_music_pack(char *dir)
{
    music_pack_t *music = (music_pack_t *)malloc(sizeof(music_pack_t));

    music->dir = strdup(dir);

    TAILQ_INSERT_TAIL(&music_packs, music, entries);
}

#ifdef _WIN32

static void find_music_packs(void)
{
    char *cur_dir = getcwd(NULL, 0);

    if (!cur_dir)
        DBG_ERROR("could not determine current directory");

    HANDLE hFind;
    WIN32_FIND_DATA ffd;

    hFind = FindFirstFile("music\\*", &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ffd.cFileName[0] != '.') {
            char *dirname = (char *)malloc(strlen(cur_dir) + 7 + strlen(ffd.cFileName) + 1);

            strcpy(dirname, cur_dir);
            strcat(dirname, "\\music\\");
            strcat(dirname, ffd.cFileName);

            add_music_pack(dirname);
            free(dirname);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
    free(cur_dir);
}

#else

static void find_music_packs(void)
{
    DIR *dir;

#ifdef PATH_MAX
    char cur_dir[PATH_MAX];

    if (!getcwd(cur_dir, PATH_MAX))
        DBG_ERROR("could not determine current directory");
#else
    /* PATH_MAX not defined, try getcwd extension */
    char *cur_dir = getcwd(NULL, 0);

    if (!cur_dir)
        DBG_ERROR("could not determine current directory");
#endif

    if ((dir = opendir("music")) != NULL)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
            {
                char *dirname = (char *)malloc(strlen(cur_dir) + strlen(entry->d_name) + 8);

                strcpy(dirname, cur_dir);
                strcat(dirname, "/music/");
                strcat(dirname, entry->d_name);

                add_music_pack(dirname);
                free(dirname);
            }
        }
        closedir(dir);
    }

#ifndef PATH_MAX
    free(cur_dir);
#endif
}

#endif

void theme_find_music_packs(void)
{
    TAILQ_INIT(&music_packs);

    ch_datadir();
    find_music_packs();

    ch_userdir();
    find_music_packs();

#ifdef _WIN32
    {
        HKEY key;
        BYTE data[MAX_PATH];
        DWORD size = MAX_PATH - 1;

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DreamChess Music",
            0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &key) != ERROR_SUCCESS)
        {
            DBG_LOG("couldn't find music pack in registry");
            return;
        }

        if (RegQueryValueEx(key, "InstallLocation", NULL, NULL, data, &size)
            != ERROR_SUCCESS)
        {
            DBG_LOG("couldn't find music pack in registry");
            return;
        }

        data[size] = 0;
        DBG_LOG("music pack location: %s", data);

        chdir((char *)data);
        find_music_packs();
    }
#elif defined(__APPLE__)
    {
        if (chdir("/Library/Application Support/DreamChess") == 0)
            find_music_packs();
    }
#endif
}

music_packs_t *theme_get_music_packs(void)
{
    return &music_packs;
}
