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
#include <sys/stat.h>
#include <time.h>
#include "debug.h"
#include "dir.h"
#include "network.h"
#include <mxml.h>
#include "news.h"

enum news_state {
	NEWS_CACHE,
	NEWS_CONNECT,
	NEWS_GET,
	NEWS_DONE,
};

static net_http_handle *http_handle;
static int news_count;
static news_item *news;
static enum news_state state;

void news_init() {
	http_handle = 0;
	news_count = 0;
	news = NULL;
	state = NEWS_CACHE;
}

static char *get_entry(mxml_node_t *node, const char *name) {
	mxml_node_t *child = mxmlFindElement(node, node, name, NULL, NULL, MXML_DESCEND);
	if (!child)
		return NULL;
	child = mxmlWalkNext(child, child, MXML_DESCEND);
	if (!child || !child->value.opaque)
		return NULL;
	return strdup(child->value.opaque);
}

static void free_news() {
	if (news) {
		int i;
		for (i = 0; i < news_count; i++) {
			if (news[i].title)
				free(news[i].title);
			if (news[i].link)
				free(news[i].link);
		}
		news = NULL;
	}
}

static void parse_rss() {
	ch_userdir();
	FILE *f = fopen("news.rss", "r");
	ch_datadir();

	if (!f)
		return;

	mxml_node_t *tree = mxmlLoadFile(NULL, f, MXML_OPAQUE_CALLBACK);
	fclose(f);

	if (!tree)
		return;

	mxml_node_t *node = mxmlFindElement(tree, tree, "item", NULL, NULL, MXML_DESCEND);

	if (!node)
		goto error;

	news_count = 0;
	while (node) {
		news_count++;
		node = mxmlFindElement(node, tree, "item", NULL, NULL, MXML_NO_DESCEND);
    }

	news = calloc(sizeof(news_item), news_count);

	node = mxmlFindElement(tree, tree, "item", NULL, NULL, MXML_DESCEND);
	int i = 0;
	while (node) {
		if (!(news[i].title = get_entry(node, "title")))
			goto error;
		if (!(news[i++].link = get_entry(node, "link")))
			goto error;
		node = mxmlFindElement(node, tree, "item", NULL, NULL, MXML_NO_DESCEND);
    }

	mxmlDelete(tree);
	return;

error:
	free_news();
	mxmlDelete(tree);
	return;
}

news_item *news_get(int *count) {
	struct stat news_stat;

	switch (state) {
	case NEWS_CACHE:
		// Check the cache file and see if it's recent enough
		ch_userdir();
		if (!stat("news.rss", &news_stat)) {
			time_t age = time(NULL) - news_stat.st_mtime;
			if (age > 3600 * 24) {
				DBG_LOG("news cache file is over a day old, getting new version online");
				state = NEWS_CONNECT;
			} else {
				DBG_LOG("reading news from cache file");
				parse_rss();
				state = NEWS_DONE;
			}
		} else {
			state = NEWS_CONNECT; // No cache file found
		}
		ch_datadir();
		break;
	case NEWS_CONNECT:
		http_handle = net_http_open("news.google.com", "/news?ned=us&topic=h&output=rss");
		if (!http_handle)
			state = NEWS_DONE;
		else
			state = NEWS_GET;
		break;
	case NEWS_GET:
		if (net_http_poll(http_handle)) {
			char *rss = net_http_close(http_handle);
			http_handle = NULL;
			if (rss) {
				ch_userdir();
				FILE *f = fopen("news.rss", "w");
				if (f) {
					fputs(rss, f);
					fclose(f);
					parse_rss();
				}
			}
			state = NEWS_DONE;
		}
	default:
		break;
	}

	switch (state) {
	case NEWS_CACHE:
	case NEWS_CONNECT:
	case NEWS_GET:
		*count = 0; // Still getting news
		break;
	case NEWS_DONE:
		if (news) {
			*count = news_count;
			return news;
		} else {
			*count = -1;
		}
	}
	return NULL;
}

void news_stop() {
	if (state == NEWS_GET) {
		net_http_close(http_handle);
		http_handle = NULL;
		state = NEWS_DONE;
	}
}

void news_exit() {
	if (http_handle)
		net_http_close(http_handle);
	free_news();
}
