#include "Engine/Primitives/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

int CheckOrientation( Vec2 pointP , Vec2 pointQ , Vec2 pointR )
{
	int val = ( int ) ( ( pointQ.y - pointP.y ) * ( pointR.x - pointQ.x ) - ( pointQ.x - pointP.x ) * ( pointR.y - pointQ.y ) );

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
		size_t totalPoints = m_points.size();

		for ( size_t index = 1; index <	totalPoints; index++ )
		{
			nextEdge = m_points[ index % totalPoints ] - m_points[ ( index - 1 ) % totalPoints ];
			if ( DotProduct2D( edgeNormal , nextEdge ) <= 0 )
			{
				return false;
			}
			edge = m_points[ index % totalPoints ] - m_points[ ( index - 1 ) % totalPoints ];
			edgeNormal = edge.GetRotated90Degrees().GetNormalized();
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
	size_t totalPoints = m_points.size();
	outStart = &m_points[ idx & totalPoints ];
	outEnd = &m_points[ ( idx + 1 ) % totalPoints ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Polygon2D::SetCenter()
{
// 	if ( m_points.size() == 0 )
// 	{
// 		return;
// 	}

	DebuggerPrintf( "\n%d\n" , m_points.size() );
	m_center = Vec2::ZERO;

	for ( size_t index = 0; index < m_points.size(); index++ )
	{
		m_center += m_points[ index ];
	}

	m_center *= ( 1.f / m_points.size() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Polygon2D::SetNewCenter( Vec2 newCenter )
{
	m_center = newCenter;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Polygon2D::SetPosition( Vec2 newPos )
{
	for ( size_t index = 0 ; index < m_points.size() ; index++ )
	{
		m_points[ index ] += newPos - m_localPos;
	}

	m_localPos = newPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Polygon2D Polygon2D::MakeFromLineLoop( Vec2 const* points , uint pointCount )
{
	Polygon2D temp;
	
	for ( unsigned int index = 0; index < pointCount; index++ )
	{
		temp.m_points.push_back( points[ index ] );
	}
	
	Vec2 center = Vec2( 0.f , 0.f );
	
	for ( int index = 0; index < temp.m_points.size(); index++ )
	{
		center += points[ index ];
	}
	temp.m_center = center / ( float ) temp.m_points.size();
	
	if ( temp.IsValid() )
	{
		return temp;
	}
	else
	{
		ERROR_AND_DIE( "Invalid points" );
	}
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

	tempPolygon.SetCenter();

	return tempPolygon;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Polygon2D::Polygon2D( Polygon2D& copy )
{
	for ( size_t index = 0; index < copy.m_points.size(); index++ )
	{
		m_points.push_back( copy.m_points[ index ] );
	}
	SetCenter();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Polygon2D::Polygon2D()
{
	SetCenter();
}

//--------------------------------------------------------------------------------------------------------------------------------------------