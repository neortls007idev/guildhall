#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Game.hpp"
#include "Game/PowerUps.hpp"
#include "Ball.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

PowerUps::PowerUps( Game* owner , Vec2 position , Vec2 velocity , ePowerUpType type /*= PT_INVALID */ ) :
	Entity( owner , 1 , POWERUP ) ,
	m_pos( position ) ,
	m_velocity( velocity ) ,
	m_type( type )
{
	m_cosmeticBounds.SetCenter( m_pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

PowerUps::~PowerUps()
{
	m_owner = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::Update( float deltaSeconds )
{
	Move( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::Move( float deltaSeconds )
{
	m_velocity -= GRAVITY * deltaSeconds;
	m_pos += m_velocity * deltaSeconds;

	m_cosmeticBounds.SetCenter( m_pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::Render() const
{
	g_theRenderer->DrawAABB2( m_cosmeticBounds , WHITE );

	if( m_owner->m_isDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_cosmeticBounds , MAGENTA );
		g_theRenderer->DrawRing( m_pos , m_physicsRadius , CYAN , 2.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::PowerUpEffects()
{
	if( m_type == PT_BALLX2 )
	{
		PowerUpEffectMultipleBalls( 2 );
	}
	
	if( m_type == PT_BALLX3 )
	{
		PowerUpEffectMultipleBalls( 3 );
	}

	if( m_type == PT_BALLX8 )
	{
		PowerUpEffectMultipleBalls( 8 );
	}

	if ( m_type == PT_BRICK_THROUGH )
	{
		PowerUpEffectBrickThrough();
	}

	if ( m_type == PT_BALL_SIZE_MEGA )
	{
		PowerUpEffectScaleBall( BALL_MAX_PHYSICAL_RADIUS_FACTOR );
	}

	if( m_type == PT_BALL_SIZE_SHRINK )
	{
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::PowerUpEffectBrickThrough()
{
	Entitylist ballList = *m_owner->m_currentLevel->GetEntityList( BALL );

	for( size_t index = 0 ; index < ballList.size() ; index++ )
	{
		Ball* ball = ( Ball* ) ballList[ index ];
		if( nullptr == ball )
		{
			continue;
		}
		ball->m_brickThrough = true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::PowerUpEffectScaleBall( float scale )
{
	
	Entitylist ballList = *m_owner->m_currentLevel->GetEntityList( BALL );

	for( size_t index = 0 ; index < ballList.size() ; index++ )
	{
		Ball* ball = ( Ball* ) ballList[ index ];
		if( nullptr == ball )
		{
			continue;
		}
		ball->UpdateBounds( scale );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PowerUps::PowerUpEffectMultipleBalls( uint numballs )
{
	Entitylist ballList = *m_owner->m_currentLevel->GetEntityList( BALL );

	for( size_t index = 0 ; index < ballList.size() ; index++ )
	{
		Ball* ball = ( Ball* ) ballList[ index ];
		if( nullptr == ball )
		{
			continue;
		}

		if ( numballs == 2 )
		{
			ball->m_velocity.RotateDegrees( 45.f );
			Ball* newBall = nullptr;
			newBall = ( Ball* ) m_owner->m_currentLevel->SpawnNewEntity( BALL , ball->m_pos , ball->m_velocity.GetRotatedMinus90Degrees() ,
			                                         nullptr , PT_INVALID );
			if( nullptr != newBall )
			{
				newBall->m_brickThrough = ball->m_brickThrough;
			}
		}

		if( numballs == 3 )
		{
			Vec2 newBallVelocity1 = ball->m_velocity.GetRotatedDegrees( 45.f );
			Ball* newBall1 = nullptr;
		   newBall1 = ( Ball* ) m_owner->m_currentLevel->SpawnNewEntity( BALL , ball->m_pos , newBallVelocity1 ,
													 nullptr , PT_INVALID );
			if( nullptr != newBall1 )
			{
				newBall1->m_brickThrough = ball->m_brickThrough;
			}
			
			Vec2 newBallVelocity2 = ball->m_velocity.GetRotatedDegrees( -45.f );
			Ball* newBall2 = nullptr;
		   newBall2 = ( Ball* ) m_owner->m_currentLevel->SpawnNewEntity( BALL , ball->m_pos , newBallVelocity2 ,
													 nullptr , PT_INVALID );
			if( nullptr != newBall2 )
			{
				newBall2->m_brickThrough = ball->m_brickThrough;
			}
		}

		if( numballs == 8 )
		{
			float orientationDegrees = ball->m_velocity.GetAngleDegrees();
			for ( int newBallIndex = 1 ; newBallIndex < 8 ; newBallIndex++ )
			{
				Vec2 newBallVelocity = ball->m_velocity.GetRotatedDegrees( newBallIndex * orientationDegrees );
				Ball* newBall = nullptr;
			   newBall = ( Ball* ) m_owner->m_currentLevel->SpawnNewEntity( BALL , ball->m_pos , newBallVelocity ,
														 nullptr , PT_INVALID );
				if( nullptr != newBall )
				{
					newBall->m_brickThrough = ball->m_brickThrough;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------