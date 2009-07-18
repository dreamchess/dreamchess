require 'class'

-- Base entity class.
entity = class( function(e)	e.x=0 e.y=0 e.z=0 
    e.xrot=0 e.yrot=0 e.zrot=0
    e.xsize=1 e.ysize=1 e.zsize=1 e.r=1 e.g=1 e.b=1 e.a=1 e.glow=false end );

function entity:setpos(x, y, z)
	self.x=x;
	self.y=y;
	self.z=z;	
end

function entity:setsize(x, y, z)
    self.xsize=x;
    self.ysize=y;
    self.zsize=z;
end

function entity:setrot(x, y, z)
	self.xrot=x;
	self.yrot=y;
	self.zrot=z;	
end

function entity:setcolour(r, g, b)
    self.r=r;
    self.g=g;
    self.b=b;
end

function entity:setalpha(a)
    self.a=a;
end

function entity:collision_at( x, y, z )
    retval=false;

    if ( x > (self.x-(self.xsize/2)) and x < (self.x+(self.xsize/2)) ) then
        if ( y > (self.y-(self.ysize/2)) and y < (self.y+(self.ysize/2)) ) then
            --if ( z > (self.z-(self.zsize/2)) and z < (self.z+(self.zsize/2)) ) then
                retval=true;
            --end
        end
    end

    return retval;
end

-- Camera entity.
camera = class(entity);
function camera:position()
    s:move(-self.x,-self.y,-self.z);
    s:rotate(self.xrot,self.yrot,self.zrot);
end

-- Light entity.
light = class(entity);
function light:setcolour(r, g, b)
    self.r=r;
    self.g=g;
    self.b=b;
end

function light:render()
end

function light:update()
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

function model:update()
end
