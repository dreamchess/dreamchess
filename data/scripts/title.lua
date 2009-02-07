require 'config'

-- Create the window.
s = engine.screen(640,480);

-- Create keyboard object...
k = engine.keyboard();

while (true) do

 if (k:is_pressed(27)) then
 break
 end 

end
