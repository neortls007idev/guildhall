#include "Engine/Core/VertexUtils.hpp "
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/Polygon2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void TransformVertexArray2D(int numberOfVertices, Vertex_PCU* verts, float scale, float rotation, Vec2 translation)
{
	for ( int vertexIndex = 0; vertexIndex < numberOfVertices; vertexIndex++)
	{
		verts[vertexIndex].m_position = TransformPosition3DXY(verts[vertexIndex].m_position, scale, rotation, translation);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendLine2( std::vector<Vertex_PCU>& vertexArray , const Vec2& start , const Vec2& end , const Rgba8 color , const float thickness )
{
	Vec2 displacement = end - start;
	Vec2 forward = displacement.GetNormalized();
	forward.SetLength( thickness / 2.f );
	Vec2 left = forward.GetRotated90Degrees();

	Vec2 endLeft = end + forward + left;
	Vec2 endRight = end + forward - left;
	Vec2 startLeft = start - forward + left;
	Vec2 startRight = start - forward - left;

	Vec3 endLeftVec3( endLeft.x , endLeft.y , 0.f );
	Vec3 endRightVec3( endRight.x , endRight.y , 0.f );
	Vec3 startLeftVec3( startLeft.x , startLeft.y , 0.f );
	Vec3 startRightVec3( startRight.x , startRight.y , 0.f );

	const Vertex_PCU lineVerts[ 6 ] = { Vertex_PCU( startRightVec3, color, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( endRightVec3  , color, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( endLeftVec3   , color, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( endLeftVec3   , color, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( startLeftVec3 , color, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( startRightVec3, color, Vec2( 0.f, 0.f ) ) };
	
	for ( int index = 0; index < 6; index++ )
	{
		vertexArray.push_back( lineVerts[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendVertsForAABB2( std::vector<Vertex_PCU>& vertexArray , const AABB2& box , const Rgba8& tint )
{
	const Vertex_PCU boxVerts[ 6 ] = {
								Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,0.f ) , tint, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ),

								Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,0.f ) , tint, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ) };

	for ( int index = 0; index < 6; index++ )
	{
		vertexArray.push_back( boxVerts[index] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendVertsForAABB2( std::vector<Vertex_PCU>& vertexArray , const AABB2& box , const Rgba8& tint , const Vec2& uvAtMins , const Vec2& uvAtMaxs )
{
	const Vertex_PCU boxVerts[ 6 ] = {
							Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,0.f ) , tint, uvAtMins ),
							Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( uvAtMaxs.x, uvAtMins.y ) ),
							Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( uvAtMins.x, uvAtMaxs.y ) ),

							Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( uvAtMaxs.x, uvAtMins.y ) ),
							Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,0.f ) , tint, uvAtMaxs ),
							Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( uvAtMins.x, uvAtMaxs.y ) ) };

	for ( int index = 0; index < 6; index++ )
	{
		vertexArray.push_back( boxVerts[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendVertsForPolygon( std::vector<Vertex_PCU>& vertexArray , const Vec2* points , const unsigned int count , const Rgba8& tint )
{
	const Vertex_PCU tempVert1 = Vertex_PCU( Vec3( points->x , points->y , 0.f ) , tint , Vec2::ZERO );

	for ( unsigned int pointIndex = 2; pointIndex < count; pointIndex++ )
	{
		vertexArray.push_back( tempVert1 );
		points = points++;
		const Vertex_PCU tempVert2 = Vertex_PCU( Vec3( points->x , points->y , 0.f ) , tint , Vec2::ZERO );
		vertexArray.push_back( tempVert2 );
		points = points++;
		const Vertex_PCU tempVert3 = Vertex_PCU( Vec3( points->x , points->y , 0.f ) , tint , Vec2::ZERO );
		vertexArray.push_back( tempVert3 );
		points = points--;
	}
}

void RotateDegreesPolygonAboutPoint( Polygon2D& polygon , Vec2 worldPosition , float orientationDegrees )
{
	if ( polygon.m_points.size() < 3 )
	{
		return;
	}
	
	for ( size_t index = 0; index < polygon.m_points.size(); index++ )
	{
		polygon.m_points[ index ] = TransformPosition3DXY( Vec3( polygon.m_points[ index ] , 0.f ) , 1.f , 0.f , -worldPosition ).GetXYComponents();
	}
	for ( size_t index = 0; index < polygon.m_points.size(); index++ )
	{
		polygon.m_points[ index ] = TransformPosition3DXY( Vec3( polygon.m_points[ index ] , 0.f ) , 1.f , orientationDegrees , worldPosition ).GetXYComponents();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AddCubeVerts( std::vector<Vertex_PCU>& cubeVertexArray , const Rgba8* tint )
{
		Vertex_PCU CubeVerts[ 24 ] = {

		// FRONT FACE VERTS
						Vertex_PCU( Vec3( -1.f,-1.f,1.f ) , WHITE, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,-1.f,1.f ) , WHITE, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( 1.f,1.f,1.f ) , WHITE, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( -1.f,1.f,1.f ) , WHITE, Vec2( 1.f, 0.f ) ),
		// BACK FACE VERTS
						Vertex_PCU( Vec3( -1.f,-1.f,-1.f ) , GREEN, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,-1.f,-1.f ) , GREEN, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( 1.f,1.f,-1.f )  , GREEN, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( -1.f,1.f,-1.f ) , GREEN, Vec2( 1.f, 0.f ) ),
		// RIGHT FACE VERTS
						Vertex_PCU( Vec3( 1.f,-1.f,1.f ) ,BLUE, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f,-1.f,-1.f ) ,BLUE, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( 1.f,1.f,-1.f ) , BLUE, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( 1.f,1.f,1.f ) ,BLUE, Vec2( 1.f, 0.f ) ),
		// LEFT FACE VERTS
						Vertex_PCU( Vec3( -1.f,-1.f,1.f ) ,CYAN, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( -1.f,-1.f,-1.f ) ,CYAN, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( -1.f,1.f,-1.f ) , CYAN, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( -1.f,1.f,1.f ) ,CYAN, Vec2( 1.f, 0.f ) ),
		// TOP FACE VERTS
						Vertex_PCU( Vec3( -1.f, 1.f, 1.f ) ,RED, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3(  1.f, 1.f, 1.f ) ,RED, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3(  1.f,1.f,-1.f ) , RED, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( -1.f,1.f, -1.f ) ,RED, Vec2( 1.f, 0.f ) ),
		// BOTTOM FACE VERTS
						Vertex_PCU( Vec3( -1.f, -1.f, 1.f ) ,YELLOW, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( 1.f, -1.f, 1.f ) ,YELLOW, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( 1.f,-1.f,-1.f ) , YELLOW, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( -1.f,-1.f, -1.f ) ,YELLOW, Vec2( 1.f, 0.f ) ),

						};
		if ( !tint )
		{
			for ( int index = 0; index < 24; index++ )
			{
				cubeVertexArray.push_back( CubeVerts[ index ] );
			}
		}
		else
		{
			for ( int index = 0; index < 24; index++ )
			{
				cubeVertexArray.push_back( CubeVerts[ index ] );
				cubeVertexArray[ index ].m_color = *tint;
			}
		}		
}

uint* GetCubeIndices()
{
	static uint CubeIndices[ 36 ] = {
		// FRONT FACE INDICES
							0,1,2,
							2,3,0,
		// BACK FACE INDICES
							4,5,6,
							6,7,4,
		// RIGHT FACE INDICES
							8,9,10,
							10,11,8,
		// LEFT FACE INDICES
							12,13,14,
							14,15,12,
		// TOP FACE INDICES
							16,17,18,
							18,19,16,
		// TOP FACE INDICES
							20,21,22,
							22,23,20,
							};
	return &CubeIndices[ 0 ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateUVSphere( uint hCuts , uint vCuts , std::vector<Vertex_PCU>& sphereMeshVerts , std::vector<uint>& sphereIndices, float radius , Vec3 center , const Rgba8& tint )
{
	float deltaTheta = 360.f / ( float ) hCuts;
	float deltaPhi = 180.f / ( float ) vCuts;

	for ( float theta = 0.f; theta <= 360.0; theta += deltaTheta )
	{
		for ( float phi = -90.f; phi <= 90.0; phi += deltaPhi )
		{
			Vec3 currentCoords = Vec3::MakeFromSpericalCoordinates( theta , phi , radius );
			float u = RangeMapFloat( 0.f , 360.f , 0.f , 1.f , theta );
			float v = RangeMapFloat( -90.f , 90.f , 0.f , 1.f , phi );

			Vertex_PCU currentCoordVerts = Vertex_PCU( center + currentCoords , tint , Vec2( u , v ) );

			sphereMeshVerts.push_back( currentCoordVerts );
		}
	}

	for ( uint hcutIndex = 0; hcutIndex < hCuts; hcutIndex++ )
	{
		for ( uint vCutIndex = 0; vCutIndex < vCuts; vCutIndex++ )
		{
			uint index0 = hcutIndex + ( ( hCuts + 1 ) * vCutIndex );
			uint index1 = index0 + 1;
			uint index2 = index0 + hCuts + 1;
			uint index3 = index2 + 1;

			sphereIndices.push_back( index0 );
			sphereIndices.push_back( index1 );
			sphereIndices.push_back( index3 );

			sphereIndices.push_back( index0 );
			sphereIndices.push_back( index3 );
			sphereIndices.push_back( index2 );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendVertsForAABB3( std::vector<Vertex_PCU>& vertexArray , const AABB3& box , const Rgba8& tint )
{
	// TODO :- FIX ME
	UNUSED( box );
	UNUSED( tint );
	UNUSED( vertexArray );
	__debugbreak();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendVertsForAABB3( std::vector<Vertex_PCU>& vertexArray , const AABB3& box , const Rgba8& tint , const Vec2& uvAtMins , const Vec2& uvAtMaxs )
{
	// TODO :- FIX ME

	const Vertex_PCU boxVerts[ 6 ] = {
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y, 0.f ) , tint, uvAtMins ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y, 0.f ) , tint, Vec2( uvAtMaxs.x, uvAtMins.y ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y, 0.f ) , tint, Vec2( uvAtMins.x, uvAtMaxs.y ) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( uvAtMaxs.x, uvAtMins.y ) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,0.f ) , tint, uvAtMaxs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( uvAtMins.x, uvAtMaxs.y ) ) };

	for ( int index = 0; index < 6; index++ )
	{
		vertexArray.push_back( boxVerts[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

