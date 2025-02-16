/**
 * The module with the game "meat"
*/

//for M_PI constant
#define _USE_MATH_DEFINES
#include <cmath>

#include "game_panel.hpp"
#include "buttons.hpp"
#include <string>
#include "mls.hpp"
#include "common.hpp"
#include "person_graph.hpp"
#include "game_math/vector.hpp"

#include "animation.hpp"
#include <vector>
#include <stack>
#include <deque>


namespace game_logic {
	
	namespace game_panel {
		
		///graph representing the game field
		main_game::PersonGraph *theGraph; 
		
		namespace control_panel {
			
			multimedia::Surface overlay, infoTextSurface;
			int overlayLeftPos;
			std::string add2Text;
			mls::Template undoText;
			std::string endGameText;
			Rect infoTextRect{0,0,0,0};
			
			unsigned int undoLeft = 0;
			std::deque<std::pair<unsigned int,unsigned int>> undoList;
			void updateUndo();
			void addSwitchToUndo(unsigned int indexA, unsigned int indexB);
			
			void setInfoText(std::string info);
			
			ButtonTemplate *buttonTempl;
			UI::Space * clickSpace = nullptr;
			Button *add2Button = nullptr;
			Button *undoButton = nullptr;
			Button *endGameButton = nullptr;
			
			//setting size and position of the control panel
			void redrawAssets(const int windowWidth, const int windowHeight, const int panelWidth);
			
			//drawing
			void paint(multimedia::Surface &destSurf);
			
			//initialization/quiting
			void init();
			void unloadPanel();
			
		}//end namespace control_pane
		
		namespace graph_panel {
			
			UI::Space *clickSpace = nullptr;
			Rect panelPosition{0,0,0,0};
			multimedia::Surface panelSurf;
			
			multimedia::Font *bigFont;
			multimedia::Font *smallFont;
			multimedia::SoundSample *switchSound;
			multimedia::SoundSample *undoSound;
			multimedia::SoundSample *quickSound;
			
			///Represents one body in the graphical form
			class BodyRepr {
				public:
					BodyRepr(unsigned int _index, main_game::Body *_body);
					
					unsigned int getIndex() const;
					
					///prepares for drawing on graph surf
					void setUI(int _x, int _y, double _angle);
					
					///paints on graph surface
					void paint();
					
					bool selected;
					double animPhase;
				private:
					int x, y;
					double angle;
					main_game::Body *body;
					unsigned int index;
					bool mouseOver;
			};//end class BodyRepr
			
			///Represents connection between bodies in the graphical form
			class ConnRepr {
				public:
					ConnRepr(unsigned int from, unsigned int to);
					
					///prepares for drawing on graph surf
					void setUI(int startX, int startY, int endX, int endY);
					
					unsigned int getFrom() const;
					unsigned int getTo() const;
					
					///paints on graph surface
					void paint();
					bool inAnimation;
					bool isAConnectionBetween(int from, int to);
				private:
					int sX;
					int sY;
					int eX;
					int eY;
					unsigned int fromIndex;
					unsigned int toIndex;
			};//end ConnRepr class
			
			std::vector<BodyRepr> bodies;
			std::vector<ConnRepr> connections;
			
			main_game::Body *selectedBody = nullptr;
			void mouseOverBody(main_game::Body *bodyOver);
			bool shouldBeSelected(main_game::Body *bodyClicked);
			
			//animations
			void makeSwitchAnimation(unsigned int indexA, unsigned int indexB);
			void makeUndoAnimation(unsigned int indexA, unsigned int indexB);
			
			//setting size and position of the graph panel
			void redrawAssets(const Rect panel_rect);
			//reallocate body references and connection references
			void reallocateElements();
			//redraws elements on the panel surface
			void redrawElements();
			//assigns body from the graph into body refernce and connection reference
			void reassignReferences();
			
			//drawing
			void paint(multimedia::Surface &destSurf);
			
			//unload/init
			void init();
			void unloadPanel();
		}//end namespace graph_panel
		
