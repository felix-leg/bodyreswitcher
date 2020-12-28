/**
 * Module containing the "meat" of the game
*/

#include "game_panel.hpp"
#include "multimedia/font.hpp"
#include "animation.hpp"
#include "game_math/vector.hpp"
//below to use M_PI constant
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace game_logic {
	
	/*
	======================== Game class ========================
	*/
	
	Game::Game(multimedia::Window *theWindow): 
	mainWindow(theWindow),
	controlPane(theWindow,this),
	graphPane(this),
	inputLock(false) {
		ownRedrawFlag = mainWindow->redrawFlag * -1;//to force redraw
		exlAnim = -1.0;
		gameState = GAME_GO;
	}
	
	void Game::paint(multimedia::Surface &destSurf) {
		if( ownRedrawFlag != mainWindow->redrawFlag ) {//needs redraw?
			redrawAssets();
			//assets ready, update the flag
			ownRedrawFlag = mainWindow->redrawFlag;
		}
		graphPane.paint(destSurf);
		controlPane.paint(destSurf);
		//if animated
		if( exlAnim >= 0.0 ) {
			multimedia::Surface exlSurfCopy = exlSurf;
			
			double surfSize = exlAnim / 2 + 0.5;
			exlSurfCopy.scale(surfSize);
			
			Rect exlPosCopy = exlSurfCopy.getSize();
			exlPosCopy.setCenterX( exlPos.getCenterX() );
			exlPosCopy.setCenterY( exlPos.getCenterY() );
			
			destSurf.blit(exlPosCopy.getLeft(), exlPosCopy.getTop(), exlSurfCopy);
		}
		//if game won/lost
		if( gameState == GAME_WIN )
			destSurf.blit(labelRect.getLeft(), labelRect.getTop(), winLabel);
		else if( gameState == GAME_LOSE )
			destSurf.blit(labelRect.getLeft(), labelRect.getTop(), loseLabel);
	}
	
	void Game::mouseOver(int newMouseX, int newMouseY) {
		if( newMouseX == -1 && newMouseY == -1 ) return;
		if( inputLock ) return;
		controlPane.mouseOver(newMouseX, newMouseY);
		graphPane.mouseOver(newMouseX, newMouseY);
	}
	
	void Game::mouseClick() {
		//close the game after won/lost
		if( gameState != GAME_GO ) {
			if( controlPane.endGameEvent != nullptr )
				controlPane.endGameEvent();
		}
		
		if( inputLock ) return;
		controlPane.mouseClick();
		graphPane.mouseClick();
	}
	
	void Game::redrawAssets() {
		//::divide space between control panel and switch panel
		//assert: width>height
		int cp_width = mainWindow->getWidth() - mainWindow->getHeight();
		if( cp_width < minimalRightPaneSize)
			cp_width = minimalRightPaneSize;
		if( cp_width > maximalRightPaneSize)
			cp_width = maximalRightPaneSize;
		
		controlPane.redrawAssets( cp_width );
		//calculate the size and position for the graph panel
		int gp_width = mainWindow->getWidth() - cp_width;
		int gp_height = mainWindow->getHeight();
		int gp_size;
		if( gp_width > gp_height ) {
			gp_size = gp_height;
		} else {
			gp_size = gp_width;
		}
		Rect gp_rect{0,0,gp_size,gp_size};
		gp_rect.setCenterX( gp_width / 2 );
		gp_rect.setCenterY( gp_height / 2 );
		
		graphPane.redrawAssets(gp_rect);
		
		//reload the exclamation sign
		exlSurf = multimedia::Surface::fromFile("exclamation.png");
		exlPos = exlSurf.getSize();
		exlPos.setHeightWithConstRatio( mainWindow->getHeight() * 0.8 );
		exlSurf.scale(exlPos);
		exlPos.setCenterX( mainWindow->getWidth() / 2 );
		exlPos.setCenterY( mainWindow->getHeight() / 2 );
		
		//reload win/lose labels
		auto labelSurf = multimedia::Surface::fromFile("WinLoseBackground.png");
		labelRect = labelSurf.getSize();
		labelRect.setWidthWithConstRatio( mainWindow->getWidth() * 0.6 );
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
		winLabel.blit(textRect.getLeft(), textRect.getTop(), textSurf);
		//::lose label
		textSurf = labelFont.render(loseText, multimedia::Color(255,0,0));
		textRect = textSurf.getSize();
		textRect.setCenterX( labelRect.getCenterX() ); textRect.setCenterY( labelRect.getCenterY() );
		loseLabel = labelSurf;
		loseLabel.blit(textRect.getLeft(), textRect.getTop(), textSurf);
		//::position the labels
		labelRect.setCenterX( mainWindow->getWidth() / 2 );
		labelRect.setCenterY( mainWindow->getHeight() / 2 );
	}
	
	void Game::setEndGameEvent(std::function<void()> callback) {
		controlPane.endGameEvent = callback;
	}
	
	void Game::initializeGame(multimedia::Surface &destSurf) {
		controlPane.undoLeft = theGraph.getSize() / 3;
		if( controlPane.undoLeft < 3 ) controlPane.undoLeft = 3;
		controlPane.undoList.clear();
		controlPane.canAdd = true;
		gameState = GAME_GO;
		
		ownRedrawFlag = mainWindow->redrawFlag - 3;
		paint(destSurf);
		
		auto initSwitches = theGraph.generateInitialSwitch();
		graphPane.makeInitialSwitchAnimation(initSwitches);
	}
	
	void Game::checkWinLoseCondition() {
		if( theGraph.isFinished() ) {
			gameState = GAME_WIN;
			return;
		}
		
		if( controlPane.canAdd == false && controlPane.undoLeft == 0 && theGraph.isUnwinable() ) {
			gameState = GAME_LOSE;
			return;
		}
	}
	
	/*
	======================== RightPane class ========================
	*/
	
	RightPane::RightPane(multimedia::Window *theWindow, Game* theGame):
	mainWindow(theWindow),
	top(theGame),
	buttonTempl("GM_active.png", "GM_inactive.png",
	GM_ButtonWidthPercent, GM_ButtonHeightPercent),
	undoText(_("Undo\n(%{count}% left)")),
	add2Text(_("Call for\nreinforcements").get()),
	endGameText(_("End game").get()),
	infoTextSurface(nullptr),
	infoTextRect(0,0,0,0) {
		undoLeft = 0;
		canAdd = true;
		clickSpace = nullptr;
		endGameEvent = nullptr;
	}
	
	RightPane::~RightPane() {
		if( clickSpace != nullptr ) delete clickSpace;
	}
	
	void RightPane::paint(multimedia::Surface &destSurf) {
		if(undoLeft > 0) undoButton.paint(destSurf);
		if(canAdd) add2Button.paint(destSurf);
		endGameButton.paint(destSurf);
		//text surface
		destSurf.blit(infoTextRect.getLeft(), infoTextRect.getTop(), infoTextSurface);
		//overlay
		destSurf.blit(overlayLeftPos, 0, overlay);
	}
	
	void RightPane::mouseOver(int newMouseX, int newMouseY) {
		if( clickSpace == nullptr ) return;
		clickSpace->mouseMovedTo(newMouseX, newMouseY);
	}
	void RightPane::mouseClick() {
		if( clickSpace == nullptr ) return;
		clickSpace->mouseClick();
	}
	
	void RightPane::setInfoText(std::string info) {
		if( info.length() == 0 ) { //no text to display?
			infoTextSurface = multimedia::Surface(nullptr);//set it to empty surface
			return;
		}
		infoTextSurface = buttonTempl.drawingFont.renderMultiLine(
			info,
			infoTextRect.getWidth(),
			multimedia::TextAlign::LEFT,
			multimedia::Color(200, 200, 200)
		);
	}
	
	void RightPane::addSwitchToUndo(unsigned int indexA, unsigned int indexB) {
		if( undoLeft == 0 ) return;
		
		undoList.push_back( {indexA, indexB} );
		if( undoList.size() > undoLeft ) {
			undoList.pop_front();
		}
	}
	
	void RightPane::redrawAssets(int newWidth) {
		const int leftBorder = mainWindow->getWidth() - newWidth;
		const int windowHeight = mainWindow->getHeight();
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
		
		const int bHeight = buttonTempl.reloadAssetsForParametersW(newWidth * 0.9, theLongestText);
		//make buttons
		if( clickSpace != nullptr ) delete clickSpace;
		clickSpace = new UI::Space(leftBorder,0,newWidth,windowHeight);
		
		add2Button = buttonTempl.makeButton(add2Text, clickSpace);
		endGameButton = buttonTempl.makeButton(endGameText, clickSpace);
		undoButton = buttonTempl.makeButton(undoText.apply("count",undoLeft).get(), clickSpace);
		
		//setup buttons
		add2Button.assignEvents();
		add2Button.onClickHandler([this](){
			canAdd = false;
			top->theGraph.add2ExtraPeople();
			top->graphPane.reallocateElements();
		});
		
		endGameButton.assignEvents();
		if( endGameEvent != nullptr ) endGameButton.onClickHandler( endGameEvent );
		
		undoButton.assignEvents();
		undoButton.onClickHandler([this](){
			if( undoList.size() == 0 ) return;
			
			auto& [indexA, indexB] = undoList.back();
			undoList.pop_back();
			
			top->graphPane.makeUndoAnimation( indexA , indexB );
			--undoLeft;
			
			undoButton.setText(undoText.apply("count",undoLeft).get());
		});
		
		//position buttons and text
		int posX = leftBorder + (newWidth * 0.05);
		int posY = buttonSpacing;
		infoTextRect.setLeft( posX );
		//::add2
		add2Button.setPosition(posX, posY, false);
		//::undo
		posY += bHeight + buttonSpacing;
		undoButton.setPosition(posX, posY, false);
		posY += bHeight + buttonSpacing;
		//::top border of text
		infoTextRect.setTop( posY );
		//::end game
		posY = windowHeight - bHeight - buttonSpacing;
		endGameButton.setPosition(posX, posY, false);
		//::height and width of text
		posY -= buttonSpacing;
		infoTextRect.setHeight( posY - infoTextRect.getTop() );
		infoTextRect.setWidth( newWidth * 0.9 );
	}
	
	/*
	======================== BodyRepr class ========================
	*/
	
	constexpr int BodyReprCircleSize = 60;
	
	BodyRepr::BodyRepr(GraphPane *graphPane, int _x, int _y, double _angle, main_game::Body *_body): 
	graph(graphPane), x(_x), y(_y), angle(_angle), body(_body), animPhase(-1.0),
	mouseOver(false), selected(false)/*, comp(nullptr)*/ {}
	
	void BodyRepr::paint(multimedia::Surface &destSurf) {
		using multimedia::Color;
		
		auto drawer = destSurf.getDrawer();
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
		
		const auto black = Color(0,0,0);
		const auto green = Color(0,255,0);
		const auto ownerColor = Color::fromHSV(
			body->currentOwner->color, saturation, value);
		const auto bodyColor = Color::fromHSV(
			body->rightOwner->color, saturation, value);
		
		drawer.setColor(ownerColor);
		drawer.drawCircle(x,y, outerRadius, true);
		drawer.setColor(black);
		drawer.drawCircle(x,y, innerRadius, true);
		
		auto ownerSurf = graph->bigFont.render(body->currentOwner->short_name, green);
		auto ownerRect = ownerSurf.getSize();
		ownerRect.setCenterX(x); ownerRect.setCenterY(y);
		destSurf.blit(ownerRect.getLeft(), ownerRect.getTop(), ownerSurf);
		
		const int shiftX = x + std::cos(angle + M_PI) * outerRadius;
		const int shiftY = y + std::sin(angle + M_PI) * outerRadius;
		
		drawer.setColor(bodyColor);
		drawer.drawCircle(shiftX, shiftY, outerRadiusSmall, true);
		drawer.setColor(black);
		drawer.drawCircle(shiftX, shiftY, innerRadiusSmall, true);
		
		ownerSurf = graph->smallFont.render(body->rightOwner->short_name, green);
		ownerRect = ownerSurf.getSize();
		ownerRect.setCenterX(shiftX); ownerRect.setCenterY(shiftY);
		destSurf.blit(ownerRect.getLeft(), ownerRect.getTop(), ownerSurf);
		
	}
	
	void BodyRepr::makeUI(UI::Space *clickSpace, int shiftX, int shiftY, 
			std::function<void(main_game::Body*)> extraMouseMove,
			std::function<bool(main_game::Body*)> shouldBeSelected) {
		Rect clickable{	shiftX+x - BodyReprCircleSize, 
						shiftY+y - BodyReprCircleSize, 
						BodyReprCircleSize*2, 
						BodyReprCircleSize*2};
		auto comp = clickSpace->newComponent(clickable);
		comp->addMouseEnterListener([this,extraMouseMove](){
			mouseOver = true;
			extraMouseMove(body);
		});
		comp->addMouseLeaveListener([this,extraMouseMove](){
			mouseOver = false;
			extraMouseMove(nullptr);
		});
		comp->addClickListener([this,shouldBeSelected](){
			selected = shouldBeSelected(body);
		});
	}
	
	/*
	======================== ConnRepr class ========================
	*/
	
	ConnRepr::ConnRepr(main_game::PersonGraph *theGraph, 
			int startX, int startY, int endX, int endY,
			unsigned int from, unsigned int to):
		graph(theGraph), sX(startX), sY(startY), eX(endX), eY(endY),
		fromIndex(from), toIndex(to), inAnimation(false)	
	{}
	
	bool ConnRepr::isAConnectionBetween(int from, int to) {
		if( fromIndex != from ) return false;
		if( toIndex != to ) return false;
		return true;
	}
	
	void ConnRepr::paint(multimedia::Surface &destSurf) {
		auto drawer = destSurf.getDrawer();
		double colorH;
		if( graph->canBeSwitched(fromIndex, toIndex) ) {
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
	
	/*
	======================== GraphPane class ========================
	*/

	GraphPane::GraphPane(Game *topPane):
		top(topPane),
		bigFont("Jura-Regular.ttf",40),
		smallFont("Jura-Regular.ttf", 30),
		selectedBody(nullptr),
		clickSpace(nullptr),
		switchSound("transport.ogg"),
		undoSound("transport_back.ogg"),
		quickSound("transport_quick.ogg"),
		wahSound("wah.ogg") {
		
	}
	
	void GraphPane::redrawAssets(Rect np) {
		panelSurf = multimedia::Surface::newRGB_Surface(np.getWidth(), np.getHeight());
		panelSurf.fill(multimedia::Color(0,0,0));//fill with black
		panelPosition = np;
		
		reallocateElements();
	}
	
	///change the position of body representations class
	void GraphPane::reallocateElements() {
		if( clickSpace != nullptr ) delete clickSpace;
		Rect &np = panelPosition;
		clickSpace = new UI::Space(np.getLeft(), np.getTop(), np.getWidth(), np.getHeight());
		
		const int center = np.getWidth() / 2;//or getHeight()/2 — the size is the same
		const int size = center - BodyReprCircleSize - 5;
		const unsigned int elemNum = top->theGraph.getSize();
		const double angleStep = 2 * M_PI / elemNum;
		
		bodies.clear();
		connections.clear();
		for(unsigned int i=0; i<elemNum; ++i) {
			int posX = std::cos(i * angleStep) * size + center;
			int posY = std::sin(i * angleStep) * size + center;
			bodies.emplace_back( this , posX , posY , i * angleStep ,
				&(top->theGraph.getBodyByIndex(i)) );
			for(unsigned int j=i+1; j<elemNum; ++j) {
				int endX = std::cos(j * angleStep) * size + center;
				int endY = std::sin(j * angleStep) * size + center;
				connections.emplace_back( &(top->theGraph) , posX , posY , endX , endY , i , j );
			}
		}
		
		//assign ui elements to body representations
		for(auto& body : bodies) {
			body.makeUI(clickSpace, np.getLeft(), np.getTop(), 
			[this](main_game::Body* bodyOver) { 
				mouseOverBody(bodyOver);
				redrawElements();
			},
			[this](main_game::Body* bodyClicked) -> bool {
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
					unsigned int indexA = top->theGraph.getIndexOfBody(selectedBody);
					unsigned int indexB = top->theGraph.getIndexOfBody(bodyClicked);
					if( top->theGraph.canBeSwitched(indexA, indexB) ) {
						makeSwitchAnimation(indexA, indexB);
						return true;
					} else {
						return false;
					}
				}
			}
			);
		}
		
		redrawElements();
	}
	
	///paints elements on the panel surface
	void GraphPane::redrawElements() {
		panelSurf.fill(multimedia::Color(0,0,0));
		for(auto& connR : connections) {
			connR.paint(panelSurf);
		}
		for(auto& bodyR : bodies) {
			bodyR.paint(panelSurf);
		}
	}
	
	void GraphPane::paint(multimedia::Surface &destSurf) {
		if( panelPosition.getHeight() == 0 || panelPosition.getWidth() == 0 ) return;//draw nothing
		
		destSurf.blit(panelPosition.getLeft(), panelPosition.getTop(), panelSurf);
	}
	
	void GraphPane::mouseOverBody(main_game::Body *bodyOver) {
		std::string infoText;
		
		if( bodyOver == nullptr ) {
			top->controlPane.setInfoText(infoText);
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
				unsigned int indexA = top->theGraph.getIndexOfBody(bodyOver);
				unsigned int indexB = top->theGraph.getIndexOfBody(selectedBody);
				if( top->theGraph.canBeSwitched(indexA, indexB) )
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
		
		top->controlPane.setInfoText(infoText);
	}
	
	void GraphPane::mouseOver(int newMouseX, int newMouseY) {
		if( clickSpace == nullptr ) return;
		clickSpace->mouseMovedTo(newMouseX, newMouseY);
	}
	
	void GraphPane::mouseClick() {
		if( clickSpace == nullptr ) return;
		clickSpace->mouseClick();
	}
	
	void GraphPane::makeSwitchAnimation(unsigned int indexA, unsigned int indexB) {
		constexpr double animDelay = 1.5;
		auto timePoint = animation::addSeconds(0.1, animation::getNow());
		top->inputLock = true;
		auto reprA = &bodies[indexA];
		auto reprB = &bodies[indexB];
		
		auto it = connections.begin();
		while( it != connections.end() ) {
			if( it->isAConnectionBetween(indexA, indexB) || it->isAConnectionBetween(indexB, indexA)) {
				std::rotate(it, it+1, connections.end());
				break;
			}
			++it;
		}
		
		auto conn = &connections.back();
		
		//play the sound
		switchSound.play();
		
		conn->inAnimation = true;
		animation::newShiftedSpanAnimation(timePoint, animDelay, [this,reprA,reprB](double phase){
			reprA->animPhase = phase;
			reprB->animPhase = phase;
			redrawElements();
		});
		timePoint = animation::addSeconds(animDelay, timePoint);
		
		animation::newPointAnimation(timePoint, [this,indexA,indexB](){
			top->theGraph.switchBodiesBetween(indexA, indexB);
			top->controlPane.addSwitchToUndo(indexA, indexB);
			redrawElements();
		});
		timePoint = animation::addSeconds(0.1, timePoint);
		
		animation::newShiftedSpanAnimation(timePoint, animDelay, [this,reprA,reprB](double phase){
			reprA->animPhase = 1.0 - phase;
			reprB->animPhase = 1.0 - phase;
			redrawElements();
		});
		timePoint = animation::addSeconds(animDelay, timePoint);
		
		animation::newPointAnimation(timePoint, [this,reprA,reprB,conn](){
			reprA->animPhase = -1.0;
			reprB->animPhase = -1.0;
			conn->inAnimation = false;
			selectedBody = nullptr;
			for(auto& body : bodies) {//unselect all body representations
				body.selected = false;
			}
			redrawElements();
			top->inputLock = false;
			top->checkWinLoseCondition();
		});
	}
	
	void GraphPane::makeUndoAnimation(unsigned int indexA, unsigned int indexB) {
		constexpr double animDelay = 1.5;
		auto timePoint = animation::addSeconds(0.1, animation::getNow());
		top->inputLock = true;
		auto reprA = &bodies[indexA];
		auto reprB = &bodies[indexB];
		
		auto it = connections.begin();
		while( it != connections.end() ) {
			if( it->isAConnectionBetween(indexA, indexB) || it->isAConnectionBetween(indexB, indexA)) {
				std::rotate(it, it+1, connections.end());
				break;
			}
			++it;
		}
		
		auto conn = &connections.back();
		
		//play the sound
		undoSound.play();
		
		conn->inAnimation = true;
		animation::newShiftedSpanAnimation(timePoint, animDelay, [this,reprA,reprB](double phase){
			reprA->animPhase = phase;
			reprB->animPhase = phase;
			redrawElements();
		});
		timePoint = animation::addSeconds(animDelay, timePoint);
		
		animation::newPointAnimation(timePoint, [this,indexA,indexB](){
			top->theGraph.undoSwitchBodiesBetween(indexA, indexB);
			redrawElements();
		});
		timePoint = animation::addSeconds(0.1, timePoint);
		
		animation::newShiftedSpanAnimation(timePoint, animDelay, [this,reprA,reprB](double phase){
			reprA->animPhase = 1.0 - phase;
			reprB->animPhase = 1.0 - phase;
			redrawElements();
		});
		timePoint = animation::addSeconds(animDelay, timePoint);
		
		animation::newPointAnimation(timePoint, [this,reprA,reprB,conn](){
			reprA->animPhase = -1.0;
			reprB->animPhase = -1.0;
			conn->inAnimation = false;
			selectedBody = nullptr;
			for(auto& body : bodies) {//unselect all body representations
				body.selected = false;
			}
			redrawElements();
			top->inputLock = false;
		});
	}
	
	void GraphPane::makeInitialSwitchAnimation(std::stack<std::pair<unsigned int,unsigned int>> switches) {
		constexpr double animDelay = 0.5;
		auto timePoint = animation::addSeconds(0.01, animation::getNow());
		top->inputLock = true;
		
		//play wah
		animation::newPointAnimation(timePoint, [this](){
			wahSound.play();
			top->exlAnim = 1.0;
		});
		timePoint = animation::addSeconds(0.01, timePoint);
		//animate the exclamation mark
		animation::newShiftedSpanAnimation(timePoint, 1.0, [this](double phase){
			phase = (static_cast<int>(phase * 100.0) % 25) / 25.0;
			top->exlAnim = 1.0 - phase;
		});
		timePoint = animation::addSeconds(1.1, timePoint);
		animation::newPointAnimation(timePoint, [this](){
			top->exlAnim = -1.0;
		});
		timePoint = animation::addSeconds(0.01, timePoint);
		
		while( !switches.empty() ) {
			auto oneSwitch = switches.top(); switches.pop();
			unsigned int indexA = oneSwitch.first;
			unsigned int indexB = oneSwitch.second;
			auto reprA = &bodies[indexA];
			auto reprB = &bodies[indexB];
			ConnRepr* conn = nullptr;
			for(auto& connF : connections) {
				if( connF.isAConnectionBetween(indexA,indexB) || connF.isAConnectionBetween(indexB,indexA)) {
					conn = &connF;
					break;
				}
			}
			//initial setup
			animation::newPointAnimation(timePoint,[this,conn](){
				quickSound.play();
				conn->inAnimation = true;
			});
			timePoint = animation::addSeconds(0.01, timePoint);
			//before the switch
			animation::newShiftedSpanAnimation(timePoint, animDelay, [this,reprA,reprB](double phase){
				reprA->animPhase = phase;
				reprB->animPhase = phase;
				redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			//the switch
			animation::newPointAnimation(timePoint, [this,indexA,indexB](){
				top->theGraph.switchBodiesBetween(indexA, indexB);
				redrawElements();
			});
			timePoint = animation::addSeconds(0.01, timePoint);
			//after the switch
			animation::newShiftedSpanAnimation(timePoint, animDelay, [this,reprA,reprB](double phase){
				reprA->animPhase = 1.0 - phase;
				reprB->animPhase = 1.0 - phase;
				redrawElements();
			});
			timePoint = animation::addSeconds(animDelay, timePoint);
			//post-setup
			animation::newPointAnimation(timePoint, [this,reprA,reprB,conn](){
				reprA->animPhase = -1.0;
				reprB->animPhase = -1.0;
				conn->inAnimation = false;
				redrawElements();
			});
			timePoint = animation::addSeconds(0.01, timePoint);
		}
		//the end
		animation::newPointAnimation(timePoint,[this](){
			top->inputLock = false;
		});
	}
	
}//end namespace game_logic
