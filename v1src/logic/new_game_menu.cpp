/***/

#include "new_game_menu.hpp"
#include "mls.hpp"

namespace game_logic {
	
	NewGameMenu::NewGameMenu(multimedia::Window *mainWindow):
	theWindow(mainWindow),
	buttonTempl("MenuButton_active.png","MenuButton_inactive.png",
	MenuButtonWidthPercent, MenuButtonHeightPercent) {
		ownRedrawFlag = theWindow->redrawFlag * -1;
		
		clickSpace = nullptr;
		isEnabled = false;
		
		easyText = _("%{#Game difficulty#}%Easy").get();
		normalText = _("%{#Game difficulty#}%Normal").get();
		hardText = _("%{#Game difficulty#}%Hard").get();
		backText = _("%{#Close menu#}%Back").get();
		
		theLongestText = multimedia::getTheLongestString({
			easyText, normalText, hardText, backText
		});
		
		setGameEvent = nullptr;
	}
	
	NewGameMenu::~NewGameMenu() {
		if( clickSpace != nullptr ) delete clickSpace;
	}
	
	void NewGameMenu::setNewGameEvent(std::function<void(GameDifficulty)> callback) {
		setGameEvent = callback;
	}
	
	bool NewGameMenu::isActive() {
		return isEnabled;
	}
	
	void NewGameMenu::enable() {
		isEnabled = true;
	}
	
	void NewGameMenu::mouseOver(int newMouseX, int newMouseY) {
		if( clickSpace != nullptr ) {
			clickSpace->mouseMovedTo(newMouseX, newMouseY);
		}
	}
	
	void NewGameMenu::mouseClick() {
		if( clickSpace != nullptr ) {
			clickSpace->mouseClick();
		}
	}
	
	void NewGameMenu::paint(multimedia::Surface& destSurf) {
		if( ownRedrawFlag != theWindow->redrawFlag ) {
			makeButtons();
			//assets ready
			ownRedrawFlag = theWindow->redrawFlag;
		}
		easyButton.paint(destSurf);
		normalButton.paint(destSurf);
		hardButton.paint(destSurf);
		backButton.paint(destSurf);
	}
	
	void NewGameMenu::makeButtons() {
		constexpr int buttonSpacing = 5;//button spacing in pixels
		
		int totalButtonsHeight = theWindow->getHeight() * 0.4;
		int standardButtonHeight = (totalButtonsHeight / 4) - buttonSpacing;
		int theBottomButtonPosition = (theWindow->getHeight() * 0.7) - standardButtonHeight/2;
		buttonTempl.reloadAssetsForParametersH(standardButtonHeight, theLongestText);
		
		//make buttons
		if( clickSpace != nullptr ) delete clickSpace;
		clickSpace = new UI::Space(0,0,theWindow->getWidth(),theWindow->getHeight());
		easyButton = buttonTempl.makeButton(easyText, clickSpace);
		normalButton = buttonTempl.makeButton(normalText, clickSpace);
		hardButton = buttonTempl.makeButton(hardText, clickSpace);
		backButton = buttonTempl.makeButton(backText, clickSpace);
		
		//setup buttons
		easyButton.assignEvents();
		easyButton.onClickHandler([this](){
			if( setGameEvent != nullptr ) setGameEvent(GameDifficulty::EASY);
			isEnabled = false;
		});
		normalButton.assignEvents();
		normalButton.onClickHandler([this](){
			if( setGameEvent != nullptr ) setGameEvent(GameDifficulty::NORMAL);
			isEnabled = false;
		});
		hardButton.assignEvents();
		hardButton.onClickHandler([this](){
			if( setGameEvent != nullptr ) setGameEvent(GameDifficulty::HARD);
			isEnabled = false;
		});
		backButton.assignEvents();
		backButton.onClickHandler([this]() {
			isEnabled = false;
		});
		
		//position buttons
		int posX = theWindow->getWidth() / 2;
		int posY = theBottomButtonPosition;
		//--back
		backButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--hard
		hardButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--normal
		normalButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--easy
		easyButton.setPosition(posX, posY, true);
	}
	
}//end namespace game_logic


