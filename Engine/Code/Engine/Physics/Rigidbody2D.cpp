#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::Destroy()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Rigidbody2D::TakeCollider( Collider2D* collider )
{
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

Rigidbody2D::~Rigidbody2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------