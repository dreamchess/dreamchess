require 'entity'

-- Chess piece entity. Mesh+Quad+Texture.
chess_piece = class(entity);
function chess_piece:setres(mesh,texture)
	self.mesh=mesh;
	self.texture=texture;
    self.selected=false;
end

function chess_piece:render()
	s:push();
	s:move(self.x, self.y, self.z);
	s:rotate(self.xrot, self.yrot, self.zrot);

    if self.active then
        self:setcolour(0,0,1);
    else
        self:setcolour(1,1,1);
    end
    
    s:colour(self.r, self.g, self.b, self.a );  

	self.mesh:render(self.texture);
	s:pop();
end

function chess_piece:update()
    mouse_pos=m:position();

    --if ( self:collision_at( mouse_pos.x, mouse_pos.y, mouse_pos.z ) == false ) then
    --    if m:is_pressed(1,false) then
    --        self.active=false;
    --    end;
    --    return;
    --end

    if m:is_pressed(1) then
        if ( self:collision_at( mouse_pos.x, mouse_pos.y, mouse_pos.z ) ) then
            self:setcolour(1.0, 0.0, 0.0);
            self.active=true;
        end
    else
        self:setcolour(1.0, 1.0, 1.0);
    end
end

