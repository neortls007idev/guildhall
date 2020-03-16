#pragma once
#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Polygon2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

float CalculateMomentOfInertiaOfTriangle( const Polygon2D& polygon , const Vec2& vertex0 , const Vec2& vertex1 , const Vec2& vertex2 , const float mass );
