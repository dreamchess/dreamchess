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

#include "xml.h"
extern "C" {
#include "debug.h"
}

static mxml_node_t *find_next_element(mxml_node_t *node, mxml_node_t *top)
{
	do {
		node = mxmlWalkNext(node, top, MXML_DESCEND);
	} while (node && node->type != MXML_ELEMENT);

	return node;
}

int XMLReader::LoadXMLFile(const char *filename)
{
	FILE *f = fopen(filename, "r");

	if (f)
		tree = mxmlLoadFile(NULL, f, MXML_OPAQUE_CALLBACK);
	else {
		DBG_WARN("failed to open '%s'", filename);
		return -1;
	}

	fclose(f);

	if (!tree) {
		DBG_ERROR("failed to parse '%s'", filename);
		return -1;
	}

	walk = mxmlFindElement(tree, tree, "options", NULL, NULL, MXML_DESCEND);

	if (walk)
		walk = find_next_element(walk, tree);
}

XMLOption XMLReader::ReadNextOption()
{
	XMLOption retval = {NULL, NULL};

	if (!tree || !walk)
		return retval;

	// Go to content
	walk = mxmlWalkNext(walk, tree, MXML_DESCEND);

	if (!walk)
		return retval;

	retval.name = walk->parent->value.opaque;
	retval.value = walk->value.opaque;

	// Go to next element
	walk = find_next_element(walk, tree);

	return retval;
}

