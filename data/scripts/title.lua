require 'entity'
require 'board'
require 'san'

-- Create the window...
s = engine.screen(640,480);

-- Create keyboard object...
k = engine.keyboard();

-- Load textures...
board_texture = engine.texture("boards/classic/board.png");
white_texture = engine.texture("pieces/classic/white.png");
black_texture = engine.texture("pieces/classic/black.png");

-- Load meshes...
board_mesh = engine.mesh("boards/classic/board.dcm");
queen_mesh = engine.mesh("pieces/classic/queen.dcm");
bishop_mesh = engine.mesh("pieces/classic/bishop.dcm");
rook_mesh = engine.mesh("pieces/classic/rook.dcm");
king_mesh = engine.mesh("pieces/classic/king.dcm");

-- Entities...
board_entity = model();
board_entity:setres(board_mesh, board_texture)
board_entity:setpos(0, 0, 0);

queen_entity = model();
queen_entity:setres(queen_mesh, white_texture);
queen_entity:setpos(-0.5, 2.5, 0);

bishop_entity = model();
bishop_entity:setres(bishop_mesh, white_texture);
bishop_entity:setpos(-0.5, 1.5, 0);
bishop_entity:setrot(0, 0, 90);

rook_entity = model();
rook_entity:setres(rook_mesh, white_texture);
rook_entity:setpos(3.5, -0.5, 0);

king_entity = model();
king_entity:setres(king_mesh, black_texture);
king_entity:setpos(2.5, 3.5, 0.35); 
king_entity:setrot(96, 20, 23);

camera = entity()
camera:setpos(5.519, -0.86, 1.09);
camera:setrot(-93, -1, -59.286);

-- Enter loop...
repeat
s:start_frame();

-- Position camera.
s:rotate(camera.xrot,camera.yrot,camera.zrot);
s:move(-camera.x,-camera.y,-camera.z);

-- Render models.
board_entity:render();
queen_entity:render();
bishop_entity:render();
rook_entity:render();
king_entity:render();

s:end_frame();
until (k:is_pressed(27))


























