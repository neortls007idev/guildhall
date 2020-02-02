#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "../Math/MathUtils.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D::DiscCollider2D( Physics2D* system , Rigidbody2D* rigidbody ,Vec2 localPosition , float radius ) :
																									Collider2D( system , rigidbody , COLLIDER2D_DISC ),
																									m_localPosition( localPosition ) ,
																									m_radius( radius )
{
	m_worldPosition = m_rigidbody->m_worldPosition + m_localPosition;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D::~DiscCollider2D()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DiscCollider2D::Destroy()
{
	m_isGarbage = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DiscCollider2D::UpdateWorldShape()
{
	m_worldPosition = m_rigidbody->m_worldPosition + m_localPosition;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 DiscCollider2D::GetClosestPoint( Vec2 pos ) const
{
	Vec2 nearestPoint = GetNearestPointOnDisc2D( pos , m_worldPosition , m_radius );
	return nearestPoint;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DiscCollider2D::Contains( Vec2 pos ) const
{
	return IsPointOnDisc2D( Disc2D( m_worldPosition , m_radius ) , pos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DiscCollider2D::Intersects( Collider2D const* other ) const
{
	DiscCollider2D* temp = ( DiscCollider2D* ) other;
	return DoDiscsOverlap( m_worldPosition , m_radius , temp->m_worldPosition , temp->m_radius );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DiscCollider2D::DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor )
{
	ctx->DrawDisc( m_worldPosition , m_radius , fillColor );
	ctx->DrawRing( m_worldPosition , m_radius - ( 0.05f * m_radius ) , borderColor , 0.1f * m_radius );
	// Reducing half the thickness on the border radius so that the drawing itself does not feel buggy.
}

//--------------------------------------------------------------------------------------------------------------------------------------------
