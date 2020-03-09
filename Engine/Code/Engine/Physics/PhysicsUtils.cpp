#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

float CalculateMomentOfInertiaOfTriangle( const Vec2& vertex0 , const Vec2& vertex1 , const Vec2& vertex2 , const float mass )
{
	// TODO :- OPTIMIZE IF FEELS SLOW a can be integrated in h
	
	Vec2 u = vertex1 - vertex0;
	Vec2 v = vertex2 - vertex0;

	float b = u.GetLength();																				// base DotProduct( u , u ) = GetLengthSquared
	float a = DotProduct2D( v , u ) / b;
	float h = ( v - ( ( DotProduct2D( v , u ) / u.GetLengthSquared() ) * u ) ).GetLength();		// height =   |v (V.u) . u  |
																											//			  |   (u.u)	    |

	
	return mass * ( 1 / 18.f ) * ( ( b * b ) - ( a * b ) + ( a * a ) + ( h * h ) );
}
