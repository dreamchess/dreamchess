require 'class'

-- Base scene class.
scene = class(function(s) s.entities={} s.lights={} end);

function scene:add(name,e)
    self.entities[name]=e;
    
    -- Check if it's a light, if it is, also add to light list...
    if e:is_a(light) then
        self.lights[name]=e;
    end
end

function scene:render()
    s:start_frame();
    
    -- Look for lights...
    for i, v in pairs(self.lights) do 
        --print("Found a light!\n") 
    end
    --[[
    -- Look for the camera...
    cam:position();

    -- Render the entities...
    board_entity:render();
    queen_entity:render();
    bishop_entity:render();
    rook_entity:render();
    king_entity:render();]]--

    s:end_frame();
end