#include "Engine/Physics/Collider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Collider2D::Collider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType ) : 
																						m_system( system ),
																						m_rigidbody( rigidbody ),
																						m_colliderType( colliderType )
{

}
