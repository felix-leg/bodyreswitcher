#pragma once

#ifndef LOGIC_OPTIONS_HPP
#define LOGIC_OPTIONS_HPP

#include "multimedia/window.hpp"
#include "multimedia/sound.hpp"
#include "rect.hpp"
#include "buttons.hpp"
#include "ui.hpp"

#include "layer.hpp"

namespace game_logic {
	
	class OptionsMenu : public Layer {
		public:
			OptionsMenu(multimedia::Window *mainWindow, multimedia::Music *music);
			~OptionsMenu();
				
			virtual void paint(multimedia::Surface& destSurf) override;
			virtual void mouseOver(int newMouseX, int newMouseY) override;
			virtual void mouseClick() override;
			virtual bool isActive() override;
			
			void enable();
			void setScreenSizeEvent(std::function<void()> callback);
		private:
			multimedia::Window *theWindow;
			multimedia::Music *mainMusic;
			/**
			 * if this flag differs from the window redraw flag, 
			 * it means this object must redraw its assets
			 */
			char ownRedrawFlag;
			
			
			//buttons
			UI::Space *clickSpace;
			ButtonTemplate buttonTempl;
			void makeButtons();
			Button backButton;
			Button fullscreenButton;
			Button musicButton;
			Button screenSizeButton;
			
			std::string musicOnText;
			std::string musicOffText;
			std::string fullscreenOnText;
			std::string fullscreenOffText;
			std::string screenSizeText;
			std::string backText;
			//the longest text from above list
			std::string theLongestText;
			
			bool isMenuActive = false;
			std::function<void()> screenSizeEvent = nullptr;
			
	};//end class OptionsMenu
	
}//end namespace game_logic

#endif /* LOGIC_OPTIONS_HPP */

