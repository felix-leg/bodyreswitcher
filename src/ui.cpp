
/**
 * UI component — a space inside a UI_space object that recognizes 
 * mouse movement and clicks
*/

#include "ui.hpp"


namespace UI {
	
	Space::Space(int left, int top, int width, int height):
		spaceRect(left, top, width, height),
		compWithMouseOverIt(nullptr) {}
	
	Space::~Space() {
		for(const auto& comp : componentList) {
			delete comp;
		}
		componentList.clear();
	}
	
	Component::Component(int left, int top, int width, int height):
		compRect(left, top, width, height) {
			#ifdef CMAKE_DEBUG
			topLeftSurf = multimedia::Surface::newRGB_Surface(10,10);
			bottomRightSurf = multimedia::Surface::newRGB_Surface(10,10);
			
			topLeftSurf.fill(multimedia::Color(255,0,0));
			bottomRightSurf.fill(multimedia::Color(0,0,255));
			#endif
		}
	
	Component* Space::newComponent(int left, int top, int width, int height) {
		Component* c = new Component(left, top, width, height);
		componentList.push_back( c );
		return c;
	}
	Component* Space::newComponent(Rect compRect) {
		return newComponent(compRect.getLeft(), compRect.getTop(), 
		compRect.getWidth(), compRect.getHeight());
	}
	
	void Space::removeComponent(Component* toRemove) {
		for(auto it = componentList.begin(); it != componentList.end(); ++it) {
			if( *it == toRemove ) {
				componentList.erase(it);
				break;
			}
		}
		delete toRemove;
	}
	
	void Component::setBoundingRect(Rect newRect) {
		compRect = newRect;
	}
	
	// UI::Component listener adders -----------------------------------------
	
	void Component::addMouseOverListener(std::function<void()> callback) {
		mouseOverListeners.push_back( callback );
	}
	void Component::addMouseEnterListener(std::function<void()> callback) {
		mouseEnterListeners.push_back( callback );
	}
	void Component::addMouseLeaveListener(std::function<void()> callback) {
		mouseLeaveListeners.push_back( callback );
	}
	void Component::addClickListener(std::function<void()> callback) {
		mouseClickListeners.push_back( callback );
	}
	
	// UI::Component events -------------------------------
	
	void Component::onMouseEnter() {
		for(const auto& listener : mouseEnterListeners) {
			listener();
		}
	}
	void Component::onMouseOver() {
		for(const auto& listener : mouseOverListeners) {
			listener();
		}
	}
	void Component::onMouseLeave() {
		for(const auto& listener : mouseLeaveListeners) {
			listener();
		}
	}
	void Component::onClick() {
		for(const auto& listener : mouseClickListeners) {
			listener();
		}
	}
	
	//---------------------
	
	#ifdef CMAKE_DEBUG
	///debug function drawing little rectangles around click space
	void Component::paintMarkers(multimedia::Surface &destSurf) {
		Rect r = compRect;
		destSurf.blit(r.getLeft(), r.getTop(), topLeftSurf);
		r = bottomRightSurf.getSize();
		r.setRight( compRect.getRight() );
		r.setBottom( compRect.getBottom() );
		destSurf.blit(r.getLeft(), r.getTop(), bottomRightSurf);
	}
	#endif
	
	/**
	 * Registers a mouse move inside the space and produces onMouseEnter
	 * onMouseOver and onMouseLeave events on the right UI::Component
	*/
	void Space::mouseMovedTo(int newX, int newY) {
		
		//detect movement outside the space
		if( spaceRect.includes(newX, newY, true) == false ) {
			if( compWithMouseOverIt != nullptr ) {
				compWithMouseOverIt->onMouseLeave();
				compWithMouseOverIt = nullptr;
			}
			return;
		}
		
		if( compWithMouseOverIt != nullptr ) {//there is a component with mouse inside it
			if( compWithMouseOverIt->compRect.includes(newX, newY) ) {//mouse is still inside?
				compWithMouseOverIt->onMouseOver();
				return;
			} else {//mouse has left the component
				compWithMouseOverIt->onMouseLeave();
				compWithMouseOverIt = nullptr;
			}
		}
		if( compWithMouseOverIt == nullptr ) { //there is not yet a component with mouse over it
			for(const auto& component : componentList) {
				if( component->compRect.includes(newX, newY) ) {//mouse entered inside a component
					compWithMouseOverIt = component;
					component->onMouseEnter();
					component->onMouseOver();
					return;
				}
			}
		}
	}
	
	/**
	 * Registers a mouse click and checks if one of UI::Components
	 * should be notified of this.
	*/
	void Space::mouseClick() {
		if( compWithMouseOverIt == nullptr ) //no component to click on?
			return;
		
		compWithMouseOverIt->onClick();
	}
	
}//end namespace UI

