#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Collider2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eEntityType
{
	PLAYER_ENTITY,
	ENEMY_ENTITY,

	TOTAL_ENTITIES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity
{

public:

	Vec2			m_position				= Vec2::ZERO;
	Vec2			m_velocity				= Vec2( 0.f , 0.f );

	float			m_orientationDegrees	= 0.f;
	float			m_angularVelocity		= 0.f;

	int				m_health				= 1;
	bool			m_isDead				= true;
	bool			m_isGarbage				= true;
	eEntityType		m_type;
	Collider2D*		m_collider;

	
	Game* m_game;

public:
	Entity( Game* pointerToGameInstance , Vec2 pos , Vec2 velocity , float orientation );
	
	virtual void		Update( float deltaSeconds );

			void		Movement( float deltaSeconds );
			void		WrapAroundScreen( Collider2D* collider );
			eEntityType GetType() const																{ return m_type; }

	virtual void		Render() const;

	virtual void		IsDead();

			bool		IsAlive();
};

//--------------------------------------------------------------------------------------------------------------------------------------------