#include "Engine/Core/CoreUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GetWindingOrder( const eWindingOrder windingOrder )
{
	switch ( windingOrder )
	{
		case COUNTERCLOCKWISE:
								return true;
		case CLOCKWISE:
								return false;
		default:
								return true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------