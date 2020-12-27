
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multimedia_base_module
#include <boost/test/unit_test.hpp>

#include "multimedia/base.hpp"
#include <cmath>
#include <iostream>

BOOST_AUTO_TEST_CASE( testKnownHSVColors )
{
	using multimedia::Color;
	
	Color red = Color::fromHSV(0.0,100.0,100.0);
	BOOST_TEST_REQUIRE( (red.r == 255 && red.g == 0 && red.b == 0) );
	
	Color green = Color::fromHSV(120.0,100.0,100.0);
	BOOST_TEST_REQUIRE( (green.r == 0 && green.g == 255 && green.b == 0) );
	
	Color blue = Color::fromHSV(240.0,100.0,100.0);
	BOOST_TEST_REQUIRE( (blue.r == 0 && blue.g == 0 && blue.b == 255) );
	
	
	Color yellow = Color::fromHSV(60.0,100.0,100.0);
	BOOST_TEST_REQUIRE( (yellow.r == 255 && yellow.g == 255 && yellow.b == 0) );
	
	Color cyan = Color::fromHSV(180.0,100.0,100.0);
	BOOST_TEST_REQUIRE( (cyan.r == 0 && cyan.g == 255 && cyan.b == 255) );
	
	Color magenta = Color::fromHSV(300.0,100.0,100.0);
	BOOST_TEST_REQUIRE( (magenta.r == 255 && magenta.g == 0 && magenta.b == 255) );
	
	
	Color gray = Color::fromHSV(0.0,0.0,50.0);
	BOOST_TEST_REQUIRE( (gray.r == 127 && gray.g == 127 && gray.b == 127) );
	
	Color white = Color::fromHSV(0.0,0.0,100.0);
	BOOST_TEST_REQUIRE( (white.r == 255 && white.g == 255 && white.b == 255) );
	
	Color black = Color::fromHSV(0.0,0.0,0.0);
	BOOST_TEST_REQUIRE( (black.r == 0 && black.g == 0 && black.b == 0) );
}

BOOST_AUTO_TEST_CASE( testKnownHues )
{
	using multimedia::Color;
	
	Color red{255,0,0};
	BOOST_TEST_REQUIRE( red.h() == 0.0 );
	Color green{0,255,0};
	BOOST_TEST_REQUIRE( green.h() == 120.0 );
	Color blue{0,0,255};
	BOOST_TEST_REQUIRE( blue.h() == 240.0 );
	
	Color yellow{255,255,0};
	BOOST_TEST_REQUIRE( yellow.h() == 60.0 );
	Color cyan{0,255,255};
	BOOST_TEST_REQUIRE( cyan.h() == 180.0 );
	Color magenta{255,0,255};
	BOOST_TEST_REQUIRE( magenta.h() == 300.0 );
	
	Color nonHue{127,127,127};
	BOOST_TEST_REQUIRE( nonHue.h() == 0.0 );
}

BOOST_AUTO_TEST_CASE( testKnownSaturations )
{
	using multimedia::Color;
	
	Color white{255,255,255};
	BOOST_TEST_REQUIRE( std::round(white.s()) == 0.0 );
	
	Color halfSat{255,127,127};
	BOOST_TEST_REQUIRE( std::round(halfSat.s()) == 50.0 );
	
	Color fullSat{255,0,0};
	BOOST_TEST_REQUIRE( std::round(fullSat.s()) == 100.0 );
	
	Color fullSatHalfVal{127,0,0};
	BOOST_TEST_REQUIRE( std::round(fullSatHalfVal.s()) == 100.0 );
	
	Color halfSatHalfVal{127,63,63};
	BOOST_TEST_REQUIRE( std::round(halfSatHalfVal.s()) == 50.0 );
}

BOOST_AUTO_TEST_CASE( testKnownValues )
{
	using multimedia::Color;
	
	Color white{255,255,255};
	BOOST_TEST_REQUIRE( std::round(white.v()) == 100.0 );
	
	Color black{0,0,0};
	BOOST_TEST_REQUIRE( std::round(black.v()) == 0.0 );
	
	Color gray{127,127,127};
	BOOST_TEST_REQUIRE( std::round(gray.v()) == 50.0 );
	
	Color fullSatHalfVal{127,0,0};
	BOOST_TEST_REQUIRE( std::round(fullSatHalfVal.v()) == 50.0 );
	
	Color halfSatHalfVal{127,63,63};
	BOOST_TEST_REQUIRE( std::round(halfSatHalfVal.v()) == 50.0 );
	
	Color halfSatFullVal{255,127,127};
	BOOST_TEST_REQUIRE( std::round(halfSatFullVal.v()) == 100.0 );
}

BOOST_AUTO_TEST_CASE( testMixColor )
{
	using multimedia::Color;
	
	Color white{255,255,255};
	Color black{0,0,0};
	Color grayA{128,128,128};
	Color grayB{127,127,127};
	/*
	#define writeColor( color ) \
	std::cout 	<< " R:" << static_cast<unsigned int>(color.r) \
				<< " G:" << static_cast<unsigned int>(color.g) \
				<< " B:" << static_cast<unsigned int>(color.b) \
				<< " A:" << static_cast<unsigned int>(color.a) << std::endl;
	//*/
	Color shouldBeGray1 = Color::mix(white, black, 0.5);
	//writeColor(shouldBeGray1)
	BOOST_TEST_REQUIRE( (shouldBeGray1 == grayA || shouldBeGray1 == grayB) );
	Color shouldBeGray2 = Color::mix(black, white, 0.5);
	//writeColor(shouldBeGray2)
	BOOST_TEST_REQUIRE( (shouldBeGray2 == grayA || shouldBeGray2 == grayB) );
	
	Color shouldBeWhite1 = Color::mix(white, black, 0.0);
	Color shouldBeWhite2 = Color::mix(black, white, 1.0);
	BOOST_TEST_REQUIRE( (shouldBeWhite1 == white) );
	BOOST_TEST_REQUIRE( (shouldBeWhite2 == white) );
	
	Color shouldBeBlack1 = Color::mix(black, white, 0.0);
	Color shouldBeBlack2 = Color::mix(white, black, 1.0);
	BOOST_TEST_REQUIRE( (shouldBeBlack1 == black) );
	BOOST_TEST_REQUIRE( (shouldBeBlack2 == black) );
}

