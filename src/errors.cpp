/**
 * Small module for showing error messages to user
*/

#include "errors.hpp"

#ifdef USE_CONSOLE_OUTPUT
#	include <iostream>
#else
#	include <SDL.h>
#	include "mls.hpp"
#endif

void ShowError(std::string error_init, const char* error_get) {
	std::string text = error_init;
	text += std::string(error_get);
	
	ShowError(text);
}

void ShowError(std::string error_msg) {
	#ifdef USE_CONSOLE_OUTPUT
	std::cerr << error_msg << std::endl;
	#else
	std::string title = _("%{#Message box title#}%Program error").get();
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
							title.c_str(),
							error_msg.c_str(),
							nullptr);
	#endif
}

