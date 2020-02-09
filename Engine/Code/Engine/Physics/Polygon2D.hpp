#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Polygon2D /*: public Collider2D*/
{
public:
	Polygon2D() {};
	Polygon2D( Polygon2D &copy );
	Polygon2D( Physics2D* system , Rigidbody2D* rigidbody , Vec2 localPosition );
	~Polygon2D() {};

	virtual void UpdateWorldShape() /*override*/;
	virtual void Destroy() /*override*/;

	virtual Vec2 GetClosestPoint( Vec2 pos ) const /*override*/;
	virtual bool Contains( Vec2 pos ) const /*override*/;
	virtual bool Intersects( Collider2D const* other ) const /*override*/;

	virtual void DebugRender( RenderContext* ctx , Rgba8 const& borderColor , Rgba8 const& fillColor ) /*override*/;
	virtual Vec2 GetPosition() const /*override*/;


	bool IsValid() const; // must have at least 3 points to be considered a polygon
	bool IsConvex() const;
	float GetDistance( Vec2 point ) const;

	// accessors
	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge( int idx , Vec2* outStart , Vec2* outEnd );

public: // static constructors (feel free to just use a constructor - I just like descriptive names)
		// in this case, these two take the same parameters but behave differently

	// construct from a counter-clockwise line loop
	static Polygon2D MakeFromLineLoop( Vec2 const* points , uint pointCount );

	// create a convex wrapping of a collection of points;  
	static Polygon2D MakeConvexFromPointCloud( Vec2 const* points , uint pointCount );

	std::vector<Vec2> m_points;
private:
};