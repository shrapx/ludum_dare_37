#ifndef _TORCHLIGHT_HPP_
#define _TORCHLIGHT_HPP_

#include "collide.hpp"

#include <iostream>
#include <vector>
#include <cmath>

namespace
{
	
	std::vector<sf::Vector2i> make_discrete_line(sf::Vector2f a, sf::Vector2f b)
	{
		// how to get number of points..
		/*std::vector<int> xi;
		std::vector<int> yi;
		for (int x = a.x; x < b.x; ++x) { xi.push_back(x); }
		for (int y = a.y; y < b.y; ++y) { yi.push_back(y); }
		int num = std::max(xi.size(), yi.size());*/
		
		int num = std::max( abs(int(a.x-b.x)), abs(int(a.y-b.y)) );
		
		std::vector<sf::Vector2i> v;
		
		for (int i = 0; i<num; ++i)
		{
			sf::Vector2f l = lerp(a,b,float(1.0f/num)*i);
			
			v.push_back( {int(l.x),int(l.y)});
		}
		
		return v;
	}
}

class Torchlight : public sf::Drawable, public sf::Transformable
{

private:

	sf::VertexArray va;
	sf::VertexArray vb;
	
public:

	Collide& collide;
	
	sf::Vector2f pos {0.0,0.0};
	sf::Vector2f dir {1.0,0.0};
	
	float fov = M_PI_2*0.5f;
	float distance = 32.0f;
	
	float cone_close_distance = 1.0f;
	
	float cone_brightness = 0.0f;
	
	sf::Color cone_color = sf::Color::White;
	sf::Color circle_color = sf::Color::White;
	bool draw_cone = true;
	bool draw_circle = true;
	int points = 32;
	
	
	
	Torchlight(Collide& collide) : collide(collide)
	{
		va.setPrimitiveType(sf::PrimitiveType::TrianglesStrip);
		vb.setPrimitiveType(sf::PrimitiveType::TrianglesFan);
	}
	
	void update()
	{
		if (draw_cone)
		{
			va.clear();
			//va.append( {pos,sf::Color(32+cone_brightness,32+cone_brightness,cone_brightness)} );

			//sf::Vector2f last_point = pos;
			
			float base_ang = atan2(dir.y,dir.x)-fov;
			
			for (int i=0; i<points; ++i)
			{
				float ii = (((1.0f/points)*i) - 0.5f) * 2.0f;
				float fader = sin(1.5f+(ii*M_PI))*0.5f + 0.5f;

				//std::cout << fader << std::endl;
				
				float ang = base_ang + ((1.0f/(points/2))*i*fov);
				
				sf::Vector2f close_point = pos + sf::Vector2f{cos(ang), sin(ang)} * cone_close_distance;
				
				{
					va.append( {pos, sf::Color(cone_color.r*fader,cone_color.g*fader,cone_color.b*fader) } );
					
				}
				
				sf::Vector2f distant_point = pos + sf::Vector2f{cos(ang), sin(ang)} * distance;
				
				// check from pos and from prev
			
				// pos
				float var = collide.test_line(make_discrete_line(close_point,distant_point));
				sf::Vector2f from_pos = lerp(close_point,distant_point,var);
				
				// far cone_brightness
				float c = (1.0f-var)*0.1f*fader;
				
				// far
				if (var < 1.0f)
				{
					float var2 = collide.test_line(make_discrete_line(close_point,distant_point));
					sf::Vector2f from_prev = lerp(close_point,distant_point,var2);
					
					
					va.append( {from_prev, cone_color*sf::Color(c,c,c)} );
				}
				
				va.append( {from_pos, cone_color*sf::Color(c,c,c)} );
				//va.append( {from_pos, sf::Color(32,32,0)} );
				//last_point = from_pos;
			}
		}
		
		if (draw_circle)
		{
			sf::Vector2f last_point = pos;
			
			vb.clear();
			vb.append( {pos,circle_color} );
			for (int i=0; i<(points+1); ++i)
			{
				float ang = (1.0f/points)*i*M_PI*2.0f;
				
				float dist_mod = sin(ang*1.5f)*2.5f;
				
				sf::Vector2f distant_point = pos + sf::Vector2f{cos(ang), sin(ang)} * (distance + dist_mod);
				
				vb.append( {distant_point, sf::Color(4,4,8)} );
				
				// disabled obscuring
				/*
				// pos
				float var = collide.test_line(make_discrete_line(pos,distant_point));
				sf::Vector2f from_pos = lerp(pos,distant_point,var);
				
				// far
				if (var < 1.0f)
				{
					float var2 = collide.test_line(make_discrete_line(last_point,distant_point));
					sf::Vector2f from_prev = lerp(last_point,distant_point,var2);
					
					float c = (1.0f-var)*circle_brightness;
					vb.append( {from_prev, sf::Color(c,c,c)} );
				}
				
				float c = (1.0f-var)*circle_brightness;
				vb.append( {from_pos, sf::Color(c,c,c)} );
				vb.append( {from_pos, sf::Color(0,0,0)} );
				last_point = from_pos;*/
			}
			
		}
	}
	
	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		
		target.draw(vb, states);
		target.draw(va, states);
	}
};

#endif