#pragma once

#ifndef BRS_ABOUT_MENU_HPP
#define BRS_ABOUT_MENU_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"

namespace game_logic {
	
	namespace about_menu {
		
		void paint(multimedia::Surface &destSurf);
		void mouseAction(int mouseX, int mouseY, bool mouseClick);
		
	}//end namespace about_menu
	
	Module AboutMenuInit(multimedia::Window *mainWindow);
	
}//end namespace game_logic

#endif /* BRS_ABOUT_MENU_HPP */
