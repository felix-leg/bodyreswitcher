#pragma once

#ifndef LOGIC_BUTTONS_HPP
#define LOGIC_BUTTONS_HPP

#include "multimedia/window.hpp"
#include "multimedia/font.hpp"
#include "multimedia/sound.hpp"
#include "rect.hpp"
#include "ui.hpp"
#include <string>

namespace game_logic {
	
	class Button;
	
	const multimedia::Color activeFontColor{0,255,0};
	const multimedia::Color inactiveFontColor{200,200,200};
	
	const std::string buttonBeepFilename = "menu_beep.ogg";
	
	//based on the "MenuButton_[in]active.png" sizes
	const double MenuButtonWidthPercent = 0.796;
	const double MenuButtonHeightPercent = 0.694736842;
	//based on the "GM_[in]active.png"
	const double GM_ButtonWidthPercent = 0.88;
	const double GM_ButtonHeightPercent = 0.785964912;
	///
	class ButtonTemplate {
		const std::string defaultFontFileName = "SpecialElite-Regular.ttf";
		multimedia::SoundSample beepSound{buttonBeepFilename};
		
		public:
			///ctor
			ButtonTemplate(std::string activeFileName, 
			std::string inactiveFileName, 
			double widthPercent,
			double heightPercent);
			
			void reloadAssetsForParametersH(int buttonHeight, std::string theLongestString);
			int reloadAssetsForParametersW(int buttonWidth, std::string theLongestString);
			
			Button makeButton(std::string textOnButton, UI::Space* clickSpace);
			multimedia::Font drawingFont;
		private:
			std::string activeFile;
			std::string inactiveFile;
			//how much of % of the template image to use for text
			double widthP;
			double heightP;
			
			friend class Button;
			multimedia::Surface activeSurf;
			multimedia::Surface inactiveSurf;
	};//end class ButtonTemplate
	
	class Button {
		public:
			Button(std::string textOnButton = "", ButtonTemplate *templ = nullptr, UI::Space* clickSpace = nullptr);
			///move operator
			//Button& operator=(Button&& other);
			void onClickHandler(std::function<void()> callback);
			void assignEvents();
			
			void setPosition(int x, int y, bool isCenter);
			void setText(std::string newText);
			void paint(multimedia::Surface& destSurf);
			
			bool enabled;
		private:
			ButtonTemplate *buttonTempl;
			multimedia::Surface activeTextSurf;
			multimedia::Surface inactiveTextSurf;
			Rect position;
			Rect activeTextPos;
			Rect inactiveTextPos;
			UI::Component *clickable;
			bool isMouseOver = false;
			std::function<void()> onClick;
	};//end class Button
	
}//end namespace game_logic

#endif /* LOGIC_BUTTONS_HPP */
