require 'entity'

-- Chess piece entity. Mesh+Quad+Texture.
chess_piece = class(entity);
function chess_piece:setres(mesh,texture)
	self.mesh=mesh;
	self.texture=texture;
    self.active=false;
end

function chess_piece:render()
	s:push();
	s:move(self.x, self.y, self.z);

    if self.active then
        self:setcolour(1.0,0.0,0.0);
    else
        self:setcolour(1,1,1);
    end
  
	s:rotate(self.xrot, self.yrot, self.zrot);  
    s:colour(self.r, self.g, self.b, self.a );  

    s:scale(self.xsize, self.ysize, self.zsize);
	self.mesh:render(self.texture);
	s:pop();
end

function chess_piece:update()
    m:update_mouse();
    mouse_pos=m:position();

    if ( self:collision_at( mouse_pos.x, mouse_pos.y, mouse_pos.z ) ) then
        if m:is_waiting(1) == false then
            if m:is_pressed(1) then
                -- Go through all the pieces, set all to inactive, then set this one as active..
                for i, v in pairs(game_scene.entities) do 
                    if v:is_a(chess_piece) then
                        v.active=false;
                    end
                end
                self.active=true;
                self.glow=true;
            end
        end
    end
end

