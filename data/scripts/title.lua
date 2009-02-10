require 'entity'

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
board_entity = entity;
board_entity.mesh=board_mesh; board_entity.tex=board_texture;
board_entity.x=0; board_entity.y=0; board_entity.z=0;

queen_entity = entity;
queen_entity.mesh=queen_mesh; queen_entity.tex=white_texture;
queen_entity.x=-0.5; queen_entity.y=2.5; queen_entity.z=0;

bishop_entity = entity;
bishop_entity.mesh=bishop_mesh; bishop_entity.tex=white_texture;
bishop_entity.x=-0.5; bishop_entity.y=1.5; bishop_entity.z=0;
bishop_entity.xrot=0; bishop_entity.yrot=0; bishop_entity.zrot=90.0;

rook_entity = entity;
rook_entity.mesh=rook_mesh; rook_entity.tex=white_texture;
rook_entity.x=3.5; rook_entity.y=-0.5; rook_entity.z=0;

king_entity = entity;
king_entity.mesh=king_mesh; king_entity.tex=black_texture;
king_entity.x=2.5; king_entity.y=3.5; king_entity.z=0.35;
king_entity.xrot=96; king_entity.yrot=20; king_entity.zrot=23.0;


-- Enter loop...
repeat
s:start_frame();

s:push();
board_entity.mesh:render(board_entity.tex);
s:pop();

s:push();
s:move(queen_entity.x, queen_entity.y, queen_entity.z);
queen_entity.mesh:render(queen_entity.tex);
s:pop();

s:push();
s:move(bishop_entity.x, bishop_entity.y, bishop_entity.z);
bishop_entity.mesh:render(bishop_entity.tex);
s:pop();

s:push();
s:move(king_entity.x, king_entity.y, king_entity.z);
s:rotate(king_entity.xrot, king_entity.yrot, king_entity.zrot );
king_entity.mesh:render(king_entity.tex);
s:pop();

s:end_frame();
until (k:is_pressed(27))


























