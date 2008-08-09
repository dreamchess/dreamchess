
#include "chess_board.h"

chess_board::chess_board(std::string msh2, std::string tx2, scene *parent)
{
    mdl = new model(msh2,tx2,parent);
    bx = new box(8,8,tx2,parent);
    parent_scene=parent;
}

void chess_board::render()
{
    mdl->render();
    bx->render();
}

void chess_board::update()
{
    mdl->update();
    bx->update();
}
