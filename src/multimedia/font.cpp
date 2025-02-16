/**
 * A class representing SDL2 Font.
*/

#include "errors.hpp"
#include "font.hpp"
#include "filesystem.hpp"
#include "mls.hpp"
#include <vector>

namespace multimedia {
	
	
	std::string getTheLongestString(std::initializer_list<std::string> aList) {
		std::string theLongest{""};
		
		for(const std::string& aText : aList) {
			if( aText.length() > theLongest.length() )
				theLongest = aText;
		}
		
		return theLongest;
	}
	
	std::string getTheLongestString(std::vector<std::string> aList) {
		std::string theLongest{""};
		
		for(const std::string& aText : aList) {
			if( aText.length() > theLongest.length() )
				theLongest = aText;
		}
		
		return theLongest;
	}
	
	std::string getTheLongestMultilineString(std::initializer_list<std::string> aList) {
		std::vector<std::string> lines;
		for(const std::string& elem : aList) {
			auto CRpos = elem.find("\n");
			decltype(CRpos) startPos = 0;
			
			while( CRpos != std::string::npos ) {
				if( CRpos - startPos > 0 )//ignore empty lines
					lines.push_back( elem.substr(startPos, CRpos - startPos) );
				startPos = CRpos + 1;
				if( startPos < elem.length() )
					CRpos = elem.find("\n", startPos);
				else
					CRpos = std::string::npos;
			}
			//add the last string
			if( startPos < elem.length() )
				lines.push_back( elem.substr(startPos) );
		}
		
		return getTheLongestString(lines);
	}
	
	Font::Font():fontPtr(nullptr) {}
	
	Font::Font(std::string filename, int size) {
		filename = filesystem::getDataPath(filename);
		if( !filesystem::pathExists(filename) ) {
			ShowError( _("Can't load font '%{filename}%': file doesn't exist")
				.apply("filename", filename).get() );
			throw CriticalSDLError();
		}
		
		fontPtr = TTF_OpenFont( filename.c_str(), size);
		if( fontPtr == nullptr ) {
			ShowError( _("Can't load font '%{filename}%' with size %{size}%pt: ")
			.apply("filename",filename).apply("size", size).get()
				, TTF_GetError() );
			throw CriticalSDLError();
		}
	}
	
	Font::Font(TTF_Font* extFont):fontPtr(extFont) {}
	
	Font& Font::operator=(Font&& otherFont) {
		if( this == &otherFont ) return *this;
		
		if( fontPtr != nullptr ) TTF_CloseFont( fontPtr );
		fontPtr = otherFont.fontPtr;
		otherFont.fontPtr = nullptr;
		
		return *this;
	}
	
	Font Font::selectFontToFit(std::string filename, Rect spaceToFill, std::string text) {
		filename = filesystem::getDataPath(filename);
		if( !filesystem::pathExists(filename) ) {
			ShowError( _("Can't load font '%{filename}%': file doesn't exist")
				.apply("filename", filename).get() );
			throw CriticalSDLError();
		}
		
		int selectedFontSize = 8;//start with 8pt
		constexpr int maxFontSize = 50;
		TTF_Font * testedFont, *prevFont = nullptr;
		int width, height;
		
		while( selectedFontSize < maxFontSize ) {
			testedFont = TTF_OpenFont( filename.c_str(), selectedFontSize);
			if( testedFont == nullptr ) {
				ShowError( _("Can't load font '%{filename}%' with size %{size}%pt: ")
					.apply("filename",filename).apply("size", selectedFontSize).get()
					, TTF_GetError() );
				throw CriticalSDLError();
			}
			
			//get the metrics
			TTF_SizeUTF8(testedFont, text.c_str(), &width, &height);
			//if too big return from the loop
			if( height > spaceToFill.getHeight() ) break;
			if( width > spaceToFill.getWidth() ) break;
			
			//not yet filled, continue with the next font
			if( prevFont != nullptr ) TTF_CloseFont( prevFont );
			prevFont = testedFont;
			selectedFontSize += 1;
		}
		//font selected. The right font is in prevFont
		if( testedFont != prevFont ) TTF_CloseFont( testedFont );
		if( prevFont == nullptr ) {
			ShowError( _("Couldn't found a font to fit '%{text}%'"
			" into a %{width}%x%{height}% rectangle")
			.apply("text",text)
			.apply("width", spaceToFill.getWidth())
			.apply("height", spaceToFill.getHeight())
			.get() );
			throw CriticalSDLError();
		}
		return Font{ prevFont };
	}
	
	Font Font::selectFontWithPixelHeight(std::string filename, unsigned int pixelHeight) {
		filename = filesystem::getDataPath(filename);
		if( !filesystem::pathExists(filename) ) {
			ShowError(  _("Can't load font '%{filename}%': file doesn't exist")
				.apply("filename", filename).get() );
			throw CriticalSDLError();
		}
		
		int selectedFontSize = 8;//start with 8pt
		constexpr int maxFontSize = 50;
		TTF_Font * testedFont, *prevFont = nullptr;
		
		while( selectedFontSize < maxFontSize ) {
			testedFont = TTF_OpenFont( filename.c_str(), selectedFontSize);
			if( testedFont == nullptr ) {
				ShowError( _("Can't load font '%{filename}%' with size %{size}%pt: ")
					.apply("filename",filename).apply("size", selectedFontSize).get()
					, TTF_GetError() );
				throw CriticalSDLError();
			}
			
			//get the font size
			int fontPixelHeight = TTF_FontHeight(testedFont);
			if( fontPixelHeight > pixelHeight ) break;
			
			//not yet found, continue with the next font
			if( prevFont != nullptr ) TTF_CloseFont( prevFont );
			prevFont = testedFont;
			selectedFontSize += 1;
		}
		
		//font selected. The right font is in prevFont
		if( testedFont != prevFont ) TTF_CloseFont( testedFont );
		if( prevFont == nullptr ) {
			ShowError( _("Couldn't find a font of the desired %{size}%px size")
			.apply("size", pixelHeight).get() );
			throw CriticalSDLError();
		}
		return Font{ prevFont };
	}
	
