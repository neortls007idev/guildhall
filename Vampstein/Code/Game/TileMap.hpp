#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/AABB3.hpp"
#include "Game/Map.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class GPUMesh;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Tile
{
	friend class TileMap;
	
public:
	IntVec2				m_tileCoords	= IntVec2::ZERO;
	//AABB2				m_bounds		= AABB2::ZERO_TO_ONE;
	AABB3				m_bounds;
	bool				m_isSolid;
	bool				IsSolid();
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class TileMap : public Map
{
public:
	TileMap( char const* mapName , IntVec2 dimensions );


	//--------------------------------------------------------------------------------------------------------------------------------------------


	virtual ~TileMap();
	
			void		PopulateTiles();
	virtual void		UpdateMeshes() override;
	virtual void		Render() const override;
			void		AddVertsForTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex );
			void		AddVertsForSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex );
			void		AddVertsForNonSolidTile( std::vector< VertexMaster >& destinationVerts , std::vector< uint >& destinationIndices , int tileIndex );
			AABB3		GetTileBounds( int tileIndex );
			IntVec2		GetTileCoordsForIndex( int index );
private:
	IntVec2									m_dimensions									= IntVec2::ZERO;
	std::vector< Tile >						m_tiles;
	GPUMesh*								m_worldMesh;
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------