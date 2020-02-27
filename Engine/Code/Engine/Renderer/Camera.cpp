#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern Vec2 g_windowDimensions;


//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetPosition( Vec3 position )
{
	m_position = Vec2( position.x , position.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOutputSize() const
{
	return m_outputSize;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetPosition() const
{
	return m_position;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOutputSize( Vec2 size )
{
	m_outputSize = size;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetProjectionOrthographic( float height , float nearZ , float farZ )
{
	UNUSED( nearZ );
	UNUSED( farZ );
	float width  = height * GetAspectRatio();
	m_outputSize = Vec2( width , height );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetClientToWorldPosition( Vec2 clientPos ) const
{
	Vec2 worldPos;
// 	Vec2 outputDimensions = GetOrthoMax()-GetOrthoMin();
//  	Vec2 pixelpos = Vec2::ZERO;
//  	pixelpos.x = RangeMapFloat( 0.f , m_outputSize.x , 0.f , m_outputSize.x , worldPos.x );
//  	pixelpos.y = RangeMapFloat( 0.f , m_outputSize.y , 0.f , m_outputSize.y , worldPos.y );

	Vec2 cameraBottomLeft = GetOrthoMin();
	Vec2 cameraTopRight = GetOrthoMax();

	Vec2 outputDimensions = m_outputSize;
	worldPos.x = RangeMapFloat( 0.f , 1.f , cameraBottomLeft.x , cameraTopRight.x , clientPos.x );
	worldPos.y = RangeMapFloat( 0.f , 1.f , cameraBottomLeft.y , cameraTopRight.y , clientPos.y );
	return worldPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetWorldNormalizedToClientPosition( Vec2 worldPos ) const
{
	Vec2 clientPos;

	Vec2 cameraBottomLeft	= GetOrthoMin();
	Vec2 cameraTopRight		= GetOrthoMax();

	Vec2 outputDimensions = m_outputSize;
	clientPos.x = RangeMapFloat( 0.f,1.f, cameraBottomLeft.x , cameraTopRight.x , worldPos.x );
	clientPos.y = RangeMapFloat( 0.f,1.f, cameraBottomLeft.y , cameraTopRight.y , worldPos.y );
	return clientPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Camera::GetAspectRatio() const
{
	return m_outputSize.x / m_outputSize.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::CorrectAspectRaio( float clientAspectRatio )
{
	m_outputSize.x = m_outputSize.y * clientAspectRatio;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOrthoMin() const
{
	return ( m_position - ( m_outputSize / 2.f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOrthoMax() const
{
	return ( m_position + ( m_outputSize / 2.f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
