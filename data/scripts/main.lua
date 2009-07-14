package.preload['lpeg'] = luaopen_lpeg

for i, v in pairs(package.preload) do
	print(i .. ' = ' .. tostring(v))
end

require 'init'
require 'title'
require 'game'

