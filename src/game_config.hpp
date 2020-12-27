#pragma once

#ifndef GAME_CONFIGURATION_HPP
#define GAME_CONFIGURATION_HPP

#include "multimedia/base.hpp"
#include <string>

struct GameConfig {
	bool fullscreen;
	bool music;
	multimedia::VideoMode video;
	
	GameConfig();
	
	std::string to_string() const;
	
	void read();
	void save() const;
	
};//end class GameConfig

#endif /* GAME_CONFIGURATION_HPP */
