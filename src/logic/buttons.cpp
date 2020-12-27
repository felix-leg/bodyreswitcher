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
	
	Button ButtonTemplate::makeButton(std::string textOnButton, UI::Space *clickSpace) {
		return Button(textOnButton, this, clickSpace);
	}
	
	/*
	=========================== Button class
	*/

	Button::Button(std::string textOnButton, ButtonTemplate* templ, UI::Space* clickSpace) {
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
			buttonTempl->beepSound.play();
		});
	}
	
	void Button::onClickHandler(std::function<void()> callback) {
		if( callback == nullptr ) return;
		clickable->addClickListener(callback);
	}
	/*
	Button& Button::operator=(Button&& other) {
		if( this == &other) return *this;
		
		std::cout << "Move operator" << std::endl;
		buttonTempl = other.buttonTempl;
		activeTextSurf = std::move(other.activeTextSurf);
		inactiveTextSurf = std::move(other.inactiveTextSurf);
		position = other.position;
		activeTextPos = other.activeTextPos;
		inactiveTextPos = other.inactiveTextPos;
		clickable = other.clickable; other.clickable = nullptr;
		
		//redo the catchers
		clickable->addMouseEnterListener([this]() {
			isMouseOver = true;
		});
		clickable->addMouseLeaveListener([this]() {
			isMouseOver = false;
		});
		
		return *this;
	}//*/
	
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
		//std::cout << "Is mouse over? " << std::boolalpha << isMouseOver << std::endl;
		if( isMouseOver ) {
			destSurf.blit(position.getLeft(), position.getTop(), buttonTempl->activeSurf);
			destSurf.blit(activeTextPos.getLeft(), 
				activeTextPos.getTop(), activeTextSurf);
		} else {
			destSurf.blit(position.getLeft(), position.getTop(), buttonTempl->inactiveSurf);
			destSurf.blit(inactiveTextPos.getLeft(), 
				inactiveTextPos.getTop(), inactiveTextSurf);
		}
		#ifdef CMAKE_DEBUG
		clickable->paintMarkers(destSurf);
		#endif
	}
	
}//end namespace game_logic

