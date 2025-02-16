#pragma once

#ifndef BRS_MODULE_HPP
#define BRS_MODULE_HPP

#include <functional>

namespace game_logic {
	
	/**
	 * A special class that allows initialize and quit of a module 
	 * to be called at the right moments (RAII !!!)
	 */
	struct Module {
		Module(std::function<void()> quitFunc);
		~Module();
		private:
			std::function<void()> quit;
	};//end class Module
	
}//end namespace game_logic

#endif /* BRS_MODULE_HPP */
