/**
 * Prints some information from the filesystem module.
*/

#include "filesystem.hpp"
#include <iostream>

int main(int argc, char** argv) {
	using namespace filesystem;
	
	std::cout << "EXE directory: " << getProgramDir() << std::endl;
	std::cout << "Test file path: " << getDataPath("test.png") << std::endl;
	std::cout << "Config location: " << getConfigurationFile(false) << std::endl;
	
	return 0;
}