require 'entity'

-- Chess board entity. Mesh+Quad+Texture.
chess_board = class(entity);
function chess_board:setres(mesh,texture)
	self.mesh=mesh;
	self.texture=texture;
    self.selected=false;
end

function chess_board:render()
	s:push();
	s:move(self.x, self.y, self.z);
	s:rotate(self.xrot, self.yrot, self.zrot);
    s:colour(self.r, self.g, self.b, self.a );
	self.mesh:render(self.texture);
    s:draw_quad(8,8,self.texture);
	s:pop();
end

function chess_board:update()
    --if m:is_pressed(1,true) then
        --print("Button 0 pressed at "..tostring(m:position().x)..tostring(m:position().y)..tostring(m:position().z));
        --if self:collision_at( m:position().x, m:position().y, m:position().z ) then
        --print( "Collision?" );
        --end
    --end
end
