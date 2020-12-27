#pragma once

#ifndef MULTIMEDIA_WINDOW_HPP
#define MULTIMEDIA_WINDOW_HPP

#include "base.hpp"
#include "rect.hpp"
#include "system.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <functional>
#include <string>
#include <vector>

namespace multimedia {
	
	class Surface;
	class Drawer;
	
	class Window {
		public:
			///ctor
			Window(std::string title, VideoMode video_mode, bool fullscreen, SDL2_System *system);
			///dtor
			~Window();
			
			//registering callbacks
			
			///
			void registerUpdateFunction(std::function<void(int,int,bool)> callback);
			///
			void registerDrawFunction(std::function<void()> callback);
			
			///main game loop
			void mainLoop();
			///switches the fullscreen mode
			void switchFullscreen();
			///switches to the given video size
			void switchToVideoMode(VideoMode mode);
			///
			VideoMode getDisplayMode() const;
			
			///gets window surface for drawing on
			Surface getWindowSurface();
			///gets window drawer object for drawing with renderer
			Drawer getWindowDrawer();
			///a flag that informs other parts of the game of necessity to redraw its elements
			char redrawFlag;
			///
			int getWidth() const;
			int getHeight() const;
			bool isFullscreen() const;
		private:
			SDL2_System *sdlSystem;
			SDL_Surface* windowSurface = nullptr;
			SDL_Window* windowHandle = nullptr;
			SDL_Renderer* windowRenderer = nullptr;
			bool fullscreenState;
			const std::string windowTitle;
			
			///update(int newMouseX, int newMouseY, bool mouseClicked)
			std::function<void(int,int,bool)> updateCallback = nullptr;
			std::function<void()> drawCallback = nullptr;
	};//end class Window
	
	class Surface {
		public:
			static Surface fromFile(std::string filename);
			static Surface newRGB_Surface(unsigned int width, unsigned int height);
			static Surface newRGBA_Surface(unsigned int width, unsigned int height);
			///move operator
			Surface& operator=(Surface&& other);
			///copy operator
			Surface& operator=(const Surface& other);
			///copy ctor
			Surface(const Surface& other);
			
			//public dtor
			~Surface();
			//empty public ctor
			Surface(SDL_Surface* extSurface = nullptr);
			
			bool optimizeForDrawingOn(Surface otherSurface);
			///crops image to the cropRect
			void crop(Rect cropRect);
			///scales image by factor
			void scale(double factor);
			///scales image by x and y factor
			void scale(double x_factor, double y_factor);
			///scales image to given rectangle
			void scale(Rect destRect);
			
			///returns the bounding rectangle of the surface
			Rect getSize() const;
			///
			bool hasAlphaChannel() const;
			///
			Drawer getDrawer();
			
			///draws a surface on this surface
			void blit(int x, int y, const Surface& otherSurface );
			///draws a surface on this surface with cropping
			void blit(int x, int y, Rect sourceRect, const Surface& otherSurface);
			///fills the surface with color
			void fill(Color color);
		private:
			friend class Window;//Window can create surface objects
			///private ctor
			Surface(SDL_Surface* new_surface, bool isWindow);
			SDL_Surface* surface;
			const bool isWindowSurface;
	};//end class Surface
	
	class Drawer {
		public:
			Drawer(SDL_Renderer *rend = nullptr);
			~Drawer();
			///
			void setColor(Color color);
			
			///draws a rectangle
			void drawRectangle(int x1, int y1, int x2, int y2, bool filled);
			void drawRectangle(Rect rect, bool filled);
			///draws a circle with center on (x,y) with the given radius
			void drawCircle(int x, int y, int rad, bool filled);
			///draw a circle inside the bounding rect
			void drawCircle(Rect bound, bool filled);
			///draws an ellipse with center on (x,y) with given rx, and ry radius
			void drawEllipse(int x, int y, int radX, int radY, bool filled);
			///draws an ellipse inside the bounding rect
			void drawEllipse(Rect bound, bool filled);
			///draws a line from (x1,y1) to (x2,y2) with thickness `width`
			void drawLine(int x1, int y1, int x2, int y2, unsigned char width = 0);
			///draws a set of connected lines
			void drawCLines(const std::vector<std::pair<int,int>> &points, unsigned char width = 0);
			///draws a polygon
			void drawPolygon(const std::vector<std::pair<int,int>> &points, bool filled);
		private:
			SDL_Renderer *renderer;
			Color rendColor;
	};//end class Drawer
	
}//end namespace multimedia

#endif /* MULTIMEDIA_WINDOW_HPP */
