#include "Engine/Core/DebugRenderObject.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Time/Clock.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

DebugRenderObject::DebugRenderObject( eDebugRenderObjectType type , eDebugRenderMode mode , float duration , bool isBillboarded /*= false */ )
{
	m_objectType	= type;
	m_renderMode	= mode;
	m_duration		= duration;
	m_isBillboarded = isBillboarded;

	m_timer = new Timer();
	m_timer->SetSeconds( &Clock::g_theMasterClock , duration );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObject::Update()
{
	if ( m_timer != nullptr && m_timer->HasElapsed() )
	{
		MarkForDestroy();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObject::MarkForDestroy()
{
	m_isGarbage = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point2D::DRO_point2D( eDebugRenderMode mode , Vec2 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , bool isBillboarded /*= false */ ) :
						DebugRenderObject( DRO_POINT2D , mode , duration , isBillboarded )
{
	m_position		= pos;
	m_size			= size;
	m_startColor	= startColor;
	//m_startColor = new Rgba8();
	//m_startColor->CopyFromRGBA8( startColor );

	m_endColor		= endColor;
	//m_endColor = new Rgba8();
	//m_endColor->CopyFromRGBA8( endColor );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point3D::DRO_point3D( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode ) : 
	DebugRenderObject( DRO_POINT3D , mode , duration )
{
	m_position		= pos;
	m_size			= size;
	m_startColor	= startColor;
	m_endColor		= endColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point3D::DRO_point3D( Vec3 pos , float size , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ ) :
	DebugRenderObject( DRO_POINT3D , mode , duration )
{
	m_position		= pos;
	m_size			= size;
	m_startColor	= color;
	m_endColor		= Rgba8( 0 , 0 , 0 , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point3D::DRO_point3D( Vec3 pos , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ ) : 
	DebugRenderObject( DRO_POINT3D , mode , duration )
{
	m_position		= pos;
	m_size			= 1.f;
	m_startColor	= color;
	m_endColor		= Rgba8( 0 , 0 , 0 , 0 );
}
