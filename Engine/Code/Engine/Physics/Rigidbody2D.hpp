#pragma once
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Rigidbody2D
{
	friend class Physics2D;

public:
	Rigidbody2D( Physics2D* system , Vec2 worldPosition , Collider2D* collider );
	Rigidbody2D( Physics2D* system , Vec2 worldPosition );
	void Destroy();																	// helper for destroying myself (uses owner to mark self for destruction)
	
	void TakeCollider( Collider2D* collider );										// takes ownership of a collider (destroying my current one if present)
	Collider2D* GetCollider() const						{ return m_collider; }
	void SetCollider( Collider2D* collider );
	void SetPosition( Vec2 position );

public:
	Physics2D*	m_system			= nullptr;										// which scene created/owns this object
	Collider2D* m_collider			= nullptr;

	Vec2		m_worldPosition;													// where in the world is this rigidbody
	bool		m_isGarbage			= false;

private:
	~Rigidbody2D();																	// destroys the collider
};