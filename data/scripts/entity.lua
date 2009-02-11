require 'class'

entity = class()

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

function entity:move()
end


