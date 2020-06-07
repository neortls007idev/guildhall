#pragma once
#include <vector>
#include "Engine/Primitives/AABB2.hpp"
#include "Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class MapDefinition;
class Game;

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
	
	void SpawnNewEntity( eEntityType type , const Vec2& position );
	void AddEntityToMap( Entity* entity );
	void AddEntityToList( Entitylist& entityList , Entity* entity );

	void ResolveCollisions();
	void ResolveBallvBoundsCollisions();
	void ResolveBallvPaddleCollisions();
	
public:
	Game*							m_owner;
	AABB2							m_leftWall;
	AABB2							m_rightWall;
	AABB2							m_topWall;
	AABB2							m_pit;
	IntVec2							m_dimensions;
	//MapDefinition*					m_mapDefinition											= nullptr;
	std::string						m_name;
	Entitylist						m_entityListsByType[ NUM_ENTITY_TYPES ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------

