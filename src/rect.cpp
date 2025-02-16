#include "rect.hpp"

/**
 * Class Rect — utility class for rectangles.
*/

Rect::Rect(int _x, int _y, int _w, int _h):
	x(_x), y(_y), width(_w), height(_h) {}

//getters -------------------

int Rect::getLeft() const {
	return x;
}
int Rect::getTop() const {
	return y;
}
int Rect::getRight() const {
	return x + width;
}
int Rect::getBottom() const {
	return y + height;
}

int Rect::getCenterX() const {
	return x + width/2;
}
int Rect::getCenterY() const {
	return y + height/2;
}
int Rect::getHeight() const {
	return height;
}
int Rect::getWidth() const {
	return width;
}

//setters -----------------


void Rect::setLeft(int left) {
	x = left;
}
void Rect::setTop(int top) {
	y = top;
}
void Rect::setRight(int right) {
	x = right - width;
}
void Rect::setBottom(int bottom) {
	y = bottom - height;
}
void Rect::setCenterX(int center_x) {
	x = center_x - width/2;
}
void Rect::setCenterY(int center_y) {
	y = center_y - height/2;
}
void Rect::setWidth(int new_width) {
	width = new_width;
}
void Rect::setHeight(int new_height) {
	height = new_height;
}

//changers ------------------------

Rect Rect::move(int x_shift, int y_shift) const {
	return Rect{x + x_shift, y + y_shift, width, height};
}
Rect Rect::inflate(int value) {
	return inflate(value, value);
}
Rect Rect::inflate(int x_value, int y_value) {
	return Rect(x - x_value, y - y_value, width + 2*x_value, height + 2*y_value);
}

void Rect::setWidthWithConstRatio(int new_width) {
	double ratio = static_cast<double>(height) / static_cast<double>(width);
	int new_height = static_cast<int>( new_width * ratio );
	width = new_width;
	height = new_height;
}

void Rect::setHeightWithConstRatio(int new_height) {
	double ratio = static_cast<double>(width) / static_cast<double>(height);
	int new_width = static_cast<int>( new_height * ratio );
	width = new_width;
	height = new_height;
}

//-------------------------------

///converts this object into SDL_Rect pointer usable by SDL2 library
SDL_Rect* Rect::sdl_rect() {
	sdlRect.x = x;
	sdlRect.y = y;
	sdlRect.w = width;
	sdlRect.h = height;
	
	return &sdlRect;
}

bool Rect::includes(int test_x, int test_y, bool border) const {
	if( border ) { //include border too?
		if( test_x < x ) return false;
		if( test_x > x + width ) return false;
		if( test_y < y ) return false;
		if( test_y > y + height ) return false;
		return true;
	} else {
		if( test_x <= x ) return false;
		if( test_x >= x + width ) return false;
		if( test_y <= y ) return false;
		if( test_y >= y + height ) return false;
		return true;
	}
}


