#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

collisionCheckCB g_collisionChecks[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] = {
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscCollisionCheck,      nullptr,
	/* polygon */  DiscVPolygonCollisionCheck,   PolygonVPolygonCollisionCheck
};

//--------------------------------------------------------------------------------------------------------------------------------------------

collisionManifoldCB g_collisionManifold[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] = {
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscCollisionManiFold,      nullptr,
	/* polygon */  DiscVPolygonCollisionFold,   PolygonVPolygonCollisionFold
};

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DiscVDiscCollisionCheck( Collider2D const* me , Collider2D const* them )
{
	DiscCollider2D* discColliderMe		= ( DiscCollider2D* ) me;
	DiscCollider2D* discColliderThem	= ( DiscCollider2D* ) them;

	return DoDiscsOverlap( discColliderMe->GetPosition() , discColliderMe->GetRadius() ,
						   discColliderThem->GetPosition() , discColliderThem->GetRadius() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DiscVPolygonCollisionCheck( Collider2D const* me , Collider2D const* them )
{
	DiscCollider2D*		discColliderMe		= ( DiscCollider2D* ) me;
	PolygonCollider2D*	polyColliderThem	= ( PolygonCollider2D* ) them;

	bool earlyOut = DoDiscsOverlap( discColliderMe->GetPosition() , discColliderMe->GetRadius() ,
		polyColliderThem->m_boundingDisc.m_center , polyColliderThem->m_boundingDisc.m_radius );

	if ( earlyOut )
	{
		Vec2 closetPoint = polyColliderThem->GetClosestPoint( discColliderMe->GetPosition() );

		return ( IsPointOnDisc2D( Disc2D( discColliderMe->GetPosition() , discColliderMe->GetRadius() ) , closetPoint ) 
			|| polyColliderThem->Contains( discColliderMe->GetPosition() ) );
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool PolygonVPolygonCollisionCheck( Collider2D const* me , Collider2D const* them )
{
	UNUSED( me );
	UNUSED( them );
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Manifold2D DiscVDiscCollisionManiFold( Collider2D const* me , Collider2D const* them )
{
	Manifold2D collision;

	DiscCollider2D* discColliderMe = ( DiscCollider2D* ) me;
	DiscCollider2D* discColliderThem = ( DiscCollider2D* ) them;

	Vec2 displacementBetweenCenters = Vec2( discColliderThem->GetPosition() - discColliderMe->GetPosition() );
	float distanceBetweenCenters = displacementBetweenCenters.GetLength();

	collision.m_overlap			= discColliderMe->GetRadius() + discColliderThem->GetRadius() - distanceBetweenCenters;
	if ( collision.m_overlap <= 0 )
	{
		collision.m_overlap = 0.f;
		return collision;
	}
	collision.m_normal			= ( discColliderMe->GetPosition() - discColliderThem->GetPosition()).GetNormalized();
	collision.m_contactPoint = discColliderMe->GetPosition() + ( collision.m_normal * ( discColliderMe->GetRadius() - ( collision.m_overlap * 0.5f ) ) );

	return collision;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Manifold2D DiscVPolygonCollisionFold( Collider2D const* me , Collider2D const* them )
{
	Manifold2D collision;

	DiscCollider2D*		discColliderMe		= ( DiscCollider2D* ) me;
	PolygonCollider2D*	polyColliderThem	= ( PolygonCollider2D* ) them;


	bool earlyOut = DoDiscsOverlap( discColliderMe->GetPosition() , discColliderMe->GetRadius() ,
				polyColliderThem->m_boundingDisc.m_center , polyColliderThem->m_boundingDisc.m_radius );

	if ( earlyOut )
	{
		Vec2 closetPoint = polyColliderThem->GetClosestPoint( discColliderMe->GetPosition() );

		collision.m_normal = ( discColliderMe->GetPosition() - closetPoint ).GetNormalized();
		collision.m_overlap = discColliderMe->GetRadius() - ( discColliderMe->GetPosition() - closetPoint ).GetLength();

		if ( polyColliderThem->Contains( discColliderMe->GetPosition() ) )
		{
			collision.m_normal = -collision.m_normal;
			collision.m_overlap = ( discColliderMe->GetPosition() - closetPoint ).GetLength() +  discColliderMe->GetRadius();/*= discColliderMe->GetRadius() + ( discColliderMe->GetPosition() ).GetLength();*/
		}

		collision.m_contactPoint = discColliderMe->GetPosition() + ( collision.m_normal * ( discColliderMe->GetRadius() - ( collision.m_overlap * 0.5f ) ) );
	}

	return collision;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Manifold2D PolygonVPolygonCollisionFold( Collider2D const* me , Collider2D const* them )
{
	UNUSED( me );
	UNUSED( them );
	Manifold2D collisionManifold;
	return collisionManifold;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Collider2D::Collider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType ) :
																						m_system( system ),
																						m_rigidbody( rigidbody ),
																						m_colliderType( colliderType )
{
	m_physicsMaterial = new PhysicsMaterial();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Collider2D::Intersects( Collider2D const* other ) const
{
	COLLIDER2D_TYPE myType = GetType();
	COLLIDER2D_TYPE otherType = other->GetType();



	if ( myType <= otherType )
	{
		int idx = otherType * NUM_COLLIDER_TYPES + myType;
		collisionCheckCB check = g_collisionChecks[ idx ];
		return check( this , other );
	}
	else
	{
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPES + otherType;
		collisionCheckCB check = g_collisionChecks[ idx ];
		return check( other , this );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Manifold2D Collider2D::GenrateManifold( Collider2D const* other )
{
	COLLIDER2D_TYPE myType = GetType();
	COLLIDER2D_TYPE otherType = other->GetType();

	if ( myType <= otherType )
	{
		int idx = otherType * NUM_COLLIDER_TYPES + myType;
		collisionManifoldCB manifold = g_collisionManifold[ idx ];
		return manifold( this , other );
	}
	else
	{
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPES + otherType;
		collisionManifoldCB manifold = g_collisionManifold[ idx ];
		return manifold( other , this );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Collider2D::GetBounceWith( Collider2D const* other ) const
{
	float meBounciness	  = m_physicsMaterial->GetBounciness();
	float otherBounciness = other->GetPhysicsMaterial()->GetBounciness();

	return ( otherBounciness * meBounciness );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Collider2D::GetFrictionWith( Collider2D const* other ) const
{
	float meFriction = m_physicsMaterial->GetFriction();
	float otherFriction = other->GetPhysicsMaterial()->GetFriction();

	return ( otherFriction * meFriction );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Collider2D::~Collider2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------