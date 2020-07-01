#pragma once
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map;
class EntityDef;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity
{
public:
	Entity( EntityDef const& entityDef , Map* map );
	~Entity();
	
	bool IsPlayer();
	bool IsProjectile();
	bool IsNPC();

public:
	float	m_mass						= 1.f;
	bool	m_canBePushedByWalls		= false;
	bool	m_canBePushedByEntities		= false;
	bool	m_canPushEntities			= false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------