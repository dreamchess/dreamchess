
#include "dreamchess.h"
#include "theme.h"
#include "mxml.h"
#include "debug.h"
#include <dirent.h>

TAILQ_HEAD(, theme_struct) themes;
int theme_count=0;

void theme_printf_theme_list();

void theme_load_opaque(mxml_node_t *top, char *name, char **dest)
{
    mxml_node_t *node = mxmlFindElement(top, top, name, NULL, NULL, MXML_DESCEND);
    if (node)
    {
        node = mxmlWalkNext(node, node, MXML_DESCEND);
        if (node && node->type == MXML_OPAQUE)
            *dest=strdup(node->value.opaque);
    }
}

void theme_add_theme( char *xmlfile )
{
    // Read in the theme XML...
    FILE *fp;
    mxml_node_t *tree=NULL, *theme=NULL;
    char *lighting, *tex_spin, *text_spin_speed;

    struct theme_struct *temp_theme;

    fp = fopen(xmlfile, "r");
    if (fp)
        tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
    else
        DBG_ERROR("error opening theme file" );

    fclose(fp);

    theme = tree;

    DBG_LOG("loading %s", xmlfile );
    while ((theme = mxmlFindElement(theme, tree, "theme", NULL, NULL, MXML_DESCEND)))
    {
        mxml_node_t *node;

        temp_theme = malloc(sizeof(*temp_theme));

        temp_theme->name=strdup("Untitled");
        temp_theme->style=strdup("default");
        temp_theme->pieces=strdup("classiclow");
        temp_theme->board=strdup("classic");
        temp_theme->white_name=strdup("White");
        temp_theme->black_name=strdup("Black");
        temp_theme->lighting=TRUE;
        temp_theme->piece_tex_spin=FALSE;
        temp_theme->piece_tex_spin_speed=0;

        theme_load_opaque(theme, "name", &temp_theme->name);
        theme_load_opaque(theme, "style", &temp_theme->style);
        theme_load_opaque(theme, "pieces", &temp_theme->pieces);
        theme_load_opaque(theme, "board", &temp_theme->board);
        theme_load_opaque(theme, "white_name", &temp_theme->white_name);
        theme_load_opaque(theme, "black_name", &temp_theme->black_name);

      /*  theme_load_opaque(theme, "lighting", &temp_theme->black_name);
        theme_load_opaque(theme, "tex_spin", &temp_theme->black_name);
        theme_load_opaque(theme, "text_spin_speed", &temp_theme->black_name);

      /*  node = mxmlFindElement(theme, theme, "lighting", NULL, NULL, MXML_DESCEND);
        if (node)
        {
            node = mxmlWalkNext(node, node, MXML_DESCEND);

            if (node && node->type == MXML_OPAQUE)
                if ( !strcmp( node->value.opaque, "off" ) )
                    themes[theme_count].lighting=FALSE;
        }

        node = mxmlFindElement(theme, theme, "tex_spin", NULL, NULL, MXML_DESCEND);
        if (node)
        {
            node = mxmlWalkNext(node, node, MXML_DESCEND);

            themes[theme_count].piece_tex_spin=TRUE;
            themes[theme_count].piece_tex_spin_speed=atoi(node->value.opaque);
            /* printf( "Speed: %i\n", themes[theme_count].piece_tex_spin_speed );
            if (node && node->type == MXML_OPAQUE)
                if ( !strcmp( node->value.opaque, "off" ) )
                    themes[theme_count].lighting=FALSE; */
        //}
        //DBG_LOG("added theme: %s", themes[theme_count].name );

        TAILQ_INSERT_TAIL(&themes, temp_theme, entries);

        theme_count++;
    }

    mxmlDelete(tree);
}

void theme_read_theme_dir( char *datadir )
{
    struct dirent* themedir_entry;
    DIR* themedir;
    
    char temp[80];

    theme_count=0;

    TAILQ_INIT(&themes);

    if ( (themedir=opendir("themes")) != NULL )
    {
        while ((themedir_entry = readdir(themedir)) != NULL)
        {
            if ( themedir_entry->d_name[0] != '.' )
            {
                sprintf( temp, "themes/%s", themedir_entry->d_name );
                theme_add_theme( temp );
            }
        }
        closedir(themedir);
    }

    theme_printf_theme_list();
}

void theme_printf_theme_list()
{
    struct theme_struct *item;
    item = malloc(sizeof(*item));

    TAILQ_FOREACH(item, &themes, entries) 
    {
        printf("*Name:%s\n", item->name);
        printf("---Style:%s\n", item->style);
        printf("---Pieces:%s\n", item->pieces);
        printf("---Board:%s\n", item->board);
        printf("---White:%s\n", item->white_name);
        printf("---Black:%s\n", item->black_name);
        printf("\n");
    }
}


