#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Plane.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class FrustumCulling
{
public:
	FrustumCulling();
	FrustumCulling( const FrustumCulling& );
	~FrustumCulling();

	void ConstructFrustum( float , Mat44 , Mat44 );

	bool CheckPoint( float , float , float );
	bool CheckCube( float , float , float , float );
	bool CheckSphere( float , float , float , float );
	bool CheckRectangle( float , float , float , float , float , float );

private:
	Plane m_planes[ 6 ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------