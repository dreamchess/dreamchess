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

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#elif defined(_MSC_VER)
#include "msvc/getopt.h"
#endif // HAVE_GETOPT_H 

#ifdef HAVE_GETOPT_LONG
#define OPTION_TEXT(L, S, T) "  " L "\t" S "\t" T "\n"
#else
#define OPTION_TEXT(L, S, T) "  " S "\t" T "\n"
#endif

#include "GameConfig.h"

void GameConfig::parseLaunchOptions(LaunchArguments *arg) { //int argc, char **argv, ui_driver_t **ui_driver, cl_options_t *cl_options)
    int c;

#ifdef HAVE_GETOPT_LONG

    int optindex;

    struct option options[] =
        {
            {"help", no_argument, NULL, 'h'
            },
            {"list-drivers", no_argument, NULL, 'l'},
            {"ui", required_argument, NULL, 'u'},
            {"fullscreen", no_argument, NULL, 'f'},
            {"width", required_argument, NULL, 'W'},
            {"height", required_argument, NULL, 'H'},
            {"1st-engine", required_argument, NULL, '1'},
            {"verbose", required_argument, NULL, 'v'},
            {0, 0, 0, 0}
        };

    while ((c = getopt_long(arg->argc, arg->argv, "1:fhlu:v:W:H:", options, &optindex)) > -1) {
#else

    while ((c = getopt(arg->argc, arg->argv, "1:fhlu:v:W:H:")) > -1) {
#endif /* HAVE_GETOPT_LONG */
        switch (c)
        {
        case 'h':
            printf("Usage: dreamchess [options]\n\n"
                   "An xboard-compatible chess interface.\n\n"
                   "Options:\n"
                   OPTION_TEXT("--help\t", "-h\t", "Show help.")
                   OPTION_TEXT("--fullscreen\t", "-f\t", "Run fullscreen")
                   OPTION_TEXT("--width\t", "-W<num>\t", "Set screen width")
                   OPTION_TEXT("--height\t", "-H<num>\t", "Set screen height")
                   OPTION_TEXT("--1st-engine <eng>", "-1<eng>\t", "Use <eng> as first chess engine.\n\t\t\t\t\t  Defaults to 'dreamer'.")
                   OPTION_TEXT("--verbose <level>", "-v<level>", "Set verbosity to <level>.\n\t\t\t\t\t  Verbosity levels:\n\t\t\t\t\t  0 - Silent\n\t\t\t\t\t  1 - Errors only\n\t\t\t\t\t  2 - Errors and warnings only\n\t\t\t\t\t  3 - All\n\t\t\t\t\t  Defaults to 1")
                  );
            exit(0);
        case '1':
            engine = optarg;
            break;
        case 'f':
            fs = 1;
            break;
        case 'W':
            width = atoi(optarg);
            break;
        case 'H':
            height = atoi(optarg);
            break;
        case 'v':
            {
                int level;
                char *endptr;

                errno = 0;
                level = strtol(optarg, &endptr, 10);

                if (errno || (optarg == endptr) || (level < 0) || (level > 3))
                {
                    //DBG_ERROR("illegal verbosity level specified");
                    exit(1);
                }

                //dbg_set_level(level); FIXME
            }
        }
    }
}

/*static void set_cl_options(cl_options_t *cl_options)
{
    option_t *option;

	if (cl_options->engine) {
	    option = config_get_option("first_engine");
	    option_string_set_text(option, cl_options->engine);
	}

	if (cl_options->fs) {
	    option = config_get_option("full_screen");
	    option_select_value_by_name(option, "On");
	}

	if (cl_options->width) {
	    option = config_get_option("custom_resolution_width");
	    option->value = cl_options->width;
	    option = config_get_option("resolution");
	    option_select_value_by_name(option, "Custom");
        }

        if (cl_options->height) {
	    option = config_get_option("custom_resolution_height");
	    option->value = cl_options->height;
	    option = config_get_option("resolution");
	    option_select_value_by_name(option, "Custom");
        }
}*/
