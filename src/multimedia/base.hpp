#pragma once

#ifndef BASE_SYSTEM_HPP
#define BASE_SYSTEM_HPP

#include <string>
#include <SDL.h>

namespace multimedia {
	
	///For any errors that makes continue to run the game impossible 
	class CriticalSDLError : public std::exception {};
	
	///technically not an error, but is used to signal that the user requested to quit the game
	class SignaledGameEnd : public std::exception {};
	
	struct VideoMode {
		int width;
		int height;
		unsigned char bitsPerPixel;
		unsigned int refreshRate;
		SDL_DisplayMode source;
		
		std::string to_string() const;
		bool isValid() const;
	};
	
	struct Color {
		Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
		static Color fromHSV(double h, double s, double v, unsigned char alpha = 255);
		
		bool operator==(Color& other);
		
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		
		double h() const;
		double s() const;
		double v() const;
		
		Uint32 to_rgb(SDL_PixelFormat* format) const;
		Uint32 to_rgba(SDL_PixelFormat* format) const;
		SDL_Color to_struct() const;
		
		///linear interpolation between two colors
		static Color mix(Color a, Color b, double factor);
	};
	
}//end namespace multimedia

#endif /* BASE_SYSTEM_HPP */
