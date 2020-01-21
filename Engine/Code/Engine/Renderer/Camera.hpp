#pragma once
#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Camera
{
public:

	Camera() {};
	~Camera() {};
	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void Translate2D( const Vec2 cameraTranslate );


private:

	Vec2 bottomLeftCoordinate = Vec2( -1.f , -1.f );
	Vec2 topRightCoordinate   = Vec2::ONE;

};

//--------------------------------------------------------------------------------------------------------------------------------------------
