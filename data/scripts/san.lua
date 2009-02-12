require 'class'
require 'board'
require 'lpeg'

San = class(
	function(o)
		-- parse backwards to avoid the need for backtracking
		local file = lpeg.R('ah')
		local rank = lpeg.R('18')

		local mark = lpeg.S('+#') / function(m) o.data.mark = m end
		local piece = lpeg.S('KQRBN') / function(p) o.data.piece = p end

		local from_file = file / function(f) o.data.from_file = f end
		local from_rank = file / function(r) o.data.from_rank = r end
		local from = from_rank * from_file + from_rank + from_file

		local to = lpeg.C(rank) * lpeg.C(file) / function(r, f) o.data.to = Board:Square(f, r) o.data.type = "regular" end

		local prom = lpeg.C(lpeg.S('QRBN')) * lpeg.P('=') / function(p) o.data.promotion = p end
		local capt = lpeg.P('x') / function() o.data.type = 'capture' end

		local castle_q = lpeg.P('O-O-O') / function() o.data.type = 'castle_q' end
		local castle_k = lpeg.P('O-O') / function() o.data.type = 'castle_k' end
		local move = prom^-1 * to * capt^-1 * from^-1 * piece^-1

		o.pattern = mark^-1 * (castle_q + castle_k + move) * -1
	end
)

function San:Parse(str)
	str = str:reverse()

	-- clear data 
	self.data = {}

	return lpeg.match(self.pattern, str)
end

san = San()
san:Parse('fxg1=Q+')
for i, v in pairs(san.data) do
	print(i .. ' = ' .. tostring(v))
end

