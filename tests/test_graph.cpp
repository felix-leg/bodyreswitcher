#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE graph_module
#include <boost/test/unit_test.hpp>

#include "game_math/graph.hpp"
#include <string>

using namespace game_math;

BOOST_AUTO_TEST_CASE( testIsConnected )
{
	Graph<int> graph{4};
	int i = 0;
	
	for(auto& elem : graph) {
		elem = i++;
	}
	
	bool conn;
	conn = graph.isConnected(1,2);
	BOOST_TEST_REQUIRE( conn == true );
	graph.setConnection( 1, 2, false );
	conn = graph.isConnected(1,2);
	BOOST_TEST_REQUIRE( conn == false );
}

BOOST_AUTO_TEST_CASE( testIsConnectedWithStrings )
{
	Graph<std::string> graph{4};
	int i = 0;
	
	for(auto& elem : graph) {
		elem = std::to_string(i++);
	}
	
	bool conn;
	conn = graph.isConnected(std::string("1"), std::string("2"));
	BOOST_TEST_REQUIRE( conn == true );
	graph.setConnection(std::string("1"), std::string("2"), false);
	conn = graph.isConnected(std::string("1"), std::string("2"));
	BOOST_TEST_REQUIRE( conn == false );
}

BOOST_AUTO_TEST_CASE( testCopyAssign )
{
	Graph<int> graphA{4};
	Graph<int> graphB;
	
	int i = 0;
	for(auto& elem : graphA) { elem = i++; }
	
	bool conn;
	graphA.setConnection(1,2,false);
	
	BOOST_TEST_REQUIRE( graphA.getSize() == 4 );
	BOOST_TEST_REQUIRE( graphB.getSize() == 0 );
	
	graphB = graphA;
	BOOST_TEST_REQUIRE( graphA.getSize() == 4 );
	BOOST_TEST_REQUIRE( graphB.getSize() == 4 );
	conn = graphB.isConnected(1,2);
	BOOST_TEST_REQUIRE( conn == false );
	
	graphA.setConnection(0,3,false);
	conn = graphB.isConnected(0,3);
	BOOST_TEST_REQUIRE( conn == true );
}

BOOST_AUTO_TEST_CASE( testMoveAssign )
{
	Graph<int> graphA{4};
	Graph<int> graphB;
	
	int i = 0;
	for(auto& elem : graphA) { elem = i++; }
	
	bool conn;
	graphA.setConnection(1,2,false);
	
	BOOST_TEST_REQUIRE( graphA.getSize() == 4 );
	BOOST_TEST_REQUIRE( graphB.getSize() == 0 );
	
	graphB = std::move(graphA);
	BOOST_TEST_REQUIRE( graphA.getSize() == 0 );
	BOOST_TEST_REQUIRE( graphB.getSize() == 4 );
	conn = graphB.isConnected(1,2);
	BOOST_TEST_REQUIRE( conn == false );
}


