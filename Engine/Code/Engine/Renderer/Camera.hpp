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
	Vec2  GetWorldNormalizedToClientPosition( Vec2 worldPos ) const;
	Vec2  GetClientToWorldPosition( Vec2 clientPos ) const;
	void  SetProjectionOrthographic( float height , float nearZ = -1.0f , float farZ = 1.0f );
	Vec2  GetPosition() const;
	Vec2  GetOutputSize() const;
	void  SetPosition( Vec3 position );
	void  SetOutputSize( Vec2 size );
// 	void Translate2D( const Vec2 cameraTranslate );
// 	Vec2 GetOrthoTopRight() const;
// 	Vec2 GetOrthoBottomLeft() const;
// 	void SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	// TODO :-  write new constructors;

	void  SetPostion( const Vec3& position );
	void  Translate( const Vec3& translation );
	void  SetPitchRollYawRotation( float pitch , float yaw , float roll );
	float GetAspectRatio() const;
	void  CorrectAspectRaio( float clientAspectRatio );

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
	Vec3			ClientToWorld( Vec2 client , float ndcZ );
	Vec3			WorldToClient( Vec3 worldPos );

public:
	RenderBuffer*	m_cameraUBO		= nullptr;

	Vec2 GetOrthoMin() const;
	Vec2 GetOrthoMax() const;
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
	Vec2 m_outputSize;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
