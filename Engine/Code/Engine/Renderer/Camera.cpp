#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	delete m_colorTarget;
	m_colorTarget = nullptr;
}

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

void Camera::SetClearMode( unsigned int clearFlags , Rgba8 color , float depth /*= 0.f */ , unsigned int stencil /*= 0 */ )
{
	m_clearMode = clearFlags;
	m_clearColor = color;

	UNUSED( depth );
	UNUSED( stencil );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetColorTarget( Texture* texture )
{
	m_colorTarget = texture;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

RenderBuffer* Camera::UpdateUBO( RenderContext* ctx )
{
	if ( m_cameraUBO == nullptr )
	{
		m_cameraUBO = new RenderBuffer( ctx , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}

	//m_cameraUBO->Update()

	CameraDataT cameraData;

	cameraData.orthoMin[ 0 ] = bottomLeftCoordinate.x;
	cameraData.orthoMin[ 1 ] = bottomLeftCoordinate.y;
	cameraData.orthoMax[ 0 ] = topRightCoordinate.x;
	cameraData.orthoMax[ 1 ] = topRightCoordinate.y;

	m_cameraUBO->Update( &cameraData , sizeof( cameraData ) , sizeof( cameraData ) );
	
	return m_cameraUBO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
