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

#include "mulanstr_config.hpp"
#include "template_object.hpp"
#include "basic_parse.hpp"
#include "language_features.hpp"

namespace mls {
	
	
	StringTemplateElement::StringTemplateElement(StringElement* element) {
		theString = element->getString();
	}
	StringTemplateElement::StringTemplateElement(std::string str) {
		theString = str;
	}
	TemplateElement* StringTemplateElement::copy() {
		return new StringTemplateElement(theString);
	}
	std::string StringTemplateElement::execute(Template& parent) {
		return theString;
	}
	
	
	SubstitutionTemplateElement::SubstitutionTemplateElement(std::string valName):
		valueName(valName) {};
	
	std::string SubstitutionTemplateElement::execute(Template& parent) {
		try{
			return parent.findArgument(valueName).get();
		}
		catch(RequiredVariableNotFound e) {
			return std::to_string( parent.findNumber(valueName) );
		}
	}
	
	TemplateElement* SubstitutionTemplateElement::copy() {
		return new SubstitutionTemplateElement(valueName);
	}
	
	
	CaseSetterElement::CaseSetterElement(std::string varName, std::string caseName):
		variableName(varName), caseName(caseName) {};
	
	std::string CaseSetterElement::execute(Template& parent) {
		return parent.findArgument(variableName).apply("__CASE__",caseName).get();
	}
	
	TemplateElement* CaseSetterElement::copy() {
		return new CaseSetterElement(variableName,caseName);
	}
	
	
	
	CaseWriterElement::CaseWriterElement(std::map<std::string,std::string> &paramsList):
		params(paramsList) {};
	
	std::string CaseWriterElement::execute(Template& parent) {
		const std::string caseName = parent.findArgument("__CASE__").get();
		const auto& found = params.find(caseName);
		if( found == params.end() )
			return "";
		else
			return found->second;
	}
	
	TemplateElement* CaseWriterElement::copy() {
		return new CaseWriterElement(params);
	}
	
	
	GenderWriterElement::GenderWriterElement(std::string varName, std::map<std::string, std::string> &list):
		variableName(varName), argList(list) {};
	
	std::string GenderWriterElement::execute(Template& parent) {
		std::string gender = parent.findArgument(variableName).getGender();
		if( gender == "" ) return "";
		
		if( auto found = argList.find(gender); found != argList.end() ) {
			return found->second;
		}
		
		return "";
	}
	
	TemplateElement* GenderWriterElement::copy() {
		return new GenderWriterElement(variableName,argList);
	}
	
	
	PluralWriterElement::PluralWriterElement(std::string varName, std::map<std::string, std::string> &list):
		variableName(varName), argList(list) {};
	
	std::string PluralWriterElement::execute(Template& parent) {
		long number = parent.findNumber(variableName);
		std::string numberAsStr = std::to_string(number);
		std::string numClass = parent.languageFeature->pluralizer(number,numberAsStr);
		
		if( auto found = argList.find(numClass); found != argList.end() )
			return found->second;
		else
			return "";
	}
	
	TemplateElement* PluralWriterElement::copy() {
		return new PluralWriterElement(variableName, argList);
	}
	
	
	Template::Template(std::string string_template, Feature* langFeature):
	languageFeature(langFeature) {
		auto parsedString = basicParseString(string_template);
		
		for(auto &basic_element : parsedString) {
			switch( basic_element->getType() ) {
				case ElementType::STRING:
					elements.emplace_back( 
						new StringTemplateElement(dynamic_cast<StringElement*>(basic_element.get())) 
						);
					break;
				
				case ElementType::SUBSTITUTION: {
					elements.emplace_back(
						new SubstitutionTemplateElement(
							dynamic_cast<SubstElement*>(basic_element.get())->getVariableName()
						)	
					);
				}
				break;
				
				case ElementType::FUNCTION: {
					FunctElement* fun = dynamic_cast<FunctElement*>(basic_element.get());
					TemplateElement* newElem = makeFunction(fun->getFunctionName(),fun->arguments,"");
					if( newElem != nullptr ) {
						elements.emplace_back( newElem );
					}
				}
				break;
				
				case ElementType::FUNCTION_WITH_SUBSTITUTION: {
					FunctAndSubstElement* fun = dynamic_cast<FunctAndSubstElement*>(basic_element.get());
					TemplateElement* newElem = makeFunction(fun->getFunctionName(),fun->arguments,fun->varName);
					if( newElem != nullptr ) {
						elements.emplace_back( newElem );
					}
				}
				break;
					
				default:
					break;
			}
		}
	}
	
	 

	std::map<std::string,std::string> Template::convertTableToMap(std::vector<std::string> &table, const char* replacementList) {
		std::map<std::string,std::string> paramsMap;
		for(unsigned int n=0; n < table.size(); ++n) {
			std::string argName = mls::getNthOnSClist(n,replacementList);
			paramsMap.insert_or_assign(argName, table[n]);
		}
		return paramsMap;
	}
			
	
	 

