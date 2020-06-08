#include "Game/Ball.hpp"

#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Ball::Ball( Game* owner , int health , float cosmeticRadius , float physicsRadius , Vec2 position , Vec2 velocity , eEntityType type /*= BALL */ ) :
																								Entity( owner , health , type ),
																								m_cosmeticRadius( cosmeticRadius ) ,
																								m_physicsRadius( physicsRadius ) ,
																								m_pos( position ),
																								m_velocity( velocity )
																														
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Update( float deltaSeconds )
{
	Move( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Render() const
{
	g_theRenderer->DrawDisc( m_pos , m_cosmeticRadius , GREEN );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Ball::Move( float deltaSeconds )
{
	m_pos += m_velocity * deltaSeconds * 100.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
