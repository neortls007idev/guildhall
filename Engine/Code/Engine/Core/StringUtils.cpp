#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <windows.h>

//-----------------------------------------------------------------------------------------------

const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;

//-----------------------------------------------------------------------------------------------

const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}

//-----------------------------------------------------------------------------------------------

const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings SplitStringAtGivenDelimiter( const std::string& originalString , const char delimiter )
{
	Strings splitStrings;
	size_t subStringStartIndex = 0;

	for ( ;; )
	{
		size_t splitStringEndIndex = originalString.find( delimiter , subStringStartIndex );
		size_t splitLength = splitStringEndIndex - subStringStartIndex;
		std::string substring( originalString , subStringStartIndex , splitLength );
		splitStrings.push_back( substring );
		if ( splitStringEndIndex == std::string::npos )
		{
			break;
		}
		subStringStartIndex = splitStringEndIndex + 1;
	}
	
	return splitStrings;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Strings SplitStringOnceAtGivenDelimiter( const std::string& originalString , const char delimiter /*= ',' */ )
{
	Strings splitStrings;
	size_t subStringStartIndex = 0;

	for ( int count = 0; count <= 1; count++ )
	{
		size_t splitStringEndIndex = originalString.size();
		
		if ( splitStrings.size() == 0 )
		{
			splitStringEndIndex = originalString.find( delimiter , subStringStartIndex );
		}
		
		size_t splitLength = splitStringEndIndex - subStringStartIndex;
		std::string substring( originalString , subStringStartIndex , splitLength );
		splitStrings.push_back( substring );
		if ( splitStringEndIndex == std::string::npos )
		{
			break;
		}
		subStringStartIndex = splitStringEndIndex + 1;
		
	}

	return splitStrings;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string ReverseString( const std::string& originalString )
{
	std::string reverseString;

	if ( originalString.length() == 0 )
	{
		return originalString;
	}
	
	for ( size_t index = originalString.length() - 1 ; index > 0 ; index-- )
	{
		reverseString.push_back( originalString[ index ] );
	}
	
	return reverseString;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool StringCompare( const char* firstString , std::string secondString )
{
	const char* secondStringPtr = secondString.c_str();

	for ( size_t index = 0; ( index < secondString.length() ) || firstString[ index ]; index++ )
	{
		if ( firstString[ index ] != secondStringPtr[ index ] )
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool StringCompare( std::string firstString , std::string secondString )
{
	const char* firstStringPtr = firstString.c_str();
	const char* secondStringPtr = secondString.c_str();

	if ( firstString.length() != secondString.length() )
	{
		return false;
	}

	for ( size_t index = 0; ( index < firstString.length() ) || ( index < secondString.length() ); index++ )
	{
		if ( firstStringPtr[ index ] != secondStringPtr[ index ] )
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool StringCompare( const char* firstString , const char* secondString )
{
	for ( size_t index = 0; firstString[ index ] || secondString[ index ]; index++ )
	{
		if ( firstString[ index ] != secondString[ index ] )
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string GetClipboardDataAsText()
{
	if ( !OpenClipboard( NULL ) )
		return "";

	if ( IsClipboardFormatAvailable( CF_TEXT ) )
	{
		HANDLE handleData = GetClipboardData( CF_TEXT );
		LPCSTR data = ( LPCSTR ) GlobalLock( handleData );
		std::string clipboardDataString = data;
		GlobalUnlock( handleData );
		CloseClipboard();
		return clipboardDataString;
	}
	else
	{
		CloseClipboard();
		return "";
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void SetClipboardDataAsText( std::string clipboardStringToSet )
{
	const char* clipboardInput = clipboardStringToSet.c_str();
	const size_t len = clipboardStringToSet.length() + 1;/*strlen( clipboardInput ) + 1;*/
	HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE , len );
	memcpy( GlobalLock( hMem ) , clipboardInput , len );
	GlobalUnlock( hMem );
	OpenClipboard( 0 );
	EmptyClipboard();
	SetClipboardData( CF_TEXT , hMem );
	CloseClipboard();
}

//--------------------------------------------------------------------------------------------------------------------------------------------