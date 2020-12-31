/**
 * The game's main menu logic
*/

#include "main_menu.hpp"
#include "buttons.hpp"
#include "mls.hpp"

namespace game_logic {
	
	MainMenu::MainMenu(multimedia::Window *mainWindow):
	theWindow(mainWindow),
	buttonTempl("MenuButton_active.png","MenuButton_inactive.png",
	MenuButtonWidthPercent, MenuButtonHeightPercent)
	 {
		ownRedrawFlag = mainWindow->redrawFlag * -1;// * -1 => to force redrawing
		
		clickSpace = nullptr;
		exitEvent = nullptr;
		optionsEvent = nullptr;
		aboutEvent = nullptr;
		newGameEvent = nullptr;
	}
	
	MainMenu::~MainMenu() {
		if( clickSpace != nullptr ) delete clickSpace;
	}
	
	void MainMenu::paint(multimedia::Surface& destSurf) {
		//check if we have to redraw our assets
		if( ownRedrawFlag != theWindow->redrawFlag ) {
			makeBackgroundSurface();
			makeButtons();
			//assets ready — set the flag
			ownRedrawFlag = theWindow->redrawFlag;
		}
		//paint the background first
		destSurf.blit(0,0, background);
		//then buttons
		aboutButton.paint(destSurf);
		newGameButton.paint(destSurf);
		optionsButton.paint(destSurf);
		exitButton.paint(destSurf);
	}
	
	///called when a mouse moves over the menu
	void MainMenu::mouseOver(int newMouseX, int newMouseY) {
		if( clickSpace != nullptr ) {
			clickSpace->mouseMovedTo(newMouseX, newMouseY);
		}
	}
	
	///called when a mouse clicks over the menu
	void MainMenu::mouseClick() {
		if( clickSpace != nullptr ) {
			clickSpace->mouseClick();
		}
	}
	
	bool MainMenu::isActive() {
		return true; //main menu is always active
	}
	
	void MainMenu::makeBackgroundSurface() {
		if( ownRedrawFlag == theWindow->redrawFlag ) return;
		
		using namespace multimedia;
		Rect r{0,0,0,0};
		const int screenWidth = theWindow->getWidth();
		const int screenHeight = theWindow->getHeight();
		
		background = Surface::newRGB_Surface(screenWidth, screenHeight);
		
		//::preprare left part
		Surface leftPart = Surface::fromFile("MM_left.png");
		r = leftPart.getSize();
		r.setHeightWithConstRatio( screenHeight );
		leftPart.scale(r);
		//crop parts after the middle of the screen
		if( r.getWidth() > screenWidth/2 ) {
			r.setWidth( screenWidth / 2 );
			leftPart.crop(r);
		}
		//draw on the background
		background.blit(0,0, leftPart);
		
		//::prepare right part
		Surface rightPart = Surface::fromFile("MM_right.png");
		r = rightPart.getSize();
		r.setHeightWithConstRatio( screenHeight );
		rightPart.scale(r);
		//crop parts after the middle of the screen
		if( r.getWidth() > screenWidth/2 ) {
			r.setWidth( screenWidth / 2 );
			r.setRight( rightPart.getSize().getRight() );
			rightPart.crop(r);
		}
		r.setRight( screenWidth );
		background.blit(r.getLeft(),0, rightPart);
		
		//::prepare the middle menu background
		Surface menuBack = Surface::fromFile("MM_middle.png");
		r = menuBack.getSize();
		r.setHeightWithConstRatio( screenHeight );
		menuBack.scale(r);
		//position the menu background on the middle of the screen
		r.setCenterX( screenWidth / 2 );
		//draw the menu background
		background.blit( r.getLeft(), r.getTop(), menuBack );
		
		//::prepare the title
		Surface title = Surface::fromFile("MM_Title.png");
		r = title.getSize();
		//scale title to about 0.3 of the screen height
		r.setHeightWithConstRatio( screenHeight * 0.323148148 );
		title.scale(r);
		//position the title in the top middle of the screen
		r.setCenterX( screenWidth / 2 );
		//draw on the background
		background.blit( r.getLeft(), r.getTop(), title);
		
		//optimize for drawing on the screen
		background.optimizeForDrawingOn(theWindow->getWindowSurface());
		//background ready
		
	}
	
	void MainMenu::makeButtons() {
		std::string AboutText = _("%{#Main menu#}%What it's about?").get();
		std::string NewGameText = _("%{#Main menu#}%New game").get();
		std::string OptionsText = _("%{#Main menu#}%Options").get();
		std::string ExitText = _("%{#Main menu#}%Exit").get();
		
		constexpr int buttonSpacing = 5;//button spacing in pixels
		
		//parameters for button template
		std::string theLongestText = multimedia::getTheLongestString({
			AboutText, NewGameText, OptionsText, ExitText
		});
		int totalButtonsHeight = theWindow->getHeight() * 0.4;
		int standardButtonHeight = (totalButtonsHeight / 4) - buttonSpacing;
		int theBottomButtonPosition = (theWindow->getHeight() * 0.7) - standardButtonHeight/2;
		buttonTempl.reloadAssetsForParametersH(standardButtonHeight, theLongestText);
		
		//make buttons
		if( clickSpace != nullptr ) delete clickSpace;
		clickSpace = new UI::Space(0,0,theWindow->getWidth(),theWindow->getHeight());
		aboutButton = buttonTempl.makeButton(AboutText, clickSpace);
		newGameButton = buttonTempl.makeButton(NewGameText, clickSpace);
		optionsButton = buttonTempl.makeButton(OptionsText, clickSpace);
		exitButton = buttonTempl.makeButton(ExitText, clickSpace);
		
		//setup buttons
		aboutButton.assignEvents();
		aboutButton.onClickHandler(aboutEvent);
		newGameButton.assignEvents();
		newGameButton.onClickHandler(newGameEvent);
		optionsButton.assignEvents();
		optionsButton.onClickHandler(optionsEvent);
		exitButton.assignEvents();
		exitButton.onClickHandler(exitEvent);
		
		//position buttons
		int posX = theWindow->getWidth() / 2;
		int posY = theBottomButtonPosition;
		//--exit
		exitButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--options
		optionsButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--new game
		newGameButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--about
		aboutButton.setPosition(posX, posY, true);
	}
	
	void MainMenu::setExitEvent(std::function<void()> callback) {
		exitEvent = callback;
	}
	
	void MainMenu::setOptionsEvent(std::function<void()> callback) {
		optionsEvent = callback;
	}
	
	void MainMenu::setAboutEvent(std::function<void()> callback) {
		aboutEvent = callback;
	}
	
	void MainMenu::setNewGameEvent(std::function<void()> callback) {
		newGameEvent = callback;
	}
	
}//end namespace game_logic

