#pragma once

#ifndef BRS_OPTION_MENU_HPP
#define BRS_OPTION_MENU_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"
#include "multimedia/sound.hpp"

namespace game_logic {
	
	namespace options_menu {
		
		void paint(multimedia::Surface &destSurf);
		void mouseAction(int mouseX, int mouseY, bool mouseClick);
		
	}//end namespace options_menu
	
	Module OptionsMenuInit(multimedia::Window *mainWindow, multimedia::Music *mainMusic);
	
}//end namespace game_logic

#endif /* BRS_OPTION_MENU_HPP */
