require 'config_manager'

config = ConfigManager()

-- todo theme
config:Add('music_volume', {'Off', 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 8)
config:Add('sound_volume', {'Off', 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 8)
config:Add('time_moves', {'Whole game', 10, 20, 30, 40, 'Custom'})
config:Add('custom_time_moves', 40)
config:Add('time_time', {20, 30, 60, 120, 'Custom'})
config:Add('custom_time_time', 60)
config:Add('time_increment', {0, 10, 30, 60, 'Custom'})
config:Add('custom_time_increment', 0)
config:Add('resolution', {'640x480', '800x600', '1024x768', '1280x1024'})
config:Add('custom_resolution_width', 640)
config:Add('custom_resolution_height', 480)
config:Add('multisampling', {'Off', '2x', '4x', '6x', '8x'})
config:Add('first_engine', 'dreamer')
config:Add('game_mode', {'Human vs. CPU', 'CPU vs. Human', 'Human vs. Human'})
config:Add('difficulty', {'Easy', 'Normal'}, 'Normal')
config:Add('level', {1, 2, 3, 4, 5, 6, 7, 8})

config:Save('dreamchess.cfg')

xml = engine.XMLReader()
xml:LoadXMLFile("options.xml")
while true
do
	option = xml:ReadNextOption()
	if option.name == nil then break end
	print("Option", option.name, ":", option.value)
end

