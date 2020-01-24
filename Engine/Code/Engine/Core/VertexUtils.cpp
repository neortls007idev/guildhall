#include "Engine/Core/VertexUtils.hpp "
#include "Engine/Math/MathUtils.hpp"

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

//--------------------------------------------------------------------------------------------------------------------------------------------

