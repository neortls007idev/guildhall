#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Physics2D::Physics2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Physics2D::~Physics2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::Update( float deltaSeconds )
{
	for ( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodied2D.size(); ++rigidBodyIndex )
	{
		m_rigidBodied2D[ rigidBodyIndex ]->Update( deltaSeconds );
	}

	for ( size_t colliderIndex = 0; colliderIndex < m_colliders2D.size(); ++colliderIndex )
	{
		m_colliders2D[ colliderIndex ]->UpdateWorldShape();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::EndFrame()
{
	for ( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodied2D.size(); rigidBodyIndex++ )
	{
		if ( !m_rigidBodied2D[ rigidBodyIndex ] )
		{
			continue;
		}

		if ( m_rigidBodied2D[ rigidBodyIndex ]->m_isGarbage )
		{
			delete m_rigidBodied2D[ rigidBodyIndex ];
			m_rigidBodied2D[ rigidBodyIndex ] = nullptr;
		}
	}

	for ( size_t colliderIndex = 0; colliderIndex < m_colliders2D.size(); colliderIndex++ )
	{
		if ( m_colliders2D[ colliderIndex ]->m_isGarbage )
		{
			delete m_colliders2D[ colliderIndex ];
			m_colliders2D[ colliderIndex ] = nullptr;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Rigidbody2D* Physics2D::CreateRigidbody( Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius )
{
	
	Rigidbody2D* rigidBody = new Rigidbody2D( this , rigidBodyPosition );
	DiscCollider2D* collider = new DiscCollider2D( this , rigidBody , coliderPositionRelativeToRigidBody , ColliderRadius );
	rigidBody->TakeCollider( collider );
	m_rigidBodied2D.push_back( rigidBody );
	m_colliders2D.push_back( collider );
	
	return rigidBody;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::DestroyRigidbody( Rigidbody2D* rigidbody )
{
	
	Collider2D* collider = rigidbody->GetCollider();

	if ( !collider )
	{
		rigidbody->SetCollider( nullptr );
		collider->Destroy();
	}

	rigidbody->Destroy();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition , float radius )
{
	// since a collider can exist without a rigidBody as it was explained to me in the office
	DiscCollider2D* collider = new DiscCollider2D( this , nullptr , localPosition , radius );
	return collider;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->Destroy();
}

//--------------------------------------------------------------------------------------------------------------------------------------------