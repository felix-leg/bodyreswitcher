/**
 * Contains logic of a graph with bodies.
 * Each body has assigned a right owner and a current owner.
*/

#include "person_graph.hpp"

#include <memory>
#include <map>

namespace game_logic::main_game {
	
	Person::Person(std::string sName, mls::Template tName, double colNum):
	short_name(sName), name(tName), color(colNum * 360.0) {}
	
	Body::Body(Person *right, Person* current):
	rightOwner(right), currentOwner(current) {}
	
	bool Body::hasCorrectOwner() const {
		return rightOwner == currentOwner;
	}
	
	void Body::switchBodiesWith(Body &other) {
		Person *tmp;
		tmp = other.currentOwner;
		other.currentOwner = currentOwner;
		currentOwner = tmp;
	}
	
	bool Body::operator==(const Body& other) {
		return (rightOwner == other.rightOwner) && (currentOwner == other.currentOwner);
	}
	
	bool Body::operator!=(const Body& other) {
		return (rightOwner != other.rightOwner) || (currentOwner != other.currentOwner);
	}
	
	///global list of all kids names
	std::vector<std::unique_ptr<Person>> allPersonsList;
	
	///a function setting all the kids' names
	void initializePersonNames() {
		std::string shortNamesString = _(
			"%{# A '|' separated list of the first letters of kids' names. Must be 18 of them. #}%"
			"A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|R|S"
		).get();
		
		std::vector<std::string> shortNamesList;
		std::string sName;
		auto pipePos = shortNamesString.find("|");
		do {
			if( pipePos == std::string::npos ) {
				sName = std::move(shortNamesString);
			} else {
				sName = shortNamesString.substr( 0 , pipePos );
				shortNamesString = shortNamesString.substr( pipePos + 1 );
				pipePos = shortNamesString.find("|");
			}
			shortNamesList.push_back( sName );
		} while( shortNamesString.length() > 0 );
		
		//filling person names
		allPersonsList.emplace_back( new Person(
			shortNamesList[0],
			_("%{#1st kid name#}%Amelia"),
			0.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[1],
			_("%{#2nd kid name#}%Brooke"),
			1.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[2],
			_("%{#3rd kid name#}%Charlie"),
			2.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[3],
			_("%{#4th kid name#}%Daisy"),
			3.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[4],
			_("%{#5th kid name#}%Emily"),
			4.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[5],
			_("%{#6th kid name#}%Freddie"),
			5.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[6],
			_("%{#7th kid name#}%George"),
			6.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[7],
			_("%{#8th kid name#}%Harry"),
			7.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[8],
			_("%{#9th kid name#}%Isabel"),
			8.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[9],
			_("%{#10th kid name#}%Julia"),
			9.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[10],
			_("%{#11th kid name#}%Katie"),
			10.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[11],
			_("%{#12th kid name#}%Lilly"),
			11.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[12],
			_("%{#13th kid name#}%Matthew"),
			12.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[13],
			_("%{#14th kid name#}%Nancy"),
			13.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[14],
			_("%{#15th kid name#}%Oliver"),
			14.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[15],
			_("%{#16th kid name#}%Peter"),
			15.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[16],
			_("%{#17th kid name#}%Robert"),
			16.0 / 17.0
		) );
		allPersonsList.emplace_back( new Person(
			shortNamesList[17],
			_("%{#18th kid name#}%Sophie"),
			17.0 / 17.0
		) );
	}
	
	PersonGraph::PersonGraph():
	r_engine(r_dev())
	 {
		if( allPersonsList.size() == 0 )
			initializePersonNames();
	}
	
	PersonGraph::~PersonGraph() {
		
	}
	
	void PersonGraph::generateEasyGame() {
		std::uniform_int_distribution<unsigned int> dist(minimumKidsAmount/2, easyKidsAmount/2);
		unsigned int result = dist(r_engine) * 2;
		
		generateGame(result);
	}
	void PersonGraph::generateNormalGame() {
		std::uniform_int_distribution<unsigned int> dist(easyKidsAmount/2, normalKidsAmount/2);
		unsigned int result = dist(r_engine) * 2;
		
		generateGame(result);
	}
	void PersonGraph::generateHardGame() {
		std::uniform_int_distribution<unsigned int> dist(normalKidsAmount/2, maximumKidsAmount/2);
		unsigned int result = dist(r_engine) * 2;
		
		generateGame(result);
	}
	
	void PersonGraph::generateGame(unsigned int kidsAmount) {
		bodyGraph = game_math::Graph<Body>(kidsAmount);
		
		bool selectedKids[maxKidsCount];
		for(unsigned int i=0; i<maxKidsCount; ++i) selectedKids[i] = false;
		
		std::uniform_int_distribution<unsigned int> dist(0, maxKidsCount - 1);
		unsigned int select;
		
		for(unsigned int kidNo = 0; kidNo < kidsAmount; ++kidNo) {
			//select new kid
			do{
				select = dist(r_engine);
			} while( selectedKids[select] == true );
			//put a new kid into the graph
			bodyGraph[kidNo].currentOwner = allPersonsList[select].get();
			bodyGraph[kidNo].rightOwner = allPersonsList[select].get();
			selectedKids[select] = true;
		}
		
	}
	
