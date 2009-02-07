require 'class'
require 'log'

ConfigManager = class()

function ConfigManager:Add(name, options, default)
	if type(options) == "table" then
		default = default or options[1]
		self[name] = options
		self:Set(name, default)
	else
		self[name] = options
	end
end

function ConfigManager:SetOption(name, index)
	if index < 1 or index > #self[name] then
		log:warning('index out of bounds when setting \'' .. name ..
			'\'; need [1..' .. #self[name] .. '] got ' .. index) 
	else
		self[name].index = index
	end
end

function ConfigManager:Set(name, value)
	local t = type(self[name])
	if t == 'table' then
		for i, v in ipairs(self[name]) do
			if v == value then
				self[name].index = i
				return
			end
		end
		log:warning('unknown option \'' .. value .. '\' when setting \'' .. name .. '\'') 
	else
		if type(value) ~= t then
			log:warning('type mismatch when setting \'' .. name .. '\'; need ' .. t .. ' got ' .. type(value))
		else
			self[name] = value
		end
	end
end

function ConfigManager:Save(filename)
	local file, err = io.open(filename, "w+")

	if not file then
		log:warning(err)
	end

	for i, v in pairs(self) do
		if type(v) == "table" then
			file:write(i .. ' = ' .. self[i][v.index] .. '\n')
		else
			file:write(i .. ' = ' .. self[i] .. '\n')
		end
	end

	file:close()
end

