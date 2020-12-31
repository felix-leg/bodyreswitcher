#pragma once

#ifndef BRS_NEW_GAME_MENU_HPP
#define BRS_NEW_GAME_MENU_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"

namespace game_logic {
	
	namespace new_game_menu {
		
		void paint(multimedia::Surface &destSurf);
		void mouseAction(int mouseX, int mouseY, bool mouseClick);
		
	}//end namespace new_game_menu
	
	Module NewGameMenuInit(multimedia::Window *mainWindow);
	
}//end namespace game_logic

#endif /* BRS_NEW_GAME_MENU_HPP */
