/*  DreamChess
**  Copyright (C) 2004  The DreamChess project
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef COMM_PIPE_WIN32

#include <windows.h>
#include <stdio.h>
#include <pipe_win32.h>

void comm_init(char *engine)
{
    HANDLE to_child_rd, to_child_wr, to_child_wr_dup, from_child_rd,
        from_child_wr, from_child_rd_dup, h_stdout;

    SECURITY_ATTRIBUTES sa_attr;
    BOOL fSuccess;

    PROCESS_INFORMATION proc_info;
    STARTUPINFO start_info;
    BOOL bFuncRetn = FALSE;

    /* Make pipe handles inherited. */

    sa_attr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    sa_attr.bInheritHandle = TRUE; 
    sa_attr.lpSecurityDescriptor = NULL; 

    /* Get the STDOUT handle. */

    h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);

    /* Create a pipe for input from child. */

    if (!CreatePipe(&from_child_rd, &from_child_wr, &sa_attr, 0))
    {
        fprintf(stderr, "Stdout pipe creation failed\n");
        exit(1);
    }   

    /* Make a non-inheritable copy of the read handle and close the original
    ** one.
    */

    if (!DuplicateHandle(GetCurrentProcess(), from_child_rd,
        GetCurrentProcess(), &from_child_rd_dup, 0, FALSE,
        DUPLICATE_SAME_ACCESS))
    {
        fprintf(stderr, "DuplicateHandle failed\n");
        exit(1);
    }
    CloseHandle(from_child_rd);

    /* Create a pipe for output to child. */

    if (! CreatePipe(&to_child_rd, &to_child_wr, &sa_attr, 0))
    {
        fprintf(stderr, "Stdin pipe creation failed\n");
        exit(1);
    }

    /* Make a non-inheritable copy of the write handle and close the original
    ** one.
    */

    if (!DuplicateHandle(GetCurrentProcess(), to_child_wr,
        GetCurrentProcess(), &to_child_wr_dup, 0, FALSE,
        DUPLICATE_SAME_ACCESS))
    {
        fprintf(stderr, "DuplicateHandle failed\n");
        exit(1);
    }
    CloseHandle(to_child_wr); 

    /* Create child process. */

    ZeroMemory(&proc_info, sizeof(PROCESS_INFORMATION));

    ZeroMemory(&start_info, sizeof(STARTUPINFO));
    start_info.cb = sizeof(STARTUPINFO);
    start_info.hStdError = from_child_wr;
    start_info.hStdOutput = from_child_wr;
    start_info.hStdInput = to_child_rd;
    start_info.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(NULL, engine, NULL, NULL, TRUE, DETACHED_PROCESS,
        NULL, NULL, &start_info, &proc_info))
    {
        fprintf(stderr, "CreateProcess failed\n");
        exit(1);
    }

    /* Close unneeded handles. */
    CloseHandle(proc_info.hProcess);
    CloseHandle(proc_info.hThread);

    pipe_win32_init(from_child_rd_dup, to_child_wr_dup);
}

void comm_exit()
{
    pipe_win32_exit();
}

void comm_send_str(char *str)
{
    pipe_win32_send(str);
}

char *comm_poll()
{
    return pipe_win32_poll();
}

#endif /* COMM_PIPE_WIN32 */
