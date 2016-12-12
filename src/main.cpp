
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Audio.hpp"


#include "timing.hpp"
#include "input.hpp"

#include "common.hpp"
#include "asset.hpp"
#include "persist.hpp"

#include "scene.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <time.h>

const sf::VideoMode VIDEO_MODE(SCREEN_W, SCREEN_H);

int main()
{
	
	std::unordered_map<int, std::unordered_map<std::string, bool>> get_command;
	Input input(get_command);
	Timing timing;
	
	Persist persist;
	Asset asset;
	
	srand(time(NULL));
	
	sf::Event event;
	sf::Clock frame_clock;
	sf::Time frame_time = sf::Time::Zero;

	sf::RenderWindow window(VIDEO_MODE, "shrapx ldjam", sf::Style::Close);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
	window.setMouseCursorVisible(false);
	
	input.key_cmd[ input.get_key_id("Escape") ] = "quit"; get_command[0]["quit"] = 0;
	
	input.key_cmd[ input.get_key_id("W") ] = "up"; get_command[0]["up"] = 0;
	input.key_cmd[ input.get_key_id("A") ] = "left"; get_command[0]["left"] = 0;
	input.key_cmd[ input.get_key_id("S") ] = "down"; get_command[0]["down"] = 0;
	input.key_cmd[ input.get_key_id("D") ] = "right"; get_command[0]["right"] = 0;
	
	input.key_cmd[ input.get_key_id("E") ] = "frob"; get_command[0]["frob"] = 0;
	input.key_cmd[ input.get_key_id("MouseLeft") ] = "frob"; get_command[0]["frob"] = 0;
	
	input.key_cmd[ input.get_key_id("F") ] = "flashlight"; get_command[0]["flashlight"] = 0;
	
	
	asset.load_texture("exterior_light_collide", "data/exterior_light_collide.png");
	asset.load_shader("darkness", "data/darkness_v.glsl", "data/darkness_f.glsl");
	asset.load_texture("pointer", "data/pointer.png");
	asset.load_texture("player", "data/player.png");
	asset.load_texture("furnitures", "data/furnitures.png");
	asset.load_texture("lights", "data/lights.png");
	
	/*sf::RenderTexture ui_render;
	sf::Sprite ui_sprite;
	ui_render.create(ZOOM_W, ZOOM_H, false);
	ui_sprite.setTexture(ui_render.getTexture());
	ui_sprite.setScale( float(ZOOM), float(ZOOM) );*/
	
	//asset.load_*
	
	int level = 0;
	
	std::unique_ptr<Scene> scene = std::make_unique<Scene>(input, asset, persist, level);
	
	bool terminate = 0;
	while( terminate == 0 )
	{
		
		if ( timing.update() )
		{
			int next_level = scene->update();
			if (next_level != level)
			{
				level = next_level;
				scene = std::make_unique<Scene>(input, asset, persist, level);
				
				
				scene->update();
			}
		}
		else
		{
			window.draw( scene->get_render() );
			window.display();
		}
		
		terminate = input.events(window) || get_command.at(0).at("quit");
		
	}
	
	scene.reset();
	window.close();
}
