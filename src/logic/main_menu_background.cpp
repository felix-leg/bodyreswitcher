/**
 * The game's main menu background logic
*/

#include "main_menu_background.hpp"


namespace game_logic {
	
	namespace mm_background {
		
		//window variables
		multimedia::Window *theWindow = nullptr;//window pointer
		multimedia::DirtyFlag *paintFlag = nullptr;//repaint flag
		
		multimedia::Surface theBackground;
		
		void redrawBackground(multimedia::Surface &optSurf) {
			using namespace multimedia;
			Rect r{0,0,0,0};
			const int screenWidth = theWindow->getWidth();
			const int screenHeight = theWindow->getHeight();
			
			theBackground = Surface::newRGB_Surface(screenWidth, screenHeight);
			
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
			theBackground.blit(0,0, leftPart);
			
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
			theBackground.blit(r.getLeft(),0, rightPart);
			
			//::prepare the middle menu background
			Surface menuBack = Surface::fromFile("MM_middle.png");
			r = menuBack.getSize();
			r.setHeightWithConstRatio( screenHeight );
			menuBack.scale(r);
			//position the menu background on the middle of the screen
			r.setCenterX( screenWidth / 2 );
			//draw the menu background
			theBackground.blit( r, menuBack );
			
			//::prepare the title
			Surface title = Surface::fromFile("MM_Title.png");
			r = title.getSize();
			//scale title to about 0.3 of the screen height
			r.setHeightWithConstRatio( screenHeight * 0.323148148 );
			title.scale(r);
			//position the title in the top middle of the screen
			r.setCenterX( screenWidth / 2 );
			//draw on the background
			theBackground.blit( r, title);
			
			//optimize for drawing on the screen
			theBackground.optimizeForDrawingOn(optSurf);
			//background ready
		}
		
		//paints the background
		void paint(multimedia::Surface &destSurf) {
			if( paintFlag->needsRedraw() ) {
				redrawBackground(destSurf);
			}
			destSurf.blit(0,0,theBackground);
		}
		
		void module_unload() {
			//unload the surface
			theBackground = multimedia::Surface();
		}
		
	}//end namespace mm_background
	
	Module MainMenuBackgroundInit(multimedia::Window *mainWindow) {
		using namespace mm_background;
		
		theWindow = mainWindow;
		paintFlag = theWindow->createDirtyFlag();
		
		return Module(module_unload);
	}
	
}//end namespace game_logic

