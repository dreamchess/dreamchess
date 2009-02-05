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

#ifndef XML_H
#define XML_H

#include <mxml.h>

struct XMLOption
{
	char *name;
	char *value;
};

class XML {
	public:
		mxml_node_t *tree;
		XML() { tree = NULL; }
		~XML() { if (tree) mxmlDelete(tree); }
};

class XMLReader: private XML {
	private:
		mxml_node_t *walk;
	public:
		XMLReader() { walk = NULL; }
		int LoadXMLFile(const char *filename);
		XMLOption ReadNextOption();
};
#if 0
class XMLWriter: private XML {
	public:
		int SaveXMLFile(const char *filename);
		void WriteOption(char *option, char *value);
};
#endif
#endif

