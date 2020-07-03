#pragma once

#include "Engine/Primitives/AABB2.hpp"
#include "Game/Entity.hpp"
#include <vector>
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball;
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

	void	InitializeLevelBounds();

	//--------------------------------------------------------------------------------------------------------------------------------------------

	~Map();

	void	Update( float deltaSeconds );
	void	Render();

	void	UpdateSideCosmeticBounds();
	void	UpdateTopCosmeticBounds();
	void	RenderLevelBounds();
	void	RenderLevelSideBounds() const;

	//--------------------------------------------------------------------------------------------------------------------------------------------

	void	SpawnNewEntity( eEntityType type , const Vec2& position , TileDefinition* tileDef = nullptr );
	void	AddEntityToMap( Entity* entity );
	void	AddEntityToList( Entitylist& entityList , Entity* entity );

	void	UpdateBallPosWhenGrabbed( float newPosX );
	
	void	ResolveCollisions();
	void	ResolveBallvBoundsCollisions();
	void	ResolveBallvPaddleCollisions();
	void	ResolveBallvTileCollisions();
	void	ResolvePaddlevPowerUpCollisions();
	
	void	SpawnParticlesOnBallCollisionUsingEmitter( Ball* ball , Vec2 refPoint , uint num4XParticles , ParticleEmitter2D* emitter ,
													   float minLifeTime , float maxLifeTime , Vec2 dimensions , float velocityMultiplier );

	void	SpawnRandomParticleUsingEmitter ( Ball* ball , Vec2 refPoint , float minLifeTime , float maxLifeTime ,
				                              Vec2 dimensions , float velocityMultiplier , ParticleEmitter2D* emmiter );
	//--------------------------------------------------------------------------------------------------------------------------------------------


	Entity* GetFirstValidEntryInList( eEntityType type );

	void	GarbageCollection();
	
public:
	Game*							m_owner;
	AABB2							m_backGround;

	AABB2							m_leftWallPhysicalBounds;
	AABB2							m_rightWallPhysicalBounds;
	AABB2							m_topWallPhysicalBounds;
	AABB2							m_pitPhysicalBounds;

	AABB2							m_leftWallCosmeticBounds;
	AABB2							m_rightWallCosmeticBounds;
	AABB2							m_topWallCosmeticBounds;
	AABB2							m_pitCosmeticBounds;
	
	IntVec2							m_dimensions;
	MapDefinition*					m_mapDefinition											= nullptr;
	std::string						m_name;
	Entitylist						m_entityListsByType[ NUM_ENTITY_TYPES ];
	ParticleEmitter2D*				m_tileEmitter;
	ParticleEmitter2D*				m_boundsEmitter;
	ParticleEmitter2D*				m_paddleEmitter;
	int								m_backgroundIndex										= 0;
	int								m_numAliveBalls											= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

