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

#ifdef COMM_PIPE_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#include "pipe_unix.h"

int comm_init(char *engine)
{
    char *argv[2];
    int to_child[2], from_child[2];
    pid_t pid;

    argv[0] = engine;
    argv[1] = NULL;

    pipe(to_child);
    pipe(from_child);

    pid = fork();

    if (pid)
    {
        /* We're the parent. */

        /* Close read fd of pipe to child. */
        close(to_child[0]);
        /* Close write fd of pipe from child. */
        close(from_child[1]);

        /* Setup read fd of pipe from child as input, write fd of pipe to child as output. */
        pipe_unix_init(from_child[0], to_child[1]);
    }
    else
    {
        /* We're the child. */

        /* Copy read fd of pipe to child to stdin. */
        dup2(to_child[0], 0);
        /* Copy write fd of pipe from child to stdout. */
        dup2(from_child[1], 1);

        /* Close fd's */
        close(to_child[0]);
        close(to_child[1]);
        close(from_child[0]);
        close(from_child[1]);

        /* Execute engine. */
        execvp(engine, argv);

        /* Execute failed. */
        DBG_ERROR("failed to exec '%s'", engine);
        return 1;
    }
    return 0;
}

void comm_exit()
{
    pipe_unix_exit();
}

void comm_send_str(char *str)
{
    pipe_unix_send(str);
}

char *comm_poll()
{
    return pipe_unix_poll();
}

#endif /* COMM_PIPE_UNIX */
