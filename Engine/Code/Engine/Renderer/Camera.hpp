#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

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
	float orthoMin[ 2 ];
	float orthoMax[ 2 ];
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Camera
{
public:
	Camera() {};
	~Camera();
	void  SetOrthoView( const Vec2& bottomLeft, const Vec2& topRight );
	Vec2  GetOrthoBottomLeft() const;
	Vec2  GetOrthoTopRight() const;
	void  Translate2D( const Vec2 cameraTranslate );
	void  SetClearMode( unsigned int clearFlags , Rgba8 color , float depth = 0.f , unsigned int stencil = 0 );
	Rgba8 GetClearColor() const																						{ return m_clearColor; }
	unsigned int  GetClearMode() const																				{ return m_clearMode; }

	void			 SetColorTarget( Texture* texture );
	RenderBuffer*	 UpdateUBO( RenderContext* ctx );
	Texture*		 GetColorTarget() const																					{ return m_colorTarget; }

public:
	RenderBuffer*	m_cameraUBO		= nullptr;

private:

	Vec2 bottomLeftCoordinate = Vec2( -1.f , -1.f );
	Vec2 topRightCoordinate   = Vec2::ONE;

	unsigned int	m_clearMode		= 0;
	Rgba8			m_clearColor	= BLACK;
	Texture*		m_colorTarget	= nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
