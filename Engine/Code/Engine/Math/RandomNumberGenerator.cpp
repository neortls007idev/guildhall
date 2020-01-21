#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"
#include <stdlib.h>

//--------------------------------------------------------------------------------------------------------------------------------------------
//	GLOBAL RANDOM NUMBER GENERATOR VARIABLE
//--------------------------------------------------------------------------------------------------------------------------------------------

RandomNumberGenerator* g_RNG = new RandomNumberGenerator();

//--------------------------------------------------------------------------------------------------------------------------------------------

int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	unsigned int randomBits = Get1dNoiseUint( m_position++ , m_seed );
	return  randomBits % maxNotInclusive;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int RandomNumberGenerator::RollRandomIntLessThanInclusive( int maxInclusive )
{
	return ( rand() %  ( maxInclusive + 1 ) ) ; 
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int range = maxInclusive - minInclusive + 1;
	return ( minInclusive + RollRandomIntLessThan( range ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int RandomNumberGenerator::RollRandomWithinTwoDistinctRanges(int minRangeOne, int maxRangeOne, int minRangeTwo, int maxRangeTwo)
{
	int randomOne = RollRandomIntInRange(minRangeOne, maxRangeOne);
	int randomTwo = RollRandomIntInRange(minRangeTwo, maxRangeTwo);
	int selectorBias = RollRandomIntInRange(1, 2);
	if (randomOne % selectorBias)
	{
		return randomOne;
	}
	else
		return randomTwo;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float RandomNumberGenerator::RollRandomFloatBetweenZerotoOne()
{
	constexpr double scale = 1.f / (double) 0xFFFFFFFF;
	unsigned int randomBits = Get1dNoiseUint( m_position++ , m_seed );
	return  ( float ) ( scale * ( double ) randomBits );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float RandomNumberGenerator::RollRandomFloatBetweenZerotoAlmostOne()
{
	constexpr double scale = 1.f / ( 1.0 + ( double ) 0xFFFFFFFF );
	unsigned int randomBits = Get1dNoiseUint( m_position++ , m_seed );
	return  ( float ) ( scale * ( double ) randomBits );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	// TODO : - REMOVE RANDMAX FOR THE UPDATED NOISE

	constexpr float scale = 1.f / (float) ( RAND_MAX + 1 ) ;
	float ZerotoAlmostOne = scale * (float)rand() ;
	return ( ZerotoAlmostOne * maxNotInclusive ) ; 
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float RandomNumberGenerator::RollRandomFloatLessThaninclusive( float maxInclusive )
{
	return ( RollRandomFloatBetweenZerotoOne() * maxInclusive );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float range = maxInclusive - minInclusive /*+ 1.0f*/ ;
	float finalFloatingRandom = ( float ) RollRandomFloatBetweenZerotoOne() * range;
	return ( minInclusive + finalFloatingRandom  ) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool RandomNumberGenerator::RollPercentChance( float probablityOfReturningTrue )
{
	float percentChance = RollRandomFloatBetweenZerotoOne();

	return percentChance <= probablityOfReturningTrue;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 RandomNumberGenerator::RollRandomDirection2D()
{
	return Vec2( RollRandomFloatBetweenZerotoOne() , RollRandomFloatBetweenZerotoOne() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RandomNumberGenerator::Reset( unsigned int seed /*= 0 */ )
{
	m_seed = seed;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RandomNumberGenerator::manuallyIncrementPosition()
{
	m_position++;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
