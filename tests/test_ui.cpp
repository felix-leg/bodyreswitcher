#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ui_module
#include <boost/test/unit_test.hpp>

#include "ui.hpp"

BOOST_AUTO_TEST_CASE( testMoveOverElement )
{
	bool hasBeenCalled = false;
	
	UI::Space space(0,0,100,100 /*, 50,50*/);
	
	UI::Component *component = space.newComponent(10,10,20,20);
	component->addMouseOverListener([&hasBeenCalled](){
		hasBeenCalled = true;
	});
	
	space.mouseMovedTo(15,15);
	
	BOOST_TEST_REQUIRE( hasBeenCalled == true );
}

BOOST_AUTO_TEST_CASE( testMoveEnterElement )
{
	bool hasBeenCalled = false;
	
	UI::Space space(0,0,100,100 /*, 50,50*/);
	
	UI::Component *component = space.newComponent(10,10,20,20);
	component->addMouseEnterListener([&hasBeenCalled](){
		hasBeenCalled = true;
	});
	
	space.mouseMovedTo(15,15);
	
	BOOST_TEST_REQUIRE( hasBeenCalled == true );
}

BOOST_AUTO_TEST_CASE( testMoveLeaveElement )
{
	bool hasBeenCalled = false;
	
	UI::Space space(0,0,100,100 /*, 50,50*/);
	
	UI::Component *component = space.newComponent(10,10,20,20);
	component->addMouseLeaveListener([&hasBeenCalled](){
		hasBeenCalled = true;
	});
	
	space.mouseMovedTo(15,15);
	space.mouseMovedTo(90,90);
	
	BOOST_TEST_REQUIRE( hasBeenCalled == true );
}

BOOST_AUTO_TEST_CASE( testClickInsideElement )
{
	bool hasBeenCalled = false;
	
	UI::Space space(0,0,100,100 /*, 50,50*/);
	
	UI::Component *component = space.newComponent(10,10,20,20);
	component->addClickListener([&hasBeenCalled](){
		hasBeenCalled = true;
	});
	
	space.mouseMovedTo(15,15);
	space.mouseClick();
	
	BOOST_TEST_REQUIRE( hasBeenCalled == true );
}

BOOST_AUTO_TEST_CASE( testClickOutsideElement )
{
	bool hasBeenCalled = false;
	
	UI::Space space(0,0,100,100 /*, 50,50*/);
	
	UI::Component *component = space.newComponent(10,10,20,20);
	component->addClickListener([&hasBeenCalled](){
		hasBeenCalled = true;
	});
	
	space.mouseMovedTo(80,80);
	space.mouseClick();
	
	BOOST_TEST_REQUIRE( hasBeenCalled == false );
}

