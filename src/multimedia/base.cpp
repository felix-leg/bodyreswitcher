/**
 * Common elements in the multimedia supermodule 
*/

#include "base.hpp"
#include <sstream>
#include <algorithm> //for std::max()
#include <cmath> //for std::fmod()

namespace multimedia {

	std::string VideoMode::to_string() const {
		std::ostringstream result;
		result << width << "×" << height << " ";
		result << static_cast<unsigned int>(bitsPerPixel) << " (";
		result << refreshRate << " Hz)";
		return result.str();
	}
	
	bool VideoMode::isValid() const {
		if( width == 0 || height == 0) return false;
		return true;
	}
	
	//Color class --------------
	
	Color::Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha):
		r(red), g(green), b(blue), a(alpha) {}
	
	Color Color::fromHSV(double h/*0-360*/, double s/*0-100*/, double v/*0-100*/, unsigned char alpha) {
		if( h > 360.0 || h < 0.0 || s > 100.0 || s < 0.0 || v > 100.0 || v < 0.0 ) {
			return Color(0,0,0,alpha);
		}
		
		s /= 100.0;
		v /= 100.0;
		double C = s*v;
		double X = C * (1 - abs(std::fmod(h/60.0, 2) - 1));
		double m = v-C;
		
		double r,g,b;
		if( h >= 0.0 && h < 60.0 ) {
			r = C;
			g = X;
			b = 0.0;
		}
		else if( h >= 60.0 && h < 120.0 ) {
			r = X;
			g = C;
			b = 0.0;
		}
		else if( h >= 120.0 && h < 180.0 ) {
			r = 0.0;
			g = C;
			b = X;
		}
		else if( h >= 180.0 && h < 240.0 ) {
			r = 0.0;
			g = X;
			b = C;
		}
		else if( h >= 240.0 && h < 300.0 ) {
			r = X;
			g = 0.0;
			b = C;
		}
		else {
			r = C;
			g = 0.0;
			b = X;
		}
		
		return Color(
			static_cast<unsigned char>((r+m) * 255),
			static_cast<unsigned char>((g+m) * 255),
			static_cast<unsigned char>((b+m) * 255),
			alpha
		);
	}
	
	bool Color::operator==(Color& other) {
		if( r != other.r ) return false;
		if( g != other.g ) return false;
		if( b != other.b ) return false;
		if( a != other.a ) return false;
		return true;
	}
	
	double Color::h() const {
		double R = static_cast<double>(r) / 255.0;
		double G = static_cast<double>(g) / 255.0;
		double B = static_cast<double>(b) / 255.0;
		
		double CMax = std::max({R,G,B});
		double CMin = std::min({R,G,B});
		double delta = CMax - CMin;
		
		if( delta > 0 ) {
			double H;
			if( CMax == R ) {
				H = 60 * fmod((G-B)/delta, 6);
			} else if( CMax == G ) {
				H = 60 * ((B-R)/delta + 2);
			} else if( CMax == B ) {
				H = 60 * ((R-G)/delta + 4);
			}
			
			if( H < 0.0 ) H += 360.0;
			
			return H;
		} else {
			return 0.0;
		}
	}
	
	double Color::s() const {
		double R = static_cast<double>(r) / 255.0;
		double G = static_cast<double>(g) / 255.0;
		double B = static_cast<double>(b) / 255.0;
		
		double CMax = std::max({R,G,B});
		double CMin = std::min({R,G,B});
		double delta = CMax - CMin;
		
		if( delta > 0.0 ) {
			if( CMax > 0.0 ) {
				return (delta / CMax) * 100.0;
			} else {
				return 0.0;
			}
		} else {
			return 0.0;
		}
	}
	
	double Color::v() const {
		double R = static_cast<double>(r) / 255.0;
		double G = static_cast<double>(g) / 255.0;
		double B = static_cast<double>(b) / 255.0;
		
		return std::max({R,G,B}) * 100.0;		
	}
	
	Uint32 Color::to_rgb(SDL_PixelFormat* format) const {
		if( format == nullptr ) {
			return (static_cast<Uint32>(r) << 24 |
					static_cast<Uint32>(g) << 16 |
					static_cast<Uint32>(b) << 8 |
					0
			);
		} else {
			return SDL_MapRGB(format, r, g, b);
		}
	}
	Uint32 Color::to_rgba(SDL_PixelFormat* format) const {
		if( format == nullptr ) {
			return (static_cast<Uint32>(r) << 24 |
					static_cast<Uint32>(g) << 16 |
					static_cast<Uint32>(b) << 8 |
					static_cast<Uint32>(a)
			);
		} else {
			return SDL_MapRGBA(format, r, g, b, a);
		}
	}
	
	SDL_Color Color::to_struct() const {
		return SDL_Color{r,g,b,a};
	}
	
	template<typename OutputType>
	OutputType lerp(OutputType a, OutputType b, double factor) {
		return static_cast<OutputType>((b-a) * factor) + a;
	}
	
	Color Color::mix(Color first, Color second, double factor) {
		unsigned char r,g,b,a;
		
		r = lerp(first.r, second.r, factor);
		g = lerp(first.g, second.g, factor);
		b = lerp(first.b, second.b, factor);
		a = lerp(first.a, second.a, factor);
		
		return Color{r,g,b,a};
	}

}//end namespace multimedia
