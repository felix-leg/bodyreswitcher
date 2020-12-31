/**
 * Provides the Options menu for the main menu.
*/

#include "options_menu.hpp"
#include "mls.hpp"


namespace game_logic {
	
	OptionsMenu::OptionsMenu(multimedia::Window *mainWindow, multimedia::Music *music):
		theWindow(mainWindow),
		mainMusic(music),
		isMenuActive(false),
		buttonTempl("MenuButton_active.png","MenuButton_inactive.png",
		MenuButtonWidthPercent, MenuButtonHeightPercent) {
			ownRedrawFlag = theWindow->redrawFlag * -1;//* -1 => to force redraw
			
			clickSpace = nullptr;
			musicOnText = _("Music: On").get();
			musicOffText = _("Music: Off").get();
			fullscreenOnText = _("Fullscreen: On").get();
			fullscreenOffText = _("Fullscreen: Off").get();
			screenSizeText = _("Set screen size").get();
			backText = _("%{#Back to main menu#}%Back").get();
			
			theLongestText = multimedia::getTheLongestString({
				musicOnText, musicOffText, fullscreenOnText, fullscreenOffText,
				screenSizeText, backText
			});
		}
	
	OptionsMenu::~OptionsMenu() {
		if( clickSpace != nullptr ) delete clickSpace;
	}
	
	bool OptionsMenu::isActive() {
		return isMenuActive;
	}
	///activates the menu
	void OptionsMenu::enable() {
		isMenuActive = true;
	}
	
	void OptionsMenu::mouseOver(int mouseX, int mouseY) {
		if( clickSpace != nullptr ) clickSpace->mouseMovedTo(mouseX, mouseY);
	}
	
	void OptionsMenu::mouseClick() {
		if( clickSpace != nullptr ) clickSpace->mouseClick();
	}
	
	void OptionsMenu::setScreenSizeEvent(std::function<void()> callback) {
		screenSizeEvent = callback;
	}
	
	void OptionsMenu::paint(multimedia::Surface& destSurf) {
		//check if we have to redraw our assets
		if( ownRedrawFlag != theWindow->redrawFlag ) {
			makeButtons();
			//assets ready — set the flag
			ownRedrawFlag = theWindow->redrawFlag;
		}
		//draw buttons
		backButton.paint(destSurf);
		fullscreenButton.paint(destSurf);
		musicButton.paint(destSurf);
		screenSizeButton.paint(destSurf);
	}
	
	void OptionsMenu::makeButtons() {
		constexpr int buttonSpacing = 5;//button spacing in pixels
		
		//parameters for button template
		int totalButtonsHeight = theWindow->getHeight() * 0.4;
		int standardButtonHeight = (totalButtonsHeight / 4) - buttonSpacing;
		int theBottomButtonPosition = (theWindow->getHeight() * 0.7) - standardButtonHeight/2;
		buttonTempl.reloadAssetsForParametersH(standardButtonHeight, theLongestText);
		
		//make buttons
		if( clickSpace != nullptr ) delete clickSpace;
		clickSpace = new UI::Space(0,0,theWindow->getWidth(),theWindow->getHeight());
		
		backButton = buttonTempl.makeButton(backText, clickSpace);
		fullscreenButton = buttonTempl.makeButton(
			(theWindow->isFullscreen()) ? fullscreenOnText : fullscreenOffText
			, clickSpace);
		musicButton = buttonTempl.makeButton(
			(mainMusic->isPlaying()) ? musicOnText : musicOffText
			, clickSpace
		);
		screenSizeButton = buttonTempl.makeButton( screenSizeText, clickSpace );
		
		backButton.assignEvents();
		backButton.onClickHandler([this]() {
			isMenuActive = false;
		});
		fullscreenButton.assignEvents();
		fullscreenButton.onClickHandler([this](){
			theWindow->switchFullscreen();
		});
		musicButton.assignEvents();
		musicButton.onClickHandler([this]() {
			if( mainMusic->isPlaying() ) {
				mainMusic->stop(100);
				musicButton.setText( musicOffText );
			} else {
				mainMusic->play();
				musicButton.setText( musicOnText );
			}
		});
		screenSizeButton.assignEvents();
		screenSizeButton.onClickHandler( screenSizeEvent );
		
		//position the buttons
		int posX = theWindow->getWidth() / 2;
		int posY = theBottomButtonPosition;
		//--back
		backButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--screen size
		screenSizeButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--fullscreen state
		fullscreenButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--music state
		musicButton.setPosition(posX, posY, true);
	}
	
	
}//end namespace game_logic

