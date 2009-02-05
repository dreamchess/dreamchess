require 'class'
require 'log'

ConfigManager = class(
	function(o)
		o.options = {}
	end
)

function ConfigManager:AddOption(name, values)
	log:warning("testing the error logger with a warning")
	--log:error("testing the error logger with an error")
	self.options[name] = values
end

ConfigOption = class(
	function(o, name)
		o.name = name
	end
)

ConfigOptionInt = class(ConfigOption,
	function(o, name, value)
		ConfigOption.init(o, name)
		o.value = value
	end
)

function ConfigOptionInt:Set(value)
end

xml = engine.XMLReader()
xml:LoadXMLFile("options.xml")
while true
do
	option = xml:ReadNextOption()
	if option.name == nil then break end
	print("Option", option.name, ":", option.value)
end

cm = ConfigManager()
cm:AddOption("sound volume", {1, 2, 3})
for i, v in pairs(cm.options) do print(i, v) end

