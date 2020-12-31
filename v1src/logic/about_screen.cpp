/**
 * About screen with the little story for the game
*/

#include "about_screen.hpp"
#include "rect.hpp"
#include "mls.hpp"
#include <string>

namespace game_logic {
	
	constexpr unsigned int aboutTextPrefereableLinesCount = 20U;
	const multimedia::Color aboutTextColor{250,250,250};
	const multimedia::Color aboutTextColorHighlight{255,100,100};
	
	About::About(multimedia::Window *mainWindow): theWindow(mainWindow),
	buttonTempl("MenuButton_active.png","MenuButton_inactive.png",
	MenuButtonWidthPercent, MenuButtonHeightPercent) {
		ownRedrawFlag = theWindow->redrawFlag * -1;//to force redraw
		clickSpace = nullptr;
		isEnabled = false;
		
		backText = _("%{#About screen#}%Back").get();
		
		introText = _(
			"One night The Crazy Professor made an amazing machine that gives one the ability "
			"to switch body with another person. Such an invention! "
			"Unfortunately he forgot to close his home and a bunch of kids entered his laboratory "
			"and started playing with his invention. What a mess! "
			"They switched their bodies and now they need You to make things right."
		).get();
		rulesText = _(
			"But The Crazy Profesor has return and has got bad news! "
			"He says two bodies that were switched "
			"now can't be switched back. Oh noes! "
			"But you have got an idea: what if we \"go around\" the problem and try to put back "
			"persons in in their right bodies by switching them with other bodies? "
			"That's the plan!"
		).get();
		outroText = _(
			"So, you became the last hope for those poor kids. "
			"Can you solve the puzzle and make them happy again?"
		).get();
	}
	
	About::~About() {
		if( clickSpace != nullptr ) delete clickSpace;
	}
	
	///activates the layer
	void About::enable() {
		isEnabled = true;
	}
	
	bool About::isActive() {
		return isEnabled;
	}
	
	void About::mouseOver(int newMouseX, int newMouseY) {
		if( clickSpace != nullptr ) {
			clickSpace->mouseMovedTo(newMouseX, newMouseY);
		}
	}
	
	void About::mouseClick() {
		if( clickSpace != nullptr ) {
			clickSpace->mouseClick();
		}
	}
	
	void About::paint(multimedia::Surface& destSurf) {
		if( ownRedrawFlag != theWindow->redrawFlag ) {
			makeBackgroundSurface();
			makeButtons();
			//assets ready
			ownRedrawFlag = theWindow->redrawFlag;
		}
		//draw background
		destSurf.blit(0,0, background);
		//draw button
		backButton.paint(destSurf);
	}
	
