#pragma once

#ifndef BRS_GAME_PANEL_HPP
#define BRS_GAME_PANEL_HPP

#include "multimedia/window.hpp"
#include "buttons.hpp"
#include "ui.hpp"
#include "mls.hpp"
#include "person_graph.hpp"
#include <string>
#include <vector>
#include <stack>
#include <deque>

namespace game_logic {
	
	const int minimalRightPaneSize = 200/*px*/;
	const int maximalRightPaneSize = 300/*px*/;
	
	class Game;
	
	class RightPane {
		public:
			RightPane(multimedia::Window *theWindow, Game* theGame);
			~RightPane();
			
			void paint(multimedia::Surface &destSurf);
			void mouseOver(int newMouseX, int newMouseY);
			void mouseClick();
			
			void setInfoText(std::string info);
			void addSwitchToUndo(unsigned int indexA, unsigned int indexB);
		private:
			multimedia::Window *mainWindow;
			multimedia::Surface infoTextSurface;
			Rect infoTextRect;
			Game* top;
		
			friend class Game;
			void redrawAssets(int newWidth);
			multimedia::Surface overlay;
			int overlayLeftPos;
			std::string add2Text;
			mls::Template undoText;
			std::string endGameText;
			
			unsigned int undoLeft;
			std::deque<std::pair<unsigned int,unsigned int>> undoList;
			//bool canAdd;
			
			ButtonTemplate buttonTempl;
			UI::Space * clickSpace;
			Button add2Button;
			Button undoButton;
			Button endGameButton;
			std::function<void()> endGameEvent;
	};//end class RightPane
	
	
	class GraphPane;
	
	///Represents one body in the graphical form
	class BodyRepr {
		public:
			BodyRepr(GraphPane *graphPane, int _x, int _y, double _angle, 
				main_game::Body *_body);
			
			void paint(multimedia::Surface &destSurf);
			
			void makeUI(UI::Space *clickSpace, int shiftX, int shiftY, 
			std::function<void(main_game::Body*)> extraMouseMove,
			std::function<bool(main_game::Body*)> shouldBeSelected);
			bool selected;
			double animPhase;
		private:
			GraphPane *graph;
			int x;
			int y;
			double angle;
			main_game::Body *body;
			
			//UI::Component *comp;
			bool mouseOver;
	};//end BodyRepr class
	
	///Represents connection between bodies in the graphical form
	class ConnRepr {
		public:
			ConnRepr(main_game::PersonGraph *theGraph, 
			int startX, int startY, int endX, int endY,
			unsigned int from, unsigned int to);
			
			void paint(multimedia::Surface &destSurf);
			bool inAnimation;
			bool isAConnectionBetween(int from, int to);
		private:
			main_game::PersonGraph *graph;
			int sX;
			int sY;
			int eX;
			int eY;
			int fromIndex;
			int toIndex;
	};//end ConnRepr class
	
	class GraphPane {
		public:
			GraphPane(Game *topPane);
			
			void paint(multimedia::Surface &destSurf);
			void mouseOver(int newMouseX, int newMouseY);
			void mouseClick();
		private:
			friend class Game;
			friend class BodyRepr;
			friend class RightPane;
			Game *top;
			std::vector<BodyRepr> bodies;
			std::vector<ConnRepr> connections;
			main_game::Body *selectedBody;
			void mouseOverBody(main_game::Body *bodyOver);
			
			void redrawAssets(Rect newPosition);
			void reallocateElements();
			void redrawElements();
			Rect panelPosition;
			multimedia::Surface panelSurf;
			UI::Space *clickSpace;
			
			multimedia::Font bigFont;
			multimedia::Font smallFont;
			multimedia::SoundSample switchSound;
			multimedia::SoundSample undoSound;
			multimedia::SoundSample quickSound;
			multimedia::SoundSample wahSound;
			
			void makeSwitchAnimation(unsigned int indexA, unsigned int indexB);
			void makeUndoAnimation(unsigned int indexA, unsigned int indexB);
			void makeInitialSwitchAnimation(std::stack<std::pair<unsigned int,unsigned int>> switches);
	};//end class GraphPane
	
	class Game {
		public:
			Game(multimedia::Window *theWindow);
			
			///draws the game panel to surface
			void paint(multimedia::Surface &destSurf);
			///called when mouse over the panel
			void mouseOver(int newMouseX, int newMouseY);
			///called when mouse clicks the panel
			void mouseClick();
			
			///sets the end game event
			void setEndGameEvent(std::function<void()> callback);
			
			///
			void initializeGame(multimedia::Surface &destSurf);
			///
			void checkWinLoseCondition();
			
			///graph representing the game field
			main_game::PersonGraph theGraph;
		private:
			multimedia::Window *mainWindow;
			char ownRedrawFlag;
			bool inputLock;
			
			multimedia::Surface exlSurf;
			Rect exlPos;
			double exlAnim;
			
			multimedia::Surface winLabel;
			multimedia::Surface loseLabel;
			Rect labelRect;
			enum {
				GAME_WIN,
				GAME_LOSE,
				GAME_GO
			} gameState;
			
			friend class RightPane;
			friend class GraphPane;
			void redrawAssets();
			RightPane controlPane;
			GraphPane graphPane;
	};//end class Game
	
}//end namespace game_logic

#endif /* BRS_GAME_PANEL_HPP */

