/**
 * Module for math vector objects
*/

#include "vector.hpp"
//for M_PI constant
#define _USE_MATH_DEFINES
#include <cmath>

namespace game_math {
	
	Vector::Vector(double newX, double newY):x(newX),y(newY) {}
	
	Vector Vector::fromArgMod(double arg, double mod) {
		double x = std::cos(arg) * mod;
		double y = std::sin(arg) * mod;
		return Vector(x,y);
	}
	
	int Vector::intX() const {
		return static_cast<int>(std::round(x));
	}
	
	int Vector::intY() const {
		return static_cast<int>(std::round(y));
	}
	
	Vector operator+(const Vector a, const Vector b) {
		return Vector(a.x + b.x, a.y + b.y);
	}
	
	Vector operator-(const Vector a, const Vector b) {
		return Vector(a.x - b.x, a.y - b.y);
	}
	
	Vector operator/(const Vector vec, const double scale) {
		return Vector( vec.x / scale , vec.y / scale );
	}
	
	Vector operator*(const Vector vec, const double scale) {
		return Vector( vec.x * scale , vec.y * scale );
	}
	
	double Vector::length() const {
		return std::sqrt( x * x + y * y );
	}
	
	Vector Vector::normalize() const {
		return *this / length();
	}
	
	double Vector::Arg() const {
		if( x == 0.0 && y == 0.0 ) return 0.0;
		double cal = std::atan2(y,x);
		if( cal < 0.0 ) cal += M_PI * 2;
		return cal;
	}
	
	Vector Vector::rotate(double arg) const {
		arg = Arg() + arg;
		double mod = length();
		return Vector::fromArgMod(arg, mod);
	}
	
	bool operator==(const Vector a, const Vector b) {
		return (a.x == b.x) && (a.y == b.y);
	}
	bool operator!=(const Vector a, const Vector b) {
		return (a.x != b.x) || (a.y != b.y);
	}
	
}//end namespace game_math

