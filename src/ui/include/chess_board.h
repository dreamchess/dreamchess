
#include <string>
#include "model.h"
#include "box.h"

class chess_board: public entity
{
    public:
        chess_board(std::string name2, std::string type2, std::string msh2, std::string tx2, scene *parent);
        void render();
        void update();
        void generate_bbox();
    private:
        model *mdl;
        box *bx;
};

