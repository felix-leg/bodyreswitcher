/**
 * Main Menu Module
*/

#include "main_menu.hpp"
#include "buttons.hpp"
#include "common.hpp"
#include "mls.hpp"

namespace game_logic {
	
	namespace main_menu {
		
		//buttons of the menu
		Button *AboutButton = nullptr;
		Button *NewGameButton = nullptr;
		Button *OptionsButton = nullptr;
		Button *ExitButton = nullptr;
		Menu *theMenu = nullptr;
		
		void makeMenu();
		void mouseAction(int mouseX, int mouseY, bool mouseClick) {
			if( theMenu == nullptr ) return;
			theMenu->mouseActions(mouseX, mouseY, mouseClick);
		}
		
		//drawing functions
		multimedia::Window *theWindow;
		multimedia::DirtyFlag *repaintFlag;
		void paint(multimedia::Surface &destSurf) {
			if( repaintFlag->needsRedraw() ) {
				makeMenu();
			}
			
			if( AboutButton != nullptr ) AboutButton->paint(destSurf);
			if( NewGameButton != nullptr ) NewGameButton->paint(destSurf);
			if( OptionsButton != nullptr ) OptionsButton->paint(destSurf);
			if( ExitButton != nullptr ) ExitButton->paint(destSurf);
		}
		
		void makeMenu() {
			if( theMenu != nullptr ) delete theMenu;
			
			theMenu = new Menu(theWindow, {
				_("%{#Main menu#}%What it's about?").get(),
				_("%{#Main menu#}%New game").get(),
				_("%{#Main menu#}%Options").get(),
				_("%{#Main menu#}%Exit").get()
			});
			
			AboutButton = theMenu->getButton(0);
			NewGameButton = theMenu->getButton(1);
			OptionsButton = theMenu->getButton(2);
			ExitButton = theMenu->getButton(3);
			
			AboutButton->onClickHandler([](){
				theGameState = GameState::AboutMenu;
			});
			
			NewGameButton->onClickHandler([](){
				theGameState = GameState::DiffMenu;
			});
			
			OptionsButton->onClickHandler([](){
				theGameState = GameState::OptionsMenu;
			});
			
			ExitButton->onClickHandler([](){
				throw multimedia::SignaledGameEnd();
			});
		}
		
		//clean function
		void unloadModule() {
			delete theMenu;
		}
		
	}//end namespace main_menu
	
	Module MainMenuInit(multimedia::Window *mainWindow) {
		using namespace main_menu;
		repaintFlag = mainWindow->createDirtyFlag();
		theWindow = mainWindow;
		
		return Module(unloadModule);
	}
	
}//end namespace game_logic
