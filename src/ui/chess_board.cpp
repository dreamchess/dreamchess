
#include "chess_board.h"

chess_board::chess_board(std::string name2, std::string type2, std::string msh2, std::string tx2, scene *parent)
{
    mdl = new model(name2,msh2,tx2,parent);
    bx = new box(name2,8,8,tx2,parent);
    parent_scene=parent;
    name=name2;
    type=type2;
}

void chess_board::generate_bbox()
{
	
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
