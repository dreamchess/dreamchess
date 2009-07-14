require 'entity'
require 'scene'
require 'board'
require 'san'

require 'chess_board'

-- Load textures...
board_texture = engine.texture("boards/classic/board.png");
white_texture = engine.texture("pieces/classic/white.png");
black_texture = engine.texture("pieces/classic/black.png");

-- Load meshes...
board_mesh = engine.dcm("boards/classic/board.dcm");
queen_mesh = engine.dcm("pieces/classic/queen.dcm");
bishop_mesh = engine.dcm("pieces/classic/bishop.dcm");
rook_mesh = engine.dcm("pieces/classic/rook.dcm");
king_mesh = engine.dcm("pieces/classic/king.dcm");

title_scene = scene();

-- Add the board...
board_entity = chess_board();
board_entity:setres(board_mesh, board_texture);
board_entity:setpos(0, 0, 0);
title_scene:add("Board", board_entity);

queen_entity = model();
queen_entity:setres(queen_mesh, white_texture);
queen_entity:setpos(-0.5, 2.5, 0);
title_scene:add("Queen", queen_entity);

bishop_entity = model();
bishop_entity:setres(bishop_mesh, white_texture);
bishop_entity:setpos(-0.5, 1.5, 0);
bishop_entity:setrot(0, 0, 90);
title_scene:add("Bishop", bishop_entity);

rook_entity = model();
rook_entity:setres(rook_mesh, white_texture);
rook_entity:setpos(3.5, -0.5, 0);
title_scene:add("Rook", rook_entity);

king_entity = model();
king_entity:setres(king_mesh, black_texture);
king_entity:setpos(2.5, 3.5, 0.35); 
king_entity:setrot(96, 20, 23);
title_scene:add("King", king_entity);

cam = camera();
cam:setpos(5.519, -0.86, 1.09);
cam:setrot(-93, -1, -59.286);
title_scene:setcam(cam);

light1 = light();
light1:setcolour(1, 1, 1);
title_scene:add("Light", light1);

-- Enter loop...
repeat
    title_scene:render();
until (k:is_pressed(27,true))


























