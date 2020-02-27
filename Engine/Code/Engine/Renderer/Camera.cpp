#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern Vec2 g_windowDimensions;

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/Vec4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight ) 
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

void Camera::SetPitchRollYawRotation( float pitch , float yaw , float roll )
{
	m_transform.SetRotation( pitch , yaw , roll );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Camera::SetOrthoView( const Vec2& bottomLeft , const Vec2& topRight )
{
	bottomLeftCoordinate = bottomLeft;
	topRightCoordinate   = topRight;
	return m_outputSize.x / m_outputSize.y;
	m_projection	= CreateOrthoGraphicProjeciton( Vec3( bottomLeft, 0.0f ) , Vec3( topRight , 1.0f ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 Camera::GetOrthoBottomLeft() const
void Camera::CorrectAspectRaio( float clientAspectRatio )
Vec3 Camera::GetOrthoMin() const
{
	return bottomLeftCoordinate;
	m_outputSize.x = m_outputSize.y * clientAspectRatio;
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

Vec2 Camera::GetOrthoTopRight() const
Vec2 Camera::GetOrthoMin() const
Vec3 Camera::GetOrthoMax() const
{
	return topRightCoordinate;
	return ( m_position - ( m_outputSize / 2.f ) );
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

Vec2 Camera::GetOrthoMax() const
{
	bottomLeftCoordinate = bottomLeftCoordinate + cameraTranslate;
	topRightCoordinate   = topRightCoordinate + cameraTranslate;
	return ( m_position + ( m_outputSize / 2.f ) );
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
	UNUSED( x );
	UNUSED( y );
	UNUSED( z );
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
