/**
 * Menu for changing screen sizes
*/

#include "screen_size_menu.hpp"
#include "mls.hpp"
#include "multimedia/font.hpp"


namespace game_logic {
	
	
	ScreenSizeMenu::ScreenSizeMenu(multimedia::Window *mainWindow, 
	std::vector<multimedia::VideoMode> vModes):
		videoModes(vModes),
		theWindow(mainWindow),
		buttonTempl("MenuButton_active.png","MenuButton_inactive.png",
		MenuButtonWidthPercent, MenuButtonHeightPercent) {
		ownRedrawFlag = theWindow->redrawFlag * -1;
		isMenuActive = false;
		
		prevText = _("%{#Video modes list#}%<- Previous").get();
		nextText = _("%{#Video modes list#}%Next ->").get();
		cancelText = _("%{#Video modes list#}%Cancel").get();
		
		std::vector<std::string> textModes;
		for(const auto& mode : videoModes) {
			textModes.push_back( mode.to_string() );
		}
		theLongestText = multimedia::getTheLongestString( textModes );
		theLongestText = multimedia::getTheLongestString({
			theLongestText, prevText, nextText, cancelText
		});
	}
	
	ScreenSizeMenu::~ScreenSizeMenu() {
		if( clickSpace != nullptr ) delete clickSpace;
	}
	
	bool ScreenSizeMenu::isActive() {
		return isMenuActive;
	}
	
	void ScreenSizeMenu::enable() {
		isMenuActive = true;
	}
	
	void ScreenSizeMenu::mouseOver(int mouseX, int mouseY) {
		if( clickSpace != nullptr ) clickSpace->mouseMovedTo(mouseX, mouseY);
	}
	
	void ScreenSizeMenu::mouseClick() {
		if( clickSpace != nullptr ) clickSpace->mouseClick();
	}
	
	void ScreenSizeMenu::paint(multimedia::Surface& destSurf) {
		if( ownRedrawFlag != theWindow->redrawFlag ) {
			makeButtons();
			//assets ready — set the flag
			ownRedrawFlag = theWindow->redrawFlag;
		}
		//draw buttons
		if( displayNext ) nextButton.paint(destSurf);
		if( displayPrev ) prevButton.paint(destSurf);
		cancelButton.paint(destSurf);
		for(std::size_t modeNo = 0; modeNo < displayModes.size(); ++modeNo) {
			if( displayModes[modeNo] ) {
				modeButtons[modeNo].paint(destSurf);
			}
		}
	}
	
	void ScreenSizeMenu::makeButtons() {
		constexpr int buttonSpacing = 5;//button spacing in pixels
		
		//parameters for button template
		int totalButtonsHeight = theWindow->getHeight() * 0.7;
		int standardButtonHeight = (totalButtonsHeight / (modesPerPage+3)) - buttonSpacing;
		int theBottomButtonPosition = (theWindow->getHeight() * 0.9) - standardButtonHeight/2;
		buttonTempl.reloadAssetsForParametersH(standardButtonHeight, theLongestText);
		
		//make buttons
		if( clickSpace != nullptr ) delete clickSpace;
		clickSpace = new UI::Space(0,0,theWindow->getWidth(),theWindow->getHeight());
		
		prevButton = buttonTempl.makeButton(prevText, clickSpace);
		nextButton = buttonTempl.makeButton(nextText, clickSpace);
		cancelButton = buttonTempl.makeButton(cancelText, clickSpace);
		//--mode buttons
		for(std::size_t modeNo = 0; modeNo < modeButtons.size(); ++modeNo) {
			modeButtons[modeNo] = buttonTempl.makeButton(" ", clickSpace);
			modeButtons[modeNo].assignEvents();
			modeButtons[modeNo].onClickHandler([this,modeNo]() {
				switchModeToIndex(modeButtons.size() - modeNo - 1);
			});
		}
		setModeTexts();
		
		prevButton.assignEvents();
		nextButton.assignEvents();
		cancelButton.assignEvents();
		
		cancelButton.onClickHandler([this]() {
			isMenuActive = false;
		});
		nextButton.onClickHandler([this]() {
			++pageNumber;
			setModeTexts();
		});
		prevButton.onClickHandler([this]() {
			--pageNumber;
			setModeTexts();
		});
		
		//position the buttons
		int posX = theWindow->getWidth() / 2;
		int posY = theBottomButtonPosition;
		//--cancel
		cancelButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--next
		nextButton.setPosition(posX, posY, true);
		posY -= standardButtonHeight + buttonSpacing;
		//--mode buttons
		for(auto& modeButton : modeButtons) {
			modeButton.setPosition(posX, posY, true);
			posY -= standardButtonHeight + buttonSpacing;
		}
		//--prev
		prevButton.setPosition(posX, posY, true);
	}
	
	///sets texts on mode buttons
	void ScreenSizeMenu::setModeTexts() {
		std::size_t modeIndex = pageNumber * modesPerPage;
		if( pageNumber == 0 ) displayPrev = false;
		else displayPrev = true;
		
		//write text on buttons (we are going backwards because the bottomest button is the first)
		for(auto no = modeButtons.size(); no > 0; --no) {
			if( modeIndex < videoModes.size() ) {
				displayModes[no-1] = true;
				modeButtons[no-1].setText( videoModes[modeIndex].to_string() );
			} else {
				displayModes[no-1] = false;
			}
			++modeIndex;
		}
		
		if( modeIndex >= videoModes.size() ) displayNext = false;
		else displayNext = true;
	}
	
	///switches video mode from button click
	void ScreenSizeMenu::switchModeToIndex(std::size_t index) {
		std::size_t modeIndex = pageNumber * modesPerPage + index;
		
		theWindow->switchToVideoMode( videoModes[modeIndex] );
	}
	
}//end namespace game_logic


