#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball : public Entity
{
public:
	Ball( Game* owner , int health , float cosmeticRadius , float physicsRadius , Vec2 position , Vec2 velocity , eEntityType type = BALL );
	~Ball();

	void Update( float deltaSeconds ) override;
	void Render() const override;

	void Move( float deltaSeconds );
	void AddVelocityNudge();
	void UpdateCurrentTexture( Rgba8 newColor );
	
public:
	float			m_cosmeticRadius;
	float			m_physicsRadius;
	Vec2			m_pos;
	Vec2			m_velocity;
	Texture*		m_currentTexture				= nullptr;
	float			m_ballPaddleMaxCoolDownTime		= BALL_PADDLE_MAX_COLLISION_COOLDOWN_TIME;
	bool			m_isInCooldown					= false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
