#ifndef _ERROR_H_
#define _ERROR_H_

#include <string>
#include <iostream>

[[ noreturn ]] void RaiseError(std::string errorMessage);
#endif