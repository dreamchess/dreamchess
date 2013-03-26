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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef COMM_SDL_THREADS

#include <stdio.h>

#include "SDL.h"
#include "SDL_thread.h"

#include "dreamchess.h"
#include "engine.h"
#include "pipe_mem.h"

/* Message list from engine to ui. */
char *to_ui;
SDL_mutex *to_ui_mutex;

/* Message list from ui to engine. */
char *to_engine;
SDL_mutex *to_engine_mutex;

int main(int argc, char **argv)
{
    SDL_Thread *thread;
    int retval;
    arguments_t arg;

    arg.argc = argc;
    arg.argv = argv;

    pipe_mem_init(&to_ui);
    pipe_mem_init(&to_engine);

    to_ui_mutex = SDL_CreateMutex();
    to_engine_mutex = SDL_CreateMutex();

    if (!to_ui_mutex || !to_engine_mutex)
    {
        fprintf(stderr, "Unable to create mutexes: %s\n", SDL_GetError());
        return 1;
    }

#ifndef __APPLE__
    thread = SDL_CreateThread(dreamchess, &arg);
    if ( thread == NULL )
    {
        fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
        return 1;
    }

    retval = engine();

    printf("Waiting for thread to exit\n");
    SDL_WaitThread(thread, NULL);
    printf("Thread exit ok\n");
#else
    thread = SDL_CreateThread(engine, NULL);
    if( thread == NULL )
    {
        fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
        return 1;
    }

    dreamchess(&arg);

    printf("Waiting for thread to exit\n");
    SDL_WaitThread(thread, &retval);
#endif

    pipe_mem_exit(&to_ui);
    pipe_mem_exit(&to_engine);

    SDL_DestroyMutex(to_ui_mutex);
    SDL_DestroyMutex(to_engine_mutex);

    return retval;
}

#endif /* COMM_SDL_THREADS */
