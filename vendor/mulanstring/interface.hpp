/*
mulan-string stands for MUltiLANguage-STRING: an i18n and l10n library.
Copyright (C) 2020 felix-leg <felix.leg@gmail.comu>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef MULANSTR_INTERFACE_HPP
#define MULANSTR_INTERFACE_HPP

#include "language_features.hpp"
#include "template_object.hpp"

namespace mls {
	
	class LibraryNotInitialized : std::exception {};
	
	#ifdef MULANSTR_MAIN_HPP
	extern Feature* defaultLanguage;
	#else
	Feature* defaultLanguage = nullptr;
	#endif
	
	
	void initLocale(const char* locale);
	
	
	Template translate(const char* message);
	Template translate(const char* catalogue, const char* message);
	Template translateWithLocale(const char* message, const char* locale);
	Template translateWithLocale(const char* catalogue, const char* message, const char* locale);
}

#endif

