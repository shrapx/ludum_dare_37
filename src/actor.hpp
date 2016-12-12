#ifndef _ACTOR_HPP_
#define _ACTOR_HPP_

#include "SFML/Graphics.hpp"


class Actor : public sf::Sprite
{

public:

	//sf::Sprite sprite;
	sf::Vector2f velocity{0.0,0.0};
	sf::Vector2f movedir{0.0,0.0};
	
	int gait = 21;
	float speed = 0.6f;
	float friction = 0.9f;
	
	// speed mode
	/*int gait = 1;
	float speed = 0.8f;
	float friction = 0.4f;*/
	
};

#endif