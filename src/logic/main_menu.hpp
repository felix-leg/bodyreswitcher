#pragma once

#ifndef LOGIC_MAIN_MENU_HPP
#define LOGIC_MAIN_MENU_HPP


#include "multimedia/window.hpp"
#include "rect.hpp"
#include "buttons.hpp"
#include "ui.hpp"

#include "layer.hpp"

namespace game_logic {
	
	class MainMenu : public Layer {
		public:
		MainMenu(multimedia::Window *mainWindow);
		~MainMenu();
		
		virtual void paint(multimedia::Surface& destSurf) override;
		virtual void mouseOver(int newMouseX, int newMouseY) override;
		virtual void mouseClick() override;
		virtual bool isActive() override;
		
		//events
		void setExitEvent(std::function<void()>);
		void setOptionsEvent(std::function<void()>);
		void setAboutEvent(std::function<void()>);
		void setNewGameEvent(std::function<void()>);
		
		private:
			multimedia::Window *theWindow;
			/**
			 * if this flag differs from the window redraw flag, 
			 * it means this object must redraw its assets
			 */
			char ownRedrawFlag;
			
			//background surface
			void makeBackgroundSurface();
			multimedia::Surface background;
			
			//buttons
			void makeButtons();
			UI::Space * clickSpace;
			ButtonTemplate buttonTempl;
			Button aboutButton;
			Button newGameButton;
			Button optionsButton;
			Button exitButton;
			
			//events
			std::function<void()> exitEvent;
			std::function<void()> optionsEvent;
			std::function<void()> aboutEvent;
			std::function<void()> newGameEvent;
		
	};//end class MainMenu
	
}//end namespace game_logic

#endif /* LOGIC_MAIN_MENU_HPP */
