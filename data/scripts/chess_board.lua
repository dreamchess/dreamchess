require 'entity'

-- Chess board entity. Mesh+Quad+Texture.
chess_board = class(entity);
function chess_board:setres(mesh,texture)
	self.mesh=mesh;
	self.texture=texture;
end

function chess_board:render()
	s:push();
	s:move(self.x, self.y, self.z);
	s:rotate(self.xrot, self.yrot, self.zrot);
	self.mesh:render(self.texture);
    s:draw_quad(8,8,self.texture);
	s:pop();
end

function chess_board:update()
    if m:is_pressed(1,true) then
        print("Button 0 pressed!");
        if self:collision_at( 0, 0, 0 ) then

        end
    end
end
