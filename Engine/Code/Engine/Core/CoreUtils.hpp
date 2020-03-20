#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eWindingOrder
{
	COUNTERCLOCKWISE ,
	CLOCKWISE ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

// By default returns CounterClockWise as Winding Order
bool GetWindingOrder( const eWindingOrder windingOrder );