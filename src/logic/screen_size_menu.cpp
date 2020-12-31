/**
 * Menu for changing screen sizes
*/

#include "screen_size_menu.hpp"
#include "buttons.hpp"
#include <string>
#include <array>
#include "mls.hpp"
#include "common.hpp"

namespace game_logic {
	
	namespace screen_menu {
		
		//menu
		void makeMenu();
		Menu *theMenu = nullptr;
		std::string theLongestTextModeString, prevText, nextText, cancelText;
		Button *prevButton = nullptr;
		Button *nextButton = nullptr;
		Button *cancelButton = nullptr;
		
		//video modes list
		std::vector<multimedia::VideoMode> videoModes;
		///how much video modes display per page
		constexpr std::size_t  modesPerPage = 4;
		std::size_t pageNumber = 0;
		std::array<Button*, modesPerPage> modeButtons;
		void setModeTexts();
		void switchModeToIndex(std::size_t index);
		
		//draw 
		multimedia::Window *theWindow;
		multimedia::DirtyFlag *repaintFlag;
		void paint(multimedia::Surface &destSurf) {
			if( repaintFlag->needsRedraw() ) {
				makeMenu();
			}
			if( prevButton != nullptr ) prevButton->paint(destSurf);
			if( nextButton != nullptr ) nextButton->paint(destSurf);
			if( cancelButton != nullptr ) cancelButton->paint(destSurf);
			for(auto& button : modeButtons) button->paint(destSurf);
		}
		
		void makeMenu() {
			if( theMenu != nullptr ) delete theMenu;
			
			theMenu = new Menu(theWindow,{
				prevText,
				"1", "2", "3", "4",//<-- depends on modesPerPage
				nextText,
				cancelText
			},{
				prevText,
				nextText,
				cancelText,
				theLongestTextModeString
			});
			
			prevButton = theMenu->getButton(0);
			for(std::size_t i=0; i<modesPerPage; ++i) {
				modeButtons[i] = theMenu->getButton(1+i);
				modeButtons[i]->onClickHandler([i](){
					switchModeToIndex(i);
				});
			}
			nextButton = theMenu->getButton(modesPerPage+1);
			cancelButton = theMenu->getButton(modesPerPage+2);
			
			setModeTexts();
			
			prevButton->onClickHandler([](){
				--pageNumber;
				setModeTexts();
			});
			
			nextButton->onClickHandler([](){
				++pageNumber;
				setModeTexts();
			});
			
			cancelButton->onClickHandler([](){
				theGameState = GameState::OptionsMenu;
			});
		}
		
		void setModeTexts() {
			std::size_t modeIndex = pageNumber * modesPerPage;
			if( pageNumber == 0 ) prevButton->enabled = false;
			else prevButton->enabled = true;
			
			//write text on buttons
			for(auto modeButton : modeButtons) {
				if( modeIndex < videoModes.size() ) {
					modeButton->enabled = true;
					modeButton->setText( videoModes[modeIndex].to_string() );
				} else {
					modeButton->enabled = false;
				}
				++modeIndex;
			}
			
			if( modeIndex >= videoModes.size() ) nextButton->enabled = false;
			else nextButton->enabled = true;
		}
		
		void switchModeToIndex(std::size_t index) {
			std::size_t modeIndex = pageNumber * modesPerPage + index;
			
			theWindow->switchToVideoMode( videoModes[modeIndex] );
		}
		
		void mouseAction(int mouseX, int mouseY, bool mouseClick) {
			if( theMenu == nullptr ) return;
			theMenu->mouseActions(mouseX, mouseY, mouseClick);
		}
		
		void unloadModule() {
			delete theMenu;
		}
		
	}//end namespace screen_menu
	
	Module ScreenSizeMenuInit(multimedia::Window *mainWindow, 
		std::vector<multimedia::VideoMode> vModes) {
		using namespace screen_menu;
		
		theWindow = mainWindow;
		repaintFlag = theWindow->createDirtyFlag();
		videoModes = vModes;
		
		prevText = _("%{#Video modes list#}%<- Previous").get();
		nextText = _("%{#Video modes list#}%Next ->").get();
		cancelText = _("%{#Video modes list#}%Cancel").get();
		
		std::vector<std::string> videoModesStrings;
		for(auto &mode : vModes) {
			videoModesStrings.push_back( mode.to_string() );
		}
		theLongestTextModeString = multimedia::getTheLongestString(videoModesStrings);
		
		return Module(unloadModule);
	}
	
}//end namespace game_logic
