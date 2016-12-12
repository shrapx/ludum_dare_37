#ifndef _COLLIDE_HPP_
#define _COLLIDE_HPP_

#include <array>
#include <cassert>

class Collide : public sf::Sprite
{

public:

	std::array<bool,128*128> m_array;
	
	Collide()
	{
		for (bool& b : m_array) b = false;
	}

	void setTexture(const sf::Texture& tex)
	{
		sf::Sprite::setTexture(tex);
		load( tex );
	}
		
	void load(const sf::Texture& t )
	{
		assert(t.getSize() == sf::Vector2u(128,128));
		
		sf::Image i = t.copyToImage();

		for (int x = 0; x < i.getSize().x; ++x)
		{
			for (int y = 0; y < i.getSize().y; ++y)
			{
				if (i.getPixel(x,y).a != 0) m_array[x+(y*128)] = true;
			}
		}
	}

	bool test_point(const sf::Vector2i& p)
	{
		return p.x > -1 && p.x < 128
			&& p.y > -1 && p.y < 128
			&& m_array[p.x+(p.y*128)];
	}
	
	float test_line(const std::vector<sf::Vector2i>& line) 
	{
		for ( int i=0; i<line.size(); ++i )
		{
			if (test_point( line[i] )) return (1.0f/line.size()) * i; // collision
		}
		
		return 1.0f;
	}
	
	// range 2 = 5*5 = 25 pixel square around pos;
	bool test_area(sf::Vector2f pos, int range)
	{
		int sx = pos.x;
		int sy = pos.y;
		for (int x = sx-range; x < sx+range; ++x)
			for (int y = sy-range; y < sy+range; ++y)
				if (test_point({x,y})) return true;
		return false;
	}
	
	bool test_area(const sf::FloatRect& rect)
	{
		for (int x = rect.left; x < rect.left+rect.width; ++x)
			for (int y = rect.top; y < rect.top+rect.height; ++y)
				if (test_point({x,y})) return true;
		return false;
	}
};

#endif