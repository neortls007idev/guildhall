#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength , const char* format , ... );

Strings SplitStringAtGivenDelimiter( const std::string& originalString , const char delimiter = ',' );
Strings SplitStringOnceAtGivenDelimiter( const std::string& originalString , const char delimiter = ',' );

std::string ReverseString( const std::string& originalString );

bool StringCompare( const char* firstString , std::string  secondString ); 
bool StringCompare( std::string firstString , std::string  secondString ); 
bool StringCompare( const char* firstString , const char* secondString );