/**
 * Template class and a class for drawing and using buttons
*/

#include "buttons.hpp"

namespace game_logic {
	
	ButtonTemplate::ButtonTemplate(std::string activeFileName, 
			std::string inactiveFileName, 
			double widthPercent,
			double heightPercent):
				activeFile(activeFileName),
				inactiveFile(inactiveFileName),
				widthP(widthPercent),
				heightP(heightPercent),
				drawingFont(defaultFontFileName, 8) {}
	
	void ButtonTemplate::reloadAssetsForParametersH(int buttonHeight, std::string theLongestString) {
		activeSurf = multimedia::Surface::fromFile(activeFile);
		inactiveSurf = multimedia::Surface::fromFile(inactiveFile);
		
		//scale assets
		Rect size = activeSurf.getSize();
		size.setHeightWithConstRatio( buttonHeight );
		activeSurf.scale(size);
		
		size = inactiveSurf.getSize();
		size.setHeightWithConstRatio( buttonHeight );
		inactiveSurf.scale(size);
		
		//calculate size for text
		size.setWidth( size.getWidth() * widthP );
		size.setHeight( size.getHeight() * heightP );
		
		//get the right font
		drawingFont = multimedia::Font::selectFontToFit(
			defaultFontFileName,
			size,
			theLongestString);
	}
	
	int ButtonTemplate::reloadAssetsForParametersW(int buttonWidth, std::string theLongestString) {
		activeSurf = multimedia::Surface::fromFile(activeFile);
		inactiveSurf = multimedia::Surface::fromFile(inactiveFile);
		
		int buttonHeight;
		
		//scale assets
		Rect size = activeSurf.getSize();
		size.setWidthWithConstRatio( buttonWidth );
		activeSurf.scale(size);
		
		size = inactiveSurf.getSize();
		size.setWidthWithConstRatio( buttonWidth );
		inactiveSurf.scale(size);
		
		buttonHeight = size.getHeight();
		
		//calculate size for text
		size.setWidth( size.getWidth() * widthP );
		size.setHeight( size.getHeight() * heightP );
		
		//get the right font
		drawingFont = multimedia::Font::selectFontToFit(
			defaultFontFileName,
			size,
			theLongestString);
		
		return buttonHeight;
	}
	
	Button* ButtonTemplate::makeButton(std::string textOnButton, UI::Space *clickSpace) {
		return new Button(textOnButton, this, clickSpace);
	}
	
	/*
	=========================== Button class
	*/

	Button::Button(std::string textOnButton, ButtonTemplate* templ, UI::Space* clickSpace):
	enabled(true), onClick(nullptr) {
		buttonTempl = templ;
		
		if( templ == nullptr || clickSpace == nullptr ) return;
		
		if( textOnButton.find("\n") != std::string::npos ) {
			int textWidth = templ->activeSurf.getSize().getWidth() * templ->widthP;
			
			activeTextSurf = templ->drawingFont.renderMultiLine(
				textOnButton, textWidth, multimedia::TextAlign::CENTER, activeFontColor
			);
			inactiveTextSurf = templ->drawingFont.renderMultiLine(
				textOnButton, textWidth, multimedia::TextAlign::CENTER, inactiveFontColor
			);
		} else {
			activeTextSurf = templ->drawingFont.render(textOnButton, activeFontColor);
			inactiveTextSurf = templ->drawingFont.render(textOnButton, inactiveFontColor);
		}
		
		position = templ->activeSurf.getSize();
		activeTextPos = activeTextSurf.getSize();
		activeTextPos.setCenterX( position.getCenterX() );
		activeTextPos.setCenterY( position.getCenterY() );
		inactiveTextPos = inactiveTextSurf.getSize();
		inactiveTextPos.setCenterX( position.getCenterX() );
		inactiveTextPos.setCenterY( position.getCenterY() );
		
		clickable = clickSpace->newComponent(position.inflate(-5));
	}
	
	
	
	void Button::setText(std::string newText) {
		if( newText.find("\n") != std::string::npos ) {
			int textWidth = buttonTempl->activeSurf.getSize().getWidth() * buttonTempl->widthP;
			
			activeTextSurf = buttonTempl->drawingFont.renderMultiLine(
				newText, textWidth, multimedia::TextAlign::CENTER, activeFontColor
			);
			inactiveTextSurf = buttonTempl->drawingFont.renderMultiLine(
				newText, textWidth, multimedia::TextAlign::CENTER, inactiveFontColor
			);
		} else {
			activeTextSurf = buttonTempl->drawingFont.render(newText, activeFontColor);
			inactiveTextSurf = buttonTempl->drawingFont.render(newText, inactiveFontColor);
		}
		
		activeTextPos = activeTextSurf.getSize();
		activeTextPos.setCenterX( position.getCenterX() );
		activeTextPos.setCenterY( position.getCenterY() );
		
		inactiveTextPos = inactiveTextSurf.getSize();
		inactiveTextPos.setCenterX( position.getCenterX() );
		inactiveTextPos.setCenterY( position.getCenterY() );
	}
	
