#pragma once
#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RandomNumberGenerator
{
public:
	int   RollRandomIntLessThan( int maxNotInclusive );
	int   RollRandomIntLessThanInclusive( int maxInclusive );
	int   RollRandomIntInRange( int minInclusive, int maxInclusive );
	int	  RollRandomWithinTwoDistinctRanges(int minRangeOne, int maxRangeOne, int minRangeTwo, int msxRangeTwo);
	float RollRandomFloatBetweenZerotoOne();
	float RollRandomFloatBetweenZerotoAlmostOne();
	float RollRandomFloatLessThan( float maxNotInclusive );
	float RollRandomFloatLessThaninclusive( float maxInclusive );
	float RollRandomFloatInRange( float minInclusive, float maxInclusive );
	bool  RollPercentChance( float probablityOfReturningTrue );
	Vec2  RollRandomDirection2D();

	void Reset( unsigned int seed = 0 );
	void manuallyIncrementPosition();

private:
	unsigned int m_seed = 0;
    int			 m_position = 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------