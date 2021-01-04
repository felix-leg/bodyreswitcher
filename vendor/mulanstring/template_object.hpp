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

#ifndef MULANSTR_TEMPLATE_OBJECT_HPP
#define MULANSTR_TEMPLATE_OBJECT_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

#include "basic_parse.hpp"
#include "language_features.hpp"

namespace mls {
	
	
	
	class WrongFunctionCall : public std::exception {};
	class WrongFunctionArguments : public std::exception {};
	class RequiredVariableNotFound : public std::exception {};
	
#	ifdef MULANSTR_THROW_ON_INVALID_TEMPLATE
	class InvalidTemplateState : public std::exception {};
#	endif
	
	
	
	class Template; 
	 

	class TemplateElement {
		public:
			virtual std::string execute(Template&) = 0;
			virtual TemplateElement* copy() = 0;
	};
	
	 

	class StringTemplateElement : public TemplateElement {
		public:
			StringTemplateElement(StringElement* basic_element );
			virtual std::string execute(Template&) override;
			virtual TemplateElement* copy() override;
		private:
			std::string theString;
			StringTemplateElement(std::string other);
	};
	
	 

	class SubstitutionTemplateElement : public TemplateElement {
		public:
			SubstitutionTemplateElement(std::string valName);
			virtual std::string execute(Template&) override;
			virtual TemplateElement* copy() override;
		private:
			std::string valueName;
	};
	
	 

	class CaseSetterElement : public TemplateElement   {
		public:
			CaseSetterElement(std::string varName, std::string caseName);
			virtual std::string execute(Template&) override;
			virtual TemplateElement* copy() override;
			
		private:
			std::string variableName = "";
			std::string caseName = "";
	};
	
	 

	class CaseWriterElement : public TemplateElement {
		public:
			CaseWriterElement(std::map<std::string,std::string> &paramsList);
			virtual std::string execute(Template&) override;
			virtual TemplateElement* copy() override;
		private:
			std::map<std::string,std::string> params;
	};
	
	 

	class GenderWriterElement : public TemplateElement {
		public:
			GenderWriterElement(std::string varName, std::map<std::string, std::string> &list);
			virtual std::string execute(Template&) override;
			virtual TemplateElement* copy() override;
		private:
			std::map<std::string, std::string> argList;
			std::string variableName;
	};
	
	 

	class PluralWriterElement : public TemplateElement {
		public:
			PluralWriterElement(std::string varName, std::map<std::string, std::string>& list);
			virtual std::string execute(Template&) override;
			virtual TemplateElement* copy() override;
		private:
			std::map<std::string, std::string> argList;
			std::string variableName;
	};
	
	 

	class Template {
		public:
			Template(std::string string_template, Feature* langFeature);
			Template(const Template& other);
			Template(Template&& other) = default;
			Template& operator=(const Template& other);
			
			
			Template();
			
			
			Template& apply(std::string name, std::string value);
			Template& apply(std::string name, Template value);
			Template& apply(std::string name, long value);
			
			
			std::string get();
			
			
			std::string getGender();
		private:
			std::vector<std::unique_ptr<TemplateElement>> elements;
			typedef std::variant<long, Template> ArgType;
			std::map<std::string,ArgType> arguments;
			
			std::string gender = "";
			Feature* languageFeature;
			
			std::map<std::string,std::string> convertTableToMap(std::vector<std::string>& table, const char* replacementList);
			TemplateElement* makeFunction(std::string name, FunctArgsBase* arguments, std::string varName);
			Template& findArgument(std::string name);
			long findNumber(std::string name);
			
		friend class SubstitutionTemplateElement;
		friend class CaseSetterElement;
		friend class CaseWriterElement;
		friend class GenderWriterElement;
		friend class PluralWriterElement;
	};
	
}


#endif

