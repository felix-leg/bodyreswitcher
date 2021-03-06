#pragma once

#ifndef BRS_MAIN_MENU_HPP
#define BRS_MAIN_MENU_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"

namespace game_logic {
	
	namespace main_menu {
		
		void paint(multimedia::Surface &destSurf);
		void mouseAction(int mouseX, int mouseY, bool mouseClick);
		
	}//end namespace main_menu
	
	Module MainMenuInit(multimedia::Window *mainWindow);
	
}//end namespace game_logic

#endif /* BRS_MAIN_MENU_HPP */
