#pragma once

#ifndef BRS_MULANSTRING_INSTALL_HPP
#define BRS_MULANSTRING_INSTALL_HPP

//We want to use GNU GetText as the backend for the game
#define MULANSTR_USE_GETTEXT

//include the mulanstring header file
#include <mulanstring/main.hpp>

//function to initialize the mls
void mls_init();

#endif /* BRS_MULANSTRING_INSTALL_HPP */
