#pragma once

#ifndef ABOUT_SCREEN_HPP
#define ABOUT_SCREEN_HPP

#include "buttons.hpp"
#include "ui.hpp"
#include "layer.hpp"
#include "multimedia/font.hpp"

namespace game_logic {
	
	class About : public Layer {
		public:
			About(multimedia::Window *mainWindow);
			~About();
			
			virtual void paint(multimedia::Surface& destSurf) override;
			virtual void mouseOver(int newMouseX, int newMouseY) override;
			virtual void mouseClick() override;
			virtual bool isActive() override;
			
			void enable();
		private:
			multimedia::Window *theWindow;
			/**
			 * if this flag differs from the window redraw flag, 
			 * it means this object must redraw its assets
			 */
			char ownRedrawFlag;
			bool isEnabled;
			
			//background surface
			void makeBackgroundSurface();
			multimedia::Surface background;
			//multimedia::Font aboutFont; //needed only once
			
			//about texts
			std::string introText;
			std::string rulesText;
			std::string outroText;
			
			std::string backText;
			
			//buttons
			void makeButtons();
			UI::Space * clickSpace;
			ButtonTemplate buttonTempl;
			Button backButton;
	};//end class About
	
}//end namespace game_logic

#endif /* ABOUT_SCREEN_HPP */

