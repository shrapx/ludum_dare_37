#ifndef _ASSET_HPP_
#define _ASSET_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <cassert>

#include <string>
#include <unordered_map>
#include <memory>

#include "common.hpp"

class Asset
{
public:

	sf::Shader* load_shader(const std::string& tag, const std::string& file_frag)
	{
		auto emplace_result = shaders.emplace(tag, std::make_unique<sf::Shader>());
		assert(emplace_result.second);
		sf::Shader* shad = emplace_result.first->second.get();
		assert(shad->loadFromFile(file_frag, sf::Shader::Fragment) );
		return shad;
	}
	
	sf::Shader* load_shader(const std::string& tag, const std::string& file_vert, const std::string& file_frag)
	{
		auto emplace_result = shaders.emplace(tag, std::make_unique<sf::Shader>());
		assert(emplace_result.second);
		sf::Shader* shad = emplace_result.first->second.get();
		assert(shad->loadFromFile(file_vert, file_frag) );
		return shad;
	}

	sf::Font* load_font(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = fonts.emplace(tag, std::make_unique<sf::Font>());
		assert(emplace_result.second);
		sf::Font* font = emplace_result.first->second.get();
		assert( font->loadFromFile(filepath) );
		return font;
	}

	sf::Texture* load_texture(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = textures.emplace(tag, std::make_unique<sf::Texture>());
		sf::Texture* tex = emplace_result.first->second.get();
		assert( tex->loadFromFile(filepath) );
		return tex;
	}

	sf::SoundBuffer* load_soundbuffer(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = soundbuffers.emplace(tag, std::make_unique<sf::SoundBuffer>());
		sf::SoundBuffer* sb = emplace_result.first->second.get();
		assert( sb->loadFromFile(filepath) );
		return sb;
	}
	
	sf::Music* load_music(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = music.emplace(tag, std::make_unique<sf::Music>());
		sf::Music* sb = emplace_result.first->second.get();
		assert( sb->openFromFile(filepath) );
		return sb;
	}
	std::unordered_map<std::string, std::unique_ptr<sf::Shader>> shaders;
	std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
	std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundbuffers;
	std::unordered_map<std::string, std::shared_ptr<sf::Music>> music;
};

#endif // _ASSET_HPP_
