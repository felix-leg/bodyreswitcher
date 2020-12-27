#pragma once

#ifndef BRS_ERRORS_HPP
#define BRS_ERRORS_HPP

#include <string>

void ShowError(std::string error_msg);

void ShowError(std::string error_init, const char* error_get);

#endif /* BRS_ERRORS_HPP */
