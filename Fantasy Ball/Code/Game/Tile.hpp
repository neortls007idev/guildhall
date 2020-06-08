#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball;
class TileDefinition;
class Map;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Tile : public Entity
{
public:
	Tile( Map* owner , IntVec2 tileCoordinates , TileDefinition* tileType );
		
	void					Update( float deltaSeconds );
	void					Render() const;
	
	int						GetTileIndex();
	IntVec2					GetTileCoordinates()														{ return m_tileCoords; }
	TileDefinition*			GetTileType() const															{ return m_tileDef; }
	void					SetTileType( TileDefinition* tileType )										{ m_tileDef = tileType; }
	bool					TileCollisionResponse( Ball* ball );
	AABB2					GetCollider() const;
	
protected:
	Map*							m_owner											= nullptr;
	IntVec2							m_tileCoords;
	TileDefinition*					m_tileDef										= nullptr;
	int								m_solidAtHealth									= 0.f;
	int								m_visibleAtHealth								= 0.f;
	bool							m_isSolid										= false;
	bool							m_isVisible										= false;
	Rgba8							m_tileColor;
private:
};

//--------------------------------------------------------------------------------------------------------------------------------------------