	std::stack<std::pair<unsigned int, unsigned int>> PersonGraph::generateInitialSwitch() {
		std::stack<std::pair<unsigned int, unsigned int>> result;
		
		const unsigned int kidCount = bodyGraph.getSize();
		bool *selectedKid = new bool[kidCount];
		for(unsigned int i=0; i<kidCount; ++i) selectedKid[i] = false;
		
		std::uniform_int_distribution<unsigned int> dist(0, kidCount - 1);
		unsigned int selectA, selectB;
		unsigned int toSelect = kidCount;
		
		while( toSelect > 0 ) {
			//randomize selection
			do{
				selectA = dist(r_engine);
				selectB = dist(r_engine);
			} while( 
				selectedKid[selectA] == true || 
				selectedKid[selectB] == true || 
				selectA == selectB);
			//set selection
			selectedKid[selectA] = true;
			selectedKid[selectB] = true;
			result.push( {selectA, selectB} );
			toSelect -= 2;
		}
		
		delete[] selectedKid;
		return result;
	}
	
	bool PersonGraph::isUnwinable() const {
		//unwinable means there are no connections in the person graph
		const unsigned int kidCount = bodyGraph.getSize();
		for(unsigned int a=0; a<kidCount; ++a) {
			for(unsigned int b=a+1; b<kidCount; ++b) {
				if( bodyGraph.isConnected(a,b) ) {
					return false;
				}
			}
		}
		return true;
	}
	
	bool PersonGraph::isFinished() const {
		//finished means everybody is in their own bodies
		for(auto body = bodyGraph.cbegin(); body != bodyGraph.cend(); ++body) {
			if( !(*body).hasCorrectOwner() ) {
				return false;
			}
		}
		return true;
	}
	
	void PersonGraph::switchBodiesBetween(unsigned int indexA, unsigned int indexB) {
		if( indexA >= bodyGraph.getSize() ) return;
		if( indexB >= bodyGraph.getSize() ) return;
		
		if( !bodyGraph.isConnected(indexA, indexB) )
			throw ForbidenOperation();
		
		bodyGraph[indexA].switchBodiesWith(bodyGraph[indexB]);
		bodyGraph.setConnection(indexA, indexB, false);
	}
	
	void PersonGraph::undoSwitchBodiesBetween(unsigned int indexA, unsigned int indexB) {
		if( indexA >= bodyGraph.getSize() ) return;
		if( indexB >= bodyGraph.getSize() ) return;
		
		if( bodyGraph.isConnected(indexA, indexB) )
			throw ForbidenOperation();
		
		bodyGraph[indexA].switchBodiesWith(bodyGraph[indexB]);
		bodyGraph.setConnection(indexA, indexB, true);
	}
	
	bool PersonGraph::canBeSwitched(unsigned int indexA, unsigned int indexB) {
		if( indexA >= bodyGraph.getSize() ) return false;
		if( indexB >= bodyGraph.getSize() ) return false;
		
		return bodyGraph.isConnected(indexA, indexB);
	}
	
	unsigned int PersonGraph::getSize() const {
		return bodyGraph.getSize();
	}
	
	Body& PersonGraph::getBodyByIndex(unsigned int n) {
		return bodyGraph[n];
	}
	
	unsigned int PersonGraph::getIndexOfBody(Body *body) {
		for(unsigned int index=0; index<bodyGraph.getSize(); ++index)
		{
			if( bodyGraph[index] == *body ) {
				return index;
			}
		}
		return bodyGraph.getSize();
	}
	
	void PersonGraph::add2ExtraPeople() {
		decltype(bodyGraph) newGraph{bodyGraph.getSize() + 2};
		std::map<Person*,bool> choosen;
		
		//select all person choosen to false by default
		for(const auto& person : allPersonsList) {
			choosen[person.get()] = false;
		}
		
		//copy bodies
		for(int i=0; i<bodyGraph.getSize(); ++i) {
			newGraph[i] = bodyGraph[i];
			choosen[bodyGraph[i].rightOwner] = true;
		}
		
		//copy connections
		for(int i=0; i<bodyGraph.getSize(); ++i) {
			for(int j=i+1; j<bodyGraph.getSize(); ++j) {
				if( !bodyGraph.isConnected(i,j) ) {
					newGraph.setConnection(i,j,false);
				}
			}
		}
		
		//make new bodies
		std::uniform_int_distribution<unsigned int> dist(0, maxKidsCount - 1);
		unsigned int select;
		Person* selectedPerson;
		
		for(int i=bodyGraph.getSize(); i<bodyGraph.getSize()+2; ++i) {
			do{
				select = dist(r_engine);
				selectedPerson = allPersonsList[select].get();
			} while(choosen[selectedPerson] == true);
			
			newGraph[i].currentOwner = newGraph[i].rightOwner = selectedPerson;
		}
		
		//replace the old graph
		bodyGraph = newGraph;
	}
	
}//end namespace game_logic::main_game

