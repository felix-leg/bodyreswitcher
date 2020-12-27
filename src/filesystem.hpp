#pragma once

#ifndef BRS_FILESYSTEM_HPP
#define BRS_FILESYSTEM_HPP

#include <string>

namespace filesystem {
	
	std::string getProgramDir();
	std::string getDataPath(std::string dataName);
	bool pathExists(std::string path);
	std::string getConfigurationFile(bool createIfNecessary);
	
}//end namespace filesystem

#endif /* BRS_FILESYSTEM_HPP */
