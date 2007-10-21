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

#include <stdlib.h>
#include <mxml.h>

#include "options.h"
#include "debug.h"

static char *remove_spaces(const char *str)
{
	char *sc = strdup(str);
	char *s = sc;

	while (*s) {
		if (*s == ' ')
			*s = '_';
		s++;
	}

	return sc;
}

static const char *whitespace_cb(mxml_node_t *node, int where)
{
	if (!strcmp(node->value.element.name, "?xml version=\"1.0\"") && (where == MXML_WS_AFTER_OPEN))
		return "\n";

	if (where == MXML_WS_AFTER_CLOSE)
		return "\n";

	return NULL;
}

option_group_t *option_group_create(char *name)
{
	option_group_t *group = malloc(sizeof(option_group_t));

	group->name = remove_spaces(name);
	TAILQ_INIT(&group->options);
	return group;
}

option_t *option_group_add_option(option_group_t *group, char *name)
{
	option_t *option = malloc(sizeof(option_t));

	option->name = remove_spaces(name);
	option->selected = NULL;
	option->size = 0;
	TAILQ_INIT(&option->values);
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

int option_group_save_xml(option_group_t *group)
{
	mxml_node_t *xml;
	option_t *option;
	FILE *f;
	char *filename;
	int error;

	xml = mxmlNewElement(MXML_NO_PARENT, "?xml version=\"1.0\"");

	TAILQ_FOREACH(option, &group->options, entries) {
		if (option->selected) {
			mxml_node_t *data;

			data = mxmlNewElement(xml, option->name);
			mxmlNewText(data, 0, option->selected->name);
		}
	}

	filename = malloc(strlen(group->name) + 4 + 1);
	strcpy(filename, group->name);
	strcat(filename, ".xml");

	ch_userdir();
	f = fopen(filename, "w");

	if (!f) {
		mxmlDelete(xml);
		DBG_ERROR("failed to open '%s'", filename);
		free(filename);
		return -1;
	}

	error = mxmlSaveFile(xml, f, whitespace_cb);
	free(filename);
	mxmlDelete(xml);

	return error;
}

void option_add_value(option_t *option, char *name, void *data)
{
	option_value_t *value = malloc(sizeof(option_value_t));

	value->name = strdup(name);
	value->data = data;
	value->index = option->size++;
	TAILQ_INSERT_TAIL(&option->values, value, entries);
}

int option_select_value_by_name(option_t *option, char *name)
{
	option_value_t *value;

	TAILQ_FOREACH(value, &option->values, entries) {
		if (!strcmp(value->name, name))
			break;
	}

	if (value) {
		option->selected = value;
		return 0;
	}

	return -1;
}

int option_select_value_by_index(option_t *option, int index) {
	option_value_t *value;

	TAILQ_FOREACH(value, &option->values, entries) {
		if (index-- == 0)
			break;
	}

	if (value) {
		option->selected = value;
		return 0;
	}

	return -1;
}

int option_select_next_value(option_t *option)
{
	if (!option->selected)
		return -1;

	if (!TAILQ_NEXT(option->selected, entries))
		return -1;

	option->selected = TAILQ_NEXT(option->selected, entries);
}

int option_select_prev_value(option_t *option)
{
	if (!option->selected)
		return -1;

	if (!TAILQ_PREV(option->selected, values_head, entries))
		return -1;

	option->selected = TAILQ_PREV(option->selected, values_head, entries);
}

option_t *option_group_find_option(option_group_t *group, char *name)
{
	char *namews = remove_spaces(name);
	option_t *option;

	TAILQ_FOREACH(option, &group->options, entries) {
		if (!strcmp(option->name, namews))
			break;
	}

	free(namews);
	return option;
}
