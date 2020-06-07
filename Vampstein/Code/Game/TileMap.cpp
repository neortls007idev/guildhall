#include "Game/TileMap.hpp"

#include "TheApp.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Primitives/AABB3.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern DevConsole*			g_theDevConsole;
extern AudioSystem*			g_theAudioSystem;
extern TheApp*				g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eCubeFaces
{
	FRONT_FACE,
	BACK_FACE,
	RIGHT_FACE,
	LEFT_FACE,
	TOP_FACE,
	BOTTOM_FACE,

	NUM_TOTAL_CUBE_FACES
};

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
	AddVertsForSolidTile( destinationVerts , destinationIndices , tileIndex );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TileMap::AddVertsForSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex )
{
	AABB3 bounds = GetTileBounds( tileIndex );
	m_tiles[ tileIndex ].m_bounds = bounds;
	std::vector< VertexMaster >		tileMeshVerts;
	std::vector< uint >				tileMeshIndices;
	CreateCuboidXInYLeftZUp( tileMeshVerts , tileMeshIndices , bounds , WHITE );

	AppendIndexedVerts( tileMeshVerts , tileMeshIndices , destinationVerts , destinationIndices );
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
