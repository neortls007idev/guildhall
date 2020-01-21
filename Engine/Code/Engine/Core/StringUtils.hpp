#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

Strings SplitStringAtGivenDelimiter( const std::string& originalString , const char delimiter = ',' );

int StringCompare( const char* , std::string ); //TODO - CREATE ALL VERSIONS