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

#include "language_features.hpp"
#include <cstring>


bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (fullString.compare (fullString.length() - ending.length(), ending.length(), ending) == 0);
    } else {
        return false;
    }
}

int twoDigitsEnding(std::string number) {
	if(number.length() >= 2)
		return std::stoi(number.substr(number.length() - 2));
	else
		return  std::stoi(number);
}





const char * PluralRule0(long numberAsInt, std::string numberAsString) {
	return "other";
}


const char * PluralRule1(long numberAsInt, std::string numberAsString) {
	if(numberAsInt == 1) return "one";
	return "other";
}


const char * PluralRule2(long numberAsInt, std::string numberAsString) {
	if( numberAsInt == 1 || numberAsInt == 0) return "zeroORone";
	return "other";
}


const char * PluralRule3(long numberAsInt, std::string numberAsString) {
	if( hasEnding(numberAsString,"0")) return "zero";
	if( hasEnding(numberAsString,"1") && !hasEnding(numberAsString,"11")) return "one";
	return "other";
}


const char * PluralRule4(long numberAsInt, std::string numberAsString) {
	if( numberAsInt == 1 || numberAsInt == 11 ) return "one";
	if( numberAsInt == 2 || numberAsInt == 12 ) return "two";
	if( 
		(numberAsInt >= 3 && numberAsInt <= 10)
		||
		(numberAsInt >= 13 && numberAsInt <= 19)
		) return "three";
	return "other";
}


const char * PluralRule5(long numberAsInt, std::string numberAsString) {
	if( numberAsInt == 1 ) return "one";
	int ending = twoDigitsEnding(numberAsString);
	if( numberAsInt == 0 || (ending >= 1 && ending <= 19) ) return "few";
	return "other";
}


const char * PluralRule6(long numberAsInt, std::string numberAsString) {
	if( numberAsInt != 11 && hasEnding(numberAsString,"1") ) return "one";
	int ending = twoDigitsEnding(numberAsString);
	if( hasEnding(numberAsString,"0") ||
		(ending >= 11 && ending <= 19)
	 ) return "few";
	 return "other";
}


const char * PluralRule7(long numberAsInt, std::string numberAsString) {
	if( hasEnding(numberAsString,"1") && numberAsInt != 11 ) return "one";
	int twoDigits = twoDigitsEnding(numberAsString);
	if(
		!( twoDigits >= 12 && twoDigits <= 14 )
		&&
		( hasEnding(numberAsString,"2") || hasEnding(numberAsString,"3") || hasEnding(numberAsString,"4") )
	) return "few";
	return "other";
}


const char * PluralRule8(long numberAsInt, std::string numberAsString) {
	if( numberAsInt == 1 ) return "one";
	if( numberAsInt >=2 && numberAsInt <= 4) return "few";
	return "other";
}


const char * PluralRule9(long numberAsInt, std::string numberAsString) {
	if( numberAsInt == 1 ) return "one";
	if(
		!( hasEnding(numberAsString,"12") || hasEnding(numberAsString,"13") || hasEnding(numberAsString,"14") )
		&&
		( hasEnding(numberAsString,"2") || hasEnding(numberAsString,"3") || hasEnding(numberAsString,"4") )
	) return "few";
	return "other";
}





#define LF(ownName,name,dSep,tSep,pluralFun,cases,genders,plurals) \
	{ std::string(ownName) , mls::Feature{name,dSep,tSep,pluralFun,cases,genders,plurals}}

std::map<std::string, mls::Feature> mls::FeaturesByLang = {
	LF("British English","en_GB",".",",",PluralRule1,"","","one;other"),
	LF("American English","en_US",".",",",PluralRule1,"","","one;other"),
	LF("Polski","pl_PL;Polish_Poland",","," ",PluralRule9,
	 

	"nom;gen;dat;acc;ins;loc;voc","m;f;n","one;few;other")
	
	
};

#undef LF

std::string mls::getNthOnSClist(const unsigned int n, const char* list, const char* defaultValue) {
	
	const unsigned int listSize = std::strlen(list);
	unsigned int startPosition = 0;
	unsigned int stopPosition;
	
	for(unsigned int index = 0; index < n; index++) {
		while( startPosition < listSize && list[startPosition] != ';' )
			++startPosition;
		
		++startPosition;
	}
	if( startPosition >= listSize ) return std::string(defaultValue);
	stopPosition = startPosition;
	
	while( stopPosition < listSize && list[stopPosition] != ';' )
		++stopPosition;
	
	return std::string(&list[startPosition], stopPosition - startPosition);
}

