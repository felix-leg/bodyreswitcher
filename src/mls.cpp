/**
 * This file installs MuLan String library into the game
*/


//We are using GNU GetText as our backend
#define MULANSTR_USE_GETTEXT


//include the library file
#include <mulanstring/main.cpp>

#include "filesystem.hpp"
#include <string>

//function to initialize the mls
void mls_init() {
	std::string localeDir;
	localeDir = filesystem::getAssetRoot();
	#if __has_include(<windows.h>)
	localeDir += "\\";
	#else
	localeDir += "/";
	#endif
	localeDir += "locale";
	
	mls::backend::init("bodyreswitcher", nullptr, localeDir.c_str());
}

