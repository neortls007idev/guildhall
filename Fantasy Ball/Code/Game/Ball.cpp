#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Ball.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include <map>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern BallTexEnumRGBA8Map	g_theBallTexTable[ NUM_GAME_TEX ];
//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::Ball( Game* owner , int health , float cosmeticRadius , float physicsRadius , Vec2 position , Vec2 velocity , eEntityType type /*= BALL */ ) :
																								Entity( owner , health , type ),
																								m_cosmeticRadius( cosmeticRadius ) ,
																								m_physicsRadius( physicsRadius ) ,
																								m_pos( position ),
																								m_velocity( velocity )
																														
{
	m_currentTexture = m_owner->m_gameTex[ TEX_BALL_CYAN ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::~Ball()
{
	m_currentTexture = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Update( float deltaSeconds )
{
	Move( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Render() const
{
	g_theRenderer->BindTexture( m_currentTexture );
	g_theRenderer->DrawDisc( m_pos , m_cosmeticRadius , WHITE );
	g_theRenderer->BindTexture( nullptr );
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
