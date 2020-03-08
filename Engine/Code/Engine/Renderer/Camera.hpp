#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec3.hpp"

class Texture;
class RenderBuffer;
class RenderContext;

//--------------------------------------------------------------------------------------------------------------------------------------------
// TODO :-

enum eCameraClearBitFlag : unsigned int
{
	CLEAR_NONE		  = 0,
	CLEAR_COLOR_BIT	  = ( 1 << 0 ),
	CLEAR_DEPTH_BIT	  = ( 1 << 1 ),
	CLEAR_STENCIL_BIT = ( 1 << 2 ),
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct CameraDataT
{
	Mat44 cameraToClipTransform;
	Mat44 view;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Camera
{
public:
	Camera();
	~Camera();
	Camera( const Camera& copy ) = delete;

	Vec2			GetWorldNormalizedToClientPosition( Vec2 worldPos ) const;
	Vec2			GetClientToWorldPosition( Vec2 clientPos ) const;
	
	Vec2			GetOutputSize() const;

	Vec3			GetPosition() const;
	Vec3			GetOrthoMin() const;
	Vec3			GetOrthoMax() const;

	Rgba8			GetClearColor() const																			{ return m_clearColor; }
	unsigned int	GetClearMode() const																			{ return m_clearMode; }
	Texture*		GetColorTarget() const																			{ return m_colorTarget; }
	Vec2			GetColorTargetSize() const;
	Texture*		GetDepthStencilTarget() const																	{ return m_depthStencilTarget; }

	Mat44			GetProjectionMatrix() const																		{ return m_projection; }
	Mat44			GetViewMatrix();
	float			GetAspectRatio() const;

	void			SetPosition( const Vec3& position );
	void			Translate( const Vec3& translation );
	void			Translate2D( const Vec2 cameraTranslate );
	void			SetPitchYawRollRotation( float pitch , float yaw , float roll );
	void			SetOutputSize( Vec2 size );
	void			CorrectAspectRatio( float clientAspectRatio );

	void			SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	void			SetClearMode( unsigned int clearFlags , Rgba8 color , float depth = 0.f , unsigned int stencil = 0 );
	void			SetColorTarget( Texture* texture );
	void			CreateMatchingDepthStencilTarget();
	void			SetDepthStencilTarget( Texture* texture );

	void			SetProjectionOrthographic( float height , float nearZ = 0.f , float farZ = 1.f );
//	void			SetProjectionOrthographic( float height , float nearZ = -1.0f , float farZ = 1.0f );
	void			SetProjectionPerspective( float fov , float aspectRatio , float nearZ , float farZ );


	RenderBuffer*	UpdateUBO( RenderContext* ctx );
	Vec3			ClientToWorld( Vec2 client , float ndcZ );
	Vec3			WorldToClient( Vec3 worldPos );

public:
	RenderBuffer*	m_cameraUBO		= nullptr;

private:

	eCameraClearBitFlag	m_clearMode				= CLEAR_COLOR_BIT;
	Rgba8				m_clearColor			= BLACK;

	Mat44				m_projection;
	Mat44				m_view;
	Transform			m_transform;
	
	Texture*			m_colorTarget			= nullptr;
	Texture*			m_depthStencilTarget	= nullptr;
	float				m_clearDepth			= 1.0f;
	float				m_clearStencil			= 0.f;
	Vec2				m_outputSize;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
