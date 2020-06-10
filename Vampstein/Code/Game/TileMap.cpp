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

void CreateTopAndBottomFaces( std::vector< VertexMaster >& cubeMeshVerts , std::vector< uint >& faceIndices ,
							  const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs );

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Tile::IsSolid()
{
	return false;
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
			m_tiles.push_back( newTile );
		}
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
	if ( m_tiles[ tileIndex ].IsSolid() )
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
	CreateCuboidXInYLeftZUp( tileMeshVerts , tileMeshIndices , bounds , WHITE );

	AppendIndexedVerts( tileMeshVerts , tileMeshIndices , 
						destinationVerts , destinationIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForNonSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	AABB3 bounds = GetTileBounds( tileIndex );
	m_tiles[ tileIndex ].m_bounds = bounds;
	std::vector< VertexMaster >		tileMeshVerts;
	std::vector< uint >				tileMeshIndices;
	CreateTopAndBottomFaces( tileMeshVerts , tileMeshIndices , bounds , WHITE , Vec2::ZERO , Vec2::ONE );

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
	return m_tiles[ index ].m_tileCoords;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//			UTILITY FUNCTION - MAP CREATION
//--------------------------------------------------------------------------------------------------------------------------------------------

void CreateTopAndBottomFaces( std::vector< VertexMaster >& faceMeshVerts , std::vector< uint >& faceIndices ,
							  const AABB3 box , const Rgba8& tint /*= WHITE */ , Vec2 minUVS , Vec2 maxUvs )
{
//--------------------------------------------------------------------------------
//			NOTES
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//
//												(Z - Upwards(facing ceiling)/Height/ /Right hand - Middle Finger)
//																				^
//	(X - Inwards(point away from myself)/Depth-width/East/Right hand - Thumb)	|
//																			  \	|
//				   (Y - Left/Length/North/Right hand - Index Finger) <---------\|
//				   
//--------------------------------------------------------------------------------

	Vec2 minXUVMaxYUV( minUVS.x , maxUvs.y );
	Vec2 maxXUVMinYUV( maxUvs.x , minUVS.y );
	
	Vertex_PCU faceVerts[ 8 ] = {
		
		// TOP FACE VERTS
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_maxs.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_maxs.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_maxs.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_maxs.z ) , tint, minXUVMaxYUV ),

		// BOTTOM FACE VERTS
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,box.m_mins.z ) , tint, maxUvs ),
						Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,box.m_mins.z ) , tint, maxXUVMinYUV ),

						Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,box.m_mins.z ) , tint, minUVS ),
						Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,box.m_mins.z ) , tint, minXUVMaxYUV ),
	};

	std::vector<Vertex_PCU> faceVertPCUS;

	for( uint index = 0; index < 8; index++ )
	{
		faceVertPCUS.push_back( faceVerts[ index ] );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 topNormal = CrossProduct3D( faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position , faceVerts[ 3 ].m_position - faceVerts[ 0 ].m_position );
	Vec3 bottomNormal = -topNormal;

	Vec3 faceNormals[ 2 ] = { topNormal , bottomNormal };

//--------------------------------------------------------------------------------------------------------------------------------------------
//			FACE TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec3 topTangent = faceVerts[ 1 ].m_position - faceVerts[ 0 ].m_position;
	Vec3 bottomTangent = -topTangent;
	
	Vec3 faceTangents[ 2 ] = { topTangent , bottomTangent };

//--------------------------------------------------------------------------------------------------------------------------------------------
	Vertex_PCU::ConvertVertexPCUToVertexMaster( faceMeshVerts , faceVertPCUS );

	for( uint faces = 0 ; faces < 2 ; faces++ )
	{
		for( uint vert = 0 ; vert < 4 ; vert++ )
		{
			faceMeshVerts[ ( faces * 4 ) + vert ].m_normal	= faceNormals[ faces ];
			faceMeshVerts[ ( faces * 4 ) + vert ].m_tangent = Vec4( faceTangents[ faces ] , 1.f );
		}
	}

	uint FaceIndicesValues[ 12 ] = {
								// TOP FACE INDICES
									0,1,2,
									2,3,0,
								// BOTTOM FACE INDICES
									4,5,6,
									6,7,4,
								};

	for( uint index = 0; index < 12; index++ )
	{
		faceIndices.push_back( FaceIndicesValues[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------