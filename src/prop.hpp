#ifndef _PROP_HPP_
#define _PROP_HPP_

#include "SFML/Graphics.hpp"

struct propframe
{
	sf::IntRect texrect;
	sf::Vector2f origin;
	std::string actkey;
	propframe(const sf::IntRect& texrect, const sf::Vector2f& origin, const std::string& actkey)
	:
		texrect(texrect),
		origin(origin),
		actkey(actkey)
	{}
};

class Prop : public sf::Sprite
{

public:
	
	// frames with event key and rect
	std::vector<propframe> frames;
	
	int current_frame = 0;
	bool loop = true;
	
	Prop(const sf::Texture& tex, const sf::Vector2f& pos)
	{
		setTexture(tex);
		setPosition(pos);
	}
	
	void add_frame(const sf::IntRect& texrect, const sf::Vector2f& origin, const std::string& actkey)
	{
		frames.emplace_back( texrect, origin, actkey);
	}
	
	std::string trigger()
	{
		return set_frame(current_frame+1);
	}
	
	std::string set_frame(int i)
	{
		if (!loop)
			i = max(min(i,int(frames.size()-1)),0);
		else
			i = i % frames.size();
			
		current_frame = i;
		
		const propframe& frame = frames[i];
		
		setTextureRect(frame.texrect);
		setOrigin(frame.origin);
		
		return frame.actkey;
	}
	
	int get_frame() { return current_frame; }
};

#endif