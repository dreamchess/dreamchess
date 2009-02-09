require 'config'

-- Create the window.
s = engine.screen(640,480);

-- Create keyboard object...
k = engine.keyboard();
t = engine.texture("boards/classic/board.png");
m = engine.mesh("boards/classic/board.dcm");

repeat
s:pre_render();
m:render(t);
s:post_render();
until (k:is_pressed(27))