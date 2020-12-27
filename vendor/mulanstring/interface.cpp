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

#include "interface.hpp"
#include "backend.hpp"
#include <string>


namespace mls {
	
	
	std::string parseLocaleString(const char* locale) {
		size_t stop = 0;
		while( locale[stop] != '\0' ) {
			if( locale[stop] == '.' ) break; 
			if( locale[stop] == '@' ) break; 
			++stop;
		}
		std::string strLoc(locale, stop);
		
		if( strLoc == "C" || strLoc == "POSIX" )
			return std::string("en_US");
		else 
			return strLoc;
	}
	
	void initLocale(const char* locale) {
		std::string strLoc = parseLocaleString(locale);
		
		if( auto found = FeaturesByLang.find(strLoc); found != FeaturesByLang.end() ) {
			defaultLanguage = &(found->second);
		} else {
			defaultLanguage = &FeaturesByLang["en_US"];
		}
	}
	
	Template translate(const char* message) {
		if( defaultLanguage == nullptr)
			throw LibraryNotInitialized();
		
		return Template{ backend::getTranslation(message), defaultLanguage};
	}
	Template translate(const char* catalogue, const char* message) {
		if( defaultLanguage == nullptr)
			throw LibraryNotInitialized();
		
		return Template{ backend::getTranslation(catalogue, message), defaultLanguage};
	}
	Template translateWithLocale(const char* message, const char* locale) {
		std::string strLoc = parseLocaleString(locale);
		Feature* language;
		
		if( auto found = FeaturesByLang.find(strLoc.c_str()); found != FeaturesByLang.end() ) {
			language = &(found->second);
		} else {
			language = &FeaturesByLang["en_US"];
		}
		
		return Template{ backend::getTranslation(message), language};
	}
	Template translateWithLocale(const char* catalogue, const char* message, const char* locale) {
		std::string strLoc = parseLocaleString(locale);
		Feature* language;
		
		if( auto found = FeaturesByLang.find(strLoc.c_str()); found != FeaturesByLang.end() ) {
			language = &(found->second);
		} else {
			language = &FeaturesByLang["en_US"];
		}
		
		return Template{ backend::getTranslation(catalogue, message), language};
	}
}

