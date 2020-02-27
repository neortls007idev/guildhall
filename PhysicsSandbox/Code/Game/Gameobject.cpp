#include "Game/Gameobject.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::GameObject( Physics2D* system , Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius ) :
																													m_physiscsSystem( system )
{
	m_rigidbody = m_physiscsSystem->CreateRigidbody( rigidBodyPosition , coliderPositionRelativeToRigidBody , ColliderRadius );
}


GameObject::GameObject( Physics2D* system , Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , Polygon2D convexgon ) :
																														m_physiscsSystem( system )
{
	m_rigidbody = m_physiscsSystem->CreateRigidbody( rigidBodyPosition , coliderPositionRelativeToRigidBody , convexgon );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject::~GameObject()
{
	// delete m_rigidbody; // this should be an error if setup right
	m_rigidbody->Destroy(); // destroys through the system;  

}

//--------------------------------------------------------------------------------------------------------------------------------------------