	TemplateElement* Template::makeFunction(std::string name, FunctArgsBase* arguments, std::string varName) {
		
		
		if( name == "SG" ) {
			
			if( arguments == nullptr )
				throw WrongFunctionArguments();
			if( arguments->getArgType() != FunctionArgs::ONE_ARG )
				throw WrongFunctionArguments();
			
			 gender = static_cast<OneFunctArg*>(arguments)->getValue();
			
			
			return nullptr;
		}
		
		
		if( name == "G" ) {
			
			if( varName == "" )
				throw RequiredVariableNotFound();
			
			if( arguments == nullptr )
				throw WrongFunctionArguments();
				
			switch( arguments->getArgType() ) {
				case FunctionArgs::MAP_ARGS: {
					auto &list = static_cast<MapFunctArg*>(arguments)->paramsMap;
					return new GenderWriterElement(varName,list);
				}
				break;
				
				case FunctionArgs::TABLE_ARGS: {
					auto &table = static_cast<TabFunctArg*>(arguments)->paramsTable;
					auto params = convertTableToMap(table, languageFeature->genderList);
					return new GenderWriterElement(varName, params);
				}
				break;
				
				default:
					throw WrongFunctionArguments();
			}
			
			
		}
		
		
		
		if( name == "C" ) {
			if( varName != "" ) { 
				
				if( arguments == nullptr )
					throw WrongFunctionArguments();
				if( arguments->getArgType() != FunctionArgs::ONE_ARG )
					throw WrongFunctionArguments();
				
				const std::string caseName = static_cast<OneFunctArg*>(arguments)->getValue();
				return new CaseSetterElement(varName, caseName);
			}
			else 
			{
				
				if( arguments == nullptr )
					throw WrongFunctionArguments();
				
				
				this->arguments.insert({"__CASE__",Template("",languageFeature)});
				
				switch( arguments->getArgType() ) {
					case FunctionArgs::MAP_ARGS: {
						auto &list = static_cast<MapFunctArg*>(arguments)->paramsMap;
						return new CaseWriterElement(list);
					}
					break;
					
					case FunctionArgs::TABLE_ARGS: {
						auto &table = static_cast<TabFunctArg*>(arguments)->paramsTable;
						auto params = convertTableToMap(table, languageFeature->casesList);
						return new CaseWriterElement(params);
					}
					break;
					
					default:
						throw WrongFunctionArguments();
				}
				
				
			}
		}
		
		
		if( name == "P" ) {
			
			if( varName == "" )
				throw RequiredVariableNotFound();
			
			if( arguments == nullptr )
				throw WrongFunctionArguments();
			
			switch( arguments->getArgType() ) {
				case FunctionArgs::MAP_ARGS: {
						auto &list = static_cast<MapFunctArg*>(arguments)->paramsMap;
						return new PluralWriterElement(varName, list);
					}
					break;
					
					case FunctionArgs::TABLE_ARGS: {
						auto &table = static_cast<TabFunctArg*>(arguments)->paramsTable;
						auto params = convertTableToMap(table, languageFeature->pluralsList);
						return new PluralWriterElement(varName, params);
					}
					break;
					
					default:
						throw WrongFunctionArguments();
			}
		}
		
		return nullptr;
	}
	
	
	Template& Template::findArgument(std::string name) {
		if( auto arg = arguments.find(name); arg == arguments.end() ) 
			throw RequiredVariableNotFound();
		else
			if( std::holds_alternative<Template>(arg->second) )
				return std::get<Template>(arg->second);
			else
				throw RequiredVariableNotFound();
	}
	
	
	long Template::findNumber(std::string name) {
		if( auto arg = arguments.find(name); arg == arguments.end() ) 
			throw RequiredVariableNotFound();
		else
			if( std::holds_alternative<long>(arg->second) )
				return std::get<long>(arg->second);
			else
				throw RequiredVariableNotFound();
	}
	
	Template::Template(const Template& other) {
		for(const auto& other_element : other.elements ) {
			elements.emplace_back( other_element->copy() );
		}
		for(const auto& other_argument : other.arguments ) {
			arguments.insert(other_argument);
		}
		gender = other.gender;
		languageFeature = other.languageFeature;
	}
	
	Template& Template::operator=(const Template& other) {
		if(this == &other) return *this;
		
		elements.clear();
		for(const auto& other_element : other.elements ) {
			elements.emplace_back( other_element->copy() );
		}
		arguments = other.arguments;
		gender = other.gender;
		languageFeature = other.languageFeature;
		
		return *this;
	}
	
	Template& Template::apply(std::string name, std::string value) {
		return apply(name, Template(value,languageFeature));
	}
	
	Template& Template::apply(std::string name, Template value) {
		arguments.insert_or_assign(name,value);
		
		return *this;
	}
	
	Template& Template::apply(std::string name, long value) {
		arguments.insert_or_assign(name, value);
		
		return *this;
	}
	
	std::string Template::get() {
		std::string output;
		
		for(const auto& element : elements) {
			
			output += element->execute(*this);
		}
		
		return output;
	}
	
	std::string Template::getGender() {
		return gender;
	}
	
}

