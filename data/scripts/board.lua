require 'class'
require 'move'

Board = class(
	function(o)
		-- Setup the board
		o[1] = 'R'
		o[2] = 'N'
		o[3] = 'B'
		o[4] = 'Q'
		o[5] = 'K'
		o[6] = 'B'
		o[7] = 'N'
		o[8] = 'R'
		o[57] = 'r'
		o[58] = 'n'
		o[59] = 'b'
		o[60] = 'q'
		o[61] = 'k'
		o[62] = 'b'
		o[63] = 'n'
		o[64] = 'r'
		for i = 9, 16 do
			o[i] = 'P'
		end
		for i = 49, 56 do
			o[i] = 'p'
		end

		-- Clear captured pieces
		o.captured = {}
		for i, v in ipairs({'Q', 'R', 'B', 'N', 'P', 'q', 'r', 'b', 'n', 'p'}) do
			o.captured[v] = 0
		end

		-- Set turn to white
		o.turn = 'w'
	end
)

function Board:Square(file, rank)
	return (rank - 1) * 8 + file:byte(1) - string.byte('a') + 1
end

function Board:PieceColour(piece)
	if piece:match('[KQRBNP]') then
		return 'w'
	else
		return 'b'
	end
end

function Board:PieceType(piece)
	return piece:upper()
end

function Board:MoveRayIsOK(move)
	local step

	if move:Vert() ~= 0 then
		step = (move.to - move.from) / move:Vert()
	else
		step = (move.to - move.from) / move:Hor()
	end

	for i = move.from + step, move.to - step, step do
		if self[i] then
			return false
		end
	end

	if self[move.to] and self:PieceColour(self[move.to]) == self:PieceColour(self[move.from]) then
		return false
	end

	return true
end

function Board:MoveIsSemiValid(move)
	-- check that from and to are different and that piece exists on from square
	if move.from == move.to 
	or not self[move.from] then
		return false
	end

	local piece = self:PieceType(self[move.from])
	local colour = self:PieceColour(self[move.from])

	-- horizontal and vertical distance of move
	local hor = move:Hor()
	local vert = move:Vert()

	-- check that piece matches colour of current turn
	if colour ~= self.turn then
		return false
	end

	if piece == 'P' and (move.to < 9 or move.to > 56) then
		-- check that pawn moves to ranks 1 and 8 have a promotion piece of player's colour specified
		if  not move.promotion or self:PieceColour(move.promotion) ~= self.turn then
			return false
		end
	else
		-- check that other moves have no promotion piece specified
		if move.promotion then
			return false
		end
	end

	if piece == 'N' then
		-- check knight moves
		if (hor ~= 1 and hor ~= 2)
		or (hor == 1 and vert ~= 2)
		or (hor == 2 and vert ~= 1)
		or (self[move.to] and self:PieceColor(self[move.to]) == self.turn) then
			return false
		end
	elseif piece == 'B' then
		if (hor ~= vert)
		or not self:MoveRayIsOK(move) then
			return false
		end
	end

	return true
end

function Board:__tostring()
	local s = ''

	for row = 7, 0, -1 do
		for column = 1, 8 do
			s = s .. (self[row * 8 + column] or ' ')
		end
		s = s .. '\n'
	end

	return s
end

board = Board()
print(board)

move = Move()
move:FromFAN('e2e4')
print(board:MoveIsSemiValid(move))

