#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Primitives/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

PolygonCollider2D::PolygonCollider2D( Physics2D* system , Rigidbody2D* rigidbody , Vec2 localPosition ) :
																							Collider2D( system , rigidbody , COLLIDER2D_DISC )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PolygonCollider2D::UpdateWorldShape()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PolygonCollider2D::Destroy()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 PolygonCollider2D::GetClosestPoint( Vec2 pos ) const
{
	if ( Contains( pos ) )
	{
		return pos;
	}
	else
	{
		std::vector<Vec2> nearestPointOnEdge;
		size_t totalPoints = m_polygon->m_points.size();
		
		for ( size_t index = 0; index < totalPoints; index++ )
		{
			nearestPointOnEdge.push_back( GetNearestPointOnLineSegment2D( pos , m_polygon->m_points[ index % totalPoints ] , m_polygon->m_points[ ( index + 1 ) % totalPoints ] ) );
		}

		Vec2 nearestPoint = nearestPointOnEdge[0];

		for ( size_t index = 1; index < nearestPointOnEdge.size(); index++ )
		{
			float lengthOfNearestPoint = ( pos - nearestPoint ).GetLengthSquared();
			float lengthOfNearestPointOnEdge = ( pos - nearestPointOnEdge[ index ] ).GetLengthSquared();

			if ( lengthOfNearestPoint > lengthOfNearestPointOnEdge )
			{
				nearestPoint = nearestPointOnEdge[ index ];
			}
		}
		return nearestPoint;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool PolygonCollider2D::Contains( Vec2 pos ) const
{
	if ( 3 > m_polygon->m_points.size() )
	{
		return false;
	}
	else
	{
		Vec2 edge = Vec2::ZERO;
		Vec2 edgeNormal;
		Vec2 pointRelativePos;
		size_t index = 1;

		for ( ; index < m_polygon->m_points.size(); index++ )
		{
			edge = m_polygon->m_points[ index ] - m_polygon->m_points[ index - 1 ];
			edgeNormal = edge.GetRotated90Degrees().GetNormalized();
			//edgeNormal.Normalize();
			pointRelativePos = pos - m_polygon->m_points[ index - 1 ];

			if ( DotProduct2D( edgeNormal , pointRelativePos ) <= 0 )
			{
				return false;
			}
		}

		edge = m_polygon->m_points[ 0 ] - m_polygon->m_points[ index - 1 ];
		edgeNormal = edge.GetRotated90Degrees().GetNormalized();
		//edgeNormal.Normalize();
		pointRelativePos = pos - m_polygon->m_points[ 0 ];
		if ( DotProduct2D( edgeNormal , pointRelativePos ) <= 0 )
		{
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool PolygonCollider2D::Intersects( Collider2D const* other ) const
{
	

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PolygonCollider2D::DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor )
{
	ctx->DrawPolygon( &m_polygon->m_points[ 0 ] , ( unsigned int ) m_polygon->m_points.size() , fillColor );
	
	size_t totalPoints = m_polygon->m_points.size();

	for ( size_t index = 0; index < totalPoints; index++ )
	{
		size_t startPoint	= index % totalPoints;
		size_t endPoint		= ( index + 1 ) % totalPoints;
		ctx->DrawLine( m_polygon->m_points[ startPoint ] , m_polygon->m_points[ endPoint ] , borderColor , 5.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 PolygonCollider2D::GetPosition() const
{
	return Vec2::ZERO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------