		//setting size and position of the panel elements
		const int minimalRightPaneSize = 200/*px*/;
		const int maximalRightPaneSize = 300/*px*/;
		void redrawAssets(const int width, const int height);
		//::exclamation mark
		multimedia::Surface exclSurf;
		Rect exclPos{0,0,0,0};
		double exclAnim = -1.0;
		//::win/lose surfaces
		Rect labelRect{0,0,0,0};
		multimedia::Surface winLabel, loseLabel;
		//sound
		multimedia::SoundSample *wahSound;
		//game panel state
		enum {
			GAME_WIN,
			GAME_LOSE,
			GAME_GO
		} gamePanelState;
		bool inputLock = false;
		void checkWinLoseCondition() {
			if( theGraph->isFinished() ) {
				gamePanelState = GAME_WIN;
				inputLock = true;
				return;
			}
			
			if( control_panel::add2Button->enabled == false && 
				control_panel::undoLeft == 0 && 
				theGraph->isUnwinable() ) {
				gamePanelState = GAME_LOSE;
				inputLock = true;
				return;
			}
		}
		
		//animation
		void makeInitialSwitchAnimation(std::stack<std::pair<unsigned int,unsigned int>> switches);
		
		//drawing
		multimedia::Window *theWindow = nullptr;
		multimedia::DirtyFlag *repaintFlag = nullptr;
		bool firstRedraw = true;
		void paint(multimedia::Surface &destSurf) {
			if( repaintFlag->needsRedraw() || firstRedraw ) {
				auto windowSize = destSurf.getSize();
				redrawAssets(theWindow->getWidth(), theWindow->getHeight());
				firstRedraw = false;
			}
			control_panel::paint(destSurf);
			graph_panel::paint(destSurf);
			//if animated
			if( exclAnim >= 0.0 ) {
				multimedia::Surface exclSurfCopy = exclSurf;
				
				double surfSize = exclAnim / 2 + 0.5;
				exclSurfCopy.scale(surfSize);
				
				Rect exclPosCopy = exclSurfCopy.getSize();
				exclPosCopy.setCenterX( exclPos.getCenterX() );
				exclPosCopy.setCenterY( exclPos.getCenterY() );
				
				destSurf.blit(exclPosCopy, exclSurfCopy);
			}
			//if game won/lost
			if( gamePanelState == GAME_WIN )
				destSurf.blit(labelRect, winLabel);
			else if( gamePanelState == GAME_LOSE )
				destSurf.blit(labelRect, loseLabel);
		}
		
		
		void mouseAction(int mouseX, int mouseY, bool mouseClick) {
			//return to the main menu on mouse click if the game was won/lost
			if( gamePanelState != GAME_GO && mouseClick ) {
				theGameState = GameState::PostGame;
			}
			
			if( inputLock ) return;
			
			if( mouseX != -1 && mouseY != -1 ) {
				if( control_panel::clickSpace != nullptr ) 
					control_panel::clickSpace->mouseMovedTo(mouseX, mouseY);
				if( graph_panel::clickSpace != nullptr )
					graph_panel::clickSpace->mouseMovedTo(mouseX, mouseY);
			}
			if( mouseClick ) {
				if( control_panel::clickSpace != nullptr ) 
					control_panel::clickSpace->mouseClick();
				if( graph_panel::clickSpace != nullptr )
					graph_panel::clickSpace->mouseClick();
			}
		}
		
		
		void setGameDifficultly(GameDifficultly diff) {
			switch( diff ) {
				case GameDifficultly::EASY:
					theGraph->generateEasyGame();
					break;
				case GameDifficultly::NORMAL:
					theGraph->generateNormalGame();
					break;
				case GameDifficultly::HARD:
					theGraph->generateHardGame();
					break;
			}
			
			control_panel::undoLeft = theGraph->getSize() / 3;
			if( control_panel::undoLeft < 3 ) control_panel::undoLeft = 3;
			control_panel::undoList.clear();
			control_panel::updateUndo();
		}
		
		void initializeGame() {
			graph_panel::reassignReferences();
			graph_panel::selectedBody = nullptr;
			gamePanelState = GAME_GO;
			firstRedraw = true;
			makeInitialSwitchAnimation(theGraph->generateInitialSwitch());
		}
		
		void unloadModule() {
			using namespace multimedia;
			//unload surfaces
			exclSurf = Surface();
			winLabel = Surface();
			loseLabel = Surface();
			//delete pointers
			delete wahSound;
			delete theGraph;
			//unload panels
			control_panel::unloadPanel();
			graph_panel::unloadPanel();
		}
		
	}//end namespace game_panel
	
