#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Audio.hpp"

#include "common.hpp"
#include "collide.hpp"
#include "room.hpp"
#include "actor.hpp"
#include "prop.hpp"
#include "torchlight.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <time.h>

class Scene
{
	
private:

	Input& input;
	Asset& asset;
	Persist& persist;
	
	int level;
	
	Room room;
	
	sf::Sprite m_render_sprite;
	sf::RenderTexture render_scene;
	
	sf::View view;
	
	sf::Sprite shadow;
	sf::RenderTexture render_shadow;
	
	//sf::Sprite ambient;
	//sf::Sprite light_damp;
	
	sf::Sound sound_blip;
	sf::Sound sound_switch;
	Prop* last_clickless_frob = nullptr;
	
	Torchlight torch;
	
	Collide exterior_light_collide;
	Torchlight heli_light;
	Torchlight heli_light2;
	
	sf::Shader* darkness;
	
	std::unique_ptr<Actor> player;
	std::unique_ptr<Actor> pointer;
	
	std::unordered_map<string, std::unique_ptr<Prop>> triggers;
	//std::unordered_map<string, std::unique_ptr<sf::Sound>> sounds;
	
	std::unique_ptr<Prop> light_wc;
	std::unique_ptr<Prop> light_front;
	std::unique_ptr<Prop> light_back;
	std::unique_ptr<Prop> light_tv;
	std::unique_ptr<Prop> light_out;
	
	uint has_light_wc = 0;
	uint has_light_front = 0;
	uint has_light_back = 0;
	uint has_light_tv = 0;
	
	
	//std::unordered_map<std::string, std::function<void()>> trigger_callbacks;
	std::unordered_map<std::string, Prop*> trigger_props;
	
	bool night_scene = false;
	
	uint time = 0;
	
	bool frob_on = false;
	bool player_dead = false;
	
	// level zero specifically
	
	sf::Vector2f assassin_pos;
	uint assassin_attack_duration = 150;
	uint assassin_attack_finish;
	bool assassin_attack = false;
	
	bool got_flashlight = false;
	bool flashlight_on_floor = true;
	bool flashlight_trigger = false;
	bool flashlight_toggle = false;
	
public:

	Scene(
		Input& input,
		Asset& asset,
		Persist& persist,
		int level)
	:
		input(input),
		asset(asset),
		persist(persist),
		level(level),
		room(input,asset,persist),
		torch(room.walls),
		heli_light(exterior_light_collide),
		heli_light2(exterior_light_collide)
	{
		
		sound_blip.setBuffer( *asset.soundbuffers.at("blip") );
		sound_switch.setBuffer( *asset.soundbuffers.at("switch") );
		
		exterior_light_collide.setTexture(*asset.textures.at("exterior_light_collide"));
		
		//fader = asset.shaders["fader"].get();
		darkness = asset.shaders.at("darkness").get();
		
		render_shadow.create(128, 128, false);
		shadow.setTexture(render_shadow.getTexture());
		
		render_scene.create(ZOOM_W, ZOOM_H, false);
		m_render_sprite.setTexture(render_scene.getTexture());
		m_render_sprite.setScale( float(ZOOM), float(ZOOM) );
		
		view = render_scene.getDefaultView();
		
		pointer = std::make_unique<Actor>();
		
		pointer->setTexture(*asset.textures.at("pointer"));
		pointer->setOrigin({4,4});
		
		player = std::make_unique<Actor>();
		
		switch( level )
		{
			case 0: {load_level_zero(); break;}
			case 1: {load_level_one(); break;}
			default: assert(false);
		}
	}
	
	Prop* add_trigger(
	const std::string& name,
	const sf::Texture& tex,
	const sf::Vector2f& pos,
	const sf::IntRect& texrect,
	const sf::Vector2f& origin,
	const std::string& actkey)
	{
		auto& a = triggers[name];
		a = std::make_unique<Prop>(tex, pos);
		a->add_frame(texrect, origin, actkey);
		a->set_frame(0);
		return a.get();
	}
	
