#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexMaster.hpp"
#include "Engine/Core/VertexUtils.hpp "
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
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

//--------------------------------------------------------------------------------------------------------------------------------------------

void RotateDegreesPolygonAboutPoint( Polygon2D& polygon , Vec2 worldPosition , float orientationDegrees )
{
	if ( polygon.m_points.size() < 3 )
	{
		return;
	}
	
	for ( size_t index = 0; index < polygon.m_points.size(); index++ )
	{
		polygon.m_points[ index ] = TransformPosition3DXY( Vec3( polygon.m_points[ index ] , 0.f ) , 1.f , 0.f ,
		                                                   -worldPosition ).GetXYComponents();
	}
	for ( size_t index = 0; index < polygon.m_points.size(); index++ )
	{
		polygon.m_points[ index ] = TransformPosition3DXY( Vec3( polygon.m_points[ index ] , 0.f ) , 1.f ,
		                                                   orientationDegrees , worldPosition ).GetXYComponents();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AppendIndexedVerts ( std::vector< Vertex_PCU >& sourceVerts , std::vector< uint >& sourceIndices ,
                          std::vector< Vertex_PCU >& destinationVerts , std::vector< uint >& destinationIndices )
{
	uint destinationVertsEnd = ( uint ) destinationVerts.size();
	
	for ( size_t index = 0 ; index < sourceIndices.size() ; index++ )
	{
		destinationIndices.push_back( destinationVertsEnd + sourceIndices[ index ] );
	}

	for ( size_t index = 0; index < sourceVerts.size(); index++ )
	{
		destinationVerts.push_back( sourceVerts[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TransformAndAppendIndexedVerts ( std::vector< Vertex_PCU >& destinationVerts ,
                                      std::vector< uint >& destinationIndices , std::vector< Vertex_PCU >& sourceVerts ,
                                      std::vector< uint >& sourceIndices , Mat44 transformation /*= Mat44::IDENTITY */ )
{
	// TODO :- IMPLEMENT ME
	UNUSED( destinationVerts );
	UNUSED( destinationIndices );
	UNUSED( sourceVerts );
	UNUSED( sourceIndices );
	UNUSED( transformation );
	
	DEBUGBREAK();
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
		// BOTTOM FACE INDICES
							20,21,22,
							22,23,20,
							};
	return &CubeIndices[ 0 ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateCuboid( std::vector< Vertex_PCU >& cubeMeshVerts , std::vector< uint >& cubeIndices , const AABB3 box , const Rgba8& tint /*= WHITE */ )
{
	Vertex_PCU CubeVerts[ 24 ] = {

		// FRONT FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_maxs.y ) , tint, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_maxs.y ) , tint, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),
		// BACK FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 0.f ) ),
		// RIGHT FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_maxs.y ) , tint, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),
		// LEFT FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_maxs.y ) , tint, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z,box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),
		// TOP FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x, box.m_maxs.z, box.m_maxs.y ) , tint, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( box.m_maxs.x, box.m_maxs.z, box.m_maxs.y ) , tint, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.z, box.m_mins.y ) ,tint, Vec2( 1.f, 0.f ) ),
		
		// BOTTOM FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x, box.m_mins.z, box.m_maxs.y ) ,tint, Vec2( 0.f, 0.f ) ),
						Vertex_PCU( Vec3( box.m_maxs.x, box.m_mins.z, box.m_maxs.y ) ,tint, Vec2( 1.f, 0.f ) ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.z,box.m_mins.y ) , tint, Vec2( 1.f, 1.f ) ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.z, box.m_mins.y ) ,tint, Vec2( 1.f, 0.f ) ),
	};

	for ( uint index = 0 ; index < 24 ; index++ )
	{
		cubeMeshVerts.push_back( CubeVerts[ index ] );
	}

	uint CubeIndices[ 36 ] = {
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
		// BOTTOM FACE INDICES
			20,21,22,
			22,23,20,
	};
	
	for ( uint index = 0; index < 36; index++ )
	{
		cubeIndices.push_back( CubeIndices[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateUVSphere( uint hCuts , uint vCuts , std::vector<Vertex_PCU>& sphereMeshVerts , std::vector<uint>& sphereIndices, float radius , Vec3 center , const Rgba8& tint )
{
	float deltaTheta = 360.f / ( float ) hCuts;
	float deltaPhi = 180.f / ( float ) vCuts;

	for ( float phi = -90.f; phi <= 90.0; phi += deltaPhi )
	{
		for ( float theta = 0.f; theta <= 360.0; theta += deltaTheta )
		{
			Vec3 currentCoordsNormal = Vec3::MakeFromSpericalCoordinates( theta , phi , radius );
			float u = RangeMapFloat( 0.f , 360.f , 0.f , 1.f , theta );
			float v = RangeMapFloat( -90.f , 90.f , 0.f , 1.f , phi );
		
			Vertex_PCU currentCoordVerts = Vertex_PCU( center + currentCoordsNormal , tint , Vec2( u , v ) );
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

void CreateUVSphere( uint hCuts , uint vCuts , std::vector<VertexMaster>& sphereMeshVerts , std::vector<uint>& sphereIndices , float radius , Vec3 center , const Rgba8& tint )
{
	float deltaTheta = 360.f / ( float ) hCuts;
	float deltaPhi = 180.f / ( float ) vCuts;

	for ( float phi = -90.f; phi <= 90.0; phi += deltaPhi )
	{
		for ( float theta = 0.f; theta <= 360.0; theta += deltaTheta )
		{
			Vec3 currentCoordsNormal = Vec3::MakeFromSpericalCoordinates( theta , phi , 1.f );
			float u = RangeMapFloat( 0.f , 360.f , 0.f , 1.f , theta );
			float v = RangeMapFloat( -90.f , 90.f , 0.f , 1.f , phi );

			VertexMaster currentCoordVerts = VertexMaster(
				Vertex_PCU( center + ( currentCoordsNormal * radius ) , tint , Vec2( u , v ) ) , currentCoordsNormal );
			//currentCoordVerts.m_normal = currentCoordsNormal;

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

void CreateCylinder( std::vector<Vertex_PCU>& cylinderMeshVerts , std::vector<uint>& cylinderIndices , float radius /*= 1.f */ , const Vec3& start /*= Vec3::ZERO */ , const Vec3& end /*= Vec3::ONE*/ , const Rgba8& startTint /*= WHITE */ , const Rgba8& endTint /*= WHITE */ )
{
	// TODO :- Update UV mapping if needed ::- CURRENTLY MAPPED INCORRECTLY
	
	Mat44 transformation = LookAtMatrix( start , end );

	Vec3 ibasis = transformation.GetIBasis3D();
	Vec3 jbasis = transformation.GetJBasis3D();

	const Vec3& i = ibasis;
	const Vec3& j = jbasis;
	const Vec3& s = start;
	const Vec3& e = end;
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			START DISC
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	constexpr uint  NUMBER_OF_DISC_VERTS = 120;
	constexpr uint  NUMBER_OF_TRIANGLES = ( uint ) NUMBER_OF_DISC_VERTS / 3;

	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	Vec3 secondVertStartDisc = start + ( i * radius ); // COS(0) = 1 , Sin(0) = 0
	
	cylinderMeshVerts.push_back( Vertex_PCU( start , startTint , Vec2( 0.5f , 0.5f ) ) );
	cylinderIndices.push_back( 0 );
	cylinderMeshVerts.push_back( Vertex_PCU( ( secondVertStartDisc ) , startTint , Vec2( 1.f , 0.5f ) ) );
	cylinderIndices.push_back( 1 );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	Vec3 iComponent = i *  costheta * radius;

	float sintheta = SinDegrees( angleInDegreesBetweenDiscTriangles );
	Vec3 jComponent = j *  sintheta * radius;
	
	cylinderMeshVerts.push_back( Vertex_PCU( ( s + iComponent + jComponent ) , startTint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheta ) ) );
	cylinderIndices.push_back( 2 );
	
	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 2; discVertIndex < NUMBER_OF_TRIANGLES; discVertIndex += 1 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		
		cylinderIndices.push_back( 0 );
		cylinderIndices.push_back( discVertIndex - 1 );
		
		costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
		sintheta = SinDegrees( angleInDegreesBetweenDiscTriangles );
		iComponent = i * costheta * radius;
		jComponent = j * sintheta * radius;

		Vertex_PCU temp;
		
		temp.m_position = s + iComponent + jComponent;
		temp.m_color = startTint;
		temp.m_uvTexCoords = Vec2::ZERO;

		cylinderMeshVerts.push_back( temp );
		cylinderIndices.push_back( discVertIndex + 1 );
	}
	cylinderIndices.push_back( 0 );
	cylinderIndices.push_back( discVertIndex - 1 );
	cylinderIndices.push_back( 1 );
		
//--------------------------------------------------------------------------------------------------------------------------------------------
//				END DISC
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	Vec3 secondVertEndDisc = end + ( i * radius ); // COS(0) = 1 , Sin(0) = 0

	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	iComponent = i * costheta * radius;

	sintheta = SinDegrees( angleInDegreesBetweenDiscTriangles );
	jComponent = j * sintheta * radius;

	uint endDiscCenterIndex = ( uint ) cylinderMeshVerts.size();
	cylinderMeshVerts.push_back( Vertex_PCU( end , endTint , Vec2( 0.5f , 0.5f ) ) );
	cylinderIndices.push_back( endDiscCenterIndex );

	cylinderMeshVerts.push_back( Vertex_PCU( ( secondVertEndDisc ) , endTint , Vec2( 1.f , 0.5f ) ) );
	cylinderIndices.push_back( endDiscCenterIndex + 1 );

	cylinderMeshVerts.push_back( Vertex_PCU( ( end + iComponent + jComponent ) , endTint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheta ) ) );
	cylinderIndices.push_back( endDiscCenterIndex + 2 );

	//-----------------------------------------------------------------------------------------------------------------
		
	for ( discVertIndex = 2; discVertIndex < NUMBER_OF_TRIANGLES; discVertIndex += 1 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		
		cylinderIndices.push_back( endDiscCenterIndex );
		cylinderIndices.push_back( endDiscCenterIndex + discVertIndex - 1 );

		costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
		sintheta = SinDegrees( angleInDegreesBetweenDiscTriangles );
		iComponent = i * costheta * radius;
		jComponent = j * sintheta * radius;

		Vertex_PCU temp;
		
		temp.m_position = e + iComponent + jComponent;
		temp.m_color = endTint;
		temp.m_uvTexCoords = Vec2::ZERO;

		cylinderMeshVerts.push_back( temp );
		cylinderIndices.push_back( endDiscCenterIndex + discVertIndex + 1 );
	}
	cylinderIndices.push_back( endDiscCenterIndex );
	cylinderIndices.push_back( endDiscCenterIndex + discVertIndex - 1 );
	cylinderIndices.push_back( endDiscCenterIndex + 1 );
		
//--------------------------------------------------------------------------------------------------------------------------------------------
//					SHAFT
//--------------------------------------------------------------------------------------------------------------------------------------------

	for ( uint index = 1; index < NUMBER_OF_TRIANGLES ; index++ )
	{
		cylinderIndices.push_back( index );
		cylinderIndices.push_back( index + 1 );
		cylinderIndices.push_back( endDiscCenterIndex + index + 1 );
		cylinderIndices.push_back( endDiscCenterIndex + index + 1 );
		cylinderIndices.push_back( endDiscCenterIndex + index );
		cylinderIndices.push_back( index );
	}

	cylinderIndices.push_back( endDiscCenterIndex - 1 );
	cylinderIndices.push_back( 1 );
	cylinderIndices.push_back( endDiscCenterIndex + 1 );
	cylinderIndices.push_back( endDiscCenterIndex + 1 );
	cylinderIndices.push_back( ( uint ) cylinderMeshVerts.size() - 1 );
	cylinderIndices.push_back( endDiscCenterIndex - 1 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateCone( std::vector<Vertex_PCU>& coneMeshVerts , std::vector<uint>& coneIndices , float radius /*= 1.f */ , const Vec3& start /*= Vec3::ZERO */ , const Vec3& end /*= Vec3::ONE*/ , const Rgba8& startTint /*= WHITE */ , const Rgba8& endTint /*= WHITE */ )
{
	// TODO :- Update UV mapping if needed ::- CURRENTLY MAPPED INCORRECTLY

	Mat44 transformation = LookAtMatrix( start , end );

	Vec3 ibasis = transformation.GetIBasis3D();
	Vec3 jbasis = transformation.GetJBasis3D();

	const Vec3& i = ibasis;
	const Vec3& j = jbasis;
	const Vec3& s = start;
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			START DISC
//--------------------------------------------------------------------------------------------------------------------------------------------

	constexpr uint  NUMBER_OF_DISC_VERTS = 120;
	constexpr uint  NUMBER_OF_TRIANGLES = ( uint ) NUMBER_OF_DISC_VERTS / 3;

	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	Vec3 secondVertStartDisc = start + ( i * radius ); // COS(0) = 1 , Sin(0) = 0

	coneMeshVerts.push_back( Vertex_PCU( start , startTint , Vec2( 0.5f , 0.5f ) ) );
	coneIndices.push_back( 0 );
	coneMeshVerts.push_back( Vertex_PCU( ( secondVertStartDisc ) , startTint , Vec2( 1.f , 0.5f ) ) );
	coneIndices.push_back( 1 );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	Vec3 iComponent = i * costheta * radius;

	float sintheta = SinDegrees( angleInDegreesBetweenDiscTriangles );
	Vec3 jComponent = j * sintheta * radius;

	coneMeshVerts.push_back( Vertex_PCU( ( s + iComponent + jComponent ) , startTint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheta ) ) );
	coneIndices.push_back( 2 );

	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 2; discVertIndex < NUMBER_OF_TRIANGLES; discVertIndex += 1 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );

		coneIndices.push_back( 0 );
		coneIndices.push_back( discVertIndex - 1 );

		costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
		sintheta = SinDegrees( angleInDegreesBetweenDiscTriangles );
		iComponent = i * costheta * radius;
		jComponent = j * sintheta * radius;

		Vertex_PCU temp;

		temp.m_position = s + iComponent + jComponent;
		temp.m_color = startTint;
		temp.m_uvTexCoords = Vec2::ZERO;

		coneMeshVerts.push_back( temp );
		coneIndices.push_back( discVertIndex + 1 );
	}
	coneIndices.push_back( 0 );
	coneIndices.push_back( discVertIndex - 1 );
	coneIndices.push_back( 1 );

//--------------------------------------------------------------------------------------------------------------------------------------------
//				END
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 secondVertEndDisc = end + ( i * radius ); // COS(0) = 1 , Sin(0) = 0

	Vertex_PCU endVert = Vertex_PCU( end , endTint , Vec2( 0.5f , 0.5f ) );

	uint conePeakVertIndex = ( uint ) coneMeshVerts.size();
	coneMeshVerts.push_back( endVert );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//					SHAFT
//--------------------------------------------------------------------------------------------------------------------------------------------

	for ( uint index = 1; index < NUMBER_OF_TRIANGLES; index++ )
	{
		coneIndices.push_back( index );
		coneIndices.push_back( index + 1 );
		coneIndices.push_back( conePeakVertIndex );
	}
	coneIndices.push_back( conePeakVertIndex - 1 );
	coneIndices.push_back( 1 );
	coneIndices.push_back( conePeakVertIndex );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateArrow3D ( std::vector< Vertex_PCU >& arrowMeshVerts , std::vector< uint >& arrowIndices ,
                     float shaftRadius /*= 1.f */ , float tipRadius /*= 1.1f */ , const Vec3& start /*= Vec3::ZERO */ ,
                     const Vec3& end /*= Vec3::ONE */ , const Rgba8& startTint /*= WHITE */ ,
                     const Rgba8& endTint /*= WHITE */ )
{
	std::vector<Vertex_PCU>	arrowShaftMeshVerts;
	std::vector<uint>		arrowShaftIndices;

	Vec3 shaftLength = end + ( start - end ) * .3f;								// keeping 70% length as the shaft and 30% for the tip

	CreateCylinder( arrowShaftMeshVerts , arrowShaftIndices , shaftRadius , start , shaftLength , startTint , endTint );

	std::vector<Vertex_PCU>	tipMeshVerts;
	std::vector<uint>		tipIndices;

	CreateCone( tipMeshVerts , tipIndices , tipRadius , shaftLength , end , endTint , endTint );

	AppendIndexedVerts( arrowShaftMeshVerts , arrowShaftIndices , arrowMeshVerts , arrowIndices );
	AppendIndexedVerts( tipMeshVerts , tipIndices , arrowMeshVerts , arrowIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateArrow3D ( std::vector< Vertex_PCU >& arrowMeshVerts , std::vector< uint >& arrowIndices ,
                     float shaftRadius /*= 1.f */ , float tipRadius /*= 1.1f */ , const Vec3& start /*= Vec3::ZERO */ ,
                     const Vec3& end /*= Vec3::ONE */ , const Rgba8& shaftStartTint /*= WHITE */ ,
                     const Rgba8& ShaftEndTint /*= WHITE */ , const Rgba8& tipStartTint /*= WHITE */ ,
                     const Rgba8& tipEndTint /*= WHITE */ )
{
	std::vector<Vertex_PCU>	arrowShaftMeshVerts;
	std::vector<uint>		arrowShaftIndices;

	Vec3 shaftLength = end + (start - end) * .3f;								// keeping 70% length as the shaft and 30% for the tip

	CreateCylinder( arrowShaftMeshVerts , arrowShaftIndices , shaftRadius , start , shaftLength , shaftStartTint , ShaftEndTint );

	std::vector<Vertex_PCU>	tipMeshVerts;
	std::vector<uint>		tipIndices;

	CreateCone( tipMeshVerts , tipIndices , tipRadius , shaftLength , end , tipStartTint , tipEndTint );

	AppendIndexedVerts( arrowShaftMeshVerts , arrowShaftIndices , arrowMeshVerts , arrowIndices );
	AppendIndexedVerts( tipMeshVerts , tipIndices , arrowMeshVerts , arrowIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
