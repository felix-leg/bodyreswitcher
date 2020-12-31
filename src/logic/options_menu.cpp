/**
 * Menu for selecting options like music, fullscreen, and screen size
*/

#include "options_menu.hpp"
#include "buttons.hpp"
#include <string>
#include "mls.hpp"
#include "common.hpp"

namespace game_logic {
	
	namespace options_menu {
		
		//music
		multimedia::Music *theMusic;
		//the menu
		Menu *theMenu = nullptr;
		std::string musicOnText, musicOffText, 
					fullscreenOnText, fullscreenOffText, 
					screenSizeText, backText;
		Button *musicButton = nullptr;
		Button *fullscreenButton = nullptr;
		Button *screenSizeButton = nullptr;
		Button *backButton = nullptr;
		
		//drawing functions
		multimedia::Window *theWindow;
		multimedia::DirtyFlag *repaintFlag;
		void makeMenu();
		void paint(multimedia::Surface &destSurf) {
			if( repaintFlag->needsRedraw() ) {
				makeMenu();
			}
			if( musicButton != nullptr ) musicButton->paint(destSurf);
			if( fullscreenButton != nullptr ) fullscreenButton->paint(destSurf);
			if( screenSizeButton != nullptr ) screenSizeButton->paint(destSurf);
			if( backButton != nullptr ) backButton->paint(destSurf);
		}
		
		void makeMenu() {
			if( theMenu != nullptr ) delete theMenu;
			
			theMenu = new Menu(theWindow, {
				( theMusic->isPlaying() ? musicOnText : musicOffText ),
				( theWindow->isFullscreen() ? fullscreenOnText : fullscreenOffText ),
				screenSizeText,
				backText
			},{
				musicOnText, 
				musicOffText, 
				fullscreenOnText, 
				fullscreenOffText, 
				screenSizeText, 
				backText
			});
			
			musicButton = theMenu->getButton(0);
			fullscreenButton = theMenu->getButton(1);
			screenSizeButton = theMenu->getButton(2);
			backButton = theMenu->getButton(3);
			
			musicButton->onClickHandler([](){
				if( theMusic->isPlaying() ) {
					theMusic->stop();
					musicButton->setText( musicOffText );
				} else {
					theMusic->play();
					musicButton->setText( musicOnText );
				}
			});
			
			fullscreenButton->onClickHandler([](){
				theWindow->switchFullscreen();
				if( theWindow->isFullscreen() ) {
					fullscreenButton->setText( fullscreenOnText );
				} else {
					fullscreenButton->setText( fullscreenOffText );
				}
			});
			
			screenSizeButton->onClickHandler([](){
				theGameState = GameState::ScreenMenu;
			});
			
			backButton->onClickHandler([](){
				theGameState = GameState::MainMenu;
			});
		}
		
		void mouseAction(int mouseX, int mouseY, bool mouseClick) {
			if( theMenu == nullptr ) return;
			theMenu->mouseActions(mouseX, mouseY, mouseClick);
		}
		
		void unloadModule() {
			delete theMenu;
		}
		
	}//end namespace options_menu
	
	Module OptionsMenuInit(multimedia::Window *mainWindow, multimedia::Music *mainMusic) {
		using namespace options_menu;
		
		theWindow = mainWindow;
		theMusic = mainMusic;
		repaintFlag = mainWindow->createDirtyFlag();
		
		musicOnText = _("Music: On").get();
		musicOffText = _("Music: Off").get();
		fullscreenOnText = _("Fullscreen: On").get();
		fullscreenOffText = _("Fullscreen: Off").get();
		screenSizeText = _("Set screen size").get();
		backText = _("%{#Back to main menu#}%Back").get();
		
		return Module(unloadModule);
	}
	
}//end namespace game_logic

