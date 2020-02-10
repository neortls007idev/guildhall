#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class Physics2D;
class Rigidbody2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum COLLIDER2D_TYPE
{
	COLLIDER2D_DISC ,
	COLLIDER2D_CONVEXGON,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Collider2D
{
	friend class Physics2D;

public:
	Collider2D( Physics2D* system , Rigidbody2D* rigidbody , COLLIDER2D_TYPE colliderType = COLLIDER2D_DISC );

	virtual void UpdateWorldShape() = 0;
	virtual void Destroy() = 0;

	virtual Vec2 GetClosestPoint( Vec2 pos ) const = 0;
	virtual bool Contains( Vec2 pos ) const = 0;
	virtual bool Intersects( Collider2D const* other ) const = 0;

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) = 0;
	virtual Vec2 GetPosition() const = 0;

	uint GetType() const																								{ return m_colliderType; }

protected:
	virtual ~Collider2D(); // private - make sure this is virtual so correct deconstructor gets called

public: 

public:
	COLLIDER2D_TYPE		m_colliderType;				// keep track of the type - will help with collision later
	Physics2D*			m_system;                   // system who created or destr
	Rigidbody2D*		m_rigidbody = nullptr;		// owning rigidbody, used for calculating world shape
	bool				m_isGarbage = false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------