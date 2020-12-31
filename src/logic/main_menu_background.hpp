#pragma once

#ifndef BRS_BACKGROUND_HPP
#define BRS_BACKGROUND_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"

namespace game_logic {
	
	namespace mm_background {
		void paint(multimedia::Surface &destSurf);
	}//end namespace mm_background
	
	Module MainMenuBackgroundInit(multimedia::Window *mainWindow);
	
}//end namespace game_logic

#endif /* BRS_BACKGROUND_HPP */

