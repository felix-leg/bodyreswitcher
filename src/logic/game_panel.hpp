#pragma once

#ifndef BRS_GAME_PANEL_HPP
#define BRS_GAME_PANEL_HPP

#include "module.hpp"
#include "multimedia/graphics.hpp"

namespace game_logic {
	
	namespace game_panel {
		
		void paint(multimedia::Surface &destSurf);
		void mouseAction(int mouseX, int mouseY, bool mouseClick);
		
		enum class GameDifficultly {
			EASY,
			NORMAL,
			HARD
		};
		
		void setGameDifficultly(GameDifficultly diff);
		void initializeGame();
		
	}//end namespace game_panel
	
	Module GamePanelInit(multimedia::Window *mainWindow);
	
}//end namespace game_logic

#endif /* BRS_GAME_PANEL_HPP */
