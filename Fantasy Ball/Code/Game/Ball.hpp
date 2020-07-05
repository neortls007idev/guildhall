#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class SpriteSheet;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Ball : public Entity
{
public:
	Ball( Game* owner , int health , float physicsRadius , Vec2 position , Vec2 velocity , IntVec2 spriteCoords , eEntityType type = BALL );
	~Ball();

	void Update( float deltaSeconds ) override;
	void Render() const override;

	void Move( float deltaSeconds );
	void AddVelocityNudge();
	void UpdateCurrentTexture( Rgba8 newColor );
	void UpdateCurrentTexture( IntVec2 spriteCoords );
	void UpdateBounds( float scale );
	
public:
	float			m_physicsRadius;
	Vec2			m_pos;
	Vec2			m_velocity;
	SpriteSheet*	m_currentSpriteSheet				= nullptr;
	float			m_ballPaddleMaxCoolDownTime			= BALL_PADDLE_MAX_COLLISION_COOLDOWN_TIME;
	bool			m_isInCooldown						= false;
	AABB2			m_cosmeticRadius;
	IntVec2			m_spriteCoords;
	Vec2			m_minUVs;
	Vec2			m_maxUVs;

	bool			m_brickThrough						= false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
