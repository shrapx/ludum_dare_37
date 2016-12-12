#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#define M_PI	3.14159265358979323846
#define M_PI_2	1.57079632679489661923

#define SCREEN_W 1440
#define SCREEN_H 720
#define ZOOM 12.0f
#define ZOOM_W SCREEN_W/ZOOM
#define ZOOM_H SCREEN_H/ZOOM
#define ZOOM_WH ZOOM_W/2
#define ZOOM_HH ZOOM_H/2
#define ZOOM_WQ ZOOM_W/4
#define ZOOM_HQ ZOOM_H/4

#include <cmath>

using uint = unsigned int;

float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return (a.x*b.x)+(a.y*b.y);
};

float length(const sf::Vector2f& a) {
	return std::sqrt(a.x * a.x + a.y * a.y);
}

float dist(const sf::Vector2f& a, const sf::Vector2f& b) {
	sf::Vector2f c{b.x - a.x, b.y - a.y};
	return length(c);
}

sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, float t)
{
	return {a.x + t*(b.x-a.x), a.y + t*(b.y-a.y)};
}

sf::Color lerp(const sf::Color& a, const sf::Color& b, float t)
{
	return {
		sf::Uint8(a.r + t*(b.r-a.r)),
		sf::Uint8(a.g + t*(b.g-a.g)),
		sf::Uint8(a.b + t*(b.b-a.b))
		};
}


#endif
