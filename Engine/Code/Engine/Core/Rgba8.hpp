#pragma once
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Timer;
typedef unsigned char uchar;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Rgba8
{

public:
	Rgba8();
	//Rgba8( Rgba8 &copy );
	~Rgba8() = default;
	constexpr explicit Rgba8( uchar r, uchar g, uchar b, uchar a=255 );
	const Rgba8 SetFromText( const char* text );
	const void	CopyFromRGBA8( Rgba8 &copy );
	const Rgba8 RollRandomColor( RandomNumberGenerator rng );
	void ScaleAlpha( float multipler );

	bool IsRedChannelEqual( const Rgba8& compareWith );
	bool IsGreenChannelEqual( const Rgba8& compareWith );
	bool IsBlueChannelEqual( const Rgba8& compareWith );
	bool IsAlphaChannelEqual( const Rgba8& compareWith );
	void		operator=( const Rgba8& copyFrom );				
	bool		operator==( const Rgba8& compareWith );
	bool		operator!=( const Rgba8& compareWith );
	void		operator*=( const float multiplier );
	void		operator*=( const Rgba8& multipleWith );
	void		operator+=( const float addition );
	void		operator+=( const Rgba8& addWith );
		
	void CycleRGBChannelsClockwise();
	void CycleRGBChannelsAntiClockwise();

	void LerpColorOverTime( Rgba8 startColor , Rgba8 endColor , float lerpDuration , Timer* timer );
	Vec4 GetAsNormalizedFloat4() const;

	void SetColorFromNormalizedFloat( Vec4 color );
	
public:
	uchar r = 255;
	uchar g = 255;
	uchar b = 255;
	uchar a = 255;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr Rgba8::Rgba8( uchar r , uchar g , uchar b , uchar a )
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// RGBA8 Colors
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr Rgba8 RED		= Rgba8( 255 , 0   , 0   , 255 );
constexpr Rgba8 GREEN	= Rgba8( 0   , 255 , 0   , 255 );
constexpr Rgba8 BLUE	= Rgba8( 0   , 0   , 255 , 255 );
constexpr Rgba8 WHITE	= Rgba8( 255 , 255 , 255 , 255 );
constexpr Rgba8 BLACK	= Rgba8( 0   , 0   , 0   , 255 );
constexpr Rgba8 MAGENTA = Rgba8( 255 , 0   , 255 , 255 );
constexpr Rgba8 YELLOW	= Rgba8( 255 , 255 , 0   , 255 );
constexpr Rgba8 CYAN	= Rgba8( 0	 , 255 , 255 , 255 );
constexpr Rgba8 ORANGE  = Rgba8( 255 , 127 , 0   , 255 );
constexpr Rgba8 PINK	= Rgba8( 255 , 0   , 127 , 255 );
constexpr Rgba8 PURPLE  = Rgba8( 127 , 0   , 255 , 255 );
constexpr Rgba8 GRAY	= Rgba8( 127 , 127 , 127 , 255 );

//--------------------------------------------------------------------------------------------------------------------------------------------
