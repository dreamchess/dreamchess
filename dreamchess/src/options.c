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
#include <mxml.h>

#include "options.h"
#include "debug.h"
#include "dir.h"

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
	if ((!strcmp(node->value.element.name, "?xml version=\"1.0\"?")
            || !strcmp(node->value.element.name, "options"))
               && (where == MXML_WS_AFTER_OPEN))
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

	option->type = OPTION_TYPE_OPTION;
	option->name = remove_spaces(name);
	option->selected = NULL;
	option->size = 0;
	TAILQ_INIT(&option->values);
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

option_t *option_group_add_int(option_group_t *group, char *name)
{
	option_t *option = malloc(sizeof(option_t));

	option->type = OPTION_TYPE_INT;
	option->name = remove_spaces(name);
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

option_t *option_group_add_string(option_group_t *group, char *name)
{
	option_t *option = malloc(sizeof(option_t));

	option->type = OPTION_TYPE_STRING;
	option->name = remove_spaces(name);
	option->string = NULL;
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

static void option_group_save(option_group_t *group, FILE *f)
{
	option_t *option;

	fputs("<?xml version=\"1.0\"?>\n<options>\n", f);

	TAILQ_FOREACH(option, &group->options, entries) {
		fprintf(f, "<%s>", option->name);

		if (option->type == OPTION_TYPE_OPTION)
			fputs(option->selected->name, f);
		else if (option->type == OPTION_TYPE_INT)
			fprintf(f, "%d", option->value);
		else
			fputs(option->string, f);

		fprintf(f, "</%s>\n", option->name);
	}

	fputs("</options>\n", f);
}

int option_group_save_xml(option_group_t *group)
{
	FILE *f;
	char *filename;
	int error = 0;

	filename = malloc(strlen(group->name) + 4 + 1);
	strcpy(filename, group->name);
	strcat(filename, ".xml");

	ch_userdir();
	f = fopen(filename, "w");

	if (!f) {
		DBG_ERROR("failed to open '%s'", filename);
		free(filename);
		return -1;
	}

	option_group_save(group, f);

	if (ferror(f)) {
		DBG_ERROR("failed to write '%s'", filename);
		error = -1;
	}

	free(filename);
	fclose(f);

	return error;
}

static void option_group_load(option_group_t *group, mxml_node_t *tree)
{
	mxml_node_t *node;

	node = tree;
	while ((node = mxmlWalkNext(node, tree, MXML_DESCEND))) {
		if (node->type == MXML_ELEMENT) {
			option_t *option;
			node = mxmlWalkNext(node, tree, MXML_DESCEND);
			if (!node)
				break;
			option = option_group_find_option(group, node->parent->value.opaque);
			if (!option) {
				DBG_WARN("option '%s' does not exist", node->parent->value.opaque);
				continue;
			}
			if (option->type == OPTION_TYPE_OPTION) {
				if (option_select_value_by_name(option, node->value.opaque) == -1)
					DBG_WARN("option '%s' has no value '%s'", option->name, node->value.opaque);
				DBG_LOG("setting option '%s' to '%s'", option->name, node->value.opaque);
			} else if (option->type == OPTION_TYPE_INT) {
				int val;

				errno = 0;
				val = strtol(node->value.opaque, NULL, 10);
				if (errno) {
					DBG_WARN("value '%s' for option '%s' is not an integer", node->value.opaque, option->name);
				} else {
					option->value = val;
				}
			} else
                                option->string = strdup(node->value.opaque);
		}
	}
}

int option_group_load_xml(option_group_t *group)
{
	FILE *f;
	mxml_node_t *tree;
	mxml_node_t *node;
	char *filename;

	filename = malloc(strlen(group->name) + 4 + 1);
	strcpy(filename, group->name);
	strcat(filename, ".xml");

	f = fopen(filename, "r");
	if (f)
		tree = mxmlLoadFile(NULL, f, MXML_OPAQUE_CALLBACK);
	else {
		DBG_WARN("failed to open '%s'", filename);
		free(filename);
		return -1;
	}

	fclose(f);

	if (!tree) {
		DBG_ERROR("failed to parse '%s'", filename);
		free(filename);
		return -1;
	}

	node = mxmlFindElement(tree, tree, "options", NULL, NULL, MXML_DESCEND);

	if (!node)
		node = tree;

	free(filename);
	option_group_load(group, tree);

	return 0;
}

void option_add_value(option_t *option, char *name, void *data)
{
	option_value_t *value = malloc(sizeof(option_value_t));

	value->name = strdup(name);
	value->data = data;
	value->index = option->size++;
	TAILQ_INSERT_TAIL(&option->values, value, entries);
	if (option->size == 1)
		option->selected = value;
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
        return 0;
}

int option_select_prev_value(option_t *option)
{
	if (!option->selected)
		return -1;

	if (!TAILQ_PREV(option->selected, values_head, entries))
		return -1;

	option->selected = TAILQ_PREV(option->selected, values_head, entries);
        return 0;
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

void option_string_set_text(option_t *option, char *text)
{
	if (option->string)
		free(option->string);

	option->string = strdup(text);
}

