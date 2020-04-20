#pragma once
#include "Engine/Experimental/Raytracer/Ray.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RaytracerCamera
{
public:
	RaytracerCamera();

	Ray GetRay( float u , float v );
	
public:
	Vec3 origin;
	Vec3 lowerLeftCorner;
	Vec3 horizontal;
	Vec3 vertical;
};

//--------------------------------------------------------------------------------------------------------------------------------------------