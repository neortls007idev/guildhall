#include "Engine/Primitives/Frustum.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Frustum::Frustum()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Frustum::Frustum( const Frustum& frustum )
{
	m_planes[ 0 ] = frustum.m_planes[ 0 ];
	m_planes[ 1 ] = frustum.m_planes[ 1 ];
	m_planes[ 2 ] = frustum.m_planes[ 2 ];
	m_planes[ 3 ] = frustum.m_planes[ 3 ];
	m_planes[ 4 ] = frustum.m_planes[ 4 ];
	m_planes[ 5 ] = frustum.m_planes[ 5 ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Frustum::~Frustum()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Frustum::ConstructFrustum( float screenDepth , Mat44 projectionMatrix , Mat44 viewMatrix )
{
	float zMinimum;
	float depthRatio;
	Mat44 matrix;

	// Calculate the minimum Z distance in the frustum.
//	zMinimum = -projectionMatrix.Kw / projectionMatrix.Kz;
	//zMinimum = -projectionMatrix.Tz / projectionMatrix.Kz;
	//zMinimum = -projectionMatrix._43 / projectionMatrix._33;
//	depthRatio = screenDepth / ( screenDepth - zMinimum );
	//depthRatio = 1.f / screenDepth;
//	projectionMatrix.Kz = depthRatio;
//	projectionMatrix.Kw = -depthRatio * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
//	matrix = viewMatrix;
	matrix = projectionMatrix;
	matrix.TransformBy( viewMatrix );
//	matrix = viewMatrix;
//	matrix.TransformBy( projectionMatrix );
	/*
	// Calculate near plane of frustum.
	m_planes[ 0 ].a = matrix.Tx + matrix.Kx;
	m_planes[ 0 ].b = matrix.Ty + matrix.Ky;
	m_planes[ 0 ].c = matrix.Tz + matrix.Kz;
	m_planes[ 0 ].d = matrix.Tw + matrix.Kw;
	
	m_planes[ 0 ] = m_planes[ 0 ].GetNormalizedPlane();

	// Calculate far plane of frustum.
	m_planes[ 1 ].a = matrix.Tx - matrix.Kx;
	m_planes[ 1 ].b = matrix.Ty - matrix.Ky;
	m_planes[ 1 ].c = matrix.Tz - matrix.Kz;
	m_planes[ 1 ].d = matrix.Tw - matrix.Kw;

	m_planes[ 1 ] = m_planes[ 1 ].GetNormalizedPlane();
	
	// Calculate left plane of frustum.
	m_planes[ 2 ].a = matrix.Tx + matrix.Ix;
	m_planes[ 2 ].b = matrix.Ty + matrix.Iy;
	m_planes[ 2 ].c = matrix.Tz + matrix.Iz;
	m_planes[ 2 ].d = matrix.Tw + matrix.Iw;
	m_planes[ 2 ] = m_planes[ 2 ].GetNormalizedPlane();
	
	// Calculate right plane of frustum.
	m_planes[ 3 ].a = matrix.Tx - matrix.Ix;
	m_planes[ 3 ].b = matrix.Ty - matrix.Iy;
	m_planes[ 3 ].c = matrix.Tz - matrix.Iz;
	m_planes[ 3 ].d = matrix.Tw - matrix.Iw;
	m_planes[ 3 ] = m_planes[ 3 ].GetNormalizedPlane();
	
	// Calculate top plane of frustum.
	m_planes[ 4 ].a = matrix.Tx - matrix.Jx;
	m_planes[ 4 ].b = matrix.Ty - matrix.Jy;
	m_planes[ 4 ].c = matrix.Tz - matrix.Jz;
	m_planes[ 4 ].d = matrix.Tw - matrix.Jw;
	m_planes[ 4 ] = m_planes[ 4 ].GetNormalizedPlane();
	
	// Calculate bottom plane of frustum.
	m_planes[ 5 ].a = matrix.Tx + matrix.Jx;
	m_planes[ 5 ].b = matrix.Ty + matrix.Jy;
	m_planes[ 5 ].c = matrix.Tz + matrix.Jz;
	m_planes[ 5 ].d = matrix.Tw + matrix.Jw;
	m_planes[ 5 ] = m_planes[ 5 ].GetNormalizedPlane();
	*/
	m_planes[ 0 ].a = matrix.Iw + matrix.Iz;
	m_planes[ 0 ].b = matrix.Jw + matrix.Jz;
	m_planes[ 0 ].c = matrix.Kw + matrix.Kz;
	m_planes[ 0 ].d = matrix.Tw + matrix.Tz;

	m_planes[ 0 ] = m_planes[ 0 ].GetNormalizedPlane();

	// Calculate far plane of frustum.
	m_planes[ 1 ].a = matrix.Iw - matrix.Iz;
	m_planes[ 1 ].b = matrix.Jw - matrix.Jz;
	m_planes[ 1 ].c = matrix.Kw - matrix.Kz;
	m_planes[ 1 ].d = matrix.Tw - matrix.Tz;

	m_planes[ 1 ] = m_planes[ 1 ].GetNormalizedPlane();

	// Calculate left plane of frustum.
	m_planes[ 2 ].a = matrix.Iw - matrix.Ix;
	m_planes[ 2 ].b = matrix.Jw - matrix.Jx;
	m_planes[ 2 ].c = matrix.Kw - matrix.Kx;
	m_planes[ 2 ].d = matrix.Tw - matrix.Tx;
	m_planes[ 2 ] = m_planes[ 2 ].GetNormalizedPlane();

	// Calculate right plane of frustum.
	m_planes[ 3 ].a = matrix.Iw + matrix.Ix;
	m_planes[ 3 ].b = matrix.Jw + matrix.Jx;
	m_planes[ 3 ].c = matrix.Kw + matrix.Kx;
	m_planes[ 3 ].d = matrix.Tw + matrix.Tx;
	m_planes[ 3 ] = m_planes[ 3 ].GetNormalizedPlane();

	// Calculate top plane of frustum.
	m_planes[ 4 ].a = matrix.Iw - matrix.Iy;
	m_planes[ 4 ].b = matrix.Jw - matrix.Jy;
	m_planes[ 4 ].c = matrix.Kw - matrix.Ky;
	m_planes[ 4 ].d = matrix.Tw - matrix.Ty;
	m_planes[ 4 ] = m_planes[ 4 ].GetNormalizedPlane();

	// Calculate bottom plane of frustum.
	m_planes[ 5 ].a = matrix.Iw + matrix.Iy;
	m_planes[ 5 ].b = matrix.Jw + matrix.Jy;
	m_planes[ 5 ].c = matrix.Kw + matrix.Ky;
	m_planes[ 5 ].d = matrix.Tw + matrix.Ty;
	m_planes[ 5 ] = m_planes[ 5 ].GetNormalizedPlane();
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Frustum::IsPointInsideFrustum( const Vec3& point )
{
	for( int index = 0; index < 6; index++ )
	{
		if( Plane::DotCoord( m_planes[ index ] , point ) < 0.0f )
		{
			return false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Frustum::IsCubeInsideFrustum( const Vec3& center , const float& radius )
{
	for( int index = 0; index < 6; index++ )
	{
		if( !DoPlaneAndCubeOverlap( m_planes[ index ] , center , radius ) )
		{
			return false;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Frustum::IsSphereInsideFrustum( const Vec3& center , const float& radius )
{
	for( int index = 0; index < 6; index++ )
	{
		float res = Plane::DotCoord( m_planes[ index ] , center );
		float res2 = m_planes[ index ].SignedDistance( center );
		//if( ( Plane::DotCoord( m_planes[ index ] , center ) ) <= -radius )
		if( m_planes[ index ].SignedDistance( center ) <= -radius )
		{
			return false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------