#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/Texture.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOutputSize() const
{
	return m_outputSize;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec3 Camera::GetPosition() const
{
	return m_transform.GetPostion();
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

	Vec3 cameraBottomLeft = GetOrthoMin();
	Vec3 cameraTopRight = GetOrthoMax();

	Vec2 outputDimensions = m_outputSize;
	worldPos.x = RangeMapFloat( 0.f , 1.f , cameraBottomLeft.x , cameraTopRight.x , clientPos.x );
	worldPos.y = RangeMapFloat( 0.f , 1.f , cameraBottomLeft.y , cameraTopRight.y , clientPos.y );
	return worldPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetWorldNormalizedToClientPosition( Vec2 worldPos ) const
{
	Vec2 clientPos;

	Vec3 cameraBottomLeft	= GetOrthoMin();
	Vec3 cameraTopRight		= GetOrthoMax();

	Vec2 outputDimensions = m_outputSize;
	clientPos.x = RangeMapFloat( 0.f,1.f, cameraBottomLeft.x , cameraTopRight.x , worldPos.x );
	clientPos.y = RangeMapFloat( 0.f,1.f, cameraBottomLeft.y , cameraTopRight.y , worldPos.y );
	return clientPos;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Camera::Camera()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

//float Camera::GetAspectRatio() const

Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	//delete m_colorTarget;
	m_colorTarget = nullptr;

	delete m_depthStencilTarget;
	m_depthStencilTarget = nullptr;
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

void Camera::SetPitchYawRollRotation( float pitch , float yaw , float roll )
{
	m_transform.SetRotation( pitch , yaw , roll );
}

float Camera::GetAspectRatio() const
{
	return  ( GetOrthoMax().x - GetOrthoMin().x ) / ( GetOrthoMax().y - GetOrthoMin().y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::CorrectAspectRaio( float clientAspectRatio )
{
	// TODO - IMPLEMENT ME
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight )
{
	m_projection	= CreateOrthoGraphicProjeciton( Vec3( bottomLeft, 0.0f ) , Vec3( topRight , 1.0f ) );
	m_outputSize = ( GetOrthoMax() - GetOrthoMin() ).GetXYComponents();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
// TODO :- FIND ME - void Camera::CorrectAspectRaio( float clientAspectRatio )

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

void Camera::SetClearMode( unsigned int clearFlags , Rgba8 color , float depth /*= 0.f */ , unsigned int stencil /*= 0 */ )
{
	m_clearMode = ( eCameraClearBitFlag ) clearFlags;
	m_clearColor = color;

	m_clearDepth = depth;
	
	m_clearStencil =  stencil;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetProjectionPerspective( float fov , float aspectRatio , float nearZ , float farZ )
{
	m_projection = CreatePerpsectiveProjectionMatrixD3D( fov , aspectRatio , nearZ , farZ );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetColorTarget( Texture* texture )
{
	m_colorTarget = texture;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::CreateMatchingDepthStencilTarget()
{
	if ( !m_depthStencilTarget )
	{
		m_depthStencilTarget->GetOrCreateDepthStencilView();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetDepthStencilTarget( Texture* texture )
{
	m_depthStencilTarget = texture;
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

	// CameraToWorld Space Transform
	// View -> worldToCamera
	// Mat44 View  = Invert(cameraModel);

	cameraData.view = GetViewMatrix()/*Mat44::CreateTranslation3D( -m_transform.GetPostion() )*/;

	m_cameraUBO->Update( &cameraData , sizeof( cameraData ) , sizeof( cameraData ) );

	return m_cameraUBO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetColorTargetSize() const // is needed for depth buffers
{
	if ( m_colorTarget != nullptr )
	{
		return Vec2( m_colorTarget->GetDimensions() );
	}
	else
	{
		return Vec2( g_theRenderer->m_swapChain->GetBackBuffer()->GetDimensions() );
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------

Mat44 Camera::GetViewMatrix()
{
	Mat44 cameraModel = m_transform.GetAsMatrix();

	m_view = MatrixInvertOrthoNormal( cameraModel );
	return m_view;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec3 Camera::ClientToWorld( Vec2 client , float ndcZ )
{
	return Vec3::ZERO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec3 Camera::WorldToClient( Vec3 worldPos )
{
	return m_view.TransformVector3D( worldPos );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
