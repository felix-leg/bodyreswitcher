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
#include "basic_parse.hpp"

#if __has_include(<version>)
#	include <version>
#endif

#if __has_include(<string_view>)
#	if __cpp_lib_starts_ends_with
#		define CAN_USE_STARTS_ENDS_WITH
#		include <string_view>
#	endif
#endif

namespace mls {
	
	
	ElementType StringElement::getType() {
		return ElementType::STRING;
	}
	std::string StringElement::getString() {
		return strElem;
	}
	
	
	ElementType SubstElement::getType() {
		return ElementType::SUBSTITUTION;
	}
	std::string SubstElement::getVariableName() {
		return varName;
	}
	
	
	ElementType FunctElement::getType() {
		return ElementType::FUNCTION;
	}
	std::string FunctElement::getFunctionName() {
		return fnName;
	}
	
	
	ElementType FunctAndSubstElement::getType() {
		return ElementType::FUNCTION_WITH_SUBSTITUTION;
	}
	std::string FunctAndSubstElement::getFunctionName() {
		return fnName;
	}
	
	
	FunctionArgs OneFunctArg::getArgType() {
		return FunctionArgs::ONE_ARG;
	}
	std::string OneFunctArg::getValue() {
		return argValue;
	}
	
	
	FunctionArgs TabFunctArg::getArgType() {
		return FunctionArgs::TABLE_ARGS;
	}
	
	
	FunctionArgs MapFunctArg::getArgType() {
		return FunctionArgs::MAP_ARGS;
	}
	
	
	
	 

	class BasicTagParser {
		public:
			BasicTagParser(std::string& templateString, std::size_t startPos):
			cPos(startPos), str(templateString) {};
			
			
			bool hasNext() {
				std::size_t pos = str.find(TAG_START,cPos);
				if( pos == std::string::npos ) return false;
				if( pos != cPos ) return false;
				return true;
			}
			
			bool hasText() {
				std::size_t pos = str.find(TAG_START,cPos);
				if( pos == std::string::npos ) {
					return cPos < str.size();
				}
				if( pos > cPos ) return true;
				return false;
			}
			
			std::string getText() {
				std::size_t pos = str.find(TAG_START,cPos);
				std::string returnStr;
				if( pos == std::string::npos ) {
					returnStr = str.substr(cPos);
					cPos = std::string::npos;
				} else {
					returnStr = str.substr(cPos, pos - cPos);
					cPos = pos;
				}
				return returnStr;
			}
			std::size_t getLastPosition() {
				return cPos;
			}
			
			
			TemplateElementBase* getNext() {
				advance( TAG_START.length() );
				
				if( isAtTheEndOfATag() ) 
					throw BadlyFormatedTemplateString();
				
				
				if( viewed() == '#') {
					advance();
					while( viewed() != '#') advance();
					advance();
					
					if( !isAtTheEndOfATag() ) throw BadlyFormatedTemplateString();
					advance( TAG_END.length() );
					
					return nullptr;
				}
				
				
				if( viewed() == '+') {
					advance();
					auto funcCall = parseFunctionAndArgs<FunctElement>();
					advance( TAG_END.length() );
					return funcCall;
				}
				
				std::string varName;
				while( viewed() != '!' && !isAtTheEndOfATag() ) {
					varName += viewed();
					advance();
				}
				
				if( viewed() == '!') {
					advance();
					auto funcCall = parseFunctionAndArgs<FunctAndSubstElement>();
					funcCall->varName = varName;
					advance( TAG_END.length() );
					return funcCall;
				} else {				
					advance( TAG_END.length() );
					return new SubstElement(varName);
				}
			}
		private:
			std::string& str;
			std::size_t cPos;
			
			 

			#ifdef CAN_USE_STARTS_ENDS_WITH
			std::string_view toBeParsedAsSV() {
				return std::string_view(str.data() + cPos, str.length()-cPos);
			}
			#endif
			
			 

			void advance(std::size_t howMuch = 1) {
				cPos += howMuch;
				if( cPos > str.length() ) {
					throw BadlyFormatedTemplateString();
				}
			}
			
			char viewed() {
				return str[cPos];
			}
			
			bool isAtTheEndOfATag() {
				#ifdef CAN_USE_STARTS_ENDS_WITH
				return toBeParsedAsSV().starts_with(TAG_END);
				#else				
				std::size_t pos = str.find(TAG_END,cPos);
				if( pos == std::string::npos ) return false;
				return pos == cPos;
				#endif
			}
			 

