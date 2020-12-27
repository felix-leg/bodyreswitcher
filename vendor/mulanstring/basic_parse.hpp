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

#ifndef MULANSTR_BASIC_PARSE_HPP
#define MULANSTR_BASIC_PARSE_HPP

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace mls {
	
	class BadlyFormatedTemplateString: public std::exception {};
	
	enum class ElementType {
		STRING,
		SUBSTITUTION,
		FUNCTION,
		FUNCTION_WITH_SUBSTITUTION,
	};
	enum class FunctionArgs {
		
		ONE_ARG,
		TABLE_ARGS,
		MAP_ARGS
	};
	
	
	class FunctArgsBase {
		public:
			virtual FunctionArgs getArgType() = 0;
			virtual ~FunctArgsBase() {};
	};
	
	class OneFunctArg: public FunctArgsBase {
		public:
			virtual FunctionArgs getArgType() override;	
			
			OneFunctArg(std::string value):argValue(value) {};
			std::string getValue();
		private:
			std::string argValue;
	};
	
	class TabFunctArg: public FunctArgsBase {
		public:
			virtual FunctionArgs getArgType() override;
			std::vector<std::string> paramsTable;
	};
	
	class MapFunctArg: public FunctArgsBase {
		public:
			virtual FunctionArgs getArgType() override;
			std::map<std::string,std::string> paramsMap;	
	};
	
	
	class TemplateElementBase {
		public:
			virtual ElementType getType() = 0;
			virtual ~TemplateElementBase() {};
	};
	
	
	class StringElement: public TemplateElementBase {
		public:
			StringElement(std::string str):strElem(str) {};
			virtual ElementType getType() override;	
			std::string getString();
		private:
			std::string strElem;
	};
	
	
	class SubstElement: public TemplateElementBase {
		public:
			SubstElement(std::string variable):varName(variable) {};
			virtual ElementType getType() override;
			std::string getVariableName();
		private:
			std::string varName;
	};
	
	
	class FunctElement: public TemplateElementBase {
		public:
			FunctElement(std::string functionName):fnName(functionName),arguments(nullptr) {};
			~FunctElement() { delete arguments; };
			
			virtual ElementType getType() override;
			std::string getFunctionName();
			FunctArgsBase* arguments;
		private:
			std::string fnName;
	};
	
	
	class FunctAndSubstElement: public TemplateElementBase {
		public:
			FunctAndSubstElement(std::string functionName):fnName(functionName),arguments(nullptr) {};
			~FunctAndSubstElement() { delete arguments; };
			virtual ElementType getType() override;
			std::string getFunctionName();
			FunctArgsBase* arguments;
			std::string varName;
		private:
			std::string fnName;
	};
	
	using TemplateElementsList = std::vector<std::unique_ptr<TemplateElementBase>>;
	TemplateElementsList basicParseString(std::string templateString);
}; 

#endif

