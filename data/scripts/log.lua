require 'class'

Log = class()

function Log:error(e)
	error(e, 2)
end

function Log:warning(e)
	local info = debug.getinfo(2)

	print(string.sub(info.source, 2) .. ':' .. info.currentline .. ': warning: ' .. e)
end

log = Log()

