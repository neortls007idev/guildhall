#pragma once
#include <Engine/Physics/Collider2D.hpp>
#include <Engine/Math/Vec2.hpp>

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class Physics2D;
class Rigidbody2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

class DiscCollider2D : public Collider2D
{
public: 
	DiscCollider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType, Vec2 localPosition , float radius = 1.f );
	virtual ~DiscCollider2D() override;

public:
	Vec2	m_localPosition; // my local offset from my parent
	Vec2	m_worldPosition; // calculated from local position and owning rigidbody if present
	float	m_radius;
};

//--------------------------------------------------------------------------------------------------------------------------------------------