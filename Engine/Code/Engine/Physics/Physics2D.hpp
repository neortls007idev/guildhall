#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Physics2D
{
public:
	void BeginFrame();
	void Update( float deltaSeconds );
	void EndFrame();    // cleanup destroyed objects

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void DestroyRigidbody( Rigidbody2D* rigidbody );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition , float radius );
	void DestroyCollider( Collider2D* collider );

public:
	// add members you may need to store these
	// storage for all rigid bodies
	std::vector<Rigidbody2D*>	m_rigidBodied2D;
	// storage for all colliders
	std::vector<Collider2D*>	m_colliders2D;
	// ...
};

//--------------------------------------------------------------------------------------------------------------------------------------------