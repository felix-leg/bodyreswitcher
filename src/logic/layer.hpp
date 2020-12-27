#pragma once

#ifndef LOGIC_LAYER_HPP
#define LOGIC_LAYER_HPP

#include "multimedia/window.hpp"
#include <vector>

namespace game_logic {
	
	///base class for all logic layers
	class Layer {
		public:
			virtual void mouseOver(int x, int y) = 0;
			virtual void mouseClick() = 0;
			virtual void paint(multimedia::Surface &destSurf) = 0;
			
			virtual bool isActive() = 0;
	};//end class Layer
	
	/**
	 * Represents a group of layers stacked one on each other.
	 * The top active layer receives events.
	 * All the layers from bottom to top are drawn in order.
	*/
	class LayerStack {
		public:
			LayerStack(multimedia::Window *theWindow);
		
			void addLayer(Layer* newLayer);
			
			void onMouseEvent(int mouseX, int mouseY, bool mouseClicked);
			void paint(multimedia::Surface &destSurf);
		private:
			std::vector<Layer*> layerStack;
			multimedia::Surface separator;
			
			char redrawFlag;
			multimedia::Window *mainWindow;
	};//end class LayerStack
	
}//end namespace game_logic

#endif /* LOGIC_LAYER_HPP */
