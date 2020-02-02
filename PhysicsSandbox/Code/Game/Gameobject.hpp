#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Rigidbody2D;
class Physics2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameObject
{
public:
	GameObject( Physics2D* system , Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius );
//	GameObject( Physics2D* system , Rigidbody2D* rigidbody );
	~GameObject();

public:
	Physics2D*		m_physiscsSystem	= nullptr;
	Rigidbody2D*	m_rigidbody			= nullptr;
};