	Module GamePanelInit(multimedia::Window *mainWindow) {
		using namespace game_panel;
		
		theWindow = mainWindow;
		repaintFlag = theWindow->createDirtyFlag();
		wahSound = new multimedia::SoundSample("wah.ogg");
		theGraph = new main_game::PersonGraph();
		gamePanelState = GAME_GO;
		
		control_panel::init();
		graph_panel::init();
		
		return Module(unloadModule);
	}
	
	
	//================================= definitions 
	
	void game_panel::makeInitialSwitchAnimation(std::stack<std::pair<unsigned int,unsigned int>> switches) {
		using namespace game_panel;
		constexpr double animDelay = 0.5;
		auto timePoint = animation::addSeconds(0.01, animation::getNow());
		inputLock = true;
		
		//play wah
		animation::newPointAnimation(timePoint, [](){
			wahSound->play();
			exclAnim = 1.0;
		});
		timePoint = animation::addSeconds(0.01, timePoint);
		//animate the exclamation mark
		animation::newShiftedSpanAnimation(timePoint, 1.0, [](double phase){
			phase = (static_cast<int>(phase * 100.0) % 25) / 25.0;
			exclAnim = 1.0 - phase;
		});
		timePoint = animation::addSeconds(1.1, timePoint);
		animation::newPointAnimation(timePoint, [](){
			exclAnim = -1.0;
		});
		timePoint = animation::addSeconds(0.01, timePoint);
		
		//animate switching
		while( !switches.empty() ) {
			auto oneSwitch = switches.top(); switches.pop();
			unsigned int indexA = oneSwitch.first;
			unsigned int indexB = oneSwitch.second;
			auto reprA = &graph_panel::bodies[indexA];
			auto reprB = &graph_panel::bodies[indexB];
			graph_panel::ConnRepr* conn = nullptr;
			for(auto& connF : graph_panel::connections) {
				if( connF.isAConnectionBetween(indexA,indexB)) {
					conn = &connF;
					break;
				}
			}
			//initial setup
			animation::newPointAnimation(timePoint,[conn](){
				graph_panel::quickSound->play();
				conn->inAnimation = true;
			});
			timePoint = animation::addSeconds(0.01, timePoint);
			//before the switch
			animation::newShiftedSpanAnimation(timePoint, animDelay, [reprA,reprB](double phase){
				reprA->animPhase = phase;
				reprB->animPhase = phase;
				graph_panel::redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			//the switch
			animation::newPointAnimation(timePoint, [indexA,indexB](){
				theGraph->switchBodiesBetween(indexA, indexB);
				graph_panel::redrawElements();
			});
			timePoint = animation::addSeconds(0.01, timePoint);
			//after the switch
			animation::newShiftedSpanAnimation(timePoint, animDelay, [reprA,reprB](double phase){
				reprA->animPhase = 1.0 - phase;
				reprB->animPhase = 1.0 - phase;
				graph_panel::redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			//post-setup
			animation::newPointAnimation(timePoint, [reprA,reprB,conn](){
				reprA->animPhase = -1.0;
				reprB->animPhase = -1.0;
				conn->inAnimation = false;
				graph_panel::redrawElements();
			});
			timePoint = animation::addSeconds(0.01, timePoint);
		}
		//the end
		animation::newPointAnimation(timePoint,[](){
			inputLock = false;
		});
	}
	
	namespace game_panel {
		
		void redrawAssets(const int width, const int height) {
			//::divide space between control panel and switch panel
			//assert: width>height
			int cp_width = width - height;
			if( cp_width < minimalRightPaneSize)
				cp_width = minimalRightPaneSize;
			if( cp_width > maximalRightPaneSize)
				cp_width = maximalRightPaneSize;
			
			control_panel::redrawAssets(width, height, cp_width);
			
			//calculate the size and position for the graph panel
			int gp_width = width - cp_width;
			int gp_height = height;
			int gp_size;
			if( gp_width > gp_height ) {
				gp_size = gp_height;
			} else {
				gp_size = gp_width;
			}
			Rect gp_rect{0,0,gp_size,gp_size};
			gp_rect.setCenterX( gp_width / 2 );
			gp_rect.setCenterY( gp_height / 2 );
			
			graph_panel::redrawAssets(gp_rect);
			
			//reload the exclamation sign
			exclSurf = multimedia::Surface::fromFile("exclamation.png");
			exclPos = exclSurf.getSize();
			exclPos.setHeightWithConstRatio( height* 0.8 );
			exclSurf.scale(exclPos);
			exclPos.setCenterX( width / 2 );
			exclPos.setCenterY( height / 2 );
			
			//reload win/lose labels
			auto labelSurf = multimedia::Surface::fromFile("WinLoseBackground.png");
			labelRect = labelSurf.getSize();
			labelRect.setWidthWithConstRatio( width * 0.6 );
			labelSurf.scale(labelRect);
			
			std::string winText = _("You Won!").get();
			std::string loseText = _("You Lose!").get();
			std::string theLongest = multimedia::getTheLongestString({winText,loseText});
			
			auto labelFont = multimedia::Font::selectFontToFit(
				"SpecialElite-Regular.ttf",
				labelRect.inflate(-20),
				theLongest
			);
			//::win label
			auto textSurf = labelFont.render(winText, multimedia::Color(0,255,0));
			Rect textRect = textSurf.getSize();
			textRect.setCenterX( labelRect.getCenterX() ); textRect.setCenterY( labelRect.getCenterY() );
			winLabel = labelSurf;
			winLabel.blit(textRect, textSurf);
			//::lose label
			textSurf = labelFont.render(loseText, multimedia::Color(255,0,0));
			textRect = textSurf.getSize();
			textRect.setCenterX( labelRect.getCenterX() ); textRect.setCenterY( labelRect.getCenterY() );
			loseLabel = labelSurf;
			loseLabel.blit(textRect, textSurf);
			//::position the labels
			labelRect.setCenterX( width / 2 );
			labelRect.setCenterY( height / 2 );
		}
		
	}//end namespace game_panel
	
	namespace game_panel::control_panel {
		
		
		
		void redrawAssets(const int windowWidth, const int windowHeight, const int panelWidth) {
			const int leftBorder = windowWidth - panelWidth;
			constexpr int buttonSpacing = 10;//button spacing in pixels
			
			//load overlay
			overlay = multimedia::Surface::fromFile("right_panel_overlay.png");
			Rect ov = overlay.getSize();
			ov.setHeightWithConstRatio( windowHeight );
			overlay.scale(ov);
			overlayLeftPos = leftBorder - 10;
			
			std::string theLongestText = multimedia::getTheLongestMultilineString({
				undoText.apply("count",undoLeft).get(), add2Text, endGameText
			});
			
			const int bHeight = buttonTempl->reloadAssetsForParametersW(panelWidth * 0.9, theLongestText);
			//make buttons
			if( clickSpace != nullptr ) delete clickSpace;
			clickSpace = new UI::Space(leftBorder,0,panelWidth,windowHeight);
			
			add2Button = buttonTempl->makeButton(add2Text, clickSpace);
			endGameButton = buttonTempl->makeButton(endGameText, clickSpace);
			undoButton = buttonTempl->makeButton(undoText.apply("count",undoLeft).get(), clickSpace);
			
			add2Button->assignEvents();
			endGameButton->assignEvents();
			undoButton->assignEvents();
			
			undoButton->onClickHandler([](){
				if( undoList.size() == 0 ) return;
				
				auto& [indexA, indexB] = undoList.back();
				undoList.pop_back();
				
				graph_panel::makeUndoAnimation( indexA , indexB );
				--undoLeft;
				
				updateUndo();
			});
			add2Button->onClickHandler([](){
				theGraph->add2ExtraPeople();
				graph_panel::reassignReferences();
				graph_panel::reallocateElements();
				add2Button->enabled = false;
			});
			endGameButton->onClickHandler([](){
				theGameState = GameState::PostGame;
			});
			
			//position buttons and text
			int posX = leftBorder + (panelWidth * 0.05);
			int posY = buttonSpacing;
			infoTextRect.setLeft( posX );
			//::add2
			add2Button->setPosition(posX, posY, false);
			//::undo
			posY += bHeight + buttonSpacing;
			undoButton->setPosition(posX, posY, false);
			posY += bHeight + buttonSpacing;
			//::top border of text
			infoTextRect.setTop( posY );
			//::end game
			posY = windowHeight - bHeight - buttonSpacing;
			endGameButton->setPosition(posX, posY, false);
			//::height and width of text
			posY -= buttonSpacing;
			infoTextRect.setHeight( posY - infoTextRect.getTop() );
			infoTextRect.setWidth( panelWidth * 0.9 );
		}
		
		void setInfoText(std::string info) {
			if( info.length() == 0 ) { //no text to display?
				infoTextSurface = multimedia::Surface(nullptr);//set it to empty surface
				return;
			}
			infoTextSurface = buttonTempl->drawingFont.renderMultiLine(
				info,
				infoTextRect.getWidth(),
				multimedia::TextAlign::LEFT,
				multimedia::Color(200, 200, 200)
			);
		}
		
		void updateUndo() {
			if( undoButton == nullptr ) return;
			undoButton->setText(undoText.apply("count",undoLeft).get());
			if( undoLeft == 0 )
				undoButton->enabled = false;
			else
				undoButton->enabled = true;
		}
		
		void addSwitchToUndo(unsigned int indexA, unsigned int indexB) {
			if( undoLeft == 0 ) return;
			
			undoList.push_back( {indexA, indexB} );
			if( undoList.size() > undoLeft ) {
				undoList.pop_front();
			}
		}
		
		void init() {
			undoText = _("Undo\n(%{count}% left)");
			add2Text = _("Call for\nreinforcements").get();
			endGameText = _("End game").get();
			buttonTempl = new ButtonTemplate("GM_active.png", "GM_inactive.png",
									GM_ButtonWidthPercent, GM_ButtonHeightPercent);
		}
		
		void unloadPanel() {
			using namespace multimedia;
			//unload surfaces
			overlay = Surface();
			infoTextSurface = Surface();
			//delete pointers
			delete clickSpace;
			delete buttonTempl;
		}
		
		void paint(multimedia::Surface &destSurf) {
			undoButton->paint(destSurf);
			add2Button->paint(destSurf);
			endGameButton->paint(destSurf);
			//text surface
			destSurf.blit(infoTextRect, infoTextSurface);
			//overlay
			destSurf.blit(overlayLeftPos, 0, overlay);
		}
		
	}//end namespace game_panel::control_panel
	
	namespace game_panel::graph_panel {
		
		constexpr int BodyReprCircleSize = 60;
		
		BodyRepr::BodyRepr(unsigned int _index, main_game::Body *_body):
		body(_body), index(_index), x(-1), y(-1), angle(-1.0), 
		mouseOver(false), selected(false), animPhase(-1.0) {}
		
		unsigned int BodyRepr::getIndex() const {
			return index;
		}
		
		void BodyRepr::setUI(int _x, int _y, double _angle) {
			x = _x; y = _y; angle = _angle;
			//panelPosition
			Rect clickable{	panelPosition.getLeft() +x - BodyReprCircleSize, 
						panelPosition.getTop() +y - BodyReprCircleSize, 
						BodyReprCircleSize*2, 
						BodyReprCircleSize*2};
			auto comp = clickSpace->newComponent(clickable);
			comp->addMouseEnterListener([this](){
				mouseOver = true;
				graph_panel::redrawElements();
				mouseOverBody(body);
			});
			comp->addMouseLeaveListener([this](){
				mouseOver = false;
				graph_panel::redrawElements();
				mouseOverBody(nullptr);
			});
			comp->addClickListener([this](){
				selected = shouldBeSelected(body);
			});
		}
		
		bool shouldBeSelected(main_game::Body *bodyClicked) {
			if( selectedBody == bodyClicked ) {//unselect if clicked on the same selected body
					selectedBody = nullptr;
					return false;
				}
				//selecting new body
				if( selectedBody == nullptr ) {//not yet selected?
					selectedBody = bodyClicked;//select and...
					return true;//mark body as selected
				} else {//already selected a different body?
					//initiate body switch animation
					unsigned int indexA = theGraph->getIndexOfBody(selectedBody);
					unsigned int indexB = theGraph->getIndexOfBody(bodyClicked);
					if( theGraph->canBeSwitched(indexA, indexB) ) {
						makeSwitchAnimation(indexA, indexB);
						return true;
					} else {
						return false;
					}
				}
		}
		
		void mouseOverBody(main_game::Body *bodyOver) {
			std::string infoText;
			
			if( bodyOver == nullptr ) {
				control_panel::setInfoText(infoText);
				return;
			}
			
			if( selectedBody == nullptr ) {
				infoText = _(
					"%{#When selected one body#}%"
					"Switch %{firstKid}% (in %{firstBody}%'s body) with..."
				).apply("firstKid", bodyOver->currentOwner->name)
				.apply("firstBody", bodyOver->rightOwner->name)
				.get();
			} else {
				if( bodyOver == selectedBody ) 
					infoText = _("Unselect").get();
				else {
					unsigned int indexA = theGraph->getIndexOfBody(bodyOver);
					unsigned int indexB = theGraph->getIndexOfBody(selectedBody);
					if( theGraph->canBeSwitched(indexA, indexB) )
						infoText = _(
							"%{#When selected two bodies#}%"
							"Switch %{firstKid}% (in %{firstBody}%'s body) with "
							"%{secondKid}% (in %{secondBody}%'s body)"
						).apply("firstKid", selectedBody->currentOwner->name)
						.apply("firstBody", selectedBody->rightOwner->name)
						.apply("secondKid", bodyOver->currentOwner->name)
						.apply("secondBody", bodyOver->rightOwner->name)
						.get();
					else
						infoText = _("Switching imposible!").get();
				}
			}
			
			control_panel::setInfoText(infoText);
		}
		
		void makeSwitchAnimation(unsigned int indexA, unsigned int indexB) {
			constexpr double animDelay = 1.5;
			auto timePoint = animation::addSeconds(0.1, animation::getNow());
			inputLock = true;
			auto reprA = &bodies[indexA];
			auto reprB = &bodies[indexB];
			
			auto it = connections.begin();
			while( it != connections.end() ) {
				if( it->isAConnectionBetween(indexA, indexB) ) {
					std::rotate(it, it+1, connections.end());
					break;
				}
				++it;
			}
			
			auto conn = &connections.back();
			
			//play the sound
			switchSound->play();
			
			conn->inAnimation = true;
			animation::newShiftedSpanAnimation(timePoint, animDelay, [reprA,reprB](double phase){
				reprA->animPhase = phase;
				reprB->animPhase = phase;
				redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			
			animation::newPointAnimation(timePoint, [indexA,indexB](){
				theGraph->switchBodiesBetween(indexA, indexB);
				control_panel::addSwitchToUndo(indexA, indexB);
				redrawElements();
			});
			timePoint = animation::addSeconds(0.1, timePoint);
			
			animation::newShiftedSpanAnimation(timePoint, animDelay, [reprA,reprB](double phase){
				reprA->animPhase = 1.0 - phase;
				reprB->animPhase = 1.0 - phase;
				redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			
			animation::newPointAnimation(timePoint, [reprA,reprB,conn](){
				reprA->animPhase = -1.0;
				reprB->animPhase = -1.0;
				conn->inAnimation = false;
				selectedBody = nullptr;
				for(auto& body : bodies) {//unselect all body representations
					body.selected = false;
				}
				redrawElements();
				inputLock = false;
				checkWinLoseCondition();
			});
		}
		
		void makeUndoAnimation(unsigned int indexA, unsigned int indexB) {
			constexpr double animDelay = 1.5;
			auto timePoint = animation::addSeconds(0.1, animation::getNow());
			inputLock = true;
			auto reprA = &bodies[indexA];
			auto reprB = &bodies[indexB];
			
			auto it = connections.begin();
			while( it != connections.end() ) {
				if( it->isAConnectionBetween(indexA, indexB)) {
					std::rotate(it, it+1, connections.end());
					break;
				}
				++it;
			}
			
			auto conn = &connections.back();
			
			undoSound->play();
			
			conn->inAnimation = true;
			animation::newShiftedSpanAnimation(timePoint, animDelay, [reprA,reprB](double phase){
				reprA->animPhase = phase;
				reprB->animPhase = phase;
				redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			
			animation::newPointAnimation(timePoint, [indexA,indexB](){
				theGraph->undoSwitchBodiesBetween(indexA, indexB);
				redrawElements();
			});
			timePoint = animation::addSeconds(0.1, timePoint);
			
			animation::newShiftedSpanAnimation(timePoint, animDelay, [reprA,reprB](double phase){
				reprA->animPhase = 1.0 - phase;
				reprB->animPhase = 1.0 - phase;
				redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			
			animation::newPointAnimation(timePoint, [reprA,reprB,conn](){
				reprA->animPhase = -1.0;
				reprB->animPhase = -1.0;
				conn->inAnimation = false;
				selectedBody = nullptr;
				for(auto& body : bodies) {//unselect all body representations
					body.selected = false;
				}
				redrawElements();
				inputLock = false;
			});
		}
		
		void BodyRepr::paint() {
			using multimedia::Color;
			
			if( x == -1 || y == -1 || angle == -1.0 ) return;
			
			auto drawer = panelSurf.getDrawer();
			const int outerRadius = BodyReprCircleSize;
			const int innerRadius = BodyReprCircleSize - 5 - (mouseOver ? 5 : 0);
			const int outerRadiusSmall = BodyReprCircleSize * 0.6;
			const int innerRadiusSmall = outerRadiusSmall - 5;
			
			double saturation;
			double value;
			if( animPhase >= 0.0 ) {//in animation?
				value = 100.0;
				saturation = 100.0 * (1.0 - animPhase);
			} else {//not animation
				if( selected ) {
					saturation = 20.0;
					value = 100.0;
				} else {
					saturation = 100.0;
					value = (mouseOver ? 100.0 : 60.0);
				}
			}
			//set used colors
			const auto black = Color(0,0,0);
			const auto green = Color(0,255,0);
			const auto ownerColor = Color::fromHSV(
				body->currentOwner->color, saturation, value);
			const auto bodyColor = Color::fromHSV(
				body->rightOwner->color, saturation, value);
			//draw the big circle
			drawer.setColor(ownerColor);
			drawer.drawCircle(x,y, outerRadius, true);
			drawer.setColor(black);
			drawer.drawCircle(x,y, innerRadius, true);
			//draw the owner's initials
			auto ownerSurf = bigFont->render(body->currentOwner->short_name, green);
			auto ownerRect = ownerSurf.getSize();
			ownerRect.setCenterX(x); ownerRect.setCenterY(y);
			panelSurf.blit(ownerRect, ownerSurf);
			//calculate the small circle shift
			const int shiftX = x + std::cos(angle + M_PI) * outerRadius;
			const int shiftY = y + std::sin(angle + M_PI) * outerRadius;
			//draw the small circle
			drawer.setColor(bodyColor);
			drawer.drawCircle(shiftX, shiftY, outerRadiusSmall, true);
			drawer.setColor(black);
			drawer.drawCircle(shiftX, shiftY, innerRadiusSmall, true);
			//draw the right owner's initials
			ownerSurf = smallFont->render(body->rightOwner->short_name, green);
			ownerRect = ownerSurf.getSize();
			ownerRect.setCenterX(shiftX); ownerRect.setCenterY(shiftY);
			panelSurf.blit(ownerRect, ownerSurf);
		}
		
		ConnRepr::ConnRepr(unsigned int from, unsigned int to):
			fromIndex(from), toIndex(to), inAnimation(false),
			sX(-1), sY(-1), eX(-1), eY(-1) {}
		
		unsigned int ConnRepr::getFrom() const {
			return fromIndex;
		}
		unsigned int ConnRepr::getTo() const {
			return toIndex;
		}
		
		void ConnRepr::setUI(int startX, int startY, int endX, int endY) {
			sX = startX; sY = startY;
			eX = endX; eY = endY;
		}
		
		bool ConnRepr::isAConnectionBetween(int from, int to) {
			if( fromIndex == from && toIndex == to ) return true;
			if( fromIndex == to && toIndex == from ) return true;
			return false;
		}
		
		void ConnRepr::paint() {
			if( sX == -1 || sY == -1 || eX == -1 || eY == -1 ) return;
			
			auto drawer = panelSurf.getDrawer();
			double colorH;
			if( theGraph->canBeSwitched(fromIndex, toIndex) ) {
				colorH = 240.0;//blue
			} else {
				colorH = 0.0;//red
			}
			auto color = multimedia::Color::fromHSV(colorH, 100.0, 80.0);
			
			drawer.setColor(color);
			drawer.drawLine(sX,sY, eX,eY, 3);
			
			//extra drawing if in animation
			if( inAnimation ) {
				auto stepVector = game_math::Vector(eX - sX, eY - sY) / 20;
				auto paraVector = stepVector.rotate(M_PI / 2).normalize();
				
				drawer.setColor(multimedia::Color::fromHSV(240.0, 30.0, 100.0));
				
				int prevX = sX;
				int prevY = sY;
				decltype(stepVector) next;
				double shiftSize;
				for(unsigned int i=0; i<19; ++i) {
					next = stepVector * i + game_math::Vector(sX, sY);
					//pick a value between -30.0 and 30.0
					shiftSize = std::rand() * 1.0 / RAND_MAX;
					shiftSize *= 60.0;
					shiftSize -= 30.0;
					//add a shift
					next = next + paraVector * shiftSize;
					//draw a line
					drawer.drawLine(prevX, prevY, next.x, next.y);
					prevX = next.x;
					prevY = next.y;
				}
				//draw the last line
				drawer.drawLine(prevX, prevY, eX, eY);
			}
		}
		
		void redrawAssets(const Rect panel_rect) {
			panelSurf = multimedia::Surface::newRGB_Surface(
				panel_rect.getWidth(), panel_rect.getHeight());
			panelSurf.fill(multimedia::Color(0,0,0));//fill with black
			panelPosition = panel_rect;
			
			reallocateElements();
		}
		
		void reallocateElements() {
			if( clickSpace != nullptr ) delete clickSpace;
			Rect &np = panelPosition;
			clickSpace = new UI::Space(np.getLeft(), np.getTop(), np.getWidth(), np.getHeight());
			
			const int center = np.getWidth() / 2;//or getHeight()/2 — the size is the same
			const int size = center - BodyReprCircleSize - 5;
			const unsigned int elemNum = theGraph->getSize();
			const double angleStep = 2 * M_PI / elemNum;
			
			std::pair<int, int> *coords = new std::pair<int, int>[elemNum];
			int index = 0;
			for(auto& bodyR : bodies ) {
				int posX = std::cos(index * angleStep) * size + center;
				int posY = std::sin(index * angleStep) * size + center;
				coords[bodyR.getIndex()] = {posX, posY};
				bodyR.setUI(posX, posY, index * angleStep);
				++index;
			}
			for(auto& connR : connections) {
				auto from = coords[connR.getFrom()];
				auto to = coords[connR.getTo()];
				
				connR.setUI(from.first, from.second, to.first, to.second);
			}
			
			delete[] coords;
			
			redrawElements();
		}
		
		void redrawElements() {
			panelSurf.fill(multimedia::Color(0,0,0));
			for(auto& connR : connections) {
				connR.paint();
			}
			for(auto& bodyR : bodies) {
				bodyR.paint();
			}
		}
		
		void reassignReferences() {
			connections.clear();
			bodies.clear();
			
			for(unsigned int i=0; i<theGraph->getSize(); ++i) {
				auto body = &theGraph->getBodyByIndex(i);
				bodies.emplace_back( i , body );
				
				for(unsigned int j=i+1; j<theGraph->getSize(); ++j) {
					connections.emplace_back( i , j );
				}
			}
		}
		
		void paint(multimedia::Surface &destSurf) {
			destSurf.blit(panelPosition, panelSurf);
		}
		
		void init() {
			using namespace multimedia;
			
			bigFont = new Font("Jura-Regular.ttf",40);
			smallFont = new Font("Jura-Regular.ttf", 30);
			switchSound = new SoundSample("transport.ogg");
			undoSound = new SoundSample("transport_back.ogg");
			quickSound = new SoundSample("transport_quick.ogg");
		}
		
		void unloadPanel() {
			//clean surfaces
			panelSurf = multimedia::Surface();
			//remove pointers
			delete clickSpace;
			delete bigFont;
			delete smallFont;
			delete switchSound;
			delete undoSound;
			delete quickSound;
		}
		
	}//end namespace game_panel::graph_panel
	
}//end namespace game_logic

