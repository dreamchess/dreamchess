require 'class'

-- Base scene class.
scene = class(function(s) s.entities={} s.lights={} end);

function scene:add(name,e)
    -- TODO: Check if the name exists... if so, append a number... 
    self.entities[name]=e;
    
    -- Check if it's a light, if it is, also add to light list...
    if e:is_a(light) then
        self.lights[name]=e;
    end 
end

function scene:setcam(c)
scene.active_cam=c;
end

function scene:render()
    s:start_frame();
    
    -- Look for lights...
    for i, v in pairs(self.lights) do
        -- Found a light... fix to allow more... 
        s:setlight( v.x, v.y, v.z )
    end

    self.active_cam:position();

    -- Render the entities...
    for i, v in pairs(self.entities) do 
        if ( v:is_a(model) ) then
            v:render();
        end
    end

    s:end_frame();
end
