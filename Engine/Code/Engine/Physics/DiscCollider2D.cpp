#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/EngineCommon.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D::DiscCollider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType ,
	Vec2 localPosition , float radius ) :	Collider2D( system , rigidbody , COLLIDER2D_DISC ),
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

void DiscCollider2D::UpdateWorldShape()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 DiscCollider2D::GetClosestPoint( Vec2 pos ) const
{
	return pos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DiscCollider2D::Contains( Vec2 pos ) const
{
	UNUSED( pos );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DiscCollider2D::Intersects( Collider2D const* other ) const
{
	UNUSED( other );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DiscCollider2D::DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
