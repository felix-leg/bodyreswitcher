#pragma once

#ifndef SCREEN_SIZE_MENU_HPP
#define SCREEN_SIZE_MENU_HPP

#include "layer.hpp"
#include "multimedia/system.hpp"
#include "buttons.hpp"
#include "ui.hpp"
#include <vector>
#include <array>
#include <string>

namespace game_logic {
	///how much video modes display per page
	constexpr std::size_t  modesPerPage = 4;
	
	class ScreenSizeMenu : public Layer {
		public:
			ScreenSizeMenu(multimedia::Window *mainWindow, std::vector<multimedia::VideoMode> vModes);
			~ScreenSizeMenu();
			
			virtual void paint(multimedia::Surface& destSurf) override;
			virtual void mouseOver(int newMouseX, int newMouseY) override;
			virtual void mouseClick() override;
			virtual bool isActive() override;
			
			void enable();
		private:
			multimedia::Window *theWindow;
			std::vector<multimedia::VideoMode> videoModes;
			/**
			 * if this flag differs from the window redraw flag, 
			 * it means this object must redraw its assets
			 */
			char ownRedrawFlag;
			
			//buttons
			UI::Space *clickSpace = nullptr;
			ButtonTemplate buttonTempl;
			std::size_t pageNumber = 0;
			void makeButtons();
			Button prevButton;
			Button nextButton;
			Button cancelButton;
			
			void setModeTexts();
			void switchModeToIndex(std::size_t index);
			std::array<Button, modesPerPage> modeButtons;
			bool displayPrev = false;
			bool displayNext = false;
			std::array<bool, modesPerPage> displayModes;
			
			std::string prevText;
			std::string nextText;
			std::string cancelText;
			///the longest text required for display on buttons
			std::string theLongestText;
			
			bool isMenuActive;
	};//end class ScreenSizeMenu
	
}//end namespace game_logic

#endif /* SCREEN_SIZE_MENU_HPP */