			template<class ReturnType>
			ReturnType* parseFunctionAndArgs() {
				
				if( isAtTheEndOfATag() ) 
					throw BadlyFormatedTemplateString();
				
				std::string functionName;
				
				while( viewed() >= 'A' && viewed() <= 'Z' ) {
					functionName += viewed();
					advance();
				}
				
				if( functionName.empty() ) throw BadlyFormatedTemplateString();
				
				
				ReturnType* returnValue = new ReturnType(functionName);
				
				try{
					 

					switch( viewed() ) {
						case '=': 
							advance();
							returnValue->arguments = new OneFunctArg(parseParam());
							
							
							if( !isAtTheEndOfATag() )
								throw BadlyFormatedTemplateString();
							
							break;
						
						case ':': 
						{
							advance();
							returnValue->arguments = new TabFunctArg();
							std::vector<std::string>& paramsTable = 
								static_cast<TabFunctArg*>(returnValue->arguments)->paramsTable;
							std::string parmValue;
							bool hasSeenComma = false;
							while( !isAtTheEndOfATag() ) {
								if( viewed() == ',') {
									hasSeenComma = true;
									paramsTable.push_back(parmValue);
									parmValue.clear();
								} else {
									parmValue += viewed();
								}
								advance();
							}
							
							
							if( !hasSeenComma )
								throw BadlyFormatedTemplateString();
							
							
							paramsTable.push_back(parmValue);
						}
							break;
						
						case ' ': 
							{
								advance();
								
								
								if( isAtTheEndOfATag() )
									throw BadlyFormatedTemplateString();
								
								returnValue->arguments = new MapFunctArg();
								std::map<std::string,std::string>& paramsMap = 
									static_cast<MapFunctArg*>(returnValue->arguments)->paramsMap;
								
								std::string paramName, paramValue;
								while( !isAtTheEndOfATag() ) {
									
									if( viewed() != '=') {
										paramName += viewed();
										advance();
									} else {
										
										if( paramName.empty() )
											throw BadlyFormatedTemplateString();
										advance();
										paramValue = parseParam();
										if(viewed() == ' ') advance();
										paramsMap.insert_or_assign(paramName,paramValue);
										paramName = "";
										paramValue = "";
									}
								}
							}
							
							break;
						default: 
							if( !isAtTheEndOfATag() )
								throw BadlyFormatedTemplateString();
					}
				}catch(BadlyFormatedTemplateString e) {
					delete returnValue;
					throw e;
				}
				
				return returnValue;
			}
			
			std::string parseParam() {
				std::string parmValue;
				bool brackedInInnerTag = false;
				
				if( isAtTheStartOfInnerTag() ) {
					brackedInInnerTag = true;
					advance( INNER_TAG_START.length() );
				}
				
				while( !isAtTheEndOfATag() ) {
					
					
					if( !brackedInInnerTag && viewed() == ' ') break;
					if( brackedInInnerTag && isAtTheEndOfInnerTag() ) {
						advance( INNER_TAG_END.length() );
						return parmValue;
					}
					parmValue += viewed();
					advance();
				}
				
				
				if( brackedInInnerTag )
					throw BadlyFormatedTemplateString();
				
				if( parmValue.empty() && !brackedInInnerTag )
					throw BadlyFormatedTemplateString();
				
				return parmValue;
			}
			
			bool isAtTheStartOfInnerTag() {
				#ifdef CAN_USE_STARTS_ENDS_WITH
				return toBeParsedAsSV().starts_with(INNER_TAG_START);
				#else
				std::size_t pos = str.find(INNER_TAG_START,cPos);
				if( pos == std::string::npos ) return false;
				return pos == cPos;
				#endif
			}
			bool isAtTheEndOfInnerTag() {
				#ifdef CAN_USE_STARTS_ENDS_WITH
				return toBeParsedAsSV().starts_with(INNER_TAG_END);
				#else
				std::size_t pos = str.find(INNER_TAG_END,cPos);
				if( pos == std::string::npos ) return false;
				return pos == cPos;
				#endif
			}
	};
	
	
	TemplateElementsList basicParseString(std::string templateString)
	{
		TemplateElementsList outputList;
		
		
		if( auto start = templateString.find(TAG_START); start == std::string::npos  )
		{
			
			outputList.emplace_back(new StringElement(templateString));
		}
		else
		{
			
			
			
			if( start > 0 ) {
				outputList.emplace_back(new StringElement(templateString.substr(0,start)));
			}
			
			BasicTagParser parser(templateString,start);
			while( parser.hasNext() ) {
				if( auto nextElement = parser.getNext(); nextElement != nullptr)
					outputList.emplace_back( nextElement );
				if( parser.hasText() ) {
					outputList.emplace_back( new StringElement(parser.getText()) );
				}
			}
			
			if(std::size_t lastPos = parser.getLastPosition(); lastPos != std::string::npos) {
				std::string suffix = templateString.substr(lastPos);
				if( !suffix.empty() ) {
					outputList.emplace_back(new StringElement(suffix));
				}
			}
		}
		
		return outputList;
	}
};

