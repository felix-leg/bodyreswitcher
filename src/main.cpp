/**
 * This is the file containing the "main" function for
 * "Body re-Switcher" game
 */

#include <iostream>

#include "multimedia/base.hpp"
#include "multimedia/system.hpp"
#include "multimedia/window.hpp"
#include "multimedia/font.hpp"
#include "multimedia/sound.hpp"
#include "rect.hpp"
#include "animation.hpp"

//Game Logic
#include "logic/layer.hpp"
#include "logic/main_menu.hpp"
#include "logic/options_menu.hpp"
#include "logic/screen_size_menu.hpp"
#include "logic/about_screen.hpp"
#include "logic/new_game_menu.hpp"
#include "logic/game_panel.hpp"

#include "game_config.hpp"
#include "mls.hpp"

void prepareMainMenu(game_logic::MainMenu *mainMenu, 
	game_logic::OptionsMenu *optionsMenu,
	game_logic::About *aboutMenu,
	game_logic::NewGameMenu *newGameMenu);
void prepareOptionsMenu(game_logic::OptionsMenu *options, game_logic::ScreenSizeMenu *sizeMenu);

void prepareSelectGameMenu(game_logic::NewGameMenu *newGameMenu, game_logic::Game *theGame, bool *menuSwitch);

void prepareGame(game_logic::Game *theGame, bool *menuSwitch);

int main(int argc, char** argv)
{
	mls_init();
	multimedia::SDL2_System system;
	GameConfig config;
	
	config.read();
	try{
	auto displayMode = config.video;
	if( !displayMode.isValid() )
		displayMode = system.getDefaultWindowSize();
	
	multimedia::Window mainWindow{"Body reSwitcher", displayMode, config.fullscreen, &system};
	
	multimedia::Music mainMenuMusic{"menu.ogg"};
	multimedia::Music gameMusic{"game.ogg"};
	
	game_logic::MainMenu mainMenu{&mainWindow};
	game_logic::OptionsMenu optionsMenu{&mainWindow, &mainMenuMusic};
	game_logic::ScreenSizeMenu screenSizeMenu{&mainWindow, system.getSupportedVideoModes()};
	game_logic::About aboutMenu{&mainWindow};
	game_logic::NewGameMenu newGameMenu{&mainWindow};
	game_logic::Game theGame{&mainWindow};
	bool isInMainMenu = true;//a switch between main menu and the game
	bool isInMainMenuFlag = true;
	
	game_logic::LayerStack menuLayers{&mainWindow};
	menuLayers.addLayer( &mainMenu );
	menuLayers.addLayer( &aboutMenu );
	menuLayers.addLayer( &optionsMenu );
	menuLayers.addLayer( &screenSizeMenu );
	menuLayers.addLayer( &newGameMenu );
	prepareMainMenu( &mainMenu, &optionsMenu , &aboutMenu, &newGameMenu);
	prepareOptionsMenu( &optionsMenu , &screenSizeMenu );
	prepareSelectGameMenu( &newGameMenu , &theGame , &isInMainMenu );
	prepareGame(&theGame, &isInMainMenu );
	
	mainWindow.registerUpdateFunction(
		[&menuLayers,&theGame,&isInMainMenu,&isInMainMenuFlag,&mainMenuMusic,&gameMusic,&mainWindow]
		(int newMouseX, int newMouseY, bool mouseClick){
		if( isInMainMenuFlag != isInMainMenu ) {
			if( isInMainMenu == false ){//we are switching from the main menu to the game
				if( mainMenuMusic.isPlaying() ) {
					mainMenuMusic.stop(300);
					gameMusic.play();
				}
			//call for intro animation
			auto surf = mainWindow.getWindowSurface();
			theGame.initializeGame(surf);
			} else {//we are switching from the game to the main menu
				if( gameMusic.isPlaying() ) {
					gameMusic.stop(300);
					mainMenuMusic.play();
				}
			}
			isInMainMenuFlag = isInMainMenu;
		}
		if( isInMainMenuFlag ) {//we are in the menu
			menuLayers.onMouseEvent(newMouseX, newMouseY, mouseClick);
		} else {//we are in the game
			theGame.mouseOver(newMouseX, newMouseY);
			if( mouseClick ) theGame.mouseClick();
		}
		animation::applyTime(animation::getNow());//to run animations
	});
	
	
	mainWindow.registerDrawFunction([&isInMainMenuFlag,&mainWindow,&menuLayers,&theGame](){
		multimedia::Surface windowSurf = mainWindow.getWindowSurface();
		if( isInMainMenuFlag ) {
			menuLayers.paint(windowSurf);
		} else {
			theGame.paint(windowSurf);
		}
	});
	
	if(config.music) mainMenuMusic.play();
	//enter main loop
	mainWindow.mainLoop();
	//save config
	config.music = mainMenuMusic.isPlaying();
	config.fullscreen = mainWindow.isFullscreen();
	config.video = mainWindow.getDisplayMode();
	mainMenuMusic.stop(/*300*/);
	
	config.save();
	}catch( multimedia::CriticalSDLError ) {
		std::cerr << _("Critical error happened. Terminating the program.").get() << std::endl;
		return 1;
	}
	
	return 0;
}

void prepareMainMenu(game_logic::MainMenu *mainMenu, 
	game_logic::OptionsMenu *optionsMenu,
	game_logic::About *aboutMenu,
	game_logic::NewGameMenu *newGameMenu) {
	mainMenu->setExitEvent([]() {
		throw multimedia::SignaledGameEnd();
	});
	
	mainMenu->setOptionsEvent([optionsMenu]() {
		optionsMenu->enable();
	});
	
	mainMenu->setAboutEvent([aboutMenu]() {
		aboutMenu->enable();
	});
	
	mainMenu->setNewGameEvent([newGameMenu]() {
		newGameMenu->enable();
	});
}

void prepareOptionsMenu(game_logic::OptionsMenu *options, game_logic::ScreenSizeMenu *sizeMenu) {
	options->setScreenSizeEvent([=](){
		sizeMenu->enable();
	});
}

void prepareSelectGameMenu(game_logic::NewGameMenu *newGameMenu, game_logic::Game *theGame, bool *menuSwitch) {
	newGameMenu->setNewGameEvent([menuSwitch,theGame](game_logic::GameDifficulty gameDiff){
		switch( gameDiff ) {
			case game_logic::GameDifficulty::EASY:
				//std::cout << "Easy game choosen" << std::endl;
				theGame->theGraph.generateEasyGame();
				*menuSwitch = false;
				break;
			case game_logic::GameDifficulty::NORMAL:
				//std::cout << "Normal game choosen" << std::endl;
				theGame->theGraph.generateNormalGame();
				*menuSwitch = false;
				break;
			case game_logic::GameDifficulty::HARD:
				//std::cout << "Hard game choosen" << std::endl;
				theGame->theGraph.generateHardGame();
				*menuSwitch = false;
				break;
		}
	});
}

void prepareGame(game_logic::Game *theGame, bool *menuSwitch) {
	theGame->setEndGameEvent([=]() {
		*menuSwitch = true;
	});
}
