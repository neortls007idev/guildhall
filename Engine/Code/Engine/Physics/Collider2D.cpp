#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

collisionCheckCB g_collisionChecks[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] = {
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscCollisionCheck,      PolygonVDiscCollisionCheck,
	/* polygon */  DiscVPolygonCollisionCheck,   PolygonVPolygonCollisionCheck
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

	Vec2 closetPoint = polyColliderThem->GetClosestPoint( discColliderMe->GetPosition() );

	return ( IsPointOnDisc2D( Disc2D( discColliderMe->GetPosition() , discColliderMe->GetRadius() ) , closetPoint ) ||
				polyColliderThem->Contains( discColliderMe->GetPosition() ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool PolygonVDiscCollisionCheck( Collider2D const* me , Collider2D const* them )
{
	DiscVPolygonCollisionCheck( them , me );
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

Collider2D::Collider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType ) :
																						m_system( system ),
																						m_rigidbody( rigidbody ),
																						m_colliderType( colliderType )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Collider2D::Intersects( Collider2D const* other ) const
{
	collisionCheckCB collisionCheck = g_collisionChecks[ m_colliderType * other->GetType() ];
	return collisionCheck( this , other );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Collider2D::~Collider2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------