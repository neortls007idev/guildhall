#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/DevConsole.hpp"

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
	for ( size_t index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( !m_rigidBodies2D[index] || !m_rigidBodies2D[index]->m_isSimulationActive )
		{
			continue;
		}
	ApplyEffectors( m_rigidBodies2D[index] , deltaSeconds );
	MoveRigidbodies( m_rigidBodies2D[ index ] , deltaSeconds );
	ResetCollisions();
	DetectCollisions();
	ResolveCollisions();
	CleanupDestroyedObjects();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ApplyEffectors( Rigidbody2D* rigidbody , float deltaSeconds )
{

		eSimulationMode simulationMode = rigidbody->GetSimulationMode();
		Vec2 currentVelocity = rigidbody->GetVelocity();

		switch ( simulationMode )
		{
			case SIMULATIONMODE_STATIC:
											break;

			case SIMULATIONMODE_KINEMATIC:	/*m_rigidBodied2D[ index ]->SetVeloity( currentVelocity );*/
											break;

			case SIMULATIONMODE_DYNAMIC:	rigidbody->SetVelocity( currentVelocity + ( m_sceneGravity * deltaSeconds ) );
											break;

			default:
				break;
		}
	GravityBounce( m_sceneCamera    , rigidbody );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::MoveRigidbodies( Rigidbody2D* rigidbody , float deltaSeconds )
{
	if ( rigidbody->GetSimulationMode() != SIMULATIONMODE_STATIC )
	{
		rigidbody->SetPosition( rigidbody->GetPosition() + ( rigidbody->GetVelocity() * deltaSeconds ) );
		ScreenWrapAround( m_sceneCamera , rigidbody );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResetCollisions()
{
	for ( size_t index = 0; index < m_colliders2D.size(); index++ )
	{
		m_colliders2D[ index ]->m_isColliding = false;
	}
	m_frameCollisions.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::DetectCollisions()
{
	for ( size_t firstColliderIndex = 0; firstColliderIndex < m_colliders2D.size(); firstColliderIndex++ )
	{
		if ( !m_colliders2D[firstColliderIndex] )
		{
			continue;
		}

		for ( size_t secondColliderIndex = firstColliderIndex + 1; secondColliderIndex < m_colliders2D.size(); secondColliderIndex++ )
		{
			if ( !m_colliders2D[ secondColliderIndex ] )
			{
				continue;
			}

			Rigidbody2D* firstRigidBody = m_colliders2D[ firstColliderIndex ]->GetRigidBody();
			Rigidbody2D* secondRigidBody = m_colliders2D[ secondColliderIndex ]->GetRigidBody();

			if ( firstRigidBody && secondRigidBody )
			{
				//g_theDevConsole->PrintString( RED , "Collider has no rigid body attached." );
				if ( firstRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC && secondRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC )
				{
					continue;
				}
			}

			if ( m_colliders2D[ firstColliderIndex ]->Intersects( m_colliders2D[ secondColliderIndex ] ) )
			{
				m_colliders2D[ firstColliderIndex ]->m_isColliding	= true;
				m_colliders2D[ secondColliderIndex ]->m_isColliding = true;

				Collision2D newCollision;
 				newCollision.m_me	= m_colliders2D[ firstColliderIndex ];
 				newCollision.m_them = m_colliders2D[ secondColliderIndex ];
 				newCollision.m_collisionManifold = m_colliders2D[ firstColliderIndex ]->GenrateManifold( m_colliders2D[ secondColliderIndex ] );
				m_frameCollisions.push_back( newCollision );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResolveCollision( Collision2D collision )
{
	ResolveDiscVsDiscCollisions( collision );
	ResolveDiscVsPolygonCollisions(collision);
	ResolvePolygonVsDiscCollisions(collision);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResolveDiscVsDiscCollisions( Collision2D& collision )
{
	if ( collision.m_me->GetType() == COLLIDER2D_DISC && collision.m_them->GetType() == COLLIDER2D_DISC )
	{
		Rigidbody2D* meRigidBody = collision.m_me->GetRigidBody();
		Rigidbody2D* themRigidBody = collision.m_them->GetRigidBody();

		float myMass = meRigidBody->GetMass();
		float theirMass = themRigidBody->GetMass();
		float pushMe = theirMass / ( myMass + theirMass );
		float pushThem = 1.0f - pushMe;

		float meImpulseAlongNormal = ( ( myMass * theirMass ) / ( myMass + theirMass ) ) * ( 1 + collision.m_me->GetBounceWith( collision.m_them ) )
												* DotProduct2D( ( themRigidBody->GetVelocity() - meRigidBody->GetVelocity() ) ,
											collision.m_collisionManifold.m_normal );

		float themImpulseAlongNormal = ( ( myMass * theirMass ) / ( myMass + theirMass ) ) * ( 1 + collision.m_them->GetBounceWith( collision.m_me ) )
												* DotProduct2D( ( themRigidBody->GetVelocity() - meRigidBody->GetVelocity() ) ,
											collision.m_collisionManifold.m_normal );

		if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC )
		{
			meRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * pushMe );
			themRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * -pushThem );

			meRigidBody->ApplyImpulse( meImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
			themRigidBody->ApplyImpulse( -themImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
		else if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC )
		{
			meRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * pushMe );

			meRigidBody->ApplyImpulse( meImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
		else if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC )
		{
			themRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * -pushThem );

			themRigidBody->ApplyImpulse( -themImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResolvePolygonVsDiscCollisions(Collision2D& collision)
{
	if ( collision.m_me->GetType() == COLLIDER2D_CONVEXGON && collision.m_them->GetType() == COLLIDER2D_DISC )
	{
		Rigidbody2D* meRigidBody = collision.m_me->GetRigidBody();
		Rigidbody2D* themRigidBody = collision.m_them->GetRigidBody();

		float myMass = meRigidBody->GetMass();
		float theirMass = themRigidBody->GetMass();
		float pushMe = theirMass / ( myMass + theirMass );
		float pushThem = 1.0f - pushMe;

		float meImpulseAlongNormal = ( ( myMass * theirMass ) / ( myMass + theirMass ) ) * ( 1 + collision.m_me->GetBounceWith( collision.m_them ) )
												* DotProduct2D( ( themRigidBody->GetVelocity() - meRigidBody->GetVelocity() ) ,
											collision.m_collisionManifold.m_normal );

		float themImpulseAlongNormal = ( ( myMass * theirMass ) / ( myMass + theirMass ) ) * ( 1 + collision.m_them->GetBounceWith( collision.m_me ) )
												  * DotProduct2D( ( themRigidBody->GetVelocity() - meRigidBody->GetVelocity() ) ,
											  collision.m_collisionManifold.m_normal );

		if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC )
		{
			meRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * pushMe );
			themRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * -pushThem );

			meRigidBody->ApplyImpulse( meImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
			themRigidBody->ApplyImpulse( -themImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
		else if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC )
		{
			meRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * pushMe );

			meRigidBody->ApplyImpulse( meImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
		else if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC )
		{
			themRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * -pushThem );

			themRigidBody->ApplyImpulse( -themImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResolveDiscVsPolygonCollisions( Collision2D& collision )
{
	if ( collision.m_me->GetType() == COLLIDER2D_DISC && collision.m_them->GetType() == COLLIDER2D_CONVEXGON )
	{
		Rigidbody2D* meRigidBody = collision.m_me->GetRigidBody();
		Rigidbody2D* themRigidBody = collision.m_them->GetRigidBody();

		float myMass = meRigidBody->GetMass();
		float theirMass = themRigidBody->GetMass();
		float pushMe = theirMass / ( myMass + theirMass );
		float pushThem = 1.0f - pushMe;

		float meImpulseAlongNormal = ( ( myMass * theirMass ) / ( myMass + theirMass ) ) * ( 1 + collision.m_me->GetBounceWith( collision.m_them ) )
			* DotProduct2D( ( themRigidBody->GetVelocity() - meRigidBody->GetVelocity() ) , collision.m_collisionManifold.m_normal );
		float themImpulseAlongNormal = ( ( myMass * theirMass ) / ( myMass + theirMass ) ) * ( 1 + collision.m_them->GetBounceWith( collision.m_me ) )
			* DotProduct2D( ( themRigidBody->GetVelocity() - meRigidBody->GetVelocity() ) , collision.m_collisionManifold.m_normal );

		if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC )
		{
			meRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * pushMe );
			themRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * -pushThem );

			meRigidBody->ApplyImpulse( meImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
			themRigidBody->ApplyImpulse( -themImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
		else if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC )
		{
			meRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * pushMe );

			meRigidBody->ApplyImpulse( meImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
		else if ( meRigidBody && themRigidBody &&
			meRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC &&
			themRigidBody->GetSimulationMode() != SIMULATIONMODE_STATIC )
		{
			themRigidBody->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * -pushThem );

			themRigidBody->ApplyImpulse( -themImpulseAlongNormal * collision.m_collisionManifold.m_normal , collision.m_collisionManifold.m_contactPoint );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResolveCollisions()
{
	for ( size_t index = 0; index < m_frameCollisions.size(); index++ )
	{
		ResolveCollision( m_frameCollisions[index] );
	}
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
	//ResetCollisions();
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

Rigidbody2D* Physics2D::CreateRigidbody( Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , Polygon2D convexgon )
{
	Rigidbody2D* rigidBody = new Rigidbody2D( this , rigidBodyPosition );
	PolygonCollider2D* collider = new PolygonCollider2D( this , rigidBody , coliderPositionRelativeToRigidBody , convexgon );
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
											rigidBody->ReverseVelocityY();
										}
									}break;

		case COLLIDER2D_CONVEXGON :
									{
										PolygonCollider2D* polyCollider = ( PolygonCollider2D* ) collider;
										Vec2 lowestPoint = *GetBottomMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );
										if ( lowestPoint.y <= gravityPlane.y )
										{
											rigidBody->ReverseVelocityY();
										}
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
		if ( ( rigidBody->GetPosition().x + discCollider->m_radius ) < ScreenMinX )
		{
			rigidBody->SetPosition( Vec2( ScreenMaxX + discCollider->m_radius , rbCurrentPos.y ) );
		}
		else if ( (rigidBody->GetPosition().x - discCollider->m_radius) > ScreenMaxX )
		{
			rigidBody->SetPosition( Vec2( ScreenMinX - discCollider->m_radius , rbCurrentPos.y ) );
		}
	}
	else if ( collider->GetType() == COLLIDER2D_CONVEXGON )
	{
		PolygonCollider2D* polyCollider = ( PolygonCollider2D* ) collider;
		Vec2 leftMostPoint	= *GetLeftMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );
		Vec2 rightMostPoint = *GetRightMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );

		if ( rightMostPoint.x < ScreenMinX )
		{
			Vec2 offset =  /*rigidBody->m_worldPosition*/ polyCollider->m_worldPosition - leftMostPoint ;
			rigidBody->SetPosition( Vec2( ScreenMaxX + ( offset.x /*widthOfPolygon * 0.5f */ ) , rbCurrentPos.y ) );
		}

		if ( leftMostPoint.x > ScreenMaxX )
		{
			Vec2 offset = rightMostPoint - polyCollider->m_worldPosition;
			rigidBody->SetPosition( Vec2( ScreenMinX - ( offset.x /*widthOfPolygon * 0.5f*/ ) , rbCurrentPos.y ) );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------