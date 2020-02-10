#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Primitives/Polygon2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class PolygonCollider2D : public Collider2D
{
public:
	PolygonCollider2D( Physics2D* system , Rigidbody2D* rigidbody , Vec2 localPosition );
	~PolygonCollider2D() {};

	virtual void UpdateWorldShape() override;
	virtual void Destroy() override;

	virtual Vec2 GetClosestPoint( Vec2 pos ) const override;
	virtual bool Contains( Vec2 pos ) const override;
	virtual bool Intersects( Collider2D const* other ) const override;

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) override;
	virtual Vec2 GetPosition() const override;

	// accessors

public: 
	Polygon2D* m_polygon;
	
private:
};