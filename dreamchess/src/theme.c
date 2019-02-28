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
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include "dreamchess.h"
#include "xml.h"
#include "theme.h"
#include "debug.h"
#include "options.h"
#include "dir.h"

#define THEME_DIR "themes"

static music_packs_t music_packs;

typedef struct {
	option_t *option;
    struct theme_struct *theme;
} state;

static void theme_open_cb(void *user_data)
{
    state *s = (state *)user_data;

    s->theme = malloc(sizeof(struct theme_struct));
    memset(s->theme, 0, sizeof(struct theme_struct));
    s->theme->name = strdup("Untitled");
    s->theme->style = strdup("default");
    s->theme->pieces = strdup("classiclow");
    s->theme->board = strdup("classic");
    s->theme->white_name = strdup("White");
    s->theme->black_name = strdup("Black");
    s->theme->lighting = TRUE;
    s->theme->selector.colour[0] = 1.0;
    s->theme->selector.colour[1] = 1.0;
    s->theme->selector.colour[2] = 1.0;
    s->theme->selector.colour[3] = 1.0;
    s->theme->selector.spinspeed = 0;
    s->theme->selector.size = 1;
    s->theme->selector.bouncespeed = 0;
}

static void theme_close_cb(void *user_data)
{
	state *s = (state *)user_data;

    DBG_LOG("added theme: %s %s", s->theme->name, s->theme->style);
    option_add_value(s->option, s->theme->name, s->theme);
    s->theme = NULL;
}

static void set_string(char **s, const char *t)
{
	free(*s);
	*s = strdup(t);
}

static int parse_rgba(const char *rgba, float *r, float *g, float *b, float *a)
{
    float r_, g_, b_, a_;

    if (sscanf(rgba, "rgba(%f,%f,%f,%f)", &r_, &g_, &b_, &a_) < 4) {
        DBG_ERROR("failed to parse rgba color: '%s'", rgba);
        return -1;
    }

    *r = r_;
    *g = g_;
    *b = b_;
    *a = a_;

    return 0;
}

static void theme_data_cb(void *user_data, const char *element, char *const *attrs, const char *text)
{
	state *s = (state *)user_data;

    if (!strcmp(element, "name"))
        set_string(&s->theme->name, text);
    else if (!strcmp(element, "style"))
        set_string(&s->theme->style, text);
    else if (!strcmp(element, "pieces"))
        set_string(&s->theme->pieces, text);
    else if (!strcmp(element, "board"))
        set_string(&s->theme->board, text);
    else if (!strcmp(element, "white_name"))
        set_string(&s->theme->white_name, text);
    else if (!strcmp(element, "black_name"))
        set_string(&s->theme->black_name, text);
    else if (!strcmp(element, "selector")) {
        int i = 0;
        while (attrs[i] && attrs[i + 1]) {
            if (!strcmp(attrs[i], "spinspeed"))
                s->theme->selector.spinspeed = atof(attrs[i + 1]);
            else if (!strcmp(attrs[i], "size"))
                s->theme->selector.size = atof(attrs[i + 1]);
            else if (!strcmp(attrs[i], "bouncespeed"))
                s->theme->selector.bouncespeed = atof(attrs[i + 1]);
            else if (!strcmp(attrs[i] , "colour"))
                parse_rgba(attrs[i + 1], &s->theme->selector.colour[0], &s->theme->selector.colour[1],
                           &s->theme->selector.colour[2], &s->theme->selector.colour[3]);
            i += 2;
        }
    }
}

static void theme_add_theme(char *xmlfile, option_t *option)
{
    state s;

    s.option = option;
    s.theme = NULL;

    if (xml_parse(xmlfile, "theme", theme_data_cb, theme_open_cb, theme_close_cb, &s))
        DBG_ERROR("error loading theme file '%s'", xmlfile);

    if (s.theme) {
        free(s.theme->name);
        free(s.theme->style);
        free(s.theme->pieces);
        free(s.theme->board);
        free(s.theme->white_name);
        free(s.theme->black_name);
        free(s.theme);
    }
}

#ifdef _WIN32

static void find_themes(option_t *option)
{
    HANDLE hFind;
    WIN32_FIND_DATA ffd;

    hFind = FindFirstFile(THEME_DIR "\\*.xml", &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        char *filename = malloc(strlen(ffd.cFileName) + strlen(THEME_DIR) + 1 + 1);
        strcpy(filename, THEME_DIR "\\");
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

    if ( (themedir=opendir(THEME_DIR)) != NULL )
    {
        while ((themedir_entry = readdir(themedir)) != NULL)
        {
            if ( themedir_entry->d_name[0] != '.' )
            {
                char *filename = malloc(strlen(THEME_DIR) + 1 + strlen(themedir_entry->d_name) + 1);
                strcpy(filename, THEME_DIR "/");
                strcat(filename, themedir_entry->d_name);
                theme_add_theme(filename, option);
                free(filename);
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
    music_pack_t *music = malloc(sizeof(music_pack_t));

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
            char *dirname = malloc(strlen(cur_dir) + 7 + strlen(ffd.cFileName) + 1);

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
                char *dirname = malloc(strlen(cur_dir) + strlen(entry->d_name) + 8);

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
        char data[MAX_PATH];
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

        chdir(data);
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