	/**
	 * Creates background surface from the fragments:
	 * TopLeft, TopRight, BottomLeft, BottomRight,
	 * Top, Bottom, Left, Right
	 * and Inside assets
	*/
	void About::makeBackgroundSurface() {
		using namespace multimedia;
		
		const int screenWidth = theWindow->getWidth();
		const int screenHeight = theWindow->getHeight();
		Rect screen{0,0,screenWidth,screenHeight};
		Rect topLeftR, topRightR, bottomLeftR, bottomRightR;
		Rect topR, bottomR, leftR, rightR;
		Rect insideR;
		//scale down so there will be a border around the about screen
		screen = screen.inflate(-10);
		
		background = Surface::newRGBA_Surface(screenWidth, screenHeight);
		
		//load corners
		Surface topLeftS = Surface::fromFile("TopLeft_about.png");
		Surface bottomLeftS = Surface::fromFile("BottomLeft_about.png");
		Surface topRightS = Surface::fromFile("TopRight_about.png");
		Surface bottomRightS = Surface::fromFile("BottomRight_about.png");
		//position corners
		topLeftR = topLeftS.getSize();
		topLeftR.setLeft(screen.getLeft()); topLeftR.setTop(screen.getTop());
		
		topRightR = topRightS.getSize();
		topRightR.setRight(screen.getRight()); topRightR.setTop(screen.getTop());
		
		bottomLeftR = bottomLeftS.getSize();
		bottomLeftR.setBottom(screen.getBottom()); bottomLeftR.setLeft(screen.getLeft());
		
		bottomRightR = bottomRightS.getSize();
		bottomRightR.setBottom(screen.getBottom()); bottomRightR.setRight(screen.getRight());
		
		//load borders
		Surface topS = Surface::fromFile("Top_about.png");
		Surface bottomS = Surface::fromFile("Bottom_about.png");
		Surface leftS = Surface::fromFile("Left_about.png");
		Surface rightS = Surface::fromFile("Right_about.png");
		//position and scale borders
		topR.setWidth( topRightR.getLeft() - topLeftR.getRight() );
		topR.setHeight( topLeftR.getHeight() );
		topR.setLeft(topLeftR.getRight()); topR.setTop(screen.getTop());
		topS.scale(topR);
		
		bottomR.setWidth( bottomRightR.getLeft() - bottomLeftR.getRight() );
		bottomR.setHeight( bottomLeftR.getHeight() );
		bottomR.setLeft(bottomLeftR.getRight()); bottomR.setBottom(screen.getBottom());
		bottomS.scale(bottomR);
		
		leftR.setHeight( bottomLeftR.getTop() - topLeftR.getBottom() );
		leftR.setWidth( bottomLeftR.getWidth() );
		leftR.setLeft(screen.getLeft()); leftR.setTop(topLeftR.getBottom());
		leftS.scale(leftR);
		
		rightR.setHeight( bottomRightR.getTop() - topRightR.getBottom() );
		rightR.setWidth( bottomRightR.getWidth() );
		rightR.setRight(screen.getRight()); rightR.setBottom(bottomRightR.getTop());
		rightS.scale(rightR);
		
		//load inside
		Surface insideS = Surface::fromFile("Inside_about.png");
		//position and scale inside
		insideR.setWidth(topR.getWidth());
		insideR.setHeight(leftR.getHeight());
		insideR.setLeft(leftR.getRight());
		insideR.setTop(topR.getBottom());
		
		insideS.scale(insideR);
		
		//draw background form the parts
		//--corners
		background.blit(topLeftR.getLeft(),topLeftR.getTop(),topLeftS);
		background.blit(bottomLeftR.getLeft(),bottomLeftR.getTop(),bottomLeftS);
		background.blit(topRightR.getLeft(),topRightR.getTop(),topRightS);
		background.blit(bottomRightR.getLeft(),bottomRightR.getTop(),bottomRightS);
		//--borders
		background.blit(topR.getLeft(),topR.getTop(),topS);
		background.blit(bottomR.getLeft(),bottomR.getTop(),bottomS);
		background.blit(leftR.getLeft(),leftR.getTop(),leftS);
		background.blit(rightR.getLeft(),rightR.getTop(),rightS);
		//--inside
		background.blit(insideR.getLeft(),insideR.getTop(),insideS);
		
		//background ready
		
		//select the font for the text
		Rect textSpace = insideR.inflate(-20);
		unsigned int desiredFontHeight = textSpace.getHeight() / aboutTextPrefereableLinesCount;
		Font aboutFont = Font::selectFontWithPixelHeight("SpecialElite-Regular.ttf", desiredFontHeight);
		
		//write the text
		constexpr int paragraphSpace = 6;
		Surface textSurf;
		Rect r;
		int posY, posX;
		posY = textSpace.getTop(); posX = textSpace.getLeft();
		//--intro text
		textSurf = aboutFont.renderMultiLine(introText, textSpace.getWidth(), 
			TextAlign::LEFT, aboutTextColor);
		r = textSurf.getSize();
		background.blit(posX, posY, textSurf);
		posY += r.getHeight() + paragraphSpace;
		//--rules text
		textSurf = aboutFont.renderMultiLine(rulesText, textSpace.getWidth(),
			TextAlign::LEFT, aboutTextColorHighlight);
		r = textSurf.getSize();
		background.blit(posX, posY, textSurf);
		posY += r.getHeight() + paragraphSpace;
		//--outro text
		textSurf = aboutFont.renderMultiLine(outroText, textSpace.getWidth(),
			TextAlign::LEFT, aboutTextColor);
		background.blit(posX, posY, textSurf);
	}
	
	///configures the back button
	void About::makeButtons() {
		//the button parameters
		const int bottomCenter = theWindow->getHeight() * 0.9;
		const int buttonHeight = theWindow->getHeight() * 0.08;
		
		buttonTempl.reloadAssetsForParametersH(buttonHeight, backText);
		
		//create the button
		if( clickSpace != nullptr ) delete clickSpace;
		clickSpace = new UI::Space(0,0,theWindow->getWidth(),theWindow->getHeight());
		backButton = buttonTempl.makeButton(backText,clickSpace);
		
		//setup the button
		backButton.assignEvents();
		backButton.onClickHandler([this](){
			isEnabled = false;
		});
		
		//position the button
		backButton.setPosition(theWindow->getWidth()/2, bottomCenter, true);
	}
	
}//end namespace game_logic

