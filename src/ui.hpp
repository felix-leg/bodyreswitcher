#pragma once

#ifndef UI_HPP
#define UI_HPP

#include "rect.hpp"
#include <vector>
#include <functional>


#ifdef CMAKE_DEBUG
#include <multimedia/window.hpp>
#endif

namespace UI {
	
	class Component;
	/**
	 * Defines a space inside of which are located UI elements.
	 * Detects such events like mouse move over elements (in/out) and click on them.
	*/
	class Space {
		public:
			Space(int left, int top, int width, int height);
			~Space();
			
			Component* newComponent(int left, int top, int width, int height);
			Component* newComponent(Rect compRect);
			
			void removeComponent(Component* toRemove);
			
			void mouseMovedTo(int newX, int newY);
			void mouseClick();
		private:
			Rect spaceRect;
			
			std::vector<Component*> componentList;
			Component* compWithMouseOverIt;
	};
	
	/**
	 * A rectangular space inside UI::Space that responds to mouse movement
	 * and clicks. Managed by UI::Space.
	*/
	class Component {
		friend class Space;
		
		public:
			void addMouseOverListener(std::function<void()> callback);
			void addMouseEnterListener(std::function<void()> callback);
			void addMouseLeaveListener(std::function<void()> callback);
			void addClickListener(std::function<void()> callback);
			
			void setBoundingRect(Rect newRect);
			
			#ifdef CMAKE_DEBUG
			void paintMarkers(multimedia::Surface &destSurf);
			#endif
		private: 
			Component(int left, int top, int width, int height);
			
			void onMouseEnter();
			void onMouseOver();
			void onMouseLeave();
			void onClick();
			
			Rect compRect;
			std::vector<std::function<void()>> mouseOverListeners;
			std::vector<std::function<void()>> mouseEnterListeners;
			std::vector<std::function<void()>> mouseLeaveListeners;
			std::vector<std::function<void()>> mouseClickListeners;
			
			#ifdef CMAKE_DEBUG
			multimedia::Surface topLeftSurf;
			multimedia::Surface bottomRightSurf;
			#endif
	};
	
}//end namespace ui

#endif /* UI_HPP */
