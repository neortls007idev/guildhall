#include "Engine/Primitives/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

int CheckOrientation( Vec2 pointP , Vec2 pointQ , Vec2 pointR )
{
	int val = ( int ) ( pointQ.y - pointP.y ) * ( pointR.x - pointQ.x ) - ( pointQ.x - pointP.x ) * ( pointR.y - pointQ.y );

	if ( val == 0 )
	{
		return 0; // colinear
	}

	return ( val > 0 ) ? 1 : 2; // clock or counterclock wise
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Polygon2D::IsValid() const
{
	if ( 3 > m_points.size() )
	{
		return false;
	}

	return IsConvex();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Polygon2D::IsConvex() const
{
	if ( 3 > m_points.size() )
	{
		return false;
	}
	else
	{
		Vec2 edge	= m_points[ 1 ] - m_points[ 0 ];;
		Vec2 edgeNormal = edge.GetRotated90Degrees().GetNormalized();
		Vec2 nextEdge;
		size_t index = 2;

		for ( ; index < m_points.size(); index++ )
		{
			nextEdge = m_points[ index ] - m_points[ index - 1 ];
			if ( DotProduct2D( edgeNormal , nextEdge ) < 0 )
			{
				return false;
			}
			edge = m_points[ index ] - m_points[ index - 1 ];
			edgeNormal = edge.GetRotated90Degrees().GetNormalized();
		}

		edge = m_points[ index - 1 ] - m_points[ 0 ];
		nextEdge = m_points[ 0 ] - m_points[ 1 ];
		edgeNormal = edge.GetRotated90Degrees().GetNormalized();
		
		if ( DotProduct2D( nextEdge , edgeNormal ) < 0 )
		{
			return false;
		}

	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Polygon2D::GetVertexCount() const
{
	return ( int ) m_points.size();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Polygon2D::GetEdgeCount() const
{
	return ( int ) m_points.size() - 1;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Polygon2D::GetEdge( int idx , Vec2* outStart , Vec2* outEnd )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Polygon2D Polygon2D::MakeFromLineLoop( Vec2 const* points , uint pointCount )
{
	Polygon2D temp;
	return temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Polygon2D Polygon2D::MakeConvexFromPointCloud( Vec2 const* points , uint pointCount )
{

	const int start = GetIndexOfLeftMostPointFromPointCloud( points , pointCount );
	int point = start , nextPoint;
	
	Polygon2D tempPolygon;

	do
	{
		// Search for a point 'q' such that orientation(p, i, q) is
		// counterclockwise for all points 'i'
		nextPoint = ( point + 1 ) % pointCount;

		for ( uint index = 0; index < pointCount; index++ )
		{
			if ( CheckOrientation( points[ point ] , points[ index ] , points[ nextPoint ] ) == 2 )
			{
				nextPoint = index;
			}
		}

		tempPolygon.m_points.push_back( points[ nextPoint ] ); // Add q to result as a next point of p
		point = nextPoint; // Set p as q for next iteration

	} while ( point != start );

	return tempPolygon;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Polygon2D::Polygon2D( Polygon2D& copy )
{
	for ( size_t index = 0; index < copy.m_points.size(); index++ )
	{
		m_points.push_back( copy.m_points[ index ] );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------