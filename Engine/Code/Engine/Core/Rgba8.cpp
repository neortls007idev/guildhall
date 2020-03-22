#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <cmath>

//--------------------------------------------------------------------------------------------------------------------------------------------

Rgba8::Rgba8()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rgba8::ScaleAlpha( float multipler )
{
	a = static_cast<unsigned char>( multipler * a );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Rgba8::IsRedChannelEqual( const Rgba8& compareWith )
{
	if ( r == compareWith.r )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Rgba8::IsGreenChannelEqual( const Rgba8& compareWith )
{
	if ( g == compareWith.g )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Rgba8::IsBlueChannelEqual( const Rgba8& compareWith )
{
	if ( b == compareWith.b )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Rgba8::IsAlphaChannelEqual( const Rgba8& compareWith )
{
	if ( a == compareWith.a )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rgba8::CycleRGBChannelsClockwise()
{
	Rgba8 tempCopy = *this;

	r = tempCopy.g;
	g = tempCopy.b;
	b = tempCopy.r;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rgba8::CycleRGBChannelsAntiClockwise()
{
	Rgba8 tempCopy = *this;

	r = tempCopy.b;
	g = tempCopy.r;
	b = tempCopy.g;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rgba8::LerpColor( Rgba8 startColor , Rgba8 endColor , float lerpDuration )
{
	if ( lerpDuration <= 0.f )
	{
		r = endColor.r;
		g = endColor.g;
		b = endColor.b;
		a = endColor.a;
		return;
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Rgba8::operator==( const Rgba8& compareWith )
{
	if ( r == compareWith.r && g == compareWith.g && b == compareWith.b && a == compareWith.a )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rgba8::operator=( const Rgba8& copyFrom )
{
	r = copyFrom.r;
	g = copyFrom.g;
	b = copyFrom.b;
	a = copyFrom.a;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Rgba8 Rgba8::SetFromText( const char* text )
{
	Strings rgba8FromSplitText = SplitStringAtGivenDelimiter( text , ',' );
	Rgba8 rgba8FromText;

	if ( rgba8FromSplitText.size() == 3 )
	{
		rgba8FromText.r = static_cast< uchar >( atoi( rgba8FromSplitText[ 0 ].c_str() ) );
		rgba8FromText.g = static_cast< uchar >( atoi( rgba8FromSplitText[ 1 ].c_str() ) );
		rgba8FromText.b = static_cast< uchar >( atoi( rgba8FromSplitText[ 2 ].c_str() ) );
	}
	else if ( rgba8FromSplitText.size() == 4 )
	{
		rgba8FromText.r = static_cast< uchar >( atoi( rgba8FromSplitText[ 0 ].c_str() ) );
		rgba8FromText.g = static_cast< uchar >( atoi( rgba8FromSplitText[ 1 ].c_str() ) );
		rgba8FromText.b = static_cast< uchar >( atoi( rgba8FromSplitText[ 2 ].c_str() ) );
		rgba8FromText.a = static_cast< uchar >( atoi( rgba8FromSplitText[ 3 ].c_str() ) );
	}
	else
	{
		ERROR_RECOVERABLE( "NOT ENOUGH RGBA8 COMPONENTS" );
	}

	return rgba8FromText;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const void Rgba8::CopyFromRGBA8( Rgba8& copy )
{
	r = copy.r;
	g = copy.g;
	b = copy.b;
	a = copy.a;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const Rgba8 Rgba8::RollRandomColor( RandomNumberGenerator rng )
{
// TODO:-  Needs more Testing seems like will always return a whitish color

	r = ( uchar ) rng.RollRandomIntLessThan( 256 );
	g = ( uchar ) rng.RollRandomIntLessThan( 256 );
	b = ( uchar ) rng.RollRandomIntLessThan( 256 );
	a = ( uchar ) rng.RollRandomIntLessThan( 256 );

	return *this;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
