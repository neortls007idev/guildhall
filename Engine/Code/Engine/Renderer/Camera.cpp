#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"

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

void Camera::SetClearMode( unsigned int clearFlags , Rgba8 color , float depth /*= 0.f */ , unsigned int stencil /*= 0 */ )
{
	m_clearMode = clearFlags;
	m_clearColor = color;

	UNUSED( depth );
	UNUSED( stencil );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
