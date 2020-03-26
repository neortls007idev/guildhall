#pragma once
#include "Engine/Core/DebugRenderObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Primitives/AABB3.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GPUMesh;
class Camera;
class Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

class DebugRenderObjectsManager
{
public:

	// TODO :- MAKE Contructors and destructors are private for SingleTonObject creation Purposes and Create a factorySystem to createObjects.
	DebugRenderObjectsManager() {};
	~DebugRenderObjectsManager() {};

	void Startup();																		// may be used to allocate resources to it
	void Shutdown();																	// may be used to free the allocated resources to it

	void BeginFrame();																	// Does nothing, here for completeness.
	void Update( float deltaSeconds );
	void Render() const;

	void EndFrame();																	// Clean up dead objects

	void CleanupScreenObjects();
	void CleanupWorldObjects();

//--------------------------------------------------------------------------------------------------------------------------------------------
//				HELPERS
//--------------------------------------------------------------------------------------------------------------------------------------------
	void AddDebugObjectTo( eRenderSpace space  , DebugRenderObject* object );
	void CleanupDebugObjects( std::vector<DebugRenderObject*>& droArray );
	void UpdateDebugObjects( std::vector<DebugRenderObject*>& droArray , float deltaSeconds );

	void RenderObjectsAlways(	std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode );
	void RenderObjectsUseDepth( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode );
	void RenderObjectsXRAY(		std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode );

	void RenderObjectArray( std::vector<DebugRenderObject*>& droArray , Camera* cam );
	void RenderObjectArrayXRAYPass2( std::vector<DebugRenderObject*>& droArray , Camera* cam );
	
	// control
	void EnableDebugRendering();
	void DisableDebugRendering();

	// output
	void DebugRenderWorldToCamera( Camera* cam );										// Draws all world objects to this camera 
	void DebugRenderScreenTo( Texture* output );										// Draws all screen objects onto this texture (screen coordinate system is up to you.  I like a 1080p default)

public:

	// Default DebugRenderObject Manager
	//static DebugRenderObjectsManager				s_debugRender;

	bool											m_isDebugRenderEnabled = true;
	
	std::vector<DebugRenderObject*>					m_debugRenderWorldObjectsAlways;
	std::vector<DebugRenderObject*>					m_debugRenderWorldObjectsUseDepth;
	std::vector<DebugRenderObject*>					m_debugRenderWorldObjectsXRay;
	
	std::vector<DebugRenderObject*>					m_debugRenderScreenObjects;
};

void DebugRenderWorldToCamera( Camera* cam );										// Draws all world objects to this camera 
void DebugRenderScreenTo( Texture* output );										// Draws all screen objects onto this texture (screen coordinate system is up to you.  I like a 1080p default)

//------------------------------------------------------------------------
// World Rendering
//------------------------------------------------------------------------

// points
void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode );
void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
//void DebugAddWorldPoint( Vec3 pos , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//--------------------------------------------------------------------------------------------------------------------------------------------
//		3D LINES
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLine ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                         Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration , eDebugRenderMode mode ,
                         float radius = 1.f );

void DebugAddWorldLine ( Vec3 start , Vec3 end , Rgba8 color , float duration = 0.0f ,
                         eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH , float radius = 1.f );

// line strip [extra]
void DebugAddWorldLineStrip( uint count , Vec3 const* positions ,
	Rgba8 start_p0_color , Rgba8 start_pf_color ,    // color of first/end point at the beginning of duration
	Rgba8 end_p0_color , Rgba8 end_pf_color ,        // color of first/end point at the end of duration
	float duration ,
	eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//--------------------------------------------------------------------------------------------------------------------------------------------
//		3D ARROWS
//--------------------------------------------------------------------------------------------------------------------------------------------

// Draws 3D Arrow in worldSpace of defined radius from Start to End - TIP shares SHAFT'S end color
void DebugAddWorldArrow ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                          Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration = 0.f ,
                          eDebugRenderMode mode = DEBUG_RENDER_ALWAYS , float radius = 1.f );

// Draws 3D Arrow in worldSpace of defined shaft radius , tip radius from Start to End - SHAFT & TIP end can separately LERP between different colors
void DebugAddWorldArrow( Vec3 startPos , Vec3 endPos ,
						 Rgba8 shaftStartPosStartColor , Rgba8 shaftStartPosEndColor , Rgba8 shaftEndPosStartColor , Rgba8 shaftEndPosEndColor ,
						 Rgba8 tipStartPosStartColor , Rgba8 tipStartPosEndColor , Rgba8 tipEndPosStartColor , Rgba8 tipEndPosEndColor ,
						 float duration = 0.f , eDebugRenderMode mode = DEBUG_RENDER_ALWAYS , float shaftRadius = 0.9f , float tipRadius = 1.f );

// Draws 3D Arrow in worldSpace of Unit Length from Start to End
void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

//--------------------------------------------------------------------------------------------------------------------------------------------

