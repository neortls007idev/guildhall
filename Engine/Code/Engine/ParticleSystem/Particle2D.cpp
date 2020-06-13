#include "Engine/ParticleSystem/Particle2D.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

#include "Engine/Renderer/RenderContext.hpp"
extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle2D::Particle2D( Vec2 position , float orientation , Vec2 velocity , float age , float maxAge , Rgba8 color ) :
	m_position( position ),
	m_orientation( orientation ),
	m_velocity( velocity ),
	m_age( age ),
	m_maxAge( maxAge ),
	m_color( color )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Particle2D::~Particle2D()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Particle2D::Update( float deltaSeconds )
{
	if ( m_age < m_maxAge )
	{
		m_age += deltaSeconds;
	}
	else
	{
		delete this;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
