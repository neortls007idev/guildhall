#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Rigidbody2D;
class Physics2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameObject
{
public:
	GameObject( Physics2D* system , Vec2 rigidBodyPosition , Vec2 coliderPositionRelativeToRigidBody , float ColliderRadius );

	~GameObject();

public:
	Physics2D*		m_physiscsSystem	= nullptr;
	Rigidbody2D*	m_rigidbody			= nullptr;
	Rgba8			m_fillColor			= Rgba8( 255 , 255 , 255 , 127 );
	Rgba8			m_borderColor		= BLUE;
	
	bool			m_isSelected		= false;
	bool			m_isColliding		= false;
};
