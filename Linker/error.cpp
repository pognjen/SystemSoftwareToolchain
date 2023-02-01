#include "error.h"

[[ noreturn ]] void RaiseError(std::string errorMessage)
{
	std::cout << errorMessage;
	exit(-1);
}