// Quads
void DebugAddWorldQuad ( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p4 , AABB2 uvs , Rgba8 startColor , Rgba8 endColor ,
                         float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// bounds
//void DebugAddWorldWireBounds( obb3 bounds , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
//void DebugAddWorldWireBounds( obb3 bounds , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldWireBounds( AABB3 bounds , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 color , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// basis
void DebugAddWorldBasis( Mat44 basis , Rgba8 startTint , Rgba8 endTint , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWorldBasis( Mat44 basis , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );

// text
// non-billboarded will be oriented in the world based on the passed in basis matrix 
void DebugAddWorldText( Mat44 basis , Vec2 pivot , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode , char const* text );
void DebugAddWorldTextf( Mat44 basis , Vec2 pivot , Rgba8 color , float duration , eDebugRenderMode mode , char const* text , ... );
void DebugAddWorldTextf( Mat44 basis , Vec2 pivot , Rgba8 color , char const* text , ... ); // assume DEBUG_RENDER_USE_DEPTH

// this text will always orient itself to the current camera
void DebugAddWorldBillboardText( Vec3 origin , Vec2 pivot , Rgba8 start_color , Rgba8 end_color , float duration , eDebugRenderMode mode , char const* text );
void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , Rgba8 color , float duration , eDebugRenderMode mode , char const* format , ... );
void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , Rgba8 color , char const* format , ... );

// grid [extra] 
void DebugAddWorldGrid( Vec3 origin ,
	Vec3 i , float iMin , float iMax , float iMinorSegment , float iMajorSegment , Rgba8 iMinorColor , Rgba8 iMajorColor , Rgba8 iOriginColor ,
	Vec3 j , float jMin , float jMax , float jMinorSegment , float jMajorSegment , Rgba8 jMinorcolor , Rgba8 jMajorColor , Rgba8 jOriginColor ,
	Rgba8 axisColor );
void DebugAddWorldXYGrid();
void DebugAddWorldXZGrid();
void DebugAddWorldYZGrid();

// mesh [extra]
void DebugAddWireMeshToWorld( Mat44 transform , GPUMesh* mesh , Rgba8 start_tint , Rgba8 end_tint , float duration , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );
void DebugAddWireMeshToWorld( Mat44 transform , GPUMesh* mesh , float duration = 0.0f , eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH );


//------------------------------------------------------------------------
// Screen Rendering
//------------------------------------------------------------------------
void DebugRenderSetScreenHeight( float height ); // default to 1080.0f when system starts up.  Meaning (0,0) should always be bottom left, (aspect * height, height) is top right
AABB2 DebugGetScreenBounds();                    // useful if you want to align to top right for something

//--------------------------------------------------------------------------------------------------------------------------------------------
//		2D POINT
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration );
void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 color , float duration = 0.0f );

// ASSUMED SIZE = 10 and duration = 5 seconds
void DebugAddScreenPoint( Vec2 pos , Rgba8 color );

//--------------------------------------------------------------------------------------------------------------------------------------------
//		2D LINE
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenLine( Vec2 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec2 endPos ,
						Rgba8 endPosStartColor , Rgba8 endPosEndColor ,	float duration );

void DebugAddScreenLine( Vec2 startPos , Vec2 endPos , Rgba8 color , float duration = 0.0f );

//--------------------------------------------------------------------------------------------------------------------------------------------
//		2D ARROWS
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow( Vec2 startPos , Vec2 endPos ,
						  Rgba8 shaftStartPosStartColor , Rgba8 shaftStartPosEndColor , Rgba8 shaftEndPosStartColor , Rgba8 shaftEndPosEndColor ,
						  Rgba8 tipStartPosStartColor , Rgba8 tipStartPosEndColor , Rgba8 tipEndPosStartColor , Rgba8 tipEndPosEndColor ,
						  float duration = 0.f , float thickness = 10.f );

void DebugAddScreenArrow( Vec2 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec2 endPos ,
						  Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration = 0.f , float thickness = 10.f );


void DebugAddScreenArrow( Vec2 startPos , Vec2 endPos , Rgba8 color , float duration = 0.0f , float thickness = 10.f );

//--------------------------------------------------------------------------------------------------------------------------------------------
//		2D QUADS
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenQuad( AABB2 bounds , Rgba8 start_color , Rgba8 end_color , float duration );
void DebugAddScreenQuad( AABB2 bounds , Rgba8 color , float duration = 0.0f );

// texture

void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , AABB2 uvs , Rgba8 start_tint , Rgba8 end_tint , float duration = 0.0f );
void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , AABB2 uvs , Rgba8 tint , float duration = 0.0f );
void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , Rgba8 tint = WHITE , float duration = 0.0f ); // assume UVs are full texture

// text
void DebugAddScreenText( Vec4 pos , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , char const* text );
void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , float size , Rgba8 start_color , Rgba8 end_color , float duration , char const* format , ... );
void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , float size , Rgba8 color , float duration , char const* format , ... );
void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , float size , Rgba8 color , char const* format , ... );
void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , Rgba8 color , char const* format , ... );

// screen basis [extra]
void DebugAddScreenBasis( Vec2 screen_origin_location , Mat44 basis_to_render , Rgba8 start_tint , Rgba8 end_tint , float duration );
void DebugAddScreenBasis( Vec2 screen_origin_location , Mat44 basis_to_render , Rgba8 tint = WHITE , float duration = 0.0f );

// message log system [extra]
void DebugAddMessage( float duration , Rgba8 color , char const* format , ... );
void DebugAddMessage( Rgba8 color , char const* format , ... );
