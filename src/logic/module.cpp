
#include "module.hpp"

namespace game_logic {
	
	Module::Module(std::function<void()> quitFunc):	quit(quitFunc) {}
	
	Module::~Module() {
		quit();
	}
	
}//end namespace game_logic

