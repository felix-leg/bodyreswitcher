#pragma once

#ifndef NEW_GAME_MENU_HPP
#define NEW_GAME_MENU_HPP

#include "multimedia/window.hpp"
#include "buttons.hpp"
#include "layer.hpp"
#include "ui.hpp"
#include <string>

namespace game_logic {
	
	enum class GameDifficulty {
		EASY,
		NORMAL,
		HARD
	};
	
	class NewGameMenu : public Layer {
		public:
			NewGameMenu(multimedia::Window *mainWindow);
			~NewGameMenu();
			
			virtual void paint(multimedia::Surface& destSurf) override;
			virtual void mouseOver(int newMouseX, int newMouseY) override;
			virtual void mouseClick() override;
			virtual bool isActive() override;
			
			void enable();
			void setNewGameEvent(std::function<void(GameDifficulty)> callback);
		private:
			multimedia::Window *theWindow;
			/**
			 * if this flag differs from the window redraw flag, 
			 * it means this object must redraw its assets
			 */
			char ownRedrawFlag;
			
			//buttons
			void makeButtons();
			UI::Space * clickSpace;
			ButtonTemplate buttonTempl;
			Button easyButton;
			Button normalButton;
			Button hardButton;
			Button backButton;
			
			//texts on buttons
			std::string easyText;
			std::string normalText;
			std::string hardText;
			std::string backText;
			//the longest text from above
			std::string theLongestText;
			
			bool isEnabled;
			std::function<void(GameDifficulty)> setGameEvent;
			
	};//end class NewGameMenu
	
}//end namespace game_logic

#endif /* NEW_GAME_MENU_HPP */

