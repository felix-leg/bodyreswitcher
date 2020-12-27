#pragma once

#ifndef PERSON_GRAPH_HPP
#define PERSON_GRAPH_HPP

#include "game_math/graph.hpp"

#include <string>
#include "mls.hpp"
#include <vector>
#include <random>
#include <stack>


namespace game_logic::main_game {
	
	constexpr unsigned int maxKidsCount = 18U;
	
	struct Person {
		std::string short_name;
		mls::Template name;
		double color;
		
		Person(std::string sName, mls::Template tName, double colNum);
	};//end class Person
	
	struct Body {
		Person *rightOwner;
		Person *currentOwner;
		
		Body(Person *right = nullptr, Person* current = nullptr);
		
		bool operator==(const Body& other);
		bool operator!=(const Body& other);
		
		bool hasCorrectOwner() const;
		void switchBodiesWith(Body &other);
	};//end class Body
	
	class ForbidenOperation : public std::exception {};
	
	class PersonGraph {
		const unsigned int minimumKidsAmount = 4U;
		const unsigned int easyKidsAmount = 8U;
		const unsigned int normalKidsAmount = 12U;
		const unsigned int maximumKidsAmount = 16U;
		
		public:
			PersonGraph();
			~PersonGraph();
			
			//game generators based of difficulty 
			///
			void generateEasyGame();
			///
			void generateNormalGame();
			///
			void generateHardGame();
			
			///generates initial switching pairs
			std::stack<std::pair<unsigned int, unsigned int>> generateInitialSwitch();
			///normal switching bodies
			void switchBodiesBetween(unsigned int indexA, unsigned int indexB);
			///undoing a switch
			void undoSwitchBodiesBetween(unsigned int indexA, unsigned int indexB);
			///test if two bodies can be switched
			bool canBeSwitched(unsigned int indexA, unsigned int indexB);
			///
			void add2ExtraPeople();
			
			///tests if the graph entered unwinable state
			bool isUnwinable() const;
			///test if graph represents winning state
			bool isFinished() const;
			
			///
			unsigned int getSize() const;
			///
			Body& getBodyByIndex(unsigned int n);
			///
			unsigned int getIndexOfBody(Body *body);
		private:
			///for use in randomization 
			std::random_device r_dev;
			std::mt19937 r_engine;
			
			///game graph
			game_math::Graph<Body> bodyGraph; 
			///new game generator
			void generateGame(unsigned int amountOfKids);
	};//end class PersonGraph
	
}//end namespace game_logic::main_game

#endif /* PERSON_GRAPH_HPP */
