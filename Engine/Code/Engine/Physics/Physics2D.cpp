#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"

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
	AdvanceSimulation( deltaSeconds );

	for ( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies2D.size(); ++rigidBodyIndex )
	{
		m_rigidBodies2D[ rigidBodyIndex ]->Update( deltaSeconds );
	}

	for ( size_t colliderIndex = 0; colliderIndex < m_colliders2D.size(); ++colliderIndex )
	{
		m_colliders2D[ colliderIndex ]->UpdateWorldShape();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::AdvanceSimulation( float deltaSeconds )
{
	ApplyEffectors( deltaSeconds );
	MoveRigidbodies( deltaSeconds );
	// DetectCollisions(); - A04	// determine all pairs of intersecting colliders
	// CollisionResponse(); - A04	// resolve all collisions, firing appropraite events
	CleanupDestroyedObjects();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ApplyEffectors( float deltaSeconds )
{
	for ( size_t index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( !m_rigidBodies2D[index] )
		{
			continue;
		}

		eSimulationMode simulationMode = m_rigidBodies2D[ index ]->GetSimulationMode();
		Vec2 currentVelocity = m_rigidBodies2D[ index ]->GetVelocity();
		
		switch ( simulationMode )
		{
			case SIMULATIONMODE_STATIC:		m_rigidBodies2D[ index ]->SetVeloity( Vec2::ZERO );
											break;

			case SIMULATIONMODE_KINEMATIC:	/*m_rigidBodied2D[ index ]->SetVeloity( currentVelocity );*/
											break;

			case SIMULATIONMODE_DYNAMIC:	m_rigidBodies2D[ index ]->SetVeloity( currentVelocity + ( m_sceneGravity * deltaSeconds ) );
											break;

			default:
				break;
		}
		GravityBounce( m_sceneCamera , m_rigidBodies2D[ index ] );
		ScreenWrapAround( m_sceneCamera , m_rigidBodies2D[ index ] );
		m_rigidBodies2D[ index ]->SetPosition( m_rigidBodies2D[ index ]->GetPosition() + ( m_rigidBodies2D[ index ]->GetVelocity() * deltaSeconds ) );
		
		if ( index == 1 )
		{
			int x = 9;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::MoveRigidbodies( float deltaSeconds )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::CleanupDestroyedObjects()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::EndFrame()
{
	for ( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies2D.size(); rigidBodyIndex++ )
	{
		if ( !m_rigidBodies2D[ rigidBodyIndex ] )
		{
			continue;
		}

		if ( m_rigidBodies2D[ rigidBodyIndex ]->m_isGarbage )
		{
			delete m_rigidBodies2D[ rigidBodyIndex ];
			m_rigidBodies2D[ rigidBodyIndex ] = nullptr;
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
	m_rigidBodies2D.push_back( rigidBody );
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

void Physics2D::ChangeSceneGravity( Vec2 newGravity )
{
	m_sceneGravity = newGravity;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::GravityBounce( Camera* sceneCamera , Rigidbody2D* rigidBody )
{
	Vec2 gravityPlane( sceneCamera->GetOrthoMax().x - sceneCamera->GetOrthoMin().x , sceneCamera->GetOrthoMin().y );

	Collider2D* collider = nullptr;

	if ( rigidBody!= nullptr )
	{
		collider = rigidBody->GetCollider();
	}
	else
	{
		return;
	}

	if ( collider != nullptr )
	{
		switch ( collider->GetType() )
		{
		case COLLIDER2D_DISC :
									{	
										DiscCollider2D* colliderAsDisc = ( DiscCollider2D* ) collider;

										if ( rigidBody->GetPosition().y - colliderAsDisc->m_radius <= gravityPlane.y )
										{
											rigidBody->ReverseVelocity();
										}
									}break;

		case COLLIDER2D_CONVEXGON :
									if ( collider->GetClosestPoint( gravityPlane ).y <= gravityPlane.y )
									{
										rigidBody->ReverseVelocity();
									}break;
		default:
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ScreenWrapAround( Camera* sceneCamera , Rigidbody2D* rigidBody )
{
	float ScreenMinX = sceneCamera->GetOrthoMin().x;
	float ScreenMaxX = sceneCamera->GetOrthoMax().x;
	Vec2  rbCurrentPos = rigidBody->GetPosition();

	Collider2D* collider = rigidBody->GetCollider();

	if ( collider->GetType() == COLLIDER2D_DISC )
	{
		DiscCollider2D* discCollider = ( DiscCollider2D* ) collider;
		if ( ( rigidBody->GetPosition().x - discCollider->m_radius ) < ScreenMinX )
		{
			rigidBody->SetPosition( Vec2( ScreenMaxX - discCollider->m_radius , rbCurrentPos.y ) );
		}
		else if ( (rigidBody->GetPosition().x + discCollider->m_radius) > ScreenMaxX )
		{
			rigidBody->SetPosition( Vec2( ScreenMinX + discCollider->m_radius , rbCurrentPos.y ) );
		}
	} 
	else if ( collider->GetType() == COLLIDER2D_CONVEXGON )
	{
	}
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------