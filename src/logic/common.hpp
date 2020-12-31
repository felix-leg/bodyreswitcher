#pragma once

#ifndef BRS_COMMON_HPP
#define BRS_COMMON_HPP

namespace game_logic {
	
	enum class GameState {
		MainMenu,
		OptionsMenu,
		ScreenMenu,
		AboutMenu,
		DiffMenu,
		PreGame,
		TheGame,
		PostGame
	};
	
	extern GameState theGameState;
	
}//end namespace game_logic

#endif /* BRS_COMMON_HPP */
