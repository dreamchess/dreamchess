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

function ConfigManager:Set(name, value)
	local t = type(self[name])
	if t == 'table' then
		if type(value) == "number" then
			if value < 1 or value > #self[name] then
				log:warning('index out of bounds when setting \'' .. name ..
					'\'; need [1..' .. #self[name] .. '] got ' .. value) 
			else
				self[name].index = value
			end
		else
			for i, v in ipairs(self[name]) do
				if v == value then
					self[name].index = i
					return
				end
			end
			log:warning('unknown option \'' .. value .. '\' when setting \'' .. name .. '\'') 
		end
	else
		self[name] = value
	end
end

function ConfigManager:Save(filename)
	local file, err = io.open(filename, "w+")

	if not file then
		log:warning(err)
		return
	end

	for i, v in pairs(self) do
		if type(v) == "table" then
			file:write(i .. '=' .. self[i][v.index] .. '\n')
		else
			file:write(i .. '=' .. self[i] .. '\n')
		end
	end

	file:close()
end

function ConfigManager:Load(filename)
	local file, err = io.open(filename, "r")

	if not file then
		log:warning(err)
		return
	end

	local line = file:read()
	while line do
		local name, value = string.match(line, '^(.-)=(.*)$')
		if not name then
			log:warning('failed to parse \'' .. line .. '\'')
		else
			self:Set(name, value)
		end
		line = file:read()
	end

	file:close()
end

