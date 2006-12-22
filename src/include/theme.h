
#include "gamegui/queue.h"

/* Define our booleans */
#define TRUE  1
#define FALSE 0

struct theme_struct
{
    char *name;
    char *style,*pieces,*board;
    char *white_name, *black_name;

    int lighting;
    int piece_tex_spin, piece_tex_spin_speed;

    TAILQ_ENTRY(theme_struct) entries;
};

typedef struct
{
    char *name;

}theme_style_t;

typedef struct
{
    char *name;

}theme_pieces_t;

typedef struct
{
    char *name;

}theme_board_t;


void theme_read_theme_dir( char *datadir );
