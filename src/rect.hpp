#pragma once

#ifndef RECT_HPP
#define RECT_HPP

#include <SDL.h>

class Rect {
	public:
		///ctor
		Rect(int _x = 0, int _y = 0, int _w = 0, int _h = 0);
		
		//getters ------------
		
		///gets the X position
		int getLeft() const;
		///gets the Y position
		int getTop() const;
		///gets the right X position
		int getRight() const;
		///gets the bottom Y position
		int getBottom() const;
		///gets central X position
		int getCenterX() const;
		///gets central Y position
		int getCenterY() const;
		///
		int getHeight() const;
		///
		int getWidth() const;
		
		//setters -----------------
		///
		void setLeft(int left);
		///
		void setTop(int top);
		///
		void setRight(int right);
		///
		void setBottom(int bottom);
		///
		void setCenterX(int center_x);
		///
		void setCenterY(int center_y);
		///
		void setWidth(int new_width);
		///
		void setHeight(int new_height);
		
		//changers ---------
		
		///shifts the rectangle by x and y
		Rect move(int x_shift, int y_shift) const;
		///shrink or grows the rectange by a value
		Rect inflate(int value);
		Rect inflate(int x_value, int y_value);
		///changes width keeping the ratio
		void setWidthWithConstRatio(int new_width);
		///changes height keeping the ratio
		void setHeightWithConstRatio(int new_height);
		
		///converts this object into SDL_Rect pointer usable by SDL2 library
		SDL_Rect* sdl_rect();
		
		///test for inclusion of a point
		bool includes(int test_x, int test_y, bool border = true) const;
	private:
		int x;
		int y;
		int width;
		int height;
		
		SDL_Rect sdlRect;
};//end class Rect

#endif /* RECT_HPP */
