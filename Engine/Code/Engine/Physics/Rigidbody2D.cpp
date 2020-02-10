#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Rigidbody2D::Rigidbody2D( Physics2D* system , Vec2 worldPosition , Collider2D* collider ) :
																		m_system( system ) ,
																		m_worldPosition( worldPosition ),
																		m_collider( collider )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Rigidbody2D::Rigidbody2D( Physics2D* system, Vec2 worldPosition ) :
																		m_system( system ),
																		m_worldPosition( worldPosition )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::Destroy()
{
	m_isGarbage = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::TakeCollider( Collider2D* collider )
{
	if ( nullptr != m_collider && collider != m_collider )
	{
		m_collider->Destroy();
	}
	m_collider = collider;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::SetCollider( Collider2D* collider )
{
	m_collider = collider;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::SetPosition( Vec2 position )
{
	m_worldPosition = position;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::SetVeloity( Vec2 velocity )
{
	m_velocity = velocity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::ReverseVelocity()
{
	m_velocity.y = -m_velocity.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::SetSimulationMode( eSimulationMode simulationMode )
{
	m_simulationMode = simulationMode;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::SetMass( float newMass )
{
	m_mass = newMass;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::ChangeIsSimulationActive( bool newSimulationStatus )
{
	m_isSimulationActive = newSimulationStatus;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Rigidbody2D::~Rigidbody2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------