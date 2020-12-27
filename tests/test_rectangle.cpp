#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rectangle_module
#include <boost/test/unit_test.hpp>

#include "rect.hpp"

BOOST_AUTO_TEST_CASE( testGetters )
{
	Rect r{2,3,20,30};
	
	BOOST_TEST_REQUIRE( r.getLeft() == 2 );
	BOOST_TEST_REQUIRE( r.getTop() == 3 );
	BOOST_TEST_REQUIRE( r.getBottom() == 33 );
	BOOST_TEST_REQUIRE( r.getRight() == 22 );
	
	BOOST_TEST_REQUIRE( r.getCenterX() == 12 );
	BOOST_TEST_REQUIRE( r.getCenterY() == 18 );
	
	BOOST_TEST_REQUIRE( r.getHeight() == 30 );
	BOOST_TEST_REQUIRE( r.getWidth() == 20 );
}

BOOST_AUTO_TEST_CASE( testSetters )
{
	Rect r{2,3,20,30};
	
	r.setLeft(0);
	BOOST_TEST_REQUIRE( r.getLeft() == 0 );
	r.setTop(1);
	BOOST_TEST_REQUIRE( r.getTop() == 1 );
	r.setRight(21);
	BOOST_TEST_REQUIRE( r.getLeft() == 1 );
	r.setBottom(33);
	BOOST_TEST_REQUIRE( r.getTop() == 3 );
	
	r.setCenterX( 12 );
	BOOST_TEST_REQUIRE( r.getLeft() == 2 );
	r.setCenterY( 15 );
	BOOST_TEST_REQUIRE( r.getTop() == 0 );
	
	r.setWidth(10);
	BOOST_TEST_REQUIRE( r.getWidth() == 10 );
	BOOST_TEST_REQUIRE( r.getRight() == 12 );
	r.setHeight(20);
	BOOST_TEST_REQUIRE( r.getHeight() == 20 );
	BOOST_TEST_REQUIRE( r.getBottom() == 20 );
}

BOOST_AUTO_TEST_CASE( testMove )
{
	Rect r{0,0,1,1};
	r = r.move(1,1);
	
	BOOST_TEST_REQUIRE( r.getLeft() == 1 );
	BOOST_TEST_REQUIRE( r.getTop() == 1 );
}

BOOST_AUTO_TEST_CASE( testInflate ) 
{
	Rect r{10,10,10,10};
	
	r = r.inflate(2);
	BOOST_TEST_REQUIRE( r.getLeft() == 8 );
	BOOST_TEST_REQUIRE( r.getRight() == 22 );
	BOOST_TEST_REQUIRE( r.getTop() == 8 );
	BOOST_TEST_REQUIRE( r.getBottom() == 22 );
	
	r = r.inflate(-2);
	BOOST_TEST_REQUIRE( r.getLeft() == 10 );
	BOOST_TEST_REQUIRE( r.getRight() == 20 );
	BOOST_TEST_REQUIRE( r.getTop() == 10 );
	BOOST_TEST_REQUIRE( r.getBottom() == 20 );
}

BOOST_AUTO_TEST_CASE( testConversionToSDL2Rect )
{
	Rect r{1,2,3,4};
	
	SDL_Rect *sr = r.sdl_rect();
	
	BOOST_TEST_REQUIRE( sr->x == r.getLeft() );
	BOOST_TEST_REQUIRE( sr->y == r.getTop() );
	BOOST_TEST_REQUIRE( sr->h == r.getHeight() );
	BOOST_TEST_REQUIRE( sr->w == r.getWidth() );
}

BOOST_AUTO_TEST_CASE( testInclusionOfAPoint )
{
	Rect r{10,10,10,10};
	
	BOOST_TEST_REQUIRE( r.includes(15,15) == true );
	BOOST_TEST_REQUIRE( r.includes(30,30) == false );
	BOOST_TEST_REQUIRE( r.includes(10,10,false) == false );
	BOOST_TEST_REQUIRE( r.includes(10,10,true) == true );
}

BOOST_AUTO_TEST_CASE( testRatioScaling )
{
	Rect r{0,0,1024,768};
	
	r.setWidthWithConstRatio(800);
	BOOST_TEST_REQUIRE( r.getWidth() == 800 );
	BOOST_TEST_REQUIRE( r.getHeight() == 600 );
	
	r = Rect(0,0,1024,768);
	
	r.setHeightWithConstRatio(600);
	BOOST_TEST_REQUIRE( r.getWidth() == 800 );
	BOOST_TEST_REQUIRE( r.getHeight() == 600 );
}


