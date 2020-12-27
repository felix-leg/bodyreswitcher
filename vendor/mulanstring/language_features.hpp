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

#ifndef MULANSTR_LANGUAGE_FEATURES_HPP
#define MULANSTR_LANGUAGE_FEATURES_HPP

#include <map>
#include <string>

namespace mls {
	
	struct Feature {
			const char* langName;
			const char* decimalSeparator;
			const char* thousandsSeparator;
			
			const char* (*pluralizer)(long numberAsInt, std::string numberAsString); 
			const char* casesList; 
			const char* genderList; 
			const char* pluralsList; 
	};
	
	
	std::string getNthOnSClist(const unsigned int n, const char* list, const char* defaultValue = "");
	
	extern std::map<std::string, Feature> FeaturesByLang;
	
}

#endif

