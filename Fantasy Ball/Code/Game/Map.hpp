#pragma once

#include "Engine/Primitives/AABB2.hpp"
#include "Game/Entity.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapDefinition;
class Game;
class TileDefinition;
class ParticleEmitter2D;

//--------------------------------------------------------------------------------------------------------------------------------------------
	typedef std::vector<Entity*> Entitylist;
//--------------------------------------------------------------------------------------------------------------------------------------------
class Map
{
public:
	Map( Game* owner );
	Map( Game* owner , MapDefinition* mapDefinition , std::string mapName );

	void LevelBounds();

	//--------------------------------------------------------------------------------------------------------------------------------------------


	~Map();

	void Update( float deltaSeconds );
	void Render();
	
	void SpawnNewEntity( eEntityType type , const Vec2& position , TileDefinition* tileDef = nullptr );
	void AddEntityToMap( Entity* entity );
	void AddEntityToList( Entitylist& entityList , Entity* entity );

	void ResolveCollisions();
	void ResolveBallvBoundsCollisions();
	void ResolveBallvPaddleCollisions();
	void ResolveBallvTileCollisions();
	
public:
	Game*							m_owner;
	AABB2							m_backGround;
	AABB2							m_leftWall;
	AABB2							m_rightWall;
	AABB2							m_topWall;
	AABB2							m_pit;
	IntVec2							m_dimensions;
	MapDefinition*					m_mapDefinition											= nullptr;
	std::string						m_name;
	Entitylist						m_entityListsByType[ NUM_ENTITY_TYPES ];
	ParticleEmitter2D*				m_testEmitter;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

