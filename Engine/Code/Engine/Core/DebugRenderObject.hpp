#pragma once

#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Camera.hpp"

#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Timer;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eDebugRenderObjectType
{
	DRO_INVALID = -1,

	DRO_POINT2D,
	DRO_POINT3D,

	DRO_LINE2D,
	DRO_LINE3D,

	DRO_ARROW2D ,
	DRO_ARROW3D ,

	DRO_SPHERE,
	DRO_SPHERE_WIREFRAME,
	
};

//--------------------------------------------------------------------------------------------------------------------------------------------

// Only effects "world" mode stuff; 
enum eDebugRenderMode
{
	DEBUG_RENDER_ALWAYS ,          // what is rendered always shows up
	DEBUG_RENDER_USE_DEPTH ,       // respect the depth buffer
	DEBUG_RENDER_XRAY ,            // renders twice - once darker when it shoudl be hidden, and once more saturated when it should appear
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eRenderSpace
{
	SCREENSPACE,
	WORLDSPACE,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct DebugRenderObject
{
	//DebugRenderObjects();
public:
	DebugRenderObject( eDebugRenderObjectType type , eDebugRenderMode mode , float duration , bool isBillboarded = false );
	~DebugRenderObject() {};
	void Update();
	void MarkForDestroy();
	
public:
	eDebugRenderObjectType		m_objectType;
	eDebugRenderMode			m_renderMode;
	//eRenderSpace				m_renderSpace;
	float						m_duration;
	bool						m_isBillboarded	= false;
	Rgba8						m_startColor;
	Rgba8						m_endColor;
	Timer*						m_timer			= nullptr;
	Transform					m_transform;
	bool						m_isGarbage		= false;
	//Camera*					m_cameraToDrawTo;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct DRO_point2D : public DebugRenderObject
{
public:
	DRO_point2D( eDebugRenderMode mode , Vec2 pos , float size , Rgba8 startColor , Rgba8 endColor, float duration , bool isBillboarded = false );

public:
	Vec2	m_position;
	float	m_size;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct DRO_point3D : public DebugRenderObject
{

public:
	DRO_point3D( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode );
	DRO_point3D( Vec3 pos , float size , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
	DRO_point3D( Vec3 pos , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
	
public:
	Vec3	m_position;
	float	m_size;
};