#include "Game/Gameobject.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::GameObject( Physics2D* system ) : 
												m_physiscsSystem( system )
{
// 	if ( !m_rigidbody )
// 	{
// 		m_rigidbody = new Rigidbody2D(m_physiscsSystem,)
// 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::GameObject( Physics2D* system , Rigidbody2D* rigidbody ) :
																		m_physiscsSystem( system ) ,
																		m_rigidbody( rigidbody )
{
	if ( !m_rigidbody )
	{
		//m_rigidbody = new Rigidbody2D(m_physiscsSystem,)
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::~GameObject()
{
	// delete m_rigidbody; // this should be an error if setup right
	m_rigidbody->Destroy(); // destroys through the system;  
	m_rigidbody = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------