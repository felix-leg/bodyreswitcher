#pragma once

#ifndef RECT_HPP
#define RECT_HPP

#include <SDL.h>

class Rect {
	public:
		Rect(int _x = 0, int _y = 0, int _w = 0, int _h = 0);
		
		//getters ------------
		
		int getLeft() const;
		int getTop() const;
		int getRight() const;
		int getBottom() const;
		int getCenterX() const;
		int getCenterY() const;
		int getHeight() const;
		int getWidth() const;
		
		//setters -----------------
		
		void setLeft(int left);
		void setTop(int top);
		void setRight(int right);
		void setBottom(int bottom);
		void setCenterX(int center_x);
		void setCenterY(int center_y);
		void setWidth(int new_width);
		void setHeight(int new_height);
		
		//changers ---------
		
		Rect move(int x_shift, int y_shift) const;
		Rect inflate(int value);
		Rect inflate(int x_value, int y_value);
		void setWidthWithConstRatio(int new_width);
		void setHeightWithConstRatio(int new_height);
		
		///converts this object into SDL_Rect pointer usable by SDL2 library
		SDL_Rect* sdl_rect();
		
		bool includes(int test_x, int test_y, bool border = true) const;
	private:
		int x;
		int y;
		int width;
		int height;
		
		SDL_Rect sdlRect;
};//end class Rect

#endif /* RECT_HPP */
