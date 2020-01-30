#pragma once
#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D;
class Rigidbody2D;
class Collider2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Physics2D
{
public:
	void BeginFrame();
	void Update();
	void EndFrame();    // cleanup destroyed objects

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void DestroyRigidbody( Rigidbody2D* rb );

	DiscCollider2D* CreateDiscCollider( Vec2 localPosition , float radius );
	void DestroyCollider( Collider2D* collider );

public:
	// add members you may need to store these
	// storage for all rigid bodies
	// storage for all colliders
	// ...
};

//--------------------------------------------------------------------------------------------------------------------------------------------