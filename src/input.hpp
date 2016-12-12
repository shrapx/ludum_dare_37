#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

using namespace std;

const array<std::string,107> keys =
	{"A","B","C","D","E","F","G","H","I","J","K","L","M","N",
	"O","P","Q","R","S","T","U","V","W","X","Y","Z","Num0","Num1","Num2",
	"Num3","Num4","Num5","Num6","Num7","Num8","Num9","Escape","LControl",
	"LShift","LAlt","LSystem","RControl","RShift","RAlt","RSystem","Menu",
	"LBracket","RBracket","SemiColon","Comma","Period","Quote","Slash",
	"BackSlash","Tilde","Equal","Dash","Space","Return","BackSpace","Tab",
	"PageUp","PageDown","End","Home","Insert","Delete","Add","Subtract",
	"Multiply","Divide","Left","Right","Up","Down","Numpad0","Numpad1",
	"Numpad2","Numpad3","Numpad4","Numpad5","Numpad6","Numpad7","Numpad8",
	"Numpad9","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11",
	"F12","F13","F14","F15","Pause","MouseLeft","MouseRight",
	"MouseMiddle","MouseXButton1","MouseXButton2","MouseMove"};
	


class Input
{
public:

	Input(unordered_map<int, unordered_map<string, bool>>& cmds) : get_command(cmds)
	{
	}
	
	sf::Vector2i mouse;
	
	int kvm_id = 0;
	unordered_map<int, unordered_map<string, bool>>& get_command;

	unordered_map<unsigned short,string> key_cmd;

	bool get(const string& cmd)
	{
		return get_command[kvm_id][cmd];
	}
	
	unsigned short get_key_id(const std::string& name)
	{	
		for (unsigned short i=0; i<keys.size(); ++i)
		{
			if (name == keys.at(i) )
			{
				return i;
			}
		}
		assert(false);
	}
	
	void key_input(sf::Event& event, bool value)
	{
		auto it = key_cmd.find(event.key.code);
		if ( it != key_cmd.end() )
		{
			const string& cmd = it->second;
			get_command[kvm_id][ cmd ] = value;
		}
	};
	
	void mouse_input(sf::Event& event, bool value)
	{
		uint16_t mbutton =
			sf::Keyboard::KeyCount+event.mouseButton.button;
			
		auto it = key_cmd.find(mbutton);
		if ( it != key_cmd.end() )
		{
			const string& cmd = it->second;
			get_command[kvm_id][ cmd ] = value;
		}
	};

	bool events(sf::Window& window)
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
				{
					return true;
				}
				/*case sf::Event::Resized:
				{
					break;
				}*/
				case sf::Event::KeyPressed:
				{
					key_input(event, true);
					break;
				}
				case sf::Event::KeyReleased:
				{
					key_input(event, false);
					break;
				}
				case sf::Event::MouseButtonPressed:
				{
					mouse_input(event, true);
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					mouse_input(event, false);
					break;
				}
				case sf::Event::MouseMoved:
				{
					mouse.x = event.mouseMove.x;
					mouse.y = event.mouseMove.y;
					break;
				}
			}
		}
		return false;
	}
};
#endif
