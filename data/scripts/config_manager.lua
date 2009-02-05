xml = engine.XMLReader()
xml:LoadXMLFile("options.xml")
while true
do
	option = xml:ReadNextOption()
	if option.name == nil then break end
	print("Option", option.name, ":", option.value)
end

