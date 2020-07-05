﻿#pragma once
#include "GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/EngineCommon.hpp"

enum ePowerUpType : uchar
{
	PT_INVALID			= 0,

	PT_BALLX2			= 1,
	PT_BALLX3			= 2,
	PT_BALLX8			= 3,
	PT_BALL_SIZE_UP		= 4,
	PT_BALL_SIZE_DOWN	= 5,
	PT_BALL_SIZE_MEGA	= 6,
	PT_BALL_SIZE_SHRINK = 7,
	PT_BRICK_THROUGH	= 8,

	NUM_POWERUP_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class PowerUps : public Entity
{
public:
	PowerUps( Game* owner , Vec2 position , Vec2 velocity , ePowerUpType type = PT_INVALID );
	~PowerUps();
	
			void		Update( float deltaSeconds );
			void		Move( float deltaSeconds );
	virtual void		Render() const override;
	
			void		PowerUpEffects();
			void		PowerUpEffectMultipleBalls( uint numballs );
			void		PowerUpEffectBrickThrough();
			void		PowerUpEffectScaleBall( float scale );
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
public:
	AABB2				m_cosmeticBounds		= AABB2( -POWERUP_HALF_DIMENSIONS , POWERUP_HALF_DIMENSIONS );
//	AABB2				m_physicalBounds		= AABB2( -POWERUP_HALF_DIMENSIONS * 0.9f , POWERUP_HALF_DIMENSIONS * 0.9f );
	float				m_physicsRadius			= POWERUP_PHYSICS_RADIUS;
	Vec2				m_pos					= Vec2::ZERO;
	Vec2				m_velocity				= Vec2::ZERO;

	IntVec2				m_spriteCoords			= IntVec2::ZERO;
	Vec2				m_minUVs				= Vec2::ZERO;
	Vec2				m_maxUVs				= Vec2::ONE;
		
	ePowerUpType		m_type					= PT_INVALID;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

