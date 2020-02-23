#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/Vec4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	delete m_colorTarget;
	m_colorTarget = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetPostion( const Vec3& position )
{
	m_transform.SetPosition( position );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::Translate( const Vec3& translation )
{
	m_transform.SetPosition( m_transform.GetPostion() + translation );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight )
{
	m_projection	= CreateOrthoGraphicProjeciton( Vec3( bottomLeft, 0.0f ) , Vec3( topRight , 1.0f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec3 Camera::GetOrthoMin() const
{
	Vec4 ndc( -1 , -1 , 0 , 1 );				// bottom left of renderable space
	Mat44 projectionCopy = m_projection;
	projectionCopy.TransformBy( m_view );
	Mat44 view_projection = projectionCopy;									// world to renderable space
	Mat44 clip_to_world   = view_projection.GetInverse();					// renderable space to world

	Vec4 world = ndc * clip_to_world;										// convert our bottom left corner
	world /= world.w;														// find the representitive point (w == 1)
	return Vec3( world.x , world.y , world.z );						// return the world point
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec3 Camera::GetOrthoMax() const
{
	Vec4 ndc( 1 , 1 , 0 , 1 );				// bottom left of renderable space
	Mat44 projectionCopy = m_projection;
	projectionCopy.TransformBy( m_view );
	Mat44 view_projection = projectionCopy;									// world to renderable space
	Mat44 clip_to_world = view_projection.GetInverse();						// renderable space to world

	Vec4 world = ndc * clip_to_world;										// convert our Top Right corner
	world /= world.w;														// find the representitive point (w == 1)
	return Vec3( world.x , world.y , world.z );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::Translate2D(const Vec2 cameraTranslate)
{
	// TODO :- IMPLEMENT DOES NOTHING FOR NOW
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetClearMode( eCameraClearBitFlag clearFlags , Rgba8 color , float depth /*= 0.f */ , unsigned int stencil /*= 0 */ )
{
	m_clearMode = clearFlags;
	m_clearColor = color;

	UNUSED( depth );
	UNUSED( stencil );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetProjectionOrthographic( float x , float y , float z )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetProjectionPerspective( float fov , float nearZ , float farZ )
{

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

	cameraData.cameraToClipTransform = m_projection;
	Mat44 CameraModel = Mat44::CreateTranslation3D( m_transform.GetPostion() );
	// CameraToWorld Space Transform
	// View -> worldToCamera
	// Mat44 View  = Invert(cameraModel);

	cameraData.view = Mat44::CreateTranslation3D( -m_transform.GetPostion() );

	m_cameraUBO->Update( &cameraData , sizeof( cameraData ) , sizeof( cameraData ) );

	return m_cameraUBO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
