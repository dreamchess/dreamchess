
#include <string>
#include "model.h"
#include "box.h"

class chess_board: public entity
{
    public:
        chess_board(char *msh2, char *tx2);
        void render();
        void update();
    private:
        model *mdl;
        box *bx;
};

