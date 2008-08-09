
#include <string>
#include "model.h"
#include "box.h"

class chess_board: public entity
{
    public:
        chess_board(std::string msh2, std::string tx2, void *parent);
        void render();
        void update();
    private:
        model *mdl;
        box *bx;
};

