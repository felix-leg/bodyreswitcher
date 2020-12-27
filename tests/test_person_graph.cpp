#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE person_graph_module
#include <boost/test/unit_test.hpp>

#include "logic/person_graph.hpp"

using game_logic::main_game::PersonGraph;
using game_logic::main_game::ForbidenOperation;

#include <iostream>

BOOST_AUTO_TEST_CASE( testIsNotUnwinableAtTheBegining ) 
{
	mls_init();
	
	PersonGraph graph;
	
	graph.generateHardGame();
	
	BOOST_TEST_REQUIRE( graph.isUnwinable() == false );
}

BOOST_AUTO_TEST_CASE( testIsNotFinishedAfterSwitchero )
{
	mls_init();
	
	PersonGraph graph;
	
	graph.generateHardGame();
	
	BOOST_TEST_REQUIRE( graph.isFinished() == true );
	
	auto switches = graph.generateInitialSwitch();
	while(!switches.empty()) {
		auto s = switches.top(); switches.pop();
		//std::cout << "(" << s.first << "," << s.second << ")" << std::endl;
		graph.switchBodiesBetween(s.first, s.second);
	}
	
	BOOST_TEST_REQUIRE( graph.isFinished() == false );
	
}

BOOST_AUTO_TEST_CASE( testIsFinishedAfterSwitchAndReSwitch )
{
	mls_init();
	
	PersonGraph graph;
	
	graph.generateHardGame();
	
	BOOST_TEST_REQUIRE( graph.isFinished() == true );
	
	auto switches = graph.generateInitialSwitch();
	while(!switches.empty()) {
		auto s = switches.top(); switches.pop();
		//std::cout << "(" << s.first << "," << s.second << ")" << std::endl;
		graph.switchBodiesBetween(s.first, s.second);
		graph.undoSwitchBodiesBetween(s.first, s.second);
	}
	
	BOOST_TEST_REQUIRE( graph.isFinished() == true );
	
}

BOOST_AUTO_TEST_CASE( testIfSwitchingTwiceThrowsAnException )
{
	mls_init();
	
	PersonGraph graph;
	
	graph.generateHardGame();
	
	auto switches = graph.generateInitialSwitch();
	while(!switches.empty()) {
		auto s = switches.top(); switches.pop();
		//std::cout << "(" << s.first << "," << s.second << ")" << std::endl;
		graph.switchBodiesBetween(s.first, s.second);
		try {
			graph.switchBodiesBetween(s.first, s.second);
			BOOST_FAIL("ForbidenOperation wasn't thrown");
		} catch(ForbidenOperation) {
			continue;
		}
	}
	
}

BOOST_AUTO_TEST_CASE( testIfSwitchingDisolvesTheConnection )
{
	mls_init();
	
	PersonGraph graph;
	
	graph.generateHardGame();
	
	auto switches = graph.generateInitialSwitch();
	while(!switches.empty()) {
		auto s = switches.top(); switches.pop();
		//std::cout << "(" << s.first << "," << s.second << ")" << std::endl;
		graph.switchBodiesBetween(s.first, s.second);
		BOOST_TEST_REQUIRE( graph.canBeSwitched(s.first, s.second) == false );
	}
	
}

