/*  DreamChess
**  Copyright (C) 2003-2004  The DreamChess Development Group
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

#include <stdlib.h>
#include <string.h>
#include "msglist.h"

void msglist_init(msglist_t *list)
{
    list->first = NULL;
    list->last = NULL;
}

void msglist_add(msglist_t *list, char *msg)
{
    msglist_entry_t *entry = malloc(sizeof(msglist_entry_t));
    entry->msg = strdup(msg);
    entry->next = NULL;
    if (list->last)
        list->last->next = entry;
    else
        list->first = entry;
    list->last = entry;
}

char *msglist_remove(msglist_t *list)
{
    char *retval;
    msglist_entry_t *entry = list->first;

    if (!entry)
        return NULL;

    retval = entry->msg;

    if (!entry->next)
    {
        list->first = NULL;
        list->last = NULL;
    }
    else
        list->first = entry->next;

    free(entry);
    return retval;
}

void msglist_free(msglist_t *list)
{
    char *msg;
    while ((msg = msglist_remove(list)))
        free(msg);
}

int msglist_poll(msglist_t *list)
{
    return list->first != NULL;
}
