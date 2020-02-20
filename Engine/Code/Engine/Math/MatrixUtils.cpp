#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4x4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// const Mat44 Mat44::CreateOrthoGraphicProjeciton( const Vec3& min , const Vec3& max )
// {
// 
// }

//--------------------------------------------------------------------------------------------------------------------------------------------

const Mat44 CreateOrthoGraphicProjeciton( const Vec3& min , const Vec3& max )
{
	// think of x

// min.x , max.x -> ( -1 , 1 )
//ndc.x = ( x - min.x ) / ( max.x - min.x ) * ( 1.0f - ( -1.0f ) ) + ( -1 )
//ndc.x = x / ( max.x - min.x ) - ( min.x / ( max.x - min.x ) ) * 2.0f + -1.0f;
// a = 1.0f / ( max.x - min.x )
// b = ( -2.0f * min.x - max.x + min.x ) / ( max.x - min.x )
//	 = -( max.x + min.x ) / ( max.x - min.x )

// min.z , max.z -> (0,1)
//ndc.x = ( x - min.x ) / ( max.x - min.x ) * ( 1.0f - ( -0.0f ) ) + ( -0.0f )
//ndc.x = x / ( max.x - min.x ) - ( min.x / ( max.x - min.x ) ) * 1.0f;
// a = 1.0f / ( max.x - min.x )
// b = -min.x / ( max.x - min.x )

	Vec3 diff = max - min;
	Vec3 sum = max + min;

	float mat[] = {
					 2.0f / diff.x,			 0.0f,					 0.0f,					0.0f,
					 0.0f,					 2.0f / diff.y,			 0.0f,					0.0f,
					 0.0f,					 0.0f,					 1.0f / diff.z,			0.0f,
					-sum.x / diff.x,		-sum.y / diff.y,		-min.z / diff.z,		1.0f
	};

	return Mat44( mat );
}

//--------------------------------------------------------------------------------------------------------------------------------------------