#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/AABB3.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/TileMap.hpp"
#include "TheApp.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern DevConsole*			g_theDevConsole;
extern AudioSystem*			g_theAudioSystem;
extern TheApp*				g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			UTILITY FUNCTION FORWARD DECLARATIONS - DEFINED AT THE BOTTOM OF FILE
//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateTopFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
								const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateBottomFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
								const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateNorth_FrontFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateSouth_BackFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateWest_LeftFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

void CreateEast_RightFace( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
						const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );


//--------------------------------------------------------------------------------------------------------------------------------------------
RandomNumberGenerator rng;

bool Tile::IsSolid()
{
	return m_isSolid;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileMap::TileMap( char const* mapName , IntVec2 dimensions ) :
																Map( mapName ) ,
																m_dimensions( dimensions )
{
	m_worldMesh = new GPUMesh( g_theRenderer );

	PopulateTiles();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::PopulateTiles()
{
	for( int tileYIndex = 0 ; tileYIndex < m_dimensions.y ; tileYIndex++ )
	{
		for( int tileXIndex = 0 ; tileXIndex < m_dimensions.x ; tileXIndex++ )
		{
			Tile newTile = Tile();
			newTile.m_tileCoords = IntVec2( tileXIndex , tileYIndex );
			int x = GetIndexForTileCoords( newTile.m_tileCoords );
			if ( x == 12 )
			{
				newTile.m_isSolid = true;
			}
			m_tiles.push_back( newTile );
		}
	}

	SetOuterTilesToSolid();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::SetOuterTilesToSolid()
{
	for( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
	{
		m_tiles[ horizontalIndex ].m_isSolid = true;
	}
	for( int verticalIndex = 1; verticalIndex < m_dimensions.y; verticalIndex++ )
	{
		int index = verticalIndex * m_dimensions.x ;
		int index2 = index - 1;
		m_tiles[ index2 ].m_isSolid = true;
		m_tiles[ index ].m_isSolid = true;
	}

	for( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
	{
		int index = ( ( m_dimensions.y - 1 ) * m_dimensions.x ) + horizontalIndex;
		m_tiles[ index ].m_isSolid = true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TileMap::~TileMap()
{
	delete m_worldMesh;
	m_worldMesh				= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::UpdateMeshes()
{
	std::vector< Vertex_PCU >		WorldMeshVerts;
	
	std::vector< VertexMaster >		tempWorldMeshVerts;
	std::vector< uint >				worldMeshIndices;
	
	for ( int tileYIndex = 0 ; tileYIndex < m_dimensions.y ; tileYIndex++ )
	{
		for ( int tileXIndex = 0 ; tileXIndex < m_dimensions.x ; tileXIndex++ )
		{
			int tileIndex = ( m_dimensions.x *  tileYIndex ) + tileXIndex;
			AddVertsForTile( tempWorldMeshVerts , worldMeshIndices , tileIndex );
		}
	}

	VertexMaster::ConvertVertexMasterToVertexPCU( WorldMeshVerts , tempWorldMeshVerts );

	m_worldMesh->UpdateVertices( WorldMeshVerts );
	m_worldMesh->UpdateIndices( worldMeshIndices );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::Render() const
{
	
	Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->DrawMesh( m_worldMesh );
	g_theRenderer->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	IntVec2 tileCoords = GetTileCoordsForIndex( tileIndex );
	Tile* tile = GetTileAtCoords( tileCoords );
	
	if ( tile->IsSolid() )
	{
		AddVertsForSolidTile( destinationVerts , destinationIndices , tileIndex );
	}
	else
	{
		AddVertsForNonSolidTile( destinationVerts , destinationIndices , tileIndex );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	AABB3 bounds = GetTileBounds( tileIndex );
	m_tiles[ tileIndex ].m_bounds = bounds;
	std::vector< VertexMaster >		tileMeshVerts;
	std::vector< uint >				tileMeshIndices;
	bool result = false;

	IntVec2 currTileCoords	= GetTileCoordsForIndex( tileIndex );
	IntVec2 northTileCoords( -1 , 0 );
	IntVec2 southTileCoords( 1 , 0 );
	IntVec2 eastTileCoords( 0 , -1 );
	IntVec2 westTileCoords( 0 , 1 );

	//Tile* northTile = GetTileAtCoords( currTileCoords + northTileCoords );
	//if ( northTile == nullptr )
	if ( !IsTileSolid( currTileCoords + northTileCoords ) )
	{
	//	int newindex = GetIndexForTileCoords( currTileCoords + northTileCoords );
	//	AABB3 bounds = GetTileBounds( newindex );
		CreateNorth_FrontFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );

		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
		result = true;
	}

	tileMeshVerts.clear();
	tileMeshIndices.clear();

	if( !IsTileSolid( currTileCoords + southTileCoords ) )
	{
	//	int newindex = GetIndexForTileCoords( currTileCoords + southTileCoords );
	//	AABB3 bounds = GetTileBounds( newindex );
		CreateSouth_BackFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );
		
		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );

		result = true;
	}
		tileMeshVerts.clear();
		tileMeshIndices.clear();

	if( !IsTileSolid( currTileCoords + eastTileCoords ) )
	{
	//	int newindex = GetIndexForTileCoords( currTileCoords + eastTileCoords );
	//	AABB3 bounds = GetTileBounds( newindex );
		CreateEast_RightFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );

		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
		result = true;
	}
		tileMeshVerts.clear();
		tileMeshIndices.clear();
	
	if( !IsTileSolid( currTileCoords + westTileCoords ) )
	{
	//	int newindex = GetIndexForTileCoords( currTileCoords + westTileCoords );
	//	AABB3 bounds = GetTileBounds( newindex );
		CreateWest_LeftFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );

		AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
							destinationVerts , destinationIndices );
		result = true;
	}

	//return result;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForNonSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	AABB3 bounds = GetTileBounds( tileIndex );
	m_tiles[ tileIndex ].m_bounds = bounds;
	std::vector< VertexMaster >		tileMeshVerts;
	std::vector< uint >				tileMeshIndices;
	CreateTopFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );

	AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
						destinationVerts , destinationIndices );
	
	tileMeshVerts.clear();
	tileMeshIndices.clear();
	
	CreateBottomFace( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );

	AppendIndexedVerts( tileMeshVerts , tileMeshIndices ,
						destinationVerts , destinationIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

AABB3 TileMap::GetTileBounds( int tileIndex )
{
	AABB3 box( Vec3( -0.5f , -0.5f , -0.5f ) , Vec3( 0.5f , 0.5f , 0.5f ) );
	Vec2 tileCoords = Vec2( IntVec2( GetTileCoordsForIndex( tileIndex ) ) );
	Vec2 offset( 0.5f , 0.5f );
	Vec3 translateBy = Vec3( tileCoords + offset , 0.5f );
	box.Translate( translateBy );

	return box;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

IntVec2 TileMap::GetTileCoordsForIndex( int index )
{
	//IntVec2 coords;
	//coords.x = RoundDownToInt( ( float ) index / m_dimensions.y );
	//coords.y = index - ( coords.x * m_dimensions.y );
	//return coords;
	return m_tiles[ index ].m_tileCoords;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int TileMap::GetIndexForTileCoords( IntVec2 tileCoords )
{
	int tileIndex = ( m_dimensions.x * tileCoords.y ) + tileCoords.x;
	return tileIndex;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile* TileMap::GetTileAtCoords( IntVec2 tileCoords )
{
	int index = GetIndexForTileCoords( tileCoords );
	Tile* tile = nullptr;

	if ( index >= 0 &&  index < m_tiles.size() )
	{
		tile = &m_tiles[ index ];
	}
	
	return tile;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TileMap::IsTileSolid( IntVec2 tileCoords )
{
	Tile* tile = GetTileAtCoords( tileCoords );
	if( tile == nullptr )
	{
		return true;
	}

	return tile->IsSolid();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			UTILITY FUNCTION - MAP CREATION
//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateTopFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
							  const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );
	
	Vertex_PCU faceVerts[ 4 ] = {
		
		// TOP FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 topNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 topTangent = faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position;
		
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = topNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( topTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// TOP FACE INDICES
									0,2,1,
									3,2,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateBottomFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
							  const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {

		// BOTTOM FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 bottomNormal = - CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 bottomTangent = -( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = bottomNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( bottomTangent , 1.f );
	}
	

	uint FaceIndicesValues[ 6 ] = {
								// BOTTOM FACE INDICES
									0,2,1,
									3,2,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateNorth_FrontFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
						// FRONT FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 frontNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 frontTangent = faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position;

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = frontNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( frontTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// NORTH FACE INDICES
									0,1,2,
									2,3,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateSouth_BackFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
						// BACK FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 backNormal = -CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 backTangent = -( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = backNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( backTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// SOUTH FACE INDICES
									0,1,2,
									2,3,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateEast_RightFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
						// RIGHT FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 rightNormal = -CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 rightTangent = -( faceVerts[ 0 ].m_position - faceVerts[ 3 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = rightNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( rightTangent , 1.f );
	}

	uint FaceIndicesValues[ 6 ] = {
								// EAST FACE INDICES
									0,1,2,
									2,3,0,
								};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateWest_LeftFace( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
					   const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );

	Vertex_PCU faceVerts[ 4 ] = {
									// LEFT FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 4; index++ )
	{
		faceVertPCUS.emplace_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 leftNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 leftTangent = ( faceVerts[ 0 ].m_position - faceVerts[ 3 ].m_position );

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint vert = 0 ; vert < 4 ; vert++ )
	{
		faceMeshVerts[ vert ].m_normal = leftNormal;
		faceMeshVerts[ vert ].m_tangent = Vec4( leftTangent , 1.f );
	}


	uint FaceIndicesValues[ 6 ] = {
								// WEST FACE INDICES
									0,1,2,
									2,3,0,
									};

	for( uint index = 0; index < 6; index++ )
	{
		faceIndices.emplace_back( FaceIndicesValues[ index ] );
	}
}

Vertex_PCU CubeVerts[ 24 ] = {





};
