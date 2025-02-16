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
	class Window;
	
	
	class DirtyFlag {
		public:
			///checks the dirty flag and updates it
			bool needsRedraw();
		private:
			friend class Window;
			DirtyFlag();
			void invalidate();
			
			bool flag;
	};//end class DirtyFlag
	
	class Window {
		public:
			Window(std::string title, VideoMode video_mode, bool fullscreen, SDL2_System *system);
			~Window();
			
			//registering callbacks
			
			void registerUpdateFunction(std::function<void(int,int,bool)> callback);
			void registerDrawFunction(std::function<void()> callback);
			
			void mainLoop();
			void switchFullscreen();
			void switchToVideoMode(VideoMode mode);
			VideoMode getDisplayMode() const;
			
			Surface getWindowSurface();
			Drawer getWindowDrawer();
			
			int getWidth() const;
			int getHeight() const;
			bool isFullscreen() const;
			
			DirtyFlag* createDirtyFlag();
		private:
			SDL2_System *sdlSystem;
			SDL_Surface* windowSurface = nullptr;
			SDL_Window* windowHandle = nullptr;
			SDL_Renderer* windowRenderer = nullptr;
			bool fullscreenState;
			const std::string windowTitle;
			
			///a flag list that informs other parts of the game of necessity to redraw its elements
			std::vector<DirtyFlag*> flags;
			void invalidateFlags();
			
			std::function<void(int,int,bool)> updateCallback = nullptr;
			std::function<void()> drawCallback = nullptr;
	};//end class Window
	
	class Surface {
		public:
			static Surface fromFile(std::string filename);
			static Surface newRGB_Surface(unsigned int width, unsigned int height);
			static Surface newRGBA_Surface(unsigned int width, unsigned int height);
			
			Surface& operator=(Surface&& other);
			Surface& operator=(const Surface& other);
			Surface(const Surface& other);
			
			~Surface();
			Surface(SDL_Surface* extSurface = nullptr);
			
			bool optimizeForDrawingOn(Surface otherSurface);
			void crop(Rect cropRect);
			void scale(double factor);
			void scale(double x_factor, double y_factor);
			void scale(Rect destRect);
			
			///returns the bounding rectangle of the surface
			Rect getSize() const;
			///
			bool hasAlphaChannel() const;
			///
			Drawer getDrawer();
			
			void blit(Rect topLeft, const Surface& otherSurface);
			void blit(int x, int y, const Surface& otherSurface );
			///draws a surface on this surface with cropping
			void blit(int x, int y, Rect sourceRect, const Surface& otherSurface);
			void fill(Color color);
		private:
			friend class Window;//Window can create surface objects
			Surface(SDL_Surface* new_surface, bool isWindow);
			SDL_Surface* surface;
			const bool isWindowSurface;
	};//end class Surface
	
	class Drawer {
		public:
			Drawer(SDL_Renderer *rend = nullptr);
			~Drawer();
			
			void setColor(Color color);
			
			void drawRectangle(int x1, int y1, int x2, int y2, bool filled);
			void drawRectangle(Rect rect, bool filled);
			void drawCircle(int x, int y, int rad, bool filled);
			void drawCircle(Rect bound, bool filled);
			void drawEllipse(int x, int y, int radX, int radY, bool filled);
			void drawEllipse(Rect bound, bool filled);
			void drawLine(int x1, int y1, int x2, int y2, unsigned char width = 0);
			void drawCLines(const std::vector<std::pair<int,int>> &points, unsigned char width = 0);
			void drawPolygon(const std::vector<std::pair<int,int>> &points, bool filled);
		private:
			SDL_Renderer *renderer;
			Color rendColor;
	};//end class Drawer
	
}//end namespace multimedia

#endif /* MULTIMEDIA_WINDOW_HPP */
