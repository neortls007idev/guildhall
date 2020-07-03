#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Ball.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include <map>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern BallTexEnumRGBA8Map	g_theBallTexTable[ NUM_GAME_TEX ];
//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::Ball( Game* owner , int health , float physicsRadius , Vec2 position , Vec2 velocity , eEntityType type /*= BALL */ ) :
																								Entity( owner , health , type ),
																								m_physicsRadius( physicsRadius ) ,
																								m_pos( position ),
																								m_velocity( velocity )
																														
{
	m_currentTexture = m_owner->m_gameTex[ TEX_BALL_CYAN ];
	m_cosmeticRadius.SetDimensions( Vec2 ( physicsRadius , physicsRadius ) * 3.f );
	m_cosmeticRadius.SetCenter( m_pos.x , m_pos.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::~Ball()
{
	m_currentTexture = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Update( float deltaSeconds )
{
	if ( !m_owner->m_isBallLaunchable )
	{
		Move( deltaSeconds );
		m_cosmeticRadius.SetCenter( m_pos );
	}

	if ( m_isInCooldown )
	{
		m_ballPaddleMaxCoolDownTime -= deltaSeconds;
	}

	if ( m_ballPaddleMaxCoolDownTime <= 0.f )
	{
		m_isInCooldown = false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Render() const
{
	g_theRenderer->BindTexture( m_currentTexture );
	g_theRenderer->DrawAABB2( m_cosmeticRadius , WHITE );
	g_theRenderer->BindTexture( nullptr );

	if ( m_owner->m_isDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_cosmeticRadius	, MAGENTA	, 2.5f );
		g_theRenderer->DrawRing( m_pos , m_physicsRadius	, CYAN		, 2.5f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Move( float deltaSeconds )
{
	m_pos += m_velocity * deltaSeconds * 100.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::AddVelocityNudge()
{
	Vec2 nudgeVector = g_RNG->RollRandomDirection2D();

	m_velocity += nudgeVector;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::UpdateCurrentTexture( Rgba8 newColor )
{
	eGameTextures newTex = TEX_BALL_CYAN;
	
	for( int index = 0 ; index < NUM_GAME_TEX ; index++ )
	{
		if ( g_theBallTexTable[ index ].color == newColor )
		{
			newTex = g_theBallTexTable[ index ].texture;
			break;
		}
	}
	
	m_currentTexture = m_owner->m_gameTex[ newTex ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------
