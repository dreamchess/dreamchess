
-- Create the window.
s = engine.screen(640,480)

-- Create input list.
p = engine.keyboard_event("QUIT", "INPUT_EVENT", 27, false)

while (true) do
p:update()

 print("Active...",p:is_active())
 if (p:is_active()) then
 break
 end 

end
