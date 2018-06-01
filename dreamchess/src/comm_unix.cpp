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

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "comm.h"
#include "debug.h"
#include "dreamchess.h"
#include "pipe_unix.h"

static int init_ok = 0;

static void sigpipe_handler(int number)
{
	pipe_unix_exit();
	init_ok = 0;
	game_set_engine_error(1);
}

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

	if (pid) {
		struct sigaction sig = {};
		/* We're the parent. */

		/* Close read fd of pipe to child. */
		close(to_child[0]);
		/* Close write fd of pipe from child. */
		close(from_child[1]);

		/* Setup read fd of pipe from child as input, write fd of pipe to child as output. */
		pipe_unix_init(from_child[0], to_child[1]);

		/* Install SIGPIPE handler */
		sig.sa_handler = sigpipe_handler;
		sigaction(SIGPIPE, &sig, NULL);
	} else {
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
		exit(1);
	}
	init_ok = 1;
	return 0;
}

void comm_exit(void)
{
	if (init_ok) {
		int status;
		DBG_LOG("waiting for engine to exit");

		wait(&status);

		DBG_LOG("engine exitted with status %i", status);

		pipe_unix_exit();
	}
}

void comm_send_str(const char *str)
{
	if (init_ok)
		pipe_unix_send(str);
}

char *comm_poll(void)
{
	if (init_ok) {
		int error;
		char *retval = pipe_unix_poll(&error);

		if (!error)
			return retval;

		sigpipe_handler(0);
	}

	return NULL;
}
