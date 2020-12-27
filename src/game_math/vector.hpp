#pragma once

#ifndef GAME_MATH_VECTOR_HPP
#define GAME_MATH_VECTOR_HPP

namespace game_math {
	
	struct Vector {
		double x;
		double y;
		//get value of x as integer
		int intX() const;
		int intY() const;
		
		Vector(double newX = 0.0, double newY = 0.0);
		static Vector fromArgMod(double arg, double mod);
		
		Vector normalize() const;
		double length() const;
		///Angle of the vector
		double Arg() const;
		///rotate by an angle
		Vector rotate(double arg) const;
	};//end class Vector
	
	Vector operator+(const Vector a, const Vector b);
	Vector operator-(const Vector a, const Vector b);
	Vector operator/(const Vector vec, const double scale);
	Vector operator*(const Vector vec, const double scale);
	
	bool operator==(const Vector a, const Vector b);
	bool operator!=(const Vector a, const Vector b);
	
}//end namespace game_math

#endif /* GAME_MATH_VECTOR_HPP */

