#include "Engine/Experimental/Raytracer/RaytracerCamera.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

RaytracerCamera::RaytracerCamera()
{
	lowerLeftCorner		= Vec3( -2.0 , -1.0 , -1.0 );
	horizontal			= Vec3( 4.0 , 0.0 , 0.0 );
	vertical			= Vec3( 0.0 , 2.0 , 0.0 );
	origin				= Vec3::ZERO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Ray RaytracerCamera::GetRay( float u , float v )
{
	return Ray( origin , lowerLeftCorner + u * horizontal + v * vertical - origin );
}

//--------------------------------------------------------------------------------------------------------------------------------------------