	void load_level_zero()
	{
		night_scene = true;
		
		auto h = asset.music.at("heli");
		h->setLoop(true);
		h->play();
		
		auto l = asset.music.at("loop");
		l->setLoop(true);
		l->play();
		
		auto& players = *asset.textures.at("player");
		
		player->setTexture(players);
		player->setPosition( {44,115} );
		player->setTextureRect( {1,1,4,8} );
		player->setOrigin({2,4});
		
		add_trigger("deadguy", players, {72,73}, {103,114,24,13}, {16,7}, "deadguy");
		
		auto dp = add_trigger("deadplayer", players, {-100,100}, {0,0,1,1}, {0,0}, "");
		dp->add_frame({1,16,17,10}, {8,5}, "");
		dp->loop = false;
		trigger_props["deadplayer"] = dp;
		
		auto as = add_trigger("assassin", players, {-100,-100}, {0,0,1,1}, {0,0}, "");
		as->add_frame({32,0,11,10}, {4,5}, "");
		as->add_frame({48,1,7,8}, {3,4}, "");
		as->loop = false;
		
		
		auto& lights = *asset.textures.at("lights");
		
		light_wc = std::make_unique<Prop>(lights, sf::Vector2f{61,19}); light_wc->add_frame({0,0,1,1}, {0,0}, "off");
		light_front = std::make_unique<Prop>(lights, sf::Vector2f{17,5}); light_front->add_frame({0,0,1,1}, {0,0}, "off");
		light_back = std::make_unique<Prop>(lights, sf::Vector2f{34,41}); light_back->add_frame({0,0,1,1}, {0,0}, "off");
		light_tv = std::make_unique<Prop>(lights, sf::Vector2f{36,55}); light_tv->add_frame({0,0,1,1}, {0,0}, "off");
		light_out = std::make_unique<Prop>(lights, sf::Vector2f{0,77}); light_out->add_frame({0,205,128,51}, {0,0}, "");
		
		light_wc->add_frame({1,1,22,39}, {0,0}, "on");
		light_front->add_frame({24,1,83,55}, {0,0}, "on");
		light_back->add_frame({1,58,49,58}, {0,0}, "on");
		light_tv->add_frame({52,58,47,41}, {0,0}, "on");
		
		light_wc->set_frame(0);
		light_front->set_frame(0);
		light_back->set_frame(0);
		light_tv->set_frame(0);
		light_out->set_frame(0);
		
		trigger_props["light_wc"] = light_wc.get();
		trigger_props["light_front"] = light_front.get();
		trigger_props["light_back"] = light_back.get();
		trigger_props["light_tv"] = light_tv.get();
		
		
		light_wc->setColor( {200,255,100} );
		light_front->setColor( {75,100,255} );
		light_back->setColor( {255,140,140} );
		
		// set modes of animation for this
		// white noise sim (white flickery)
		// go between several hues with time delay and flickering
		light_tv->setColor( {255,255,255} );
		
		// throb light
		light_out->setColor( {128,128,196} );
		

		auto& furnitures = *asset.textures.at("furnitures");
		
		add_trigger("switch_wc", furnitures, {82,33}, {13,1,1,2}, {0,0}, "light_wc");
		add_trigger("switch_front", furnitures, {82,14}, {13,1,1,2}, {0,0}, "light_front");
		add_trigger("switch_back" , furnitures, {60,60}, {13,1,2,1}, {0,0}, "light_back");
		
		auto fl = add_trigger("flashlight", furnitures, {42,19}, {11,10,4,4}, {0,0}, "flashlight");
		fl->add_frame({0,0,1,1}, {0,0}, "flashlight");
		fl->loop = false;
		trigger_props["flashlight"] = fl;
		
		add_trigger("tv"          , furnitures, {36,76}, {1,1,4,17}, {1,8}, "");
		add_trigger("tva"         , furnitures, {39,69}, {6,1,3,2},  {0,0}, "");
		add_trigger("tvb"         , furnitures, {40,74}, {7,4,2,6},  {0,0}, "");
		add_trigger("tv_remote",    furnitures, {40,85}, {6,11,3,2}, {0,0}, "light_tv");
		add_trigger("pc1"         , furnitures, {38,60}, {4,20,9,5}, {4,2}, "");
		add_trigger("pillow1"     , furnitures, {79,70}, {25,1,4,7}, {0,0}, "");
		
		//add_trigger("pillow2"     , furnitures, {79,80}, {25,1,4,7}, {0,0}, "");
		//add_trigger("soap"        , furnitures, {79,48}, {33,37,2,3}, {1,1},"");
		
		//printer
		auto f = add_trigger("pri", furnitures, {38,49}, {16,16,7,8}, {3,4},"");
		//fl->loop = false;
		/*f->add_frame({25,16,6,8}, {3,4}, "");
		f->add_frame({16,25,7,8}, {3,4}, "");
		f->add_frame({25,25,8,8}, {3,4}, "");*/
		
		
		torch.pos = {44.0f,21.0f};
		torch.dir = {1.0f,1.0f};
		torch.distance = 32.0f;
		torch.cone_color = {255,255,223};
		torch.circle_color = {48,48,48};
		torch.update();		
		
		heli_light.pos = {95.0f,130.0f};
		heli_light.dir = {-1.0f,-1.0f};
		heli_light.distance = 80.0f;
		heli_light2.cone_close_distance = 48.0f;
		heli_light.fov = M_PI*0.2f;
		heli_light.cone_color = {254,96,96};
		heli_light.draw_circle = false;
		heli_light.points = 128;
		heli_light.update();
		
		heli_light2.pos = {95.0f,155.0f};
		heli_light2.dir = {-1.0f,-1.0f};
		heli_light2.distance = 80.0f;
		heli_light2.cone_close_distance = 60.0f;
		heli_light2.fov = M_PI*0.1f;
		heli_light2.cone_color = {64,254,254};
		heli_light2.draw_circle = false;
		heli_light2.points = 128;
		heli_light2.update();
	}
	
	void load_level_one()
	{
		
		night_scene = false;
		
		auto& players = *asset.textures.at("player");
		
		player->setTexture(players);
		player->setPosition( {44,115} );
		player->setTextureRect( {1,1,4,8} );
		player->setOrigin({2,4});
		

		auto& furnitures = *asset.textures.at("furnitures");
		
		add_trigger("tv"          , furnitures, {36,76}, {1,1,4,17}, {1,8}, "");
		add_trigger("tva"         , furnitures, {39,69}, {6,1,3,2},  {0,0}, "");
		add_trigger("tvb"         , furnitures, {40,74}, {7,4,2,6},  {0,0}, "");
		add_trigger("tv_remote",    furnitures, {40,85}, {6,11,3,2}, {0,0}, "");
		add_trigger("pc1"         , furnitures, {38,60}, {4,20,9,5}, {4,2}, "");
		
		
		// pillow -> key
		auto p = add_trigger("pillow1"     , furnitures, {79,70}, {25,1,4,7}, {0,0}, "");
		p->add_frame({25,1,4,7}, {0,0}, "key"); // <-- key under pillow
		
		add_trigger("pillow2"     , furnitures, {79,80}, {25,1,4,7}, {0,0}, "");
		add_trigger("soap"        , furnitures, {79,48}, {33,37,2,3}, {1,1}, "");
		
		auto f = add_trigger("printer", furnitures, {38,49}, {16,16,7,8}, {3,4},"");
		//fl->loop = false;
		/*f->add_frame({25,16,6,8}, {3,4}, "");
		f->add_frame({16,25,7,8}, {3,4}, "");
		f->add_frame({25,25,8,8}, {3,4}, "");*/
	}
	
