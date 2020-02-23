#include "Engine/Physics/Collision2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Collision2D::Collision2D()
{
	m_me = nullptr;
	m_them = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Collision2D::Collision2D( const Collision2D& collision ) :	m_me( collision.m_me ) ,
															m_them( collision.m_them ) ,
															m_collisionManifold( collision.m_collisionManifold )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Collision2D::~Collision2D()
{
	m_me = nullptr;
	m_them = nullptr;
}
