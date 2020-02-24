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
	Camera() {};
	~Camera();

	void  SetPostion( const Vec3& position );
	void  Translate( const Vec3& translation );
	void  SetPitchRollYawRotation( float pitch , float roll , float yaw );

	void  SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	Vec3  GetOrthoMin() const;
	Vec3  GetOrthoMax() const;
	void  Translate2D( const Vec2 cameraTranslate );
	void  SetClearMode( eCameraClearBitFlag clearFlags , Rgba8 color , float depth = 0.f , unsigned int stencil = 0 );
	void  SetProjectionOrthographic( float x , float y , float z );
	void  SetProjectionPerspective( float fov , float aspectRatio , float nearZ , float farZ );
	Rgba8 GetClearColor() const																						{ return m_clearColor; }
	unsigned int  GetClearMode() const																				{ return m_clearMode; }

	void			SetColorTarget( Texture* texture );
	void			SetDepthStencilTarget( Texture* texture );
	RenderBuffer*	UpdateUBO( RenderContext* ctx );
	Texture*		GetColorTarget() const																			{ return m_colorTarget; }
	Mat44			GetViewMatrix();
	Mat44			GetProjectionMatrix() const																		{ return m_projection; }


public:
	RenderBuffer*	m_cameraUBO		= nullptr;

private:

	eCameraClearBitFlag	m_clearMode				= CLEAR_COLOR_BIT;
	Rgba8				m_clearColor			= BLACK;
	Texture*			m_colorTarget			= nullptr;
	Mat44				m_projection;
	Mat44				m_view;
	Transform			m_transform;
	Texture*			m_depthStencilTarget	= nullptr;
	float				m_clearDepth			= 1.0f;
	float				m_clearStencil			= 0.f;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
