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

#include <memory>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <pugixml.hpp>

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

option_group_t *option_group_create(const char *name)
{
	option_group_t *group = (option_group_t *)malloc(sizeof(option_group_t));

	group->name = remove_spaces(name);
	TAILQ_INIT(&group->options);
	return group;
}

option_t *option_group_add_option(option_group_t *group, const char *name)
{
	option_t *option = (option_t *)malloc(sizeof(option_t));

	option->type = OPTION_TYPE_OPTION;
	option->name = remove_spaces(name);
	option->selected = NULL;
	option->size = 0;
	TAILQ_INIT(&option->values);
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

option_t *option_group_add_int(option_group_t *group, const char *name)
{
	option_t *option = (option_t *)malloc(sizeof(option_t));

	option->type = OPTION_TYPE_INT;
	option->name = remove_spaces(name);
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

option_t *option_group_add_string(option_group_t *group, const char *name)
{
	option_t *option = (option_t *)malloc(sizeof(option_t));

	option->type = OPTION_TYPE_STRING;
	option->name = remove_spaces(name);
	option->string = NULL;
	TAILQ_INSERT_TAIL(&group->options, option, entries);
	return option;
}

static std::unique_ptr<pugi::xml_document> option_group_save(option_group_t *group) {
	std::unique_ptr<pugi::xml_document> doc(new pugi::xml_document);
	pugi::xml_node options = doc->append_child("options");

	option_t *option;
	TAILQ_FOREACH(option, &group->options, entries) {
		pugi::xml_node data = options.append_child(option->name);

		if (option->type == OPTION_TYPE_OPTION)
			data.text().set(option->selected->name);
		else if (option->type == OPTION_TYPE_INT)
			data.text().set(option->value);
		else
			data.text().set(option->string);
	}

	return doc;
}

int option_group_save_xml(option_group_t *group) {
	std::unique_ptr<pugi::xml_document> doc(option_group_save(group));
	std::string filename(group->name);
	filename += ".xml";

	ch_userdir();

	if (!doc->save_file(filename.c_str())) {
		DBG_ERROR("failed to save '%s'", filename.c_str());
		return -1;
	}

	return 0;
}

char *option_group_save_string(option_group_t *group)
{
	std::unique_ptr<pugi::xml_document> doc(option_group_save(group));

	std::ostringstream stream;
	doc->save(stream);
	return strdup(stream.str().c_str());
}

static void option_group_load(option_group_t *group, const pugi::xml_document &doc) {
	pugi::xml_node options = doc.child("options");
	for (pugi::xml_node_iterator it = options.begin(); it != options.end(); ++it) {
		option_t *option = option_group_find_option(group, it->name());

		if (!option) {
			DBG_WARN("option '%s' does not exist", it->name());
			continue;
		}

		if (option->type == OPTION_TYPE_OPTION) {
			if (option_select_value_by_name(option, it->text().get()) == -1)
				DBG_WARN("option '%s' has no value '%s'", option->name, it->text().get());
			DBG_LOG("setting multi option '%s' to '%s'", option->name, it->text().get());
		} else if (option->type == OPTION_TYPE_INT) {
			option->value = it->text().as_int();
			DBG_LOG("setting int option '%s' to '%d'", option->name, option->value);
		} else {
			option->string = strdup(it->text().get());
			DBG_LOG("setting string option '%s' to '%s'", option->name, option->string);
		}
	}
}

int option_group_load_xml(option_group_t *group) {
	pugi::xml_document doc;
	std::string filename(group->name);
	filename += ".xml";
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	if (!result) {
		DBG_ERROR("failed to load option file '%s': %s at offset %d", filename.c_str(), result.description(), result.offset);
		return -1;
	}

	option_group_load(group, doc);
	return 0;
}

int option_group_load_string(option_group_t *group, const char *string) {
	pugi::xml_document doc;
#if PUGIXML_VERSION >= 150
	pugi::xml_parse_result result = doc.load_string(string);
#else
	pugi::xml_parse_result result = doc.load(string);
#endif

	if (!result) {
		DBG_ERROR("failed to load option string");
		return -1;
	}

	option_group_load(group, doc);
	return 0;
}

void option_add_value(option_t *option, const char *name, void *data)
{
	option_value_t *value = (option_value_t *)malloc(sizeof(option_value_t));

	value->name = strdup(name);
	value->data = data;
	value->index = option->size++;
	TAILQ_INSERT_TAIL(&option->values, value, entries);
	if (option->size == 1)
		option->selected = value;
}

int option_select_value_by_name(option_t *option, const char *name)
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

	if (!TAILQ_PREV(option->selected, option_data::values_head, entries))
		return -1;

	option->selected = TAILQ_PREV(option->selected, option_data::values_head, entries);
        return 0;
}

option_t *option_group_find_option(option_group_t *group, const char *name)
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

void option_string_set_text(option_t *option, const char *text)
{
	if (option->string)
		free(option->string);

	option->string = strdup(text);
}
