#pragma once

#ifndef MATH_GRAPH_HPP
#define MATH_GRAPH_HPP

/**
 * Module for graph template
*/

#include <vector>
#include <algorithm>

namespace game_math {
	
	template<typename GraphElement>
	class Graph {
		public:
			Graph(unsigned int numVertices = 0):vertCount(numVertices) {
				//make vertices
				for(unsigned int i = 0; i<vertCount; ++i) {
					vertices.emplace_back();
				}
				//make edges (connected by default)
				if( vertCount == 0 ) {
					edges = nullptr;
					edgesSize = 0;
					return;
				}
				edgesSize = vertCount*vertCount;
				edges = new bool[edgesSize];
				for(unsigned int i = 0; i<edgesSize; ++i) {
					edges[i] = true;
				}
			};
			
			~Graph() {
				if(edges != nullptr) delete[] edges;
				vertices.clear();
			};
			
			//copy assignment 
			Graph<GraphElement>& operator=(const Graph<GraphElement>& other) {
				if( this == &other ) return *this;
				
				if(edges != nullptr) delete[] edges;
				vertices.clear();
				vertices = other.vertices;
				vertCount = other.vertCount;
				edgesSize = other.edgesSize;
				
				edges = new bool[edgesSize];
				for(unsigned int i=0; i<edgesSize; ++i) {
					edges[i] = other.edges[i];
				}
				
				return *this;
			};
			
			//move assignment
			Graph<GraphElement>& operator=(Graph<GraphElement>&& other) {
				if( this == &other ) return *this;
				
				delete[] edges;
				vertices.clear();
				vertices = std::move(other.vertices);
				vertCount = other.vertCount; other.vertCount = 0;
				edges = other.edges; other.edges = nullptr;
				edgesSize = other.edgesSize; other.edgesSize = 0;
				
				return *this;
			};
			
			unsigned int getSize() const {
				return vertCount;
			};
			
			bool isConnected(const unsigned int indexA, const unsigned int indexB) const {
				if( indexA == indexB ) return true;
				if( indexA >= vertCount ) return false;
				if( indexA >= vertCount ) return false;
				return *getEdge(indexA, indexB);
			}
			
			bool isConnected(const GraphElement &elA, const GraphElement &elB) const {
				if( edges == nullptr ) return false;
				
				auto it = std::find(vertices.begin(), vertices.end(), elA);
				unsigned int indexA;
				if( it != vertices.end() ) {
					indexA = std::distance(vertices.begin(),it);
				} else { return false; }
				
				it = std::find(vertices.begin(), vertices.end(), elB);
				unsigned int indexB;
				if( it != vertices.end() ) {
					indexB = std::distance(vertices.begin(), it);
				} else { return false; }
				return *getEdge(indexA, indexB);
			};
			
			///sets the vertices A and B as connected/disconnected
			void setConnection(const GraphElement &elA, const GraphElement &elB, const bool asConnected) {
				if( edges == nullptr ) return;
				
				auto it = std::find(vertices.begin(), vertices.end(), elA);
				unsigned int indexA;
				if( it != vertices.end() ) {
					indexA = std::distance(vertices.begin(),it);
				} else { return; }
				
				it = std::find(vertices.begin(), vertices.end(), elB);
				unsigned int indexB;
				if( it != vertices.end() ) {
					indexB = std::distance(vertices.begin(), it);
				} else { return; }
				
				if( indexA == indexB ) return;//vertex is always connected with itself
				
				*getEdge(indexA, indexB) = asConnected;
				*getEdge(indexB, indexA) = asConnected;
			};
			
			void setConnection(const unsigned int indexA, const unsigned int indexB, const bool asConnected) {
				if( edges == nullptr ) return;
				if( indexA >= vertCount ) return;
				if( indexB >= vertCount ) return;
				
				*getEdge(indexA, indexB) = asConnected;
				*getEdge(indexB, indexA) = asConnected;
			}
			
			GraphElement& operator[](unsigned int index) {
				return vertices[index];
			};
			
			typename std::vector<GraphElement>::iterator begin() noexcept {
				return vertices.begin();
			}
			
			typename std::vector<GraphElement>::iterator end() noexcept {
				return vertices.end();
			}
			//*
			typename std::vector<GraphElement>::const_iterator cbegin() const noexcept {
				return vertices.cbegin();
			}
			
			typename std::vector<GraphElement>::const_iterator cend() const noexcept {
				return vertices.cend();
			}
			//*/
		private:
			unsigned int vertCount;
			std::vector<GraphElement> vertices;
			bool *edges;
			size_t edgesSize;
			
			bool * getEdge(unsigned int a, unsigned int b) const {
				return &edges[ a * vertCount + b ];
			};
		
	};//end class Graph
	
}//end namespace game_math

#endif /* MATH_GRAPH_HPP */