#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE vector_module
#include <boost/test/unit_test.hpp>

#include "game_math/vector.hpp"
#include <math.h>

using namespace game_math;

BOOST_AUTO_TEST_CASE( testSumAndDiff )
{
	Vector a{1.0, 2.0};
	Vector b{1.0, 3.0};
	
	Vector sum = a + b;
	Vector diff = a - b;
	
	BOOST_TEST_REQUIRE( sum.x == 2.0 );
	BOOST_TEST_REQUIRE( sum.y == 5.0 );
	BOOST_TEST_REQUIRE( diff.x == 0.0 );
	BOOST_TEST_REQUIRE( diff.y == -1.0 );
}

BOOST_AUTO_TEST_CASE( testNormalize ) 
{
	const double scale = 4.5;
	const double origX = 0.6;
	const double origY = 0.8;
	
	Vector to_norm{origX * scale, origY * scale};
	to_norm = to_norm.normalize();
	
	BOOST_TEST_REQUIRE( to_norm.x == origX );
	BOOST_TEST_REQUIRE( to_norm.y == origY );
	BOOST_TEST_REQUIRE( to_norm.length() == 1.0 );
}

BOOST_AUTO_TEST_CASE( testGetArgument )
{
	const double wantedArg = M_PI / 4.0;
	
	Vector one{1.0,1.0};
	double arg = one.Arg();
	
	BOOST_TEST_REQUIRE( arg == wantedArg );
}

BOOST_AUTO_TEST_CASE( testKnownArguments )
{
	const double arg0 = 0.0;
	const double arg90 = M_PI / 2;
	const double arg180 = M_PI;
	const double arg270 = M_PI * 3 / 2;
	
	Vector vec0{1.0,0.0};
	Vector vec90{0.0,1.0};
	Vector vec180{-1.0,0.0};
	Vector vec270{0.0,-1.0};
	
	BOOST_TEST_REQUIRE( vec0.Arg() == arg0 );
	BOOST_TEST_REQUIRE( vec90.Arg() == arg90 );
	BOOST_TEST_REQUIRE( vec180.Arg() == arg180 );
	BOOST_TEST_REQUIRE( vec270.Arg() == arg270 );
}

BOOST_AUTO_TEST_CASE( testConversionFromArgMod )
{
	const double arg0 = 0.0;
	const double arg90 = M_PI / 2;
	const double arg180 = M_PI;
	const double arg270 = M_PI * 3 / 2;
	
	const Vector vec0{1.0,0.0};
	const Vector vec90{0.0,1.0};
	const Vector vec180{-1.0,0.0};
	const Vector vec270{0.0,-1.0};
	
	constexpr double closeness = 1.0;
	
	Vector test = Vector::fromArgMod(arg0, 1.0);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec0.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec0.y) , closeness );
	test = Vector::fromArgMod(arg90, 1.0);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec90.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec90.y) , closeness );
	test = Vector::fromArgMod(arg180, 1.0);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec180.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec180.y) , closeness );
	test = Vector::fromArgMod(arg270, 1.0);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec270.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec270.y) , closeness );
}

BOOST_AUTO_TEST_CASE( testRotation )
{
	const double arg90 = M_PI / 2;
	
	const Vector vec0{1.0,0.0};
	const Vector vec90{0.0,1.0};
	const Vector vec180{-1.0,0.0};
	const Vector vec270{0.0,-1.0};
	
	constexpr double closeness = 1.0;
	
	Vector test = vec0.rotate(arg90);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec90.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec90.y) , closeness );
	test = vec90.rotate(arg90);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec180.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec180.y) , closeness );
	test = vec180.rotate(arg90);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec270.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec270.y) , closeness );
	test = vec270.rotate(arg90);
	BOOST_REQUIRE_CLOSE( std::round(test.x) , std::round(vec0.x) , closeness );
	BOOST_REQUIRE_CLOSE( std::round(test.y) , std::round(vec0.y) , closeness );
}

