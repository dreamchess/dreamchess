require 'class'

-- Base scene class.
scene = class(function(s) s.entities={} s.lights={} end);

function scene:add(name,e)
    -- Check if the name already exists... if so, append a number... 
    if self.entities[name] then -- If the name exists...
        --Find highest count...
        count=1; done=false;
        repeat 
            if self.entities[name..tostring(count)] then
                count=count+1;
            else
                done=true;
            end
        until (done == true)            
        -- Highest count found...
        
        newname=name..tostring(count) -- Append count to the end of the orignal name.
        self.entities[newname]=e; -- Add the entity under the new name.
    else -- No instance of the name exists, so add the name as is.
        newname=name; 
        self.entities[newname]=e;
    end

    -- Check if it's a light, if it is, also add to light list...
    if e:is_a(light) then
        self.lights[name]=e;
    end 

    --print("Added new entity: "..newname) 
end

function scene:setcam(c)
scene.active_cam=c;
end

function scene:update()
    -- Update the entities...
    for i, v in pairs(self.entities) do 
            v:update();
   end
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
    --    if v:render() then
            v:render();
    --    end
    end

    m:update_mouse3d();

    s:end_frame();
end
