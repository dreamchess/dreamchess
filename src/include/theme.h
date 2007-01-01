
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

typedef TAILQ_HEAD(, music_pack) music_packs_t;

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

typedef struct music_pack
{
    char *dir;
    TAILQ_ENTRY(music_pack) entries;
} music_pack_t;

void theme_read_theme_dir( char *datadir );
void theme_find_music_packs();
music_packs_t *theme_get_music_packs();
