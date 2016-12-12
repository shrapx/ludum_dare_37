#ifndef _ROOM_HPP_
#define _ROOM_HPP_

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Audio.hpp"

#include "collide.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <cmath>

namespace
{
	template<typename T>
	sf::Vector2<T> normalize(const sf::Vector2<T>& source)
	{
		T length = sqrt((source.x * source.x) + (source.y * source.y));
		if (length != 0)
			return sf::Vector2<T>(source.x / length, source.y / length);
		else
			return source;
	}
}


class Room : public sf::Drawable, public sf::Transformable
{
	
private:
	Input& input;
	Asset& asset;
	Persist& persist;
	
	sf::Sprite floor;
	
public:

	Collide walls;
	Collide fixtures;
	Collide collide;
	
	Room(
		Input& input,
		Asset& asset,
		Persist& persist)
	:
		input(input),
		asset(asset),
		persist(persist)
	{
		floor.setTexture( *asset.load_texture("floor", "data/floor.png") );
		
		auto* wt = asset.load_texture("walls", "data/walls.png");
		walls.setTexture( *wt );
		
		auto* ft = asset.load_texture("fixtures", "data/fixtures.png");
		fixtures.setTexture( *ft );
	};

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		
		target.draw(floor, states);
		target.draw(walls, states);
		target.draw(fixtures, states);
	}
};

#endif
