#pragma once

#ifndef BRS_SDL_SYSTEM_HPP
#define BRS_SDL_SYSTEM_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>

#include "base.hpp"

namespace multimedia {
	
	class SDL2_System {
		public:
			SDL2_System();
			~SDL2_System();
			
			///tells if the system has been initialized
			bool initialized();
			
			std::vector<VideoMode> getSupportedVideoModes();
			VideoMode getDefaultWindowSize();
			
		private:
			bool sdlMainInitialized = false;
			bool imageInitialized = false;
			bool fontInitialized = false;
			bool soundInitialized = false;
	};
	
} //end namespace multimedia

#endif /* BRS_SDL_SYSTEM_HPP */
