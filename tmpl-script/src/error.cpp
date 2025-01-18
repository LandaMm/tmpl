
#include"../include/error.h"

namespace Prelude
{
	ErrorManager::ErrorManager() { }
	ErrorManager::~ErrorManager() { }
	void ErrorManager::RaiseError(std::string errorMessage)
	{
		std::cout << "Error: " << errorMessage << std::endl;
		std::exit(0);
	}
}
