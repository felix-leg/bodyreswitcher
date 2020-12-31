/**
 * Menu for choosing a new game by difficultly 
*/

#include "new_game_menu.hpp"
#include "buttons.hpp"
#include <string>
#include "mls.hpp"
#include "common.hpp"

#include "game_panel.hpp"

namespace game_logic {
	
	namespace new_game_menu {
		
		//the menu
		Menu *theMenu = nullptr;
		std::string easyText, normalText, hardText, backText;
		void makeMenu();
		Button *easyButton = nullptr;
		Button *normalButton = nullptr;
		Button *hardButton = nullptr;
		Button *backButton = nullptr;
		
		//drawing functions
		multimedia::Window *theWindow;
		multimedia::DirtyFlag *repaintFlag;
		void paint(multimedia::Surface &destSurf) {
			if( repaintFlag->needsRedraw() ) {
				makeMenu();
			}
			if( easyButton != nullptr ) easyButton->paint(destSurf);
			if( normalButton != nullptr ) normalButton->paint(destSurf);
			if( hardButton != nullptr ) hardButton->paint(destSurf);
			if( backButton != nullptr ) backButton->paint(destSurf);
		}
		
		void makeMenu() {
			if( theMenu != nullptr ) delete theMenu;
			
			theMenu = new Menu(theWindow,{
				easyText,
				normalText, 
				hardText, 
				backText
			});
			
			easyButton = theMenu->getButton(0);
			normalButton = theMenu->getButton(1);
			hardButton = theMenu->getButton(2);
			backButton = theMenu->getButton(3);
			
			easyButton->onClickHandler([](){
				game_panel::setGameDifficultly(game_panel::GameDifficultly::EASY);
				theGameState = GameState::PreGame;
			});
			
			normalButton->onClickHandler([](){
				game_panel::setGameDifficultly(game_panel::GameDifficultly::NORMAL);
				theGameState = GameState::PreGame;
			});
			
			hardButton->onClickHandler([](){
				game_panel::setGameDifficultly(game_panel::GameDifficultly::HARD);
				theGameState = GameState::PreGame;
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
		
	}//end namespace new_game_menu
	
	Module NewGameMenuInit(multimedia::Window *mainWindow) {
		using namespace new_game_menu;
		
		theWindow = mainWindow;
		repaintFlag = theWindow->createDirtyFlag();
		
		easyText = _("%{#Game difficulty#}%Easy").get();
		normalText = _("%{#Game difficulty#}%Normal").get();
		hardText = _("%{#Game difficulty#}%Hard").get();
		backText = _("%{#Close menu#}%Back").get();
		
		return Module(unloadModule);
	}
	
}//end namespace game_logic

