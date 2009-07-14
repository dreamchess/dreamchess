require 'entity'
require 'scene'
require 'board'
require 'san'

require 'chess_board'

-- Create the window...
s = engine.screen(640,480);

-- Create keyboard object...
k = engine.keyboard();

-- Load textures...
board_texture = engine.texture("boards/classic/board.png");
white_texture = engine.texture("pieces/classic/white.png");
black_texture = engine.texture("pieces/classic/black.png");

-- Load meshes...
board_mesh = engine.dcm("boards/classic/board.dcm");
queen_mesh = engine.dcm("pieces/classic/queen.dcm");
bishop_mesh = engine.dcm("pieces/classic/bishop.dcm");
rook_mesh = engine.dcm("pieces/classic/rook.dcm");
knight_mesh = engine.dcm("pieces/classic/knight.dcm");
pawn_mesh = engine.dcm("pieces/classic/pawn.dcm");
king_mesh = engine.dcm("pieces/classic/king.dcm");

title_scene = scene();

-- Add the board...
board_entity = chess_board();
board_entity:setres(board_mesh, board_texture);
board_entity:setpos(0, 0, 0);
title_scene:add("Board", board_entity);

-- Add white pieces...
rook_entity = model();
rook_entity:setres(rook_mesh, white_texture);
rook_entity:setpos(3.5, -3.5, 0);
title_scene:add("Rook", rook_entity);

rook_entity = model();
rook_entity:setres(rook_mesh, white_texture);
rook_entity:setpos(-3.5, -3.5, 0);
title_scene:add("Rook", rook_entity);

knight_entity = model();
knight_entity:setres(knight_mesh, white_texture);
knight_entity:setpos(2.5, -3.5, 0);
knight_entity:setrot(0, 0, 90);
title_scene:add("Knight", knight_entity);

knight_entity = model();
knight_entity:setres(knight_mesh, white_texture);
knight_entity:setpos(-2.5, -3.5, 0);
knight_entity:setrot(0, 0, 90)
title_scene:add("Knight", knight_entity);

bishop_entity = model();
bishop_entity:setres(bishop_mesh, white_texture);
bishop_entity:setpos(1.5, -3.5, 0);
bishop_entity:setrot(0, 0, 0);
title_scene:add("Bishop", bishop_entity);

bishop_entity = model();
bishop_entity:setres(bishop_mesh, white_texture);
bishop_entity:setpos(-1.5, -3.5, 0);
bishop_entity:setrot(0, 0, 0)
title_scene:add("Bishop", bishop_entity);

king_entity = model();
king_entity:setres(king_mesh, white_texture);
king_entity:setpos(0.5, -3.5, 0);
king_entity:setrot(0, 0, 0);
title_scene:add("King", king_entity);

queen_entity = model();
queen_entity:setres(queen_mesh, white_texture);
queen_entity:setpos(-0.5, -3.5, 0);
queen_entity:setrot(0, 0, 0)
title_scene:add("Queen", queen_entity);

i=0
repeat
    pawn_entity = model();
    pawn_entity:setres(pawn_mesh, white_texture);
    pawn_entity:setpos(-3.5+i, -2.5, 0);
    pawn_entity:setrot(0, 0, 0)
    title_scene:add("Pawn", pawn_entity);
    i=i+1;  
until (i > 7)

-- Add black pieces...
rook_entity = model();
rook_entity:setres(rook_mesh, black_texture);
rook_entity:setpos(3.5, 3.5, 0);
title_scene:add("Rook", rook_entity);

rook_entity = model();
rook_entity:setres(rook_mesh, black_texture);
rook_entity:setpos(-3.5, 3.5, 0);
title_scene:add("Rook", rook_entity);

knight_entity = model();
knight_entity:setres(knight_mesh, black_texture);
knight_entity:setpos(2.5, 3.5, 0);
knight_entity:setrot(0, 0, 90);
title_scene:add("Knight", knight_entity);

knight_entity = model();
knight_entity:setres(knight_mesh, black_texture);
knight_entity:setpos(-2.5, 3.5, 0);
knight_entity:setrot(0, 0, 90)
title_scene:add("Knight", knight_entity);

bishop_entity = model();
bishop_entity:setres(bishop_mesh, black_texture);
bishop_entity:setpos(1.5, 3.5, 0);
bishop_entity:setrot(0, 0, 0);
title_scene:add("Bishop", bishop_entity);

bishop_entity = model();
bishop_entity:setres(bishop_mesh, black_texture);
bishop_entity:setpos(-1.5, 3.5, 0);
bishop_entity:setrot(0, 0, 0)
title_scene:add("Bishop", bishop_entity);

king_entity = model();
king_entity:setres(king_mesh, black_texture);
king_entity:setpos(0.5, 3.5, 0);
king_entity:setrot(0, 0, 0);
title_scene:add("King", king_entity);

queen_entity = model();
queen_entity:setres(queen_mesh, black_texture);
queen_entity:setpos(-0.5, 3.5, 0);
queen_entity:setrot(0, 0, 0)
title_scene:add("Queen", queen_entity);

i=0
repeat
    pawn_entity = model();
    pawn_entity:setres(pawn_mesh, black_texture);
    pawn_entity:setpos(-3.5+i, 2.5, 0);
    pawn_entity:setrot(0, 0, 0)
    title_scene:add("Pawn", pawn_entity);
    i=i+1;  
until (i > 7)

cam = camera();
cam:setpos(0, -3, 15);
cam:setrot(-10, 0, 0);
title_scene:setcam(cam);

light1 = light();
light1:setcolour(1, 1, 1);
title_scene:add("Light", light1);

-- Enter loop...
repeat
    title_scene:render();
until (k:is_pressed(27))















