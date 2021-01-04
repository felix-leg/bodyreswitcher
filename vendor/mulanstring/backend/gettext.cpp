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

#include "gettext.hpp"
#include "../interface.hpp"
#include <libintl.h>
#include <locale.h>
#include <string>

namespace mls::backend {
	
	void init(const char* packageName, const char* wantedLocale, const char* folderLookup) {
		const char* theLocale;
		
		if( wantedLocale == nullptr) theLocale = setlocale(LC_ALL, "");
		else theLocale = setlocale(LC_ALL, wantedLocale);
		
		if( folderLookup == nullptr) bindtextdomain(packageName, "/usr/local/share/locale");
		else bindtextdomain(packageName, folderLookup);
		
		bind_textdomain_codeset(packageName, "utf-8");
		
		textdomain(packageName);
		
		mls::initLocale(theLocale);
	}
	
	std::string getTranslation(const char* msgid) {
		return std::string( gettext(msgid) );
	}
	
	std::string getTranslation(const char* catalogue, const char* msgid) {
		return std::string( dgettext(catalogue, msgid) );
	}
}

