require 'class'

-- Base entity class.
entity = class( function(e)	e.x=0 e.y=0 e.z=0 e.xrot=0 e.yrot=0 e.zrot=0	end );

function entity:setpos(x, y, z)
	self.x=x;
	self.y=y;
	self.z=z;	
end

function entity:setrot(x, y, z)
	self.xrot=x;
	self.yrot=y;
	self.zrot=z;	
end

-- Model entity. Mesh+texture.
model = class(entity);
function model:setres(mesh,texture)
	self.mesh=mesh;
	self.texture=texture;
end

function model:render()
	s:push();
	s:move(self.x, self.y, self.z);
	s:rotate(self.xrot, self.yrot, self.zrot);
	self.mesh:render(self.texture);
	s:pop();
end