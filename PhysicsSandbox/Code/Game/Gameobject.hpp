

class Rigidbody2D;
class Physics2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameObject
{
public:
	GameObject( Physics2D* system );
	GameObject( Physics2D* system , Rigidbody2D* rigidbody );
	~GameObject();

public:
	Physics2D*		m_physiscsSystem	= nullptr;
	Rigidbody2D*	m_rigidbody			= nullptr;
};
