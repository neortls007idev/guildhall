#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Timer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC Clock* Physics2D::s_clock;
STATIC Timer* Physics2D::s_timer;
STATIC double Physics2D::s_fixedTimeStep;

extern DevConsole* g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

Physics2D::Physics2D()
{
	s_clock			= new Clock();
	s_timer			= new Timer();
	s_fixedTimeStep = 1.0 / 60.0;

	EventArgs physicsStepArgs;

	physicsStepArgs.SetValue( "StepFrequency" , std::to_string( 120.f ) );
	g_theDevConsole->CreateCommand( "SetPhysicsTimeStep" , "Ex:- SetPhysicsTimeStep StepFrequency 120" , physicsStepArgs );
	g_theEventSystem->SubscribeToEvent( "SetPhysicsTimeStep" , Physics2D::SetPhysicsUpdateStep );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Physics2D::~Physics2D()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::Startup()
{
	s_timer->SetSeconds( s_clock , s_fixedTimeStep );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::BeginFrame()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::Update()
{
	
	while ( s_timer->CheckAndDecrement() )
	{
		for(int rigidBodyIndex=0; rigidBodyIndex<m_rigidBodies2D.size(); ++rigidBodyIndex )
		{
			if(!m_rigidBodies2D[rigidBodyIndex])
			{
				continue;
			}
			m_rigidBodies2D[ rigidBodyIndex ]->m_framePosition = m_rigidBodies2D[ rigidBodyIndex ]->GetPosition();
		}
		
		AdvanceSimulation( ( float ) s_fixedTimeStep );
		DetectCollisions();
		ResolveCollisions();
				
		for ( size_t colliderIndex = 0; colliderIndex < m_colliders2D.size(); ++colliderIndex )
		{
			if ( !m_colliders2D[ colliderIndex ] )
			{
				continue;
			}
			m_colliders2D[ colliderIndex ]->UpdateWorldShape();
		}
		
		//ResetCollisions();
 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::AdvanceSimulation( float deltaSeconds )
{
	Vec2 currentRigidBodyPos;
	for ( size_t index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( !m_rigidBodies2D[index] || !m_rigidBodies2D[index]->m_isSimulationActive )
		{
			continue;
		}

		m_rigidBodies2D[ index ]->Update( deltaSeconds );
		currentRigidBodyPos = m_rigidBodies2D[ index ]->GetPosition();

		ApplyEffectors( m_rigidBodies2D[index] , deltaSeconds );
		m_rigidBodies2D[ index ]->ApplyDrag( deltaSeconds );
		MoveRigidbodies( m_rigidBodies2D[ index ] , deltaSeconds );
		m_rigidBodies2D[ index ]->Update( deltaSeconds );
	}
	
	for ( size_t index = 0; index < m_rigidBodies2D.size(); index++ )
	{
		if ( !m_rigidBodies2D[ index ] )
		{
			continue;
		}

		Vec2 currentRigidBodyNewPos = m_rigidBodies2D[ index ]->GetPosition();
		Vec2 rbVerletVelocity = ( currentRigidBodyNewPos - currentRigidBodyPos ) / deltaSeconds;
		//m_rigidBodies2D[ index ]->SetVerletVelocity( rbVerletVelocity );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ApplyEffectors( Rigidbody2D* rigidbody , float deltaSeconds )
{

		eSimulationMode simulationMode = rigidbody->GetSimulationMode();
		Vec2 currentVelocity = rigidbody->GetVelocity();
		float currentRBMass  = rigidbody->GetMass();

		switch ( simulationMode )
		{
			case SIMULATIONMODE_STATIC:		rigidbody->SetVelocity( Vec2::ZERO );
											break;

			case SIMULATIONMODE_KINEMATIC:	/*m_rigidBodied2D[ index ]->SetVeloity( currentVelocity );*/
											break;

			case SIMULATIONMODE_DYNAMIC:	rigidbody->SetVelocity( currentVelocity + ( currentRBMass *  m_sceneGravity * deltaSeconds ) );
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
		if ( !m_colliders2D[ index ] )
		{
			continue;
		}
		
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

			if (  !firstRigidBody || !secondRigidBody  )
			{
				continue;
			}

			if ( firstRigidBody && secondRigidBody )
			{
				//g_theDevConsole->PrintString( RED , "Collider has no rigid body attached." );
				if ( firstRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC && secondRigidBody->GetSimulationMode() == SIMULATIONMODE_STATIC )
				{
					continue;
				}
			}

			if ( !firstRigidBody->m_isSimulationActive || !secondRigidBody->m_isSimulationActive )
			{
				continue;
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
// 	float myMass = collision.m_me->GetRigidBody()->GetMass();
// 	float theirMass = collision.m_them->GetRigidBody()->GetMass();
	
	if ( ( collision.m_me->GetRigidBody() == nullptr ) || ( collision.m_them->GetRigidBody() == nullptr ) )
	{
		return;
	}

	// 8 ways to move the object;
	
	if ( collision.CheckCollisionType() == DYNAMIC_VS_DYNAMIC || collision.CheckCollisionType() == KINEMATIC_VS_KINEMATIC )
	{
		collision.m_me->GetRigidBody()->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * 0.5f );
		collision.m_them->GetRigidBody()->Move( -collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap * 0.5f );
	}

	if ( collision.CheckCollisionType() == KINEMATIC_VS_STATIC || collision.CheckCollisionType() == DYNAMIC_VS_STATIC || collision.CheckCollisionType() == DYNAMIC_VS_KINEMATIC )
	{
		collision.m_me->GetRigidBody()->Move( collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap );
	}

	if ( collision.CheckCollisionType() == STATIC_VS_KINEMATIC || collision.CheckCollisionType() == STATIC_VS_DYNAMIC || collision.CheckCollisionType() == KINEMATIC_VS_DYNAMIC )
	{
		collision.m_them->GetRigidBody()->Move( -collision.m_collisionManifold.m_normal * collision.m_collisionManifold.m_overlap );
	}

	// 8 ways to apply impulse
	float friction = collision.m_me->GetFrictionWith( collision.m_them );
	
	Vec2 theRealImpulse = CalculateImpulse( collision );
	float impulseNormal = DotProduct2D( theRealImpulse , collision.m_collisionManifold.m_normal );
	float impulseTangent = DotProduct2D( theRealImpulse , collision.m_collisionManifold.m_normal.GetRotated90Degrees() );
		
	if ( abs( impulseTangent ) > friction* impulseNormal )
	{
		impulseTangent = SignFloat( impulseTangent ) * impulseNormal * friction;
	}

	if ( collision.CheckCollisionType() == DYNAMIC_VS_DYNAMIC   || collision.CheckCollisionType() == DYNAMIC_VS_KINEMATIC ||
		 collision.CheckCollisionType() == KINEMATIC_VS_DYNAMIC || collision.CheckCollisionType() == KINEMATIC_VS_KINEMATIC )
	{
		collision.m_me->GetRigidBody()->ApplyImpulse( theRealImpulse , collision.m_collisionManifold.m_contactPoint );
		collision.m_them->GetRigidBody()->ApplyImpulse( -theRealImpulse , collision.m_collisionManifold.m_contactPoint );		
		return;
	}

	if ( collision.CheckCollisionType() == STATIC_VS_KINEMATIC || collision.CheckCollisionType() == STATIC_VS_DYNAMIC )
	{
		collision.m_them->GetRigidBody()->ApplyImpulse( -theRealImpulse , collision.m_collisionManifold.m_contactPoint );
	}
	
	if ( collision.CheckCollisionType() == KINEMATIC_VS_STATIC || collision.CheckCollisionType() == DYNAMIC_VS_STATIC )
	{
		collision.m_me->GetRigidBody()->ApplyImpulse( theRealImpulse , collision.m_collisionManifold.m_contactPoint );
	}
}

Vec2 Physics2D::CalculateImpulse( Collision2D& collision )
{
	Vec2 contactPoint = collision.m_collisionManifold.m_contactPoint;

	Vec2 normal = collision.m_collisionManifold.m_normal;
	Vec2 tangent = -collision.m_collisionManifold.m_normal.GetRotatedMinus90Degrees();
	

	Vec2 meImpactVelocity = collision.m_me->GetRigidBody()->CalculateImpactVelocity( contactPoint );
	Vec2 themImpactVelocity = collision.m_them->GetRigidBody()->CalculateImpactVelocity( contactPoint );
	

	float friction = collision.m_me->GetFrictionWith( collision.m_them );
	
	float impulseNormal = -1 * ( 1 + collision.m_me->GetBounceWith( collision.m_them ) ) * DotProduct2D( ( meImpactVelocity - themImpactVelocity ) , normal );

	float impulseTangent = -1 * ( 1 + collision.m_me->GetBounceWith( collision.m_them ) ) * DotProduct2D( ( meImpactVelocity - themImpactVelocity ) , tangent );

	
	float factorMe = DotProduct2D( ( contactPoint - collision.m_me->GetRigidBody()->GetPosition() ).GetRotatedMinus90Degrees() , normal );
	factorMe *= factorMe;
	factorMe /= collision.m_me->GetRigidBody()->GetMoment();

	float factorThem = DotProduct2D( ( contactPoint - collision.m_them->GetRigidBody()->GetPosition() ).GetRotatedMinus90Degrees() , normal );
	factorThem *= factorThem;
	factorThem /= collision.m_them->GetRigidBody()->GetMoment();

	float denominator = 0.f;

	if ( collision.CheckCollisionType() == DYNAMIC_VS_DYNAMIC || collision.CheckCollisionType() == DYNAMIC_VS_KINEMATIC ||
		collision.CheckCollisionType() == KINEMATIC_VS_DYNAMIC || collision.CheckCollisionType() == KINEMATIC_VS_KINEMATIC )
	{
		denominator = ( 1.f / collision.m_me->GetRigidBody()->GetMass() ) + ( 1.f / collision.m_them->GetRigidBody()->GetMass() );
	}
	else if ( collision.CheckCollisionType() == STATIC_VS_KINEMATIC || collision.CheckCollisionType() == STATIC_VS_DYNAMIC )
	{
		denominator = ( 1.f / collision.m_them->GetRigidBody()->GetMass() );
	}
	else if ( collision.CheckCollisionType() == KINEMATIC_VS_STATIC || collision.CheckCollisionType() == DYNAMIC_VS_STATIC )
	{
		denominator = ( 1.f / collision.m_me->GetRigidBody()->GetMass() );
	}
	float denomCopy = denominator;
	
	denominator *= DotProduct2D( normal , normal );

	if ( collision.CheckCollisionType() == DYNAMIC_VS_DYNAMIC || collision.CheckCollisionType() == DYNAMIC_VS_KINEMATIC ||
		collision.CheckCollisionType() == KINEMATIC_VS_DYNAMIC || collision.CheckCollisionType() == KINEMATIC_VS_KINEMATIC )
	{
		denominator += factorMe + factorThem;
	}
	else if ( collision.CheckCollisionType() == STATIC_VS_KINEMATIC || collision.CheckCollisionType() == STATIC_VS_DYNAMIC )
	{
		denominator += factorThem;
	}
	else if ( collision.CheckCollisionType() == KINEMATIC_VS_STATIC || collision.CheckCollisionType() == DYNAMIC_VS_STATIC )
	{
		denominator += factorMe;
	}
	
	impulseNormal /= denominator;

	float factorMeTangent = DotProduct2D( ( contactPoint - collision.m_me->GetRigidBody()->GetPosition() ).GetRotatedMinus90Degrees() , tangent );
	factorMeTangent *= factorMeTangent;
	factorMeTangent /= collision.m_me->GetRigidBody()->GetMoment();

	float factorThemTangent = DotProduct2D( ( contactPoint - collision.m_them->GetRigidBody()->GetPosition() ).GetRotatedMinus90Degrees() , tangent );
	factorThemTangent *= factorThemTangent;
	factorThemTangent /= collision.m_them->GetRigidBody()->GetMoment();
	
	denomCopy   *= DotProduct2D( tangent , tangent );
	if ( collision.CheckCollisionType() == DYNAMIC_VS_DYNAMIC || collision.CheckCollisionType() == DYNAMIC_VS_KINEMATIC ||
		collision.CheckCollisionType() == KINEMATIC_VS_DYNAMIC || collision.CheckCollisionType() == KINEMATIC_VS_KINEMATIC )
	{
		denomCopy += factorMeTangent + factorThemTangent;
	}
	else if ( collision.CheckCollisionType() == STATIC_VS_KINEMATIC || collision.CheckCollisionType() == STATIC_VS_DYNAMIC )
	{
		denomCopy += factorThemTangent;
	}
	else if ( collision.CheckCollisionType() == KINEMATIC_VS_STATIC || collision.CheckCollisionType() == DYNAMIC_VS_STATIC )
	{
		denomCopy += factorMeTangent;
	}
	
	impulseTangent /= denomCopy;

	if ( abs( impulseTangent ) > friction* impulseNormal )
	{
		impulseTangent = SignFloat( impulseTangent ) * impulseNormal * friction;
	}
	
	impulseTangent = Clamp( impulseTangent , -friction * impulseNormal , friction * impulseNormal );
	impulseNormal = impulseNormal < 0 ? 0 : impulseNormal;
	   	
	return ( impulseNormal * normal ) + ( impulseTangent * tangent );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::ResolveCollisions()
{
	for ( size_t index = 0; index < m_frameCollisions.size(); index++ )
	{
		ResolveCollision( m_frameCollisions[index] );
	}
	
	//ResetCollisions();
	//m_frameCollisions.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::CleanupDestroyedObjects()
{
	CleanupDestroyedColliders();
	CleanupDestroyedRigidBodies();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::CleanupDestroyedRigidBodies()
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::CleanupDestroyedColliders()
{
	for ( size_t colliderIndex = 0; colliderIndex < m_colliders2D.size(); colliderIndex++ )
	{
		if ( !m_colliders2D[ colliderIndex ] )
		{
			continue;
		}
		
		if ( m_colliders2D[ colliderIndex ]->m_isGarbage == true )
		{
			delete m_colliders2D[ colliderIndex ];
			m_colliders2D[ colliderIndex ] = nullptr;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::EndFrame()
{
	for(int rigidBodyIndex=0; rigidBodyIndex<m_rigidBodies2D.size(); rigidBodyIndex++ )
	{
		if ( !m_rigidBodies2D[ rigidBodyIndex ] )
		{
			continue;
		}

		Vec2 verletVelocity = ( -m_rigidBodies2D[ rigidBodyIndex ]->m_framePosition + m_rigidBodies2D[ rigidBodyIndex ]->GetPosition() ) / ( float ) s_fixedTimeStep;
		m_rigidBodies2D[ rigidBodyIndex ]->SetVerletVelocity( verletVelocity );
		m_rigidBodies2D[ rigidBodyIndex ]->m_frameTorque = 0.f;
	}
	
	CleanupDestroyedObjects();
	ResetCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Physics2D::Shutdown()
{
	delete s_timer;
	delete s_clock;
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

void Physics2D::SetFixedStepTime( double newTimeStep )
{
	s_fixedTimeStep = newTimeStep;
	s_timer->SetSeconds( s_clock , s_fixedTimeStep );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Physics2D::SetPhysicsUpdateStep( EventArgs& args )
{
	//g_theDevConsole
	double dt = ( double ) args.GetValue( "StepFrequency" , ( float ) s_fixedTimeStep );
	SetFixedStepTime( ( double ) 1.0/dt );
	return false;
}


//--------------------------------------------------------------------------------------------------------------------------------------------