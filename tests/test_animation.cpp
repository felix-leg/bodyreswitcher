


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE animation_module
#include <boost/test/unit_test.hpp>

#include <chrono>
#include "animation.hpp"

BOOST_AUTO_TEST_CASE( testSpanAnimation )
{
	double tested_percentage = 0.0;
	std::chrono::milliseconds halfSecond(500);
	std::chrono::seconds aSecond(1);
	
	auto startTime = std::chrono::steady_clock::now();
	animation::newSpanAnimation( 1.0 , [&tested_percentage](double percent){
		tested_percentage = percent;
	});
	
	//test for half
	decltype(startTime) halfTime = startTime + halfSecond;
	animation::applyTime(halfTime);
	//BOOST_TEST_REQUIRE( tested_percentage == 0.5 );
	BOOST_REQUIRE_CLOSE( tested_percentage, 0.5, 0.01 );
	
	//test for full
	decltype(startTime) fullTime = startTime + aSecond;
	animation::applyTime( fullTime );
	BOOST_REQUIRE_CLOSE( tested_percentage, 1.0, 0.01 );
}

BOOST_AUTO_TEST_CASE( testSpanAnimationWithShift )
{
	double tested_percentage = 0.0;
	std::chrono::milliseconds halfSecond(500);
	std::chrono::seconds aSecond(1);
	
	auto startTime = std::chrono::steady_clock::now();
	animation::newShiftedSpanAnimation( startTime, 1.0 , [&tested_percentage](double percent){
		tested_percentage = percent;
	});
	
	//test for half
	decltype(startTime) halfTime = startTime + halfSecond;
	animation::applyTime(halfTime);
	BOOST_TEST_REQUIRE( tested_percentage == 0.5 );
	
	//test for full
	decltype(startTime) fullTime = startTime + aSecond;
	animation::applyTime( fullTime );
	BOOST_TEST_REQUIRE( tested_percentage == 1.0 );
}

BOOST_AUTO_TEST_CASE( testSpanAnimationWithBeforeStartTime )
{
	bool haveBeenCalled = false;
	
	auto startTime = std::chrono::steady_clock::now();
	animation::newShiftedSpanAnimation( startTime, 1.0 , [&haveBeenCalled](double percent){
		haveBeenCalled = true;//this should never run
	});
	
	//calculate a time point before start time
	auto testTime = startTime - std::chrono::seconds(2);
	animation::applyTime( testTime );
	BOOST_TEST_REQUIRE( haveBeenCalled == false );
}

BOOST_AUTO_TEST_CASE( testPointAnimation )
{
	bool haveBeenCalled = false;
	
	auto timePoint = std::chrono::steady_clock::now();
	auto halfSecond = std::chrono::milliseconds(500);
	auto aSecond = std::chrono::seconds(1);
	
	animation::newPointAnimation(timePoint + halfSecond, [&haveBeenCalled](){
		haveBeenCalled = true;
	});
	animation::applyTime(timePoint + aSecond);
	
	BOOST_TEST_REQUIRE( haveBeenCalled );
}

BOOST_AUTO_TEST_CASE( testRemovingAnimation )
{
	bool haveBeenCalled = false;
	
	auto timePoint = std::chrono::steady_clock::now();
	auto halfSecond = std::chrono::milliseconds(500);
	auto aSecond = std::chrono::seconds(1);
	
	long long ref = animation::newPointAnimation(timePoint + halfSecond, [&haveBeenCalled](){
		haveBeenCalled = true;
	});
	animation::remove(ref);
	animation::applyTime(timePoint + aSecond);
	
	BOOST_TEST_REQUIRE( haveBeenCalled == false );
}
