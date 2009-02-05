require 'class'

ConfigManager = class(
	function(c)
		c.options = {}
	end
)

function ConfigManager:AddOption(name, values)
	self.options[name] = values
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

