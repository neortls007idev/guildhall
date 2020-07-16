#include "Game/MostAwesomeGameplayJob.hpp"

#include <cstdio>

//--------------------------------------------------------------------------------------------------------------------------------------------


MostAwesomeGameplayJob::MostAwesomeGameplayJob( int MostAwesomeJobID ) :
																		 Job()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

MostAwesomeGameplayJob::~MostAwesomeGameplayJob()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void MostAwesomeGameplayJob::Execute()
{
	int x = 5;
	for( size_t index = 0 ; index < 1000 ; index++ )
	{
		x++;
		x ^= 109876;
		x = x << 4;
		x &= 12768931;

		for ( size_t index2 = 1 ; index2 < 1000 ; index2++   )
		{
			x *= index;
			x /= index2;
		}
	}
	printf( "\nJob Completed = %d" , m_jobID );

	ClaimJobCallbackOnComplete();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
