#pragma once
#include "Engine/Math/Vec3.hpp"
#include <vector>

#include "BillBoard.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Map;
class EntityDef;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eEntityType
{
	INVALID_ENTITY = -1,
	PLAYER,
	NPC,
	PROJECTILE,
	PORTAL,

	NUM_ENTITIES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity
{
public:
	Entity();
	Entity( EntityDef const& entityDef , Map* map );
	~Entity();
	void ConstructEntityBillboard();
	
	//----------------------------------------------------------------------------------------------------------

	void Update( float deltaSeconds );
	void Render() const;
	
	//----------------------------------------------------------------------------------------------------------
	
	bool IsPlayer();
	bool IsProjectile();
	bool IsNPC();
	
public:
	Map*											m_owner						= nullptr;
	eEntityType										m_entityType				= INVALID_ENTITY;
	Vec3											m_pos						= Vec3::ZERO;
	float											m_startYawDegrees			= 0.f;
	bool											m_canBePushedByWalls		= true;
	bool											m_canBePushedByEntities		= true;
	bool											m_canPushEntities			= true;

	float											m_physicsRadius				= 0.0f;
	float											m_height					= 0.0f;
	float											m_eyeHeight					= 0.0f;
	Vec2											m_spriteSize				= Vec2::ONE;
	float											m_mass						= 1.f;
	IntVec2											m_damage					= IntVec2::ZERO;
	float											m_speed						= 0.0f;
	eBillBoardType									m_billboardType				= CAMERA_FACING_XY;
	BillBoard*										m_billBoard					= nullptr;
	EntityDef*										m_entityDef					= nullptr;
	bool											m_isPossessed				= false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------