#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight ) 
{
	bottomLeftCoordinate = bottomLeft;
	topRightCoordinate   = topRight;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOrthoBottomLeft() const
{
	return bottomLeftCoordinate;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOrthoTopRight() const
{
	return topRightCoordinate;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::Translate2D(const Vec2 cameraTranslate)
{
	bottomLeftCoordinate = bottomLeftCoordinate + cameraTranslate;
	topRightCoordinate   = topRightCoordinate + cameraTranslate;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