	void Button::assignEvents() {
		if( clickable == nullptr ) return;
		
		clickable->addMouseEnterListener([this]() {
			isMouseOver = true;
		});
		clickable->addMouseLeaveListener([this]() {
			isMouseOver = false;
		});
		clickable->addClickListener([this]() {
			if( !enabled ) return;
			
			buttonTempl->beepSound.play();
			if( onClick != nullptr ) onClick();
		});
	}
	
	void Button::onClickHandler(std::function<void()> callback) {
		if( callback == nullptr ) return;
		onClick = callback;
	}
	
	
	void Button::setPosition(int x, int y, bool isCenter) {
		if( isCenter ) {
			position.setCenterX(x);
			position.setCenterY(y);
		} else {
			position.setLeft(x);
			position.setTop(y);
		}
		clickable->setBoundingRect( position.inflate(-5) );
		activeTextPos.setCenterX( position.getCenterX() );
		activeTextPos.setCenterY( position.getCenterY() );
		inactiveTextPos.setCenterX( position.getCenterX() );
		inactiveTextPos.setCenterY( position.getCenterY() );
	}
	
	void Button::paint(multimedia::Surface& destSurf) {
		if( !enabled ) return;
		
		if( isMouseOver ) {
			destSurf.blit(position, buttonTempl->activeSurf);
			destSurf.blit(activeTextPos, activeTextSurf);
		} else {
			destSurf.blit(position, buttonTempl->inactiveSurf);
			destSurf.blit(inactiveTextPos, inactiveTextSurf);
		}
		#ifdef CMAKE_DEBUG
		clickable->paintMarkers(destSurf);
		#endif
	}
	
	/*
	=========================== Menu class
	*/
	
	Menu::Menu(multimedia::Window *theWindow, 
		std::initializer_list<std::string> textsOnButtons,
		std::initializer_list<std::string> possibleText):
	templ("MenuButton_active.png","MenuButton_inactive.png",
	MenuButtonWidthPercent, MenuButtonHeightPercent), clickSpace(nullptr) {
		constexpr int buttonSpacing = 5;//button spacing in pixels
		
		std::string theLongestString;
		if( possibleText.size() > 0 )
			theLongestString = multimedia::getTheLongestString(possibleText);
		else
			theLongestString = multimedia::getTheLongestString(textsOnButtons);
		
		int totalButtonsHeight = theWindow->getHeight() * ( textsOnButtons.size() > 4 ? 0.7 : 0.4);
		int standardButtonHeight = (totalButtonsHeight / textsOnButtons.size()) - buttonSpacing;
		int theBottomButtonPosition = (theWindow->getHeight() * ( textsOnButtons.size() > 4 ? 0.9 : 0.7)) - standardButtonHeight/2;
		
		templ.reloadAssetsForParametersH(standardButtonHeight, theLongestString);
		
		clickSpace = new UI::Space(0,0,theWindow->getWidth(),theWindow->getHeight());
		//create buttons
		for(auto textOnButton : textsOnButtons) {
			Button *newButton = templ.makeButton(textOnButton, clickSpace);
			newButton->assignEvents();
			buttons.push_back( newButton );
		}
		//position buttons
		int posX = theWindow->getWidth() / 2;
		int posY = theBottomButtonPosition;
		for(auto it = buttons.rbegin(); it != buttons.rend(); ++it) {
			(*it)->setPosition(posX, posY, true);
			posY -= standardButtonHeight + buttonSpacing;
		}
	}
	
	Menu::~Menu() {
		for(auto button : buttons) {
			delete button;
		}
		delete clickSpace;
	}
	
	Button* Menu::getButton(unsigned int index) {
		if( index >= buttons.size() ) return nullptr;
		return buttons[index];
	}
	
	void Menu::mouseActions(int mouseX, int mouseY, bool mouseClick) {
		if( clickSpace == nullptr ) return;
		if( mouseX != -1 && mouseY != -1 ) {
			clickSpace->mouseMovedTo(mouseX, mouseY);
		}
		if( mouseClick ) clickSpace->mouseClick();
	}
	
}//end namespace game_logic

