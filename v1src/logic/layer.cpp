/**
 * Defines a game logic layer that receives events and paints its contents.
*/

#include "layer.hpp"
#include "multimedia/base.hpp"


namespace game_logic {
	
	LayerStack::LayerStack(multimedia::Window *theWindow) {
		/*separator = multimedia::Surface::newRGBA_Surface(
			theWindow->getWidth(), theWindow->getHeight());
		separator.fill(multimedia::Color{127,127,127,100});*/
		redrawFlag = theWindow->redrawFlag * -1;
		mainWindow = theWindow;
	}
	
	void LayerStack::addLayer(Layer* newLayer) {
		layerStack.push_back( newLayer );
	}
	
	void LayerStack::onMouseEvent(int mouseX, int mouseY, bool mouseClicked) {
		Layer* receiver = nullptr;
		//get the first active layer from the top
		for( auto it = layerStack.rbegin(); it != layerStack.rend(); ++it ) {
			if( (*it)->isActive() ) {
				receiver = (*it);
				break;
			}
		}
		
		if( receiver == nullptr ) return;
		
		if( mouseX >= 0 && mouseY >= 0 ) {//mouse moved?
			receiver->mouseOver(mouseX, mouseY);
		}
		if( mouseClicked ) {
			receiver->mouseClick();
		}
	}
	
	void LayerStack::paint(multimedia::Surface &destSurf) {
		if( redrawFlag != mainWindow->redrawFlag ) {
			//remake separator
			separator = multimedia::Surface::newRGBA_Surface(
				mainWindow->getWidth(), mainWindow->getHeight());
			separator.fill(multimedia::Color{127,127,127,100});
			
			redrawFlag = mainWindow->redrawFlag;
		}
		bool separatorNeeded = false;
		//paint active layers from the bottom to the top
		for( auto it = layerStack.begin(); it != layerStack.end(); ++it ) {
			if( (*it)->isActive() ) {
				if( separatorNeeded ) destSurf.blit(0,0,separator);
				(*it)->paint(destSurf);
				separatorNeeded = true;
			}
		}
	}
	
}//end namespace game_logic

