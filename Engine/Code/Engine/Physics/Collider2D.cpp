#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/Collider2D.hpp"

#include "Engine/Core/DebugRender.hpp"
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
// 		Vec2 closetPoint = polyColliderThem->GetClosestPoint( discColliderMe->GetPosition() );
// 
// 		return ( IsPointOnDisc2D( Disc2D( discColliderMe->GetPosition() , discColliderMe->GetRadius() ) , closetPoint ) 
// 			|| polyColliderThem->Contains( discColliderMe->GetPosition() ) );

		Vec2 nearestPoint = polyColliderThem->GetClosestPoint( discColliderMe->GetPosition() );
		if ( IsPointOnDisc2D( nearestPoint , discColliderMe->GetPosition() , discColliderMe->GetRadius() ) )
		{
			return true;
		}
		return false;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool PolygonVPolygonCollisionCheck( Collider2D const* me , Collider2D const* them )
{
	PolygonCollider2D* mePoly = ( PolygonCollider2D* ) me;
	PolygonCollider2D* themPoly = ( PolygonCollider2D* ) them;
	return DetectPolygonvPolygonIntersections( mePoly->m_polygon , themPoly->m_polygon );
	//return GJKPolygonPolygonIntersectionTest( mePoly->m_polygon , themPoly->m_polygon );
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
	collision.m_contactPoint = discColliderMe->GetPosition() - ( collision.m_normal * discColliderMe->GetRadius() ) + ( collision.m_normal * collision.m_overlap * 0.5f ) ;

	return collision;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Manifold2D DiscVPolygonCollisionFold( Collider2D const* me , Collider2D const* them )
{
	Manifold2D collision;
	DiscCollider2D* discColliderMe = ( DiscCollider2D* ) me;
	PolygonCollider2D* polyColliderThem = ( PolygonCollider2D* ) them;
	Vec2 closetPoint = polyColliderThem->GetClosestPoint( discColliderMe->m_worldPosition );

	collision.m_normal = ( discColliderMe->m_worldPosition - closetPoint ).GetNormalized();
	collision.m_overlap = discColliderMe->m_radius - ( discColliderMe->m_worldPosition - closetPoint ).GetLength();

	if ( discColliderMe->Contains( closetPoint ) )
	{
		closetPoint = polyColliderThem->m_polygon.GetClosestPointOnEdges( closetPoint );
		collision.m_normal = ( closetPoint - discColliderMe->m_worldPosition ).GetNormalized();
		collision.m_overlap = -( closetPoint - discColliderMe->m_worldPosition ).GetLength() + discColliderMe->m_radius;
		collision.m_normal = collision.m_normal.GetRotatedDegrees( 180.f );
	}

	if ( polyColliderThem->Contains( discColliderMe->m_worldPosition ) )
	{
		collision.m_normal = -collision.m_normal;
		collision.m_overlap = ( discColliderMe->m_worldPosition - closetPoint ).GetLength() + discColliderMe->m_radius;
	}

	collision.m_contactPoint = discColliderMe->m_worldPosition - 
								( collision.m_normal * ( discColliderMe->m_radius - ( collision.m_overlap * 0.5f ) ) );	return collision;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Manifold2D PolygonVPolygonCollisionFold( Collider2D const* me , Collider2D const* them )
{
	PolygonCollider2D* mePoly = ( PolygonCollider2D* ) me;
	PolygonCollider2D* themPoly = ( PolygonCollider2D* ) them;

	Polygon2D minkowskiDiff = GenerateMinkowskiDifferencePolygon( &mePoly->m_polygon , &themPoly->m_polygon );
	minkowskiDiff = minkowskiDiff.MakeConvexFromPointCloud( &minkowskiDiff.m_points[ 0 ] , ( uint ) minkowskiDiff.m_points.size() );
	Vec2 contactPointInMinjowskiSpace = minkowskiDiff.GetClosestPointOnEdges( Vec2::ZERO );

	Manifold2D collisionManifold;
	Vec2 mePolyCenter = mePoly->m_polygon.GetCenter(); /*AveragePoint( &mePoly->m_polygon.m_points[ 0 ] , mePoly->m_polygon.m_points.size() );*/
						
	Vec2 themPolyCenter = themPoly->m_polygon.GetCenter(); /*AveragePoint( &themPoly->m_polygon.m_points[ 0 ] , themPoly->m_polygon.m_points.size() );*/

	collisionManifold.m_overlap = contactPointInMinjowskiSpace.GetLength();
	collisionManifold.m_normal = contactPointInMinjowskiSpace.GetNormalized();

	size_t index = GetIndexOfFurthestPoint( &mePoly->m_polygon.m_points[ 0 ] , mePoly->m_polygon.m_points.size() , -collisionManifold.m_normal );

	
	//Vec2 contactPoint = themPoly->m_polygon.GetClosestPointOnEdges( themPolyCenter - collisionManifold.m_normal * collisionManifold.m_overlap );
	Vec2 contactPoint = themPoly->m_polygon.GetClosestPointOnEdges( mePoly->m_polygon.m_points[ index ] );
	//Vec2 contactPoint = mePoly->m_polygon.GetClosestPointOnEdges( themPolyCenter - collisionManifold.m_normal * collisionManifold.m_overlap );
	
	collisionManifold.m_contactPoint = contactPoint;/*themPolyCenter + contactPointInMinjowskiSpace;*//*collisionManifold.m_normal * collisionManifold.m_overlap*/;
	
	DebugAddScreenPoint( collisionManifold.m_contactPoint , 5.f , CYAN );
	DebugAddScreenPoint( contactPointInMinjowskiSpace , 5.f , MAGENTA );
	//DebugAddScreenPoint(  , 5.f , CYAN );
// 	DebugAddScreenArrow( mePolyCenter , BLUE , GREEN , mePolyCenter + collisionManifold.m_normal * 50.f ,
// 					BLUE , GREEN ,5.f );
// 	DebugAddScreenArrow( mePolyCenter , BLUE , GREEN , contactPoint ,
// 		BLUE , GREEN , 5.f );
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

void Collider2D::Destroy()
{
	m_isGarbage = true;
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

	return meFriction * otherFriction;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Collider2D::~Collider2D()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------