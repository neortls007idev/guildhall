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
	UpdateColor();
	
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
	m_currrentColor = startColor;
	//m_startColor = new Rgba8();
	//m_startColor->CopyFromRGBA8( startColor );

	m_endColor		= endColor;
	//m_endColor = new Rgba8();
	//m_endColor->CopyFromRGBA8( endColor );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DRO_point2D::UpdateColor()
{
	if ( m_endColor == m_startColor )
	{
		return;
	}

	m_currrentColor.LerpColorOverTime( m_startColor , m_endColor , m_duration , m_timer );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point3D::DRO_point3D( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode ) : 
	DebugRenderObject( DRO_POINT3D , mode , duration )
{
	m_position		= pos;
	m_size			= size;
	m_startColor	= startColor;
	m_currrentColor = startColor;
	m_endColor		= endColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point3D::DRO_point3D( Vec3 pos , float size , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ ) :
	DebugRenderObject( DRO_POINT3D , mode , duration )
{
	m_position		= pos;
	m_size			= size;
	m_startColor	= color;
	m_currrentColor = m_startColor;
	m_endColor		= m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_point3D::DRO_point3D( Vec3 pos , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ ) : 
	DebugRenderObject( DRO_POINT3D , mode , duration )
{
	m_position		= pos;
	m_size			= 1.f;
	m_startColor	= color;
	m_currrentColor = m_startColor;
	m_endColor		= m_startColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DRO_point3D::UpdateColor()
{
	if ( m_endColor == m_startColor )
	{
		return;
	}

	m_currrentColor.LerpColorOverTime( m_startColor , m_endColor , m_duration , m_timer );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_line3D::DRO_line3D ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                         Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration /*= 0.f */ ,
                         eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ , float radius /*= 1.f */ ) :
	DebugRenderObject( DRO_LINE3D , mode , duration )
{
	m_startPos = startPos;
	m_endPos = endPos;

	m_startPosStartColor = startPosStartColor;
	m_startPosEndColor = startPosEndColor;
	m_startPosCurrentColor = m_startPosStartColor;

	m_endPosStartColor = endPosStartColor;
	m_endPosEndColor = endPosEndColor;
	m_endPosCurrentColor = m_endPosStartColor;

	m_radius = radius;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DRO_line3D::DRO_line3D ( Vec3 startPos , Vec3 endPos , Rgba8 color , float duration /*= 0.0f */ ,
                         eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ , float radius /*= 1.f */ ) :
	DebugRenderObject( DRO_LINE3D , mode , duration )
{
	m_startPos = startPos;
	m_endPos = endPos;

	m_startPosStartColor = color;
	m_startPosEndColor = color;
	m_startPosCurrentColor = m_startPosStartColor;

	m_endPosStartColor = color;
	m_endPosEndColor = color;
	m_endPosCurrentColor = m_endPosStartColor;

	m_radius = radius;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DRO_line3D::UpdateColor()
{
	if ( m_startPosStartColor == m_startPosEndColor && m_endPosStartColor == m_endPosEndColor )
	{
		return;
	}

	m_startPosCurrentColor.LerpColorOverTime( m_startPosStartColor , m_startPosEndColor , m_duration , m_timer );
	m_endPosCurrentColor.LerpColorOverTime( m_endPosStartColor , m_endPosEndColor , m_duration , m_timer );
}
