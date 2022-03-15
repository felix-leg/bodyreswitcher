/**
 * Module for storing, saving and loading game configuration
*/

#include "game_config.hpp"
#include "filesystem.hpp"
#include <sstream>
#include <fstream>
#include <cstring>


///sets default values for game configuration
GameConfig::GameConfig() {
	fullscreen = false;
	music = true;
	video.width = 0;
	video.height = 0;
	video.bitsPerPixel = 0;
	video.refreshRate = 0;
}

std::string GameConfig::to_string() const {
	std::ostringstream result;
	result << "Fullscreen: " << std::boolalpha << fullscreen << ", ";
	result << "Music: " << std::boolalpha << music << ", ";
	result << "Video mode: " << video.to_string();
	
	return result.str();
}

//reading and saving

constexpr const char* configHeader = "BodyReSwitcher2.0";
constexpr unsigned int configHeaderLength = 18;
constexpr char fullscreenBit = 0b00000001;
constexpr char musicBit = 0b00000010;

///saves config to file
void GameConfig::save() const {
	std::ofstream output(filesystem::getConfigurationFile(false), 
	std::ios::out | std::ios::binary | std::ios::trunc);
	
	//save the config header
	output.write(configHeader, configHeaderLength);
	
	//write boolean flags
	char flags = 0;
	if( fullscreen ) flags |= fullscreenBit;
	if( music ) flags |= musicBit;
	output.write(&flags, 1);
	
	//write video mode
	output.write(reinterpret_cast<const char*>(&video), sizeof(video));
	
	//writing done
	output.close();
}

///reads config from file
void GameConfig::read() {
	std::ifstream source(filesystem::getConfigurationFile(true), std::ios::in | std::ios::binary );
	if( source.is_open() == false ) return;
	//test if we are reading a correct config file
	char testString[configHeaderLength];
	source.read(testString, configHeaderLength);
	if( std::strncmp(configHeader, testString,configHeaderLength) != 0 ) {
		//std::cout << "Not a correct config file" << std::endl;
		return;
	}
	
	//read boolean config bits
	char bits;
	source.read(&bits, 1);
	
	fullscreen = ((bits & fullscreenBit) == fullscreenBit);
	music = ((bits & musicBit) == musicBit);
	
	//read video mode
	source.read(reinterpret_cast<char*>(&video), sizeof(video));
	
	//finish reading
	source.close();
}