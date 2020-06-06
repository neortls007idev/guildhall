#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/IntVec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball;
class TileDefinition;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Tile : public Entity
{
public:
	Tile( int health , eEntityType eEntityType , IntVec2 tileCoordinates , TileDefinition* tileType );

	IntVec2			GetTileCoordinates();
	TileDefinition* GetTileType() const;
	void			SetTileType( TileDefinition* tileType );
	bool			TileCollisionResponse( Ball* theBall );
protected:
	IntVec2			m_tileCoords;
	TileDefinition*	m_tileDef = nullptr;
	int				m_solidAtHealth = 0.f;
	int				m_visibleAtHealth = 0.f;
	bool			m_isSolid = false;
	bool			m_isVisible = false;
private:
};

//--------------------------------------------------------------------------------------------------------------------------------------------