	int update()
	{
		++time;
		darkness->setParameter("time", float(time*0.1f));
		darkness->setParameter("strength", float(sin(time*0.05f)*0.05f + 0.2f));
		
		auto pos = player->getPosition();
		
		auto cam = pos;
		cam.y = std::min(98.f,std::max(32.f,cam.y));
		cam.x = std::min(68.f,std::max(60.f,cam.x));
		
		view.setCenter(cam);
		
		sf::Vector2f mouse = cam + (sf::Vector2f{input.mouse}/ZOOM) - sf::Vector2f(ZOOM_WH, ZOOM_HH);
		pointer->setPosition(mouse);
		
		if (!player_dead)
		{
			bool u = input.get_command.at(0).at("up");
			bool l = input.get_command.at(0).at("left");
			bool d = input.get_command.at(0).at("down");
			bool r = input.get_command.at(0).at("right");
			
			if ( (time%player->gait) == 0)
			{
				player->movedir = sf::Vector2f( (r?1.0f:0.0f) - (l?1.0f:0.0f), (d?1.0f:0.0f) - (u?1.0f:0.0f) );
			}
			else
			{
				player->movedir = {0,0};
			}

			
			player->velocity += player->movedir * player->speed;
			
			// if actor intends to move into wall, stop
			if ( abs(player->velocity.x) > 0.1f || abs(player->velocity.y) > 0.1f)
			{
				sf::Vector2f next_pos = player->getPosition() + player->velocity;
				
				if ( !room.walls.test_area(next_pos, 2) && !room.fixtures.test_area(next_pos, 2) )
				{
					
					player->setRotation( std::atan2(player->velocity.y, player->velocity.x)*(180.0/M_PI) );
					player->setPosition(next_pos);
				}
				else
				{
					player->velocity = {0,0};
				}
				
				player->velocity *= player->friction;
			}
			
			// printer animate
			// if (level == 2)
			//if (triggers["pri"]) triggers["pri"]->set_frame(time/100);
			
			// frob = using items and reactions
			
			bool frob = input.get_command.at(0).at("frob");
			if (frob_on && !frob) { frob_on = false; }
			if (frob && !frob_on)
			{
				frob_on = true;
				
				for (auto& kv : triggers)
				{
					auto& trig = *kv.second;
					
					auto p = pointer->getGlobalBounds();
					auto t = trig.getGlobalBounds();
					
					if (p.intersects(t) && dist(trig.getPosition(), pos)<10)
					{
						
						//persist.trigger_flags.insert(trig.trigger());
						std::string frob_name = trig.trigger();
						auto& prop = trigger_props[frob_name];
						if (prop)
						{
							std::string trigname = prop->trigger();
							
							if (trigname == "off") sound_switch.play();
							
							switch (level)
							{
								case 0:
								{
									if (frob_name == "light_wc") has_light_wc = time;
									if (frob_name == "light_front") has_light_front = time;
									if (frob_name == "light_back") has_light_back = time;
									
									auto w = asset.music.at("winge");
									
									if(light_wc->get_frame() ||
									light_front->get_frame() ||
									light_back->get_frame())
									{
										
										sound_switch.play();
										
										if ( w->getStatus() != sf::Music::Status::Playing )
										{
											w->setLoop(true);
											w->play();
										}
									}
									else
									{
										w->stop();
									}
									
									
									auto n = asset.music.at("noise");
									
									if (frob_name == "light_tv")
									{
										has_light_tv = time;
										
										
										if ( n->getStatus() == sf::Music::Status::Playing )
											n->stop();
										else
										{
											n->setLoop(true);
											n->play();
											
										}
									}
									
									
									auto l = asset.music.at("loop");
									if ( w->getStatus() != sf::Music::Status::Playing && n->getStatus() != sf::Music::Status::Playing )
									{
										l->setLoop(true);
										l->play();
									}
									else
									{
										l->pause();
									}
									
									
									// flashlight pickup;
									if (trigname == "flashlight")
									{
										got_flashlight = true;
										flashlight_on_floor = false;
									}
									break;
								}
								case 1:
								{
									
									auto l = asset.music.at("loop"); if ( l->getStatus() == sf::Music::Status::Playing ) l->stop();									
									auto n = asset.music.at("noise"); if ( n->getStatus() == sf::Music::Status::Playing ) n->stop();									
									auto h = asset.music.at("heli"); if ( h->getStatus() == sf::Music::Status::Playing ) h->stop();									
									auto w = asset.music.at("winge"); if ( w->getStatus() == sf::Music::Status::Playing ) w->stop();
									
									if (frob_name == "key") return 0;
									break;
								}
							}
						}
						
						//else (play_fail_noise);
					}
				}
			}
			else
			{
				
				for (auto& kv : triggers)
				{
					auto& trig = *kv.second;
					
					//auto p = pointer->getGlobalBounds();
					auto p = pointer->getPosition();
					auto t = trig.getGlobalBounds();
					
					if (t.contains(p) && last_clickless_frob != kv.second.get())
					{
						last_clickless_frob = kv.second.get();
						
						sound_blip.play();
						
						break;
					}
				}
			}
		}
		
		if (level == 0)
		{
			if (!assassin_attack && has_light_wc && has_light_front && has_light_back && has_light_tv )
			{
				
				// spawn assassin 32-64 pixels away
				assassin_attack = true;
				assassin_attack_finish = time + assassin_attack_duration;
				
				auto& a = triggers["assassin"];
				a->trigger();
				
				
				uint largest_val = std::max(has_light_wc,std::max(has_light_front,std::max(has_light_back,has_light_tv)));
				
				if (largest_val == has_light_wc) a->setPosition({69,53});	
				else if (largest_val == has_light_front) a->setPosition({29,21});
				else if (largest_val == has_light_back) a->setPosition({78,92});
				else if (largest_val == has_light_tv) a->setPosition({54,117});
				
				assassin_pos = (largest_val == has_light_wc) ? sf::Vector2f{69,53}
							: (largest_val == has_light_front) ? sf::Vector2f{29,21}
							: (largest_val == has_light_back) ? sf::Vector2f{78,92} : sf::Vector2f{54,117};
				
				//a->setRotation();
				has_light_wc = 0;
			}
			
			if (assassin_attack)
			{
				if (time < assassin_attack_finish)
				{
					auto& a = triggers["assassin"];
					float amt = float(1.0f/assassin_attack_duration) * float(assassin_attack_finish-time);
					auto apos = lerp(pos, assassin_pos, amt);
					a->setPosition(apos);
				}
				else
				{
					flashlight_toggle = false;
					assassin_attack = false;
					player_dead = true;
					//load next level!
					
					triggers["assassin"]->trigger();
					
					auto& dp = triggers["deadplayer"];
					dp->trigger();
					dp->setPosition(pos);
					
					player->setTextureRect( {0,0,1,1} );
					
					return 1;
					// all lights are on, the assassin kills u and prologue ends
					//std::cout << "level end!" << std::endl;
				}
			}
		}
		
		// night lighting
		
		if (night_scene)
		{
			// heli lights
			
			heli_light.pos.x = (sin(time*0.001f)*128.f);
			heli_light2.pos.x = (cos(time*-0.0018f)*96.f);
			
			heli_light.dir = normalize(sf::Vector2f(sin(time*0.01f), -0.75f));
			heli_light2.dir = normalize(sf::Vector2f(sin(time*-0.007f), -1.0f));
				
			if (pos.y > 85.f)
			{
				float amt = (1/35.0f) * (pos.y-85.0f);
				
				auto direct = normalize(pos - heli_light.pos);
				auto direct2 = normalize(pos - heli_light2.pos);
				heli_light.dir = lerp(heli_light.dir,direct,amt);
				heli_light2.dir = lerp(heli_light2.dir,direct2,amt);
			}
			
			heli_light.update();
			heli_light2.update();
			
			// flashlight
			
			bool fl = input.get_command.at(0).at("flashlight");
			if (fl && !flashlight_trigger)
			{
				flashlight_trigger = true;
				flashlight_toggle = !flashlight_toggle;
				sound_switch.play();
			}
			
			if (!fl && flashlight_trigger) flashlight_trigger = false;
			if (got_flashlight && flashlight_toggle)
			{
				
				
				torch.pos = pos;
				 
				auto mp = normalize(mouse - pos);
				torch.dir = mp;
				
				torch.update();
				
				for (auto& kv : triggers)
				{
					if (kv.second)
					{
						auto& trig = *kv.second;
						
						auto tp = normalize(trig.getPosition() - pos);
						float facing = dot(mp, tp);
						auto distance = dist(trig.getPosition(), pos);
						
						trig.setColor( (facing > 0.89f && distance < 16.0f) ? lerp(sf::Color{255,255,255}, sf::Color{128,128,128}, (1.0f/16)*distance) : sf::Color{128,128,128} );
						//trig.setColor( (facing > 0.89f && distance < 15) ? sf::Color{255,255,255} : sf::Color{0,0,0} ); // obvious mode
					}
				}
			}
		}
		
		return 0;
	}
	
	sf::Sprite& get_render()
	{
		
		sf::RenderStates states;
		
		if (night_scene)
		{
			render_shadow.clear(sf::Color(0,0,0));
			
			states.blendMode = sf::BlendAdd;
			
			if (flashlight_on_floor || (got_flashlight && flashlight_toggle) ) render_shadow.draw(torch, states);
			
			render_shadow.draw(heli_light, states);
			render_shadow.draw(heli_light2, states);
			
			render_shadow.draw(*light_wc,states);
			render_shadow.draw(*light_front,states);
			render_shadow.draw(*light_back,states);
			render_shadow.draw(*light_tv,states);
			
			render_shadow.draw(*light_out,states);
			
			render_shadow.display();
		}
		
		render_scene.setView(view);
		
		render_scene.clear(sf::Color(0,0,0)); //sf::Color(8,8,12)
		
		render_scene.draw(room);
		
		for (auto& kv : triggers)
		{
			if (kv.second) render_scene.draw(*kv.second.get());
		}
		
		render_scene.draw(*player);
		
		if (night_scene)
		{
			// draw shadow with shader
			states.blendMode = sf::BlendMultiply;
			states.shader = darkness;
			
			render_scene.draw(shadow, states);
		}
		
		//states.blendMode = sf::BlendAlpha;
		render_scene.draw(*pointer);
		render_scene.display();
		
		return m_render_sprite;
	}
};

#endif
