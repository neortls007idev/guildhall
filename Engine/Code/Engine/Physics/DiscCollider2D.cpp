#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D::DiscCollider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType ,
	Vec2 localPosition , float radius ) :
											m_system( system ) ,
											m_rigidbody( rigidbody ) ,
											m_colliderType( colliderType ) ,
											m_localPosition( localPosition ) ,
											m_radius( radius )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

DiscCollider2D::~DiscCollider2D()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------
