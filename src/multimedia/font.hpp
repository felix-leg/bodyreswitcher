#pragma once

#ifndef MULTIMEDIA_FONT_HPP
#define MULTIMEDIA_FONT_HPP

#include "graphics.hpp"
#include "rect.hpp"
#include <SDL_ttf.h>
#include <string>

namespace multimedia {
	
	///helper function to select the longest text from given list
	std::string getTheLongestString(std::initializer_list<std::string> aList);
	std::string getTheLongestString(std::vector<std::string> aVector);
	std::string getTheLongestMultilineString(std::initializer_list<std::string> aList);
	
	enum class TextAlign {
		LEFT,
		CENTER,
		RIGHT
	};
	
	class Font {
		public:
		///empty ctor
		Font();
		///normal ctor
		Font(std::string filename, int size);
		///selects the font suitable for fitting a text inside a specifed rect
		static Font selectFontToFit(std::string filename, Rect spaceToFill, std::string text);
		///selects the font with the desired font height
		static Font selectFontWithPixelHeight(std::string filename, unsigned int pixelHeight);
		///
		~Font();
		
		///move operator
		Font& operator=(Font&& otherFont);
		
		///produces a SDL surface with the text
		Surface render(const std::string textToRender, Color textColor) const;
		///produces a SDL surface with the multiline text
		Surface renderMultiLine(std::string textToRender,
			unsigned int maxWidth,
			TextAlign align,
			Color textColor) const;
		///gives the size of the text after render
		Rect measureText(std::string textToRender) const;
		private:
		///private ctor
		Font(TTF_Font* extFont);
		///font structure
		TTF_Font* fontPtr;
	};//end class Font
	
}//end namespace multimedia

#endif /* MULTIMEDIA_FONT_HPP */
