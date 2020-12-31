/**
 * This is the file containing the "main" function for
 * "Body re-Switcher" game
 */

#include <iostream>

#include "multimedia/base.hpp"
#include "multimedia/system.hpp"
#include "multimedia/graphics.hpp" 
#include "multimedia/font.hpp"
#include "multimedia/sound.hpp"
#include "rect.hpp"
#include "animation.hpp"

//Game Logic
#include "logic/common.hpp"
#include "logic/module.hpp"
#include "logic/main_menu_background.hpp"
#include "logic/main_menu.hpp"
#include "logic/about_menu.hpp"
#include "logic/options_menu.hpp"
#include "logic/screen_size_menu.hpp"
#include "logic/new_game_menu.hpp"
#include "logic/game_panel.hpp"


#include "game_config.hpp"
#include "mls.hpp"

int main(int argc, char** argv)
{
	mls_init();
	multimedia::SDL2_System system;
	GameConfig config;
	
	config.read();
	try {
		auto displayMode = config.video;
		if( !displayMode.isValid() )
			displayMode = system.getDefaultWindowSize();
		
		multimedia::Window mainWindow{"Body reSwitcher", displayMode, config.fullscreen, &system};
		multimedia::Music mainMenuMusic{"menu.ogg"};
		multimedia::Music gameMusic{"game.ogg"};
		
		auto MM_background = game_logic::MainMenuBackgroundInit(&mainWindow);
		auto MainMenu = game_logic::MainMenuInit(&mainWindow); 
		auto AboutMenu = game_logic::AboutMenuInit(&mainWindow);
		auto OptionsMenu = game_logic::OptionsMenuInit(&mainWindow, &mainMenuMusic);
		auto ScreenSizeMenu = game_logic::ScreenSizeMenuInit(&mainWindow, system.getSupportedVideoModes());
		auto NewGameMenu = game_logic::NewGameMenuInit(&mainWindow);
		auto TheGame = game_logic::GamePanelInit(&mainWindow);
		
		//update procedure
		mainWindow.registerUpdateFunction([&mainMenuMusic,&gameMusic](int mouseX, int mouseY, bool mouseClick){
			using namespace game_logic;
			
			//prepare the game
			if( theGameState == GameState::PreGame ) {
				if( mainMenuMusic.isPlaying() ) {
					mainMenuMusic.stop(300);
					gameMusic.play();
				}
				game_panel::initializeGame();
				theGameState = GameState::TheGame;
			}
			//quit the game
			if( theGameState == GameState::PostGame ) {
				if( gameMusic.isPlaying() ) {
					gameMusic.stop(300);
					mainMenuMusic.play();
				}
				theGameState = GameState::MainMenu;
			}
			
			switch( theGameState ) {
				case GameState::MainMenu:
					main_menu::mouseAction(mouseX, mouseY, mouseClick);
					break;
				case GameState::AboutMenu:
					about_menu::mouseAction(mouseX, mouseY, mouseClick);
					break;
				case GameState::OptionsMenu:
					options_menu::mouseAction(mouseX, mouseY, mouseClick);
					break;
				case GameState::ScreenMenu:
					screen_menu::mouseAction(mouseX, mouseY, mouseClick);
					break;
				case GameState::DiffMenu:
					new_game_menu::mouseAction(mouseX, mouseY, mouseClick);
					break;
				case GameState::TheGame:
					game_panel::mouseAction(mouseX, mouseY, mouseClick);
				default:
					break;
			}
			
			//play animations
			animation::applyTime(animation::getNow());
		});
		
		//paint procedure
		mainWindow.registerDrawFunction([&mainWindow](){
			using namespace game_logic;
			auto winSurf = mainWindow.getWindowSurface();
			//for everything but the main game
			if( theGameState != GameState::TheGame ) {
				//draw background
				mm_background::paint(winSurf);
				//draw the right menu
				switch( theGameState ) {
					case GameState::MainMenu:
						main_menu::paint(winSurf);
						break;
					case GameState::AboutMenu:
						about_menu::paint(winSurf);
						break;
					case GameState::OptionsMenu:
						options_menu::paint(winSurf);
						break;
					case GameState::ScreenMenu:
						screen_menu::paint(winSurf);
						break;
					case GameState::DiffMenu:
						new_game_menu::paint(winSurf);
					default:
						break;
				}
			} else {// theGameState == GameState::TheGame
				game_panel::paint(winSurf);
			}
		});
		
		//the loop
		if( config.music ) mainMenuMusic.play();
		mainWindow.mainLoop();
		config.music = mainMenuMusic.isPlaying();
		mainMenuMusic.stop();
		
		config.save();
		
	}catch( multimedia::CriticalSDLError ) {
		std::cerr << _("Critical error happened. Terminating the program.").get() << std::endl;
		return 1;
	}
	
	return 0;
}

