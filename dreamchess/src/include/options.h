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

#ifndef DREAMCHESS_OPTIONS_H
#define DREAMCHESS_OPTIONS_H

#include "gamegui/queue.h"

#define OPTION_FOREACH(A, B) TAILQ_FOREACH(A, &B->options, entries)
#define OPTION_VALUE_FOREACH(A, B) TAILQ_FOREACH(A, &B->values, entries)

typedef struct option_value
{
	char *name;
	void *data;
	int index;
	TAILQ_ENTRY(option_value) entries;
} option_value_t;

typedef struct option_data
{
	int type;
	char *name;

	/* Int type */
	int value;

	/* String type */
	char *string;

	/* Option type */
	option_value_t *selected;
	TAILQ_HEAD(values_head, option_value) values;
	int size;
	TAILQ_ENTRY(option_data) entries;
} option_t;

typedef struct option_group
{
	char *name;
	TAILQ_HEAD(options_head, option_data) options;
} option_group_t;

#define OPTION_TYPE_OPTION 0
#define OPTION_TYPE_INT 1
#define OPTION_TYPE_STRING 2

option_group_t *option_group_create(char *name);
option_t *option_group_add_option(option_group_t *group, char *name);
int option_group_save_xml(option_group_t *group);
char *option_group_save_string(option_group_t *group);
int option_group_load_xml(option_group_t *group);
int option_group_load_string(option_group_t *group, char *string);
void option_add_value(option_t *option, char *name, void *data);
int option_select_value_by_name(option_t *option, char *name);
int option_select_value_by_index(option_t *option, int index);
option_t *option_group_find_option(option_group_t *group, char *name);
int option_select_next_value(option_t *option);
int option_select_prev_value(option_t *option);
void option_string_set_text(option_t *option, char *text);
option_t *option_group_add_int(option_group_t *group, char *name);
option_t *option_group_add_string(option_group_t *group, char *name);

#endif