	Font::~Font() {
		if( fontPtr != nullptr ) TTF_CloseFont( fontPtr );
	}
	
	Rect Font::measureText(std::string textToRender) const {
		if( fontPtr == nullptr ) throw CriticalSDLError();
		
		int width, height;
		TTF_SizeUTF8(fontPtr, textToRender.c_str(), &width, &height);
		
		return Rect{0,0,width,height};
	}
	
	Surface Font::render(const std::string textToRender, Color textColor) const {
		if( fontPtr == nullptr ) throw CriticalSDLError();
		
		SDL_Surface* surf = TTF_RenderUTF8_Blended(fontPtr, textToRender.c_str(), textColor.to_struct());
		if( surf == nullptr ) {
			ShowError( _("Can't render the text '%{text}%': ").apply("text",textToRender).get()
				, TTF_GetError() );
			throw CriticalSDLError();
		}
		return Surface(surf);
	}
	
	Surface Font::renderMultiLine(std::string textToRender,
			unsigned int maxWidth,
			TextAlign align,
			Color textColor) const {
		if( fontPtr == nullptr ) throw CriticalSDLError();
		
		std::string oneLine{""}, oneWord, testString;
		std::vector<Surface> surfLines;
		int width, height;
		const std::string blanks{" \t\f\n\r"};
		
		const unsigned int defaultLineSpacing = TTF_FontLineSkip(fontPtr) - TTF_FontHeight(fontPtr);
		
		auto blankPosition = textToRender.find_first_of(blanks);
		
		while( textToRender.length() > 0 ) {
			oneWord = "";
			bool lineBreak = false;
			//fill the `oneLine` until it has the maximum amount of characters fiting the `maxWidth`
			do {
				if( blankPosition != std::string::npos && textToRender[blankPosition] == '\n')
					lineBreak = true;
				
				if( blankPosition == std::string::npos ) {
					oneWord = std::move(textToRender);
				} else {
					oneWord = textToRender.substr( 0 , blankPosition );
					if( blankPosition + 1 > textToRender.size() )
						textToRender = "";
					else
						textToRender = textToRender.substr( blankPosition + 1 );
					blankPosition = textToRender.find_first_of(blanks);
				}
				
				if( oneWord.length() > 0 ) {
					if( oneLine.length() > 0 ) testString = oneLine + " " + oneWord;
					else testString = oneWord;
					
					TTF_SizeUTF8(fontPtr, testString.c_str(), &width, &height);
					if( width > maxWidth ) {//max fiting line found - break
						textToRender = oneWord + " " + textToRender;
						blankPosition = textToRender.find_first_of(blanks);
						break;
					}
					
					oneLine = std::move(testString);
				}
				if( lineBreak ) break;
			} while( textToRender.length() > 0 );
			
			//test if one word isn't too wide
			if( (oneLine.length() == 0 || oneLine == oneWord) && oneWord.length() > 0 ) {
				TTF_SizeUTF8(fontPtr, oneWord.c_str(), &width, &height);
				if( width > maxWidth ) {
					ShowError( _("Unable to fit the word %{wide_word}% into "
					"the desired %{max_width}%px size")
					.apply("wide_word",oneWord).apply("max_width",maxWidth).get() );
					throw CriticalSDLError();
				}
			}
			
			//a line which fits in the desired length found - render the surface and add to the list
			if( oneLine.length() > 0 ) {//but test first if we have got a text to render
				surfLines.push_back( Surface(TTF_RenderUTF8_Blended(
					fontPtr, oneLine.c_str(), textColor.to_struct()
					)) );
			}
			
			//prepare for the next iteration
			oneLine = "";
		}
		
		
		//is there the last line to render?
		if( textToRender.length() > 0 ) {
			oneLine = std::move(textToRender);
			//test if it is not too wide
			TTF_SizeUTF8(fontPtr, oneLine.c_str(), &width, &height);
			if( width > maxWidth ) {
				ShowError( _("Unable to fit the word %{wide_word}% into "
				"the desired %{max_width}%px size")
				.apply("wide_word",oneLine).apply("max_width",maxWidth).get() );
				throw CriticalSDLError();
			}
			//the last line fits - render and add it to the `surfList`
			surfLines.push_back( Surface(TTF_RenderUTF8_Blended(
				fontPtr, oneLine.c_str(), textColor.to_struct()
				)) );
		}
		
		//calculate the size of the result surface with the rendered text
		width = 0;
		height = 0;
		for(const auto& surf : surfLines) {
			Rect surfSize = surf.getSize();
			height += surfSize.getHeight() + defaultLineSpacing;
			if( width < surfSize.getWidth() ) width = surfSize.getWidth();
		}
		if(height > 0) height -= defaultLineSpacing;
		
		//glue together the surfaces according to the desired text align
		Surface result = Surface::newRGBA_Surface(width, height);
		int yPos = 0;
		Rect pos;
		for(const auto& surf : surfLines) {
			pos = surf.getSize();
			switch( align ) {
				case TextAlign::LEFT:
					pos.setLeft(0);
					break;
				case TextAlign::CENTER:
					pos.setCenterX(width/2);
					break;
				case TextAlign::RIGHT:
					pos.setRight(width);
					break;
			}
			result.blit(pos.getLeft(), yPos, surf);
			yPos += pos.getHeight() + defaultLineSpacing;
		}
		
		return result;
	}
	
}//end namespace multimedia
