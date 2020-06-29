#include "Game/Paddle.hpp"
#include "Game/Game.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern InputSystem*		g_theInput;

//--------------------------------------------------------------------------------------------------------------------------------------------

Paddle::Paddle( Game* owner , int health , AABB2 paddleCollider , Vec2 startPos ) :
																	Entity( owner , health , eEntityType::PADDLE ) ,
																	m_paddleCollider( paddleCollider ) ,
																	m_position( startPos )
{
	m_paddleCollider.Translate( m_position );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Paddle::Update( float deltaSeconds )
{
	UpdateFromUserInput( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Paddle::Render() const
{
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_PADDLE ] );
	g_theRenderer->DrawAABB2( m_paddleCollider , ORANGE );
	g_theRenderer->BindTexture( nullptr );

	if ( m_owner->m_isDebugDraw )
	{
		g_theRenderer->DrawUnfilledAABB2( m_paddleCollider , MAGENTA	, 2.5f );
		g_theRenderer->DrawUnfilledAABB2( m_paddleCollider , CYAN		, 2.5f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Paddle::UpdateFromUserInput( float deltaSeconds )
{
	Vec2 mousePos = g_theInput->GetRelativeMovement();

	float speed = 100.f;

	float paddleColliderLength = m_paddleCollider.GetDimensions().x;

	Vec2 translate = Vec2::ZERO;
	translate.x = mousePos.x * speed * deltaSeconds;
	
	m_position.x += translate.x;
	m_position.x = Clamp( m_position.x , m_owner->m_currentLevel->m_leftWallPhysicalBounds.m_maxs.x + paddleColliderLength * 0.5f,
	                      m_owner->m_currentLevel->m_rightWallPhysicalBounds.m_mins.x - paddleColliderLength * 0.5f );

	translate.x = m_position.x;
	
	m_owner->m_currentLevel->UpdateBallPosWhenGrabbed( translate.x );
	
	m_paddleCollider.SetCenter( m_position );

}

//--------------------------------------------------------------------------------------------------------------------------------------------
