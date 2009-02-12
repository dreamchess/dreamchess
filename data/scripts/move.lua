require 'class'

Move = class(
	function(o, str)
	end
)

function Move:FromFAN(fan)
	ff, fr, tf, tr, prom = string.match(fan, '^([a-h])([1-8])([a-h])([1-8])([qrbn]?)$')
	if ff then
		self.from = (fr - 1) * 8 + ff:byte(1) - string.byte('a') + 1
		self.to = (tr - 1) * 8 + tf:byte(1) - string.byte('a') + 1
		self.prom = prom
	end
end

function Move:ToFAN()
	return string.char((self.from - 1) % 8 + string.byte('a'))
		.. math.floor((self.from - 1) / 8) + 1
		.. string.char((self.to - 1) % 8 + string.byte('a'))
		.. math.floor((self.to - 1) / 8) + 1
		.. self.prom or ''
end

function Move:Hor()
	return math.abs((self.to - 1) % 8 - (self.from - 1) % 8)
end

function Move:Vert()
	return math.abs(math.floor((move.to - 1) / 8) - math.floor((move.from - 1) / 8))
end

function Move:__tostring()
	return self:ToFAN()
end

