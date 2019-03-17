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

#include <errno.h>
#include <expat.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "xml.h"

#define BUFF_SIZE 1024

/* Returns elements (and enclosed characters) one level below requested parent elements. */

typedef struct {
	XML_Parser parser;
	int depth;					   /* Current parsing depth */
	int children_depth;			   /* The depth where we need to return elements and characters for, or -1 */
	const char *parent_element;	   /* Parent element to search for */
	xml_element_cb element_cb;	   /* Element callback */
	xml_parent_cb parent_open_cb;  /* Parent element open callback */
	xml_parent_cb parent_close_cb; /* Parent element close callback */
	void *cb_user_data;			   /* User data pointer to pass in callbacks */
	char *char_buf;				   /* Buffer for receiving character data */
	int char_buf_len;			   /* Length of character buffer */
	char **attrs;				   /* Attribute data, terminated by a NULL pointer */
} state;

static void free_char_buf(state *s) {
	free(s->char_buf);
	s->char_buf = NULL;
}

static void init_char_buf(state *s) {
	free_char_buf(s);
	s->char_buf = malloc(1);
	s->char_buf[0] = 0;
	s->char_buf_len = 0;
}

static void free_attrs(state *s) {
	if (!s->attrs)
		return;

	char **attr = s->attrs;
	while (*attr) {
		free(*attr);
		++attr;
	}

	free(s->attrs);
	s->attrs = NULL;
}

static void copy_attrs(state *s, char **attrs) {
	int count = 0;
	int i;

	char **attr = attrs;
	while (*attr) {
		++count;
		++attr;
	}

	free_attrs(s);
	s->attrs = malloc((count + 1) * sizeof(char *));

	for (i = 0; i < count; ++i) {
		s->attrs[i] = malloc(strlen(attrs[i]) + 1);
		strcpy(s->attrs[i], attrs[i]);
	}

	s->attrs[count] = NULL;
}

static void character_data(void *userData, const XML_Char *text, int len) {
	state *s = (state *)userData;

	if (s->depth == s->children_depth + 1) {
		s->char_buf_len += len;
		s->char_buf = realloc(s->char_buf, s->char_buf_len + 1);
		strncat(s->char_buf, (char *)text, len);
	}
}

static void start_element(void *userData, const XML_Char *name, const XML_Char **atts) {
	state *s = (state *)userData;

	/* If we need to return elements at this depth, we prepare the character
	** buffer and copy the attributes.
	*/
	if (s->depth == s->children_depth) {
		init_char_buf(s);
		copy_attrs(s, (char **)atts);
	}
	++s->depth;
	/* Check if this is a parent element we were searching for */
	if (!strcmp(name, s->parent_element) && s->children_depth == -1) {
		if (s->parent_open_cb)
			s->parent_open_cb(s->cb_user_data);
		s->children_depth = s->depth;
	}
}

static void end_element(void *userData, const XML_Char *name) {
	state *s = (state *)userData;

	--s->depth;
	/* If we need to return elements at this depth, do so */
	if (s->depth == s->children_depth && s->element_cb)
		s->element_cb(s->cb_user_data, name, s->attrs, s->char_buf);
	/* Check if this closes a parent element we were searching for */
	if (!strcmp(name, s->parent_element) && (s->depth == s->children_depth - 1)) {
		if (s->parent_close_cb)
			s->parent_close_cb(s->cb_user_data);
		s->children_depth = -1;
	}
}

int xml_parse(const char *filename, const char *parent_element, xml_element_cb element_cb, xml_parent_cb parent_open_cb,
			  xml_parent_cb parent_close_cb, void *cb_user_data) {
	FILE *f;
	state s;

	f = fopen(filename, "r");

	if (!f) {
		DBG_WARN("Failed to open '%s'", filename);
		return -1;
	}

	memset(&s, 0, sizeof(s));
	init_char_buf(&s);
	s.parser = XML_ParserCreate(NULL);
	XML_SetElementHandler(s.parser, start_element, end_element);
	XML_SetCharacterDataHandler(s.parser, character_data);
	s.parent_element = parent_element;
	s.children_depth = -1;
	s.element_cb = element_cb;
	s.parent_open_cb = parent_open_cb;
	s.parent_close_cb = parent_close_cb;
	s.cb_user_data = cb_user_data;
	XML_SetUserData(s.parser, &s);

	for (;;) {
		size_t bytes_read;
		void *buff = XML_GetBuffer(s.parser, BUFF_SIZE);

		if (buff == NULL) {
			DBG_ERROR("Failed to allocate XML buffer");
			XML_ParserFree(s.parser);
			fclose(f);
			free_char_buf(&s);
			return -1;
		}

		errno = 0;
		while ((bytes_read = fread(buff, 1, BUFF_SIZE, f)) == 0 && ferror(f)) {
			if (errno != EINTR) {
				DBG_ERROR("Failed to read from XML file '%s'", filename);
				XML_ParserFree(s.parser);
				fclose(f);
				free_char_buf(&s);
				return -1;
			}
			errno = 0;
			clearerr(f);
		}

		if (!XML_ParseBuffer(s.parser, bytes_read, bytes_read == 0)) {
			DBG_ERROR("Failed to parse XML file '%s'", filename);
			XML_ParserFree(s.parser);
			fclose(f);
			free_char_buf(&s);
			return -1;
		}

		if (bytes_read == 0)
			break;
	}

	XML_ParserFree(s.parser);
	fclose(f);
	free_char_buf(&s);
	return 0;
}
