#pragma once

#ifndef BRS_SCREEN_MENU_HPP
#define BRS_SCREEN_MENU_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"

namespace game_logic {
	
	namespace screen_menu {
		
		void paint(multimedia::Surface &destSurf);
		void mouseAction(int mouseX, int mouseY, bool mouseClick);
		
	}//end namespace screen_menu
	
	Module ScreenSizeMenuInit(multimedia::Window *mainWindow,
			std::vector<multimedia::VideoMode> vModes);
	
}//end namespace game_logic

#endif /* BRS_SCREEN_MENU_HPP */
