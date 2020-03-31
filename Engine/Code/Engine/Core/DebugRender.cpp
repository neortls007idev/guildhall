﻿#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Core/DebugRenderObject.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Time/Timer.hpp"
#include "StringUtils.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;
extern BitmapFont*					g_bitmapFont;

	   RenderContext*				g_debugRenderContext	= nullptr;
	   Camera*						g_debugCamera			= nullptr;
//static DebugRenderObjectsManager*	g_currentManager		= nullptr;
	   DebugRenderObjectsManager*	g_currentManager        = nullptr;


//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderBeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderWorldToCamera( Camera* cam )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();

	g_debugCamera->SetProjectionMatrix( cam->GetProjectionMatrix() );
	g_debugCamera->SetCameraTransform( cam->GetCameraTransform() );
	g_debugCamera->SetDepthStencilTargetFromAnotherCamera( cam );
	g_debugCamera->SetClearMode( originalClearMode , originalClearColor );
	//std::vector< Vertex_PCU > vertices;

	g_debugRenderContext = cam->GetColorTarget()->GetRenderContext();

	if ( g_debugRenderContext == nullptr )
	{
		g_debugRenderContext = g_theRenderer;
	}

	//g_debugRenderContext->BeginCamera( *cam );
	
	g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
	
	g_currentManager->RenderObjectsUseDepth( g_currentManager->m_debugRenderWorldObjectsUseDepth ,g_debugCamera , eBlendMode::SOLID );
	g_currentManager->RenderObjectsXRAY( g_currentManager->m_debugRenderWorldObjectsXRay , g_debugCamera , eBlendMode::SOLID );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderWorldObjectsAlways , g_debugCamera , eBlendMode::SOLID );
	//g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderScreenObjects		, g_debugCamera , eBlendMode::ALPHA );

	//g_debugRenderContext->DrawVertexArray( vertices );
	//g_debugRenderContext->EndCamera( *cam );
	g_debugCamera->ResetDepthStencilTarget();
	cam->SetClearMode( originalClearMode , originalClearColor );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderScreenTo( Texture* output )
{
	Camera camera;

	if ( output )
	{
		RenderContext* ctx = output->GetRenderContext();
		g_debugRenderContext = ctx;
		camera.SetColorTarget( output );
		camera.SetOrthoView( Vec2::ZERO , Vec2( output->GetDimensions() ) );
		//camera.SetOrthoView( Vec2( -0.5f * ( 16.f / 9.f ) * 1080.f , -0.5 * 1080.f ), Vec2( 0.5f * ( 16.f / 9.f ) * 1080.f , 0.5 * 1080.f ) );
		//camera.SetOrthoView( Vec2( -800.f , -400.f ) , Vec2( 800.f , 400.f ) );
	}
	else
	{
		g_debugRenderContext = g_theRenderer;
		//camera.SetOrthoView( Vec2( -800.f , -400.f ) , Vec2( 800.f , 400.f ) );
		camera.SetOrthoView( Vec2::ZERO, Vec2( ( 16.f / 9.f ) * 1080.f , 1080.f ) );
	}

/*camera.SetProjectionOrthographic()*/

// 	Vec2 min = Vec2::ZERO;
// 	Vec2 max = Vec2( output->GetDimensions() );
// 
// 	camera.SetProjectionOrthographic( max.y - min.y , -1.0f , 1.0f );
		// should I clear?
	camera.SetClearMode( CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK );

	g_currentManager->RenderObjectsUseDepth( g_currentManager->m_debugRenderWorldObjectsUseDepth , &camera , eBlendMode::ALPHA );
	g_currentManager->RenderObjectsXRAY( g_currentManager->m_debugRenderWorldObjectsXRay			, &camera , eBlendMode::ALPHA );
	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderWorldObjectsAlways		, &camera , eBlendMode::ALPHA );

	g_currentManager->RenderObjectsAlways( g_currentManager->m_debugRenderScreenObjects			, &camera , eBlendMode::ALPHA );

	//g_debugRenderContext->DrawVertexArray( vertices );
	g_debugRenderContext->EndCamera( camera );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_point3D* obj = new DRO_point3D( pos , size , color , duration , mode );
	obj->m_transform.SetPosition( pos );

	switch ( mode )
	{
	case DEBUG_RENDER_ALWAYS:
									g_currentManager->m_debugRenderWorldObjectsAlways.push_back( obj );
									break;
	case DEBUG_RENDER_USE_DEPTH:
									g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( obj );
									break;
	case DEBUG_RENDER_XRAY:
									g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( obj );
									break;
	default:
		break;
	}
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldPoint( Vec3 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode )
{
	DRO_point3D* obj = new DRO_point3D( pos , size , startColor , endColor , duration , mode );
	obj->m_transform.SetPosition( pos );

	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldPoint( Vec3 pos , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_point3D* obj = new DRO_point3D( pos , 1.f , color , color , duration , mode );
	obj->m_transform.SetPosition( pos );

	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLine ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                         Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration , eDebugRenderMode mode ,
                         float radius /*= 1.f */ )
{
	DRO_line3D* obj = new DRO_line3D( startPos , startPosStartColor , startPosEndColor ,
										endPos , endPosStartColor , endPosEndColor ,
	                                  duration , mode , radius );

	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLine( Vec3 start , Vec3 end , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ , float radius /*= 1.f */ )
{
	DRO_line3D* obj = new DRO_line3D( start , end , color , duration , mode , radius );
	//obj->m_transform.SetPosition( p0 );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLineStrip ( uint count , std::vector< Vec3 > positions , Rgba8 stripStartStartColor ,
                              Rgba8 stripStartEndColor , /* color of first/end point at the beginning of duration */
                              Rgba8 stripEndStartColor , Rgba8 stripEndEndColor ,
                              /* color of first/end point at the end of duration */ float duration ,
                              eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	if ( positions.size() < 2 || count < 2 )
	{
		// TODO:- display ext/ message on screen
		return;
	}
	
	for ( uint index = 1; index < ( count - 1 ); index++ )
	{
		DRO_line3D* stripSegmentObj = new DRO_line3D( positions[ index - 1 ] , stripStartStartColor ,
		                                              stripStartEndColor , positions[ index ] , stripStartStartColor ,
		                                              stripStartEndColor , duration , mode );
		g_currentManager->AddDebugObjectTo( WORLDSPACE , stripSegmentObj );
	}
	
	DRO_line3D* stripSegmentObj = new DRO_line3D( positions[ ( positions.size() - 2 ) ] , stripStartStartColor ,
		stripStartEndColor , positions[ ( positions.size() - 1 ) ] , stripEndStartColor ,
		stripEndEndColor , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , stripSegmentObj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldLineStrip( uint count , Vec3 const* positions , Rgba8 stripStartStartColor , Rgba8 stripStartEndColor , /* color of first/end point at the beginning of duration */ Rgba8 stripEndStartColor , Rgba8 stripEndEndColor , /* color of first/end point at the end of duration */ float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	if ( !positions || count < 2 )
	{
		// TODO:- display ext/ message on screen
		return;
	}

	for ( uint index = 1; index < ( count - 1 ); index++ )
	{
		DRO_line3D* stripSegmentObj = new DRO_line3D( positions[ index - 1 ] , stripStartStartColor ,
			stripStartEndColor , positions[ index ] , stripStartStartColor ,
			stripStartEndColor , duration , mode );
		g_currentManager->AddDebugObjectTo( WORLDSPACE , stripSegmentObj );
	}

	DRO_line3D* stripSegmentObj = new DRO_line3D( positions[ count - 2 ] , stripStartStartColor ,
		stripStartEndColor , positions[ count - 1 ] , stripEndStartColor ,
		stripEndEndColor , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , stripSegmentObj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldArrow ( Vec3 startPos , Vec3 endPos , Rgba8 shaftStartPosStartColor , Rgba8 shaftStartPosEndColor ,
                          Rgba8 shaftEndPosStartColor , Rgba8 shaftEndPosEndColor , Rgba8 tipStartPosStartColor ,
                          Rgba8 tipStartPosEndColor , Rgba8 tipEndPosStartColor , Rgba8 tipEndPosEndColor ,
                          float duration /*= 0.f */ , eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ ,
                          float shaftRadius /*= 0.9f */ , float tipRadius /*= 1.f */ )
{
	DRO_arrow3D* obj = new DRO_arrow3D( startPos , endPos , shaftStartPosStartColor , shaftStartPosEndColor ,
	                                    shaftEndPosStartColor , shaftEndPosEndColor , tipStartPosStartColor ,
	                                    tipStartPosEndColor , tipEndPosStartColor , tipEndPosEndColor , duration ,
	                                    mode , shaftRadius , tipRadius );
	
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldArrow( Vec3 start , Vec3 end , Rgba8 color , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_arrow3D* obj = new DRO_arrow3D( start , end , color , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldArrow ( Vec3 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec3 endPos ,
                          Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration /*= 0.f */ ,
                          eDebugRenderMode mode /*= DEBUG_RENDER_ALWAYS */ , float radius /*= 1.f */ )
{
	DRO_arrow3D* obj = new DRO_arrow3D( startPos , startPosStartColor , startPosEndColor , 
										  endPos   , endPosStartColor	 ,     endPosEndColor ,
												duration , mode , radius );
	
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldQuad ( Vec3 p0 , Vec3 p1 , Vec3 p2 , Vec3 p3 , AABB2 UVs , Rgba8 startColor , Rgba8 endColor ,
                         float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ ,
                         Texture* tex /*= nullptr */ )
{
	DRO_quad3D* obj = new DRO_quad3D( p0 , p1 , p2 , p3 , startColor , endColor , duration , mode , tex , UVs );

	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldWireBounds( AABB3 bounds , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_aabb3* obj = new DRO_aabb3( bounds , color , duration , mode , eRasterState::WIREFRAME );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_uvSphere* obj = new DRO_uvSphere( pos , radius , startColor , endColor , duration , mode , eRasterState::WIREFRAME );
	//obj->m_transform.SetPosition( pos );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldWireSphere( Vec3 pos , float radius , Rgba8 color , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	DRO_uvSphere* obj = new DRO_uvSphere( pos , radius , color , duration , mode , eRasterState::WIREFRAME );
	//obj->m_transform.SetPosition( pos );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBasis( Mat44 basis , Rgba8 startTint , Rgba8 endTint , float duration , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	Vec3 position = basis.GetTranslation3D();

	Rgba8 ibasisStartColor = RED;
	Rgba8 jbasisStartColor = GREEN;
	Rgba8 kbasisStartColor = BLUE;

	ibasisStartColor += startTint;
	jbasisStartColor += startTint;
	kbasisStartColor += startTint;

	Rgba8 ibasisEndColor = RED;
	Rgba8 jbasisEndColor = GREEN;
	Rgba8 kbasisEndColor = BLUE;

	ibasisEndColor += endTint;
	jbasisEndColor += endTint;
	kbasisEndColor += endTint;
	
	DRO_arrow3D* iBasis = new DRO_arrow3D( position , ibasisStartColor , ibasisEndColor ,
	                                       position + basis.GetIBasis3D() , ibasisStartColor , ibasisEndColor , duration ,
	                                       mode , 0.1f );
	DRO_arrow3D* jBasis = new DRO_arrow3D( position , jbasisStartColor , jbasisEndColor ,
	                                       position + basis.GetJBasis3D() , jbasisStartColor , jbasisEndColor , duration ,
	                                       mode , 0.1f );
	DRO_arrow3D* kBasis = new DRO_arrow3D( position , kbasisStartColor , kbasisEndColor ,
	                                       position + basis.GetKBasis3D() , kbasisStartColor , kbasisEndColor , duration ,
	                                       mode , 0.1f );

	g_currentManager->AddDebugObjectTo( WORLDSPACE , iBasis );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , jBasis );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , kBasis );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBasis( Mat44 basis , float duration /*= 0.0f */ , eDebugRenderMode mode /*= DEBUG_RENDER_USE_DEPTH */ )
{
	Vec3 position = basis.GetTranslation3D();
	
	DRO_arrow3D* iBasis = new DRO_arrow3D( position , position + basis.GetIBasis3D() , RED , duration , mode , 0.1f );
	DRO_arrow3D* jBasis = new DRO_arrow3D( position , position + basis.GetJBasis3D() , GREEN , duration , mode , 0.1f );
	DRO_arrow3D* kBasis = new DRO_arrow3D( position , position + basis.GetKBasis3D() , BLUE , duration , mode , 0.1f );
	
	g_currentManager->AddDebugObjectTo( WORLDSPACE , iBasis );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , jBasis );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , kBasis );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldText( Mat44 basis , Vec2 pivot , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode , char const* text )
{
	
	std::string str = text;

	DRO_text3D* obj = new DRO_text3D( str , basis , pivot , startColor , endColor , 0.14f , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldTextf( Mat44 basis , Vec2 pivot , Rgba8 color , float duration , eDebugRenderMode mode , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );
	
 	DRO_text3D* obj = new DRO_text3D( str , basis , pivot , color , 0.14f , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldTextf( Mat44 basis , Vec2 pivot , Rgba8 color , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text3D* obj = new DRO_text3D( str , basis , pivot , color , 0.14f , 2.f , DEBUG_RENDER_ALWAYS );
	
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBillboardText( Vec3 origin , Vec2 pivot , Rgba8 startColor , Rgba8 endColor , float duration , eDebugRenderMode mode , char const* text )
{
	std::string str = text;

	DRO_text3D* obj = new DRO_text3D( str , origin , pivot , startColor , endColor , 0.14f , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , Rgba8 color , float duration , eDebugRenderMode mode , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text3D* obj = new DRO_text3D( str , origin , pivot , color , 0.14f , duration , mode );
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddWorldBillboardTextf( Vec3 origin , Vec2 pivot , Rgba8 color , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text3D* obj = new DRO_text3D( str , origin , pivot , color , 0.14f , 2.f , DEBUG_RENDER_ALWAYS );
	//obj->m_model	= LookAtMatrix( origin , g_debugCamera )
	g_currentManager->AddDebugObjectTo( WORLDSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 startColor , Rgba8 endColor , float duration )
{
	DRO_point2D* obj = new DRO_point2D( pos , size , startColor , endColor , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , float size , Rgba8 color , float duration /*= 0.0f */ )
{
	DRO_point2D* obj = new DRO_point2D( pos , size , color , color , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenPoint( Vec2 pos , Rgba8 color )
{
	DRO_point2D* obj = new DRO_point2D( pos , 10.f , color , color , 5.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenLine ( Vec2 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec2 endPos ,
                          Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration )
{
	DRO_line2D* obj = new DRO_line2D( startPos , startPosStartColor , startPosEndColor , endPos , endPosStartColor , endPosEndColor , duration , 10.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenLine( Vec2 startPos , Vec2 endPos , Rgba8 color , float duration /*= 0.0f */ )
{
	DRO_line2D* obj = new DRO_line2D( startPos , endPos , color , duration , 10.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow ( Vec2 startPos , Vec2 endPos , Rgba8 shaftStartPosStartColor , Rgba8 shaftStartPosEndColor ,
                           Rgba8 shaftEndPosStartColor , Rgba8 shaftEndPosEndColor , Rgba8 tipStartPosStartColor ,
                           Rgba8 tipStartPosEndColor , Rgba8 tipEndPosStartColor , Rgba8 tipEndPosEndColor ,
                           float duration /*= 0.f */ , float thickness /*= 10.f */ )
{
	DRO_arrow2D* obj = new DRO_arrow2D( startPos , endPos , shaftStartPosStartColor , shaftStartPosEndColor ,
	                                    shaftEndPosStartColor , shaftEndPosEndColor , tipStartPosStartColor ,
	                                    tipStartPosEndColor , tipEndPosStartColor , tipEndPosEndColor , duration ,
	                                    thickness );

	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow ( Vec2 startPos , Rgba8 startPosStartColor , Rgba8 startPosEndColor , Vec2 endPos ,
                           Rgba8 endPosStartColor , Rgba8 endPosEndColor , float duration /*= 0.f */ ,
                           float thickness /*= 10.f */ )
{
	DRO_arrow2D* obj = new DRO_arrow2D( startPos , startPosStartColor , startPosEndColor , endPos , endPosStartColor ,
	                                    endPosEndColor , duration , thickness );

	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenArrow( Vec2 startPos , Vec2 endPos , Rgba8 color , float duration /*= 0.0f */ , float thickness /*= 10.f */ )
{
	DRO_arrow2D* obj = new DRO_arrow2D( startPos , endPos , color , duration , thickness );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenQuad( AABB2 bounds , Rgba8 startColor , Rgba8 endColor , float duration )
{
	DRO_quad2D* obj = new DRO_quad2D( bounds , startColor , endColor , startColor , endColor ,
									  duration , nullptr , AABB2::ZERO_TO_ONE );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenQuad( AABB2 bounds , Rgba8 color , float duration /*= 0.0f */ )
{
	DRO_quad2D* obj = new DRO_quad2D( bounds , color , color , color , color , duration , nullptr , AABB2::ZERO_TO_ONE );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , AABB2 UVs , Rgba8 startTint , Rgba8 endTint , float duration /*= 0.0f */ )
{
	DRO_quad2D* obj = new DRO_quad2D( bounds , startTint , endTint , startTint , endTint ,
									  duration , tex , UVs );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , AABB2 UVs , Rgba8 tint , float duration /*= 0.0f */ )
{
	DRO_quad2D* obj = new DRO_quad2D( bounds , tint , tint , tint , tint ,
									  duration , tex , UVs );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTexturedQuad( AABB2 bounds , Texture* tex , Rgba8 tint /*= WHITE */ , float duration /*= 0.0f */ )
{
	DRO_quad2D* obj = new DRO_quad2D( bounds , tint , tint , tint , tint ,
									  duration , tex , AABB2::ZERO_TO_ONE );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenText( Vec4 pos , Vec2 pivot , float size , Rgba8 startColor , Rgba8 endColor , float duration , char const* text )
{
	std::string str = text;

	DRO_text2D* obj = new DRO_text2D( str , pos , pivot , startColor , endColor , size , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , float size , Rgba8 color , float duration , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text2D* obj = new DRO_text2D( str , pos , pivot , color , size , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , float size , Rgba8 startColor , Rgba8 endColor , float duration , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text2D* obj = new DRO_text2D( str , pos , pivot , startColor , endColor , size , duration );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , float size , Rgba8 color , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text2D* obj = new DRO_text2D( str , pos , pivot , color , size , 5.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugAddScreenTextf( Vec4 pos , Vec2 pivot , Rgba8 color , char const* format , ... )
{
	va_list args;
	va_start( args , format );
	std::string str = Stringv( format , args );

	DRO_text2D* obj = new DRO_text2D( str , pos , pivot , color , 14.f , 5.f );
	g_currentManager->AddDebugObjectTo( SCREENSPACE , obj );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Startup()
{
	if ( g_currentManager == nullptr )
	{
		// instantiating a default DRO_Manager
		g_currentManager = new DebugRenderObjectsManager();
		return;
	}
	g_currentManager = this;

	if ( g_debugCamera == nullptr )
	{
		g_debugCamera = new Camera();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Shutdown()
{
 	delete g_debugCamera;
 	g_debugCamera = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::Update( float deltaSeconds )
{
// 	UNUSED( deltaSeconds );
// 	for ( size_t index = 0; index < m_debugRenderScreenObjects.size(); index++ )
// 	{
// 		if ( m_debugRenderScreenObjects[ index ] )
// 		{
// 			m_debugRenderScreenObjects[ index ]->Update();
// 		}
// 	}
// 
// 	for ( size_t index = 0; index < m_debugRenderWorldObjects.size(); index++ )
// 	{
// 		if ( m_debugRenderWorldObjects[ index ] )
// 		{
// 			m_debugRenderWorldObjects[ index ]->Update();
// 		}
// 	}

	UpdateDebugObjects( m_debugRenderWorldObjectsAlways		, deltaSeconds );
	UpdateDebugObjects( m_debugRenderWorldObjectsUseDepth		, deltaSeconds );
	UpdateDebugObjects( m_debugRenderWorldObjectsXRay			, deltaSeconds );

	UpdateDebugObjects( m_debugRenderScreenObjects			, deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::EndFrame()
{
	CleanupScreenObjects();
	CleanupWorldObjects();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::EnableDebugRendering()
{
	m_isDebugRenderEnabled = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::DisableDebugRendering()
{
	m_isDebugRenderEnabled = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::DebugRenderWorldToCamera( Camera* cam )
{
	UNUSED( cam );
	// TODO :- Implemement me
	DEBUGBREAK();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::DebugRenderScreenTo( Texture* output )
{
	RenderContext* ctx = output->GetRenderContext();

	Camera camera;
	camera.SetColorTarget( output );
	g_debugRenderContext = ctx;

	Vec2 min = Vec2::ZERO;
	Vec2 max = Vec2( output->GetDimensions() );

	camera.SetProjectionOrthographic( max.y - min.y , -1.0f , 1.0f );
	// should I clear?
	camera.SetClearMode( CLEAR_NONE , BLACK );

	//std::vector< Vertex_PCU > vertices;

	g_debugRenderContext->BeginCamera( camera );

	// TODO :- Implemement me
	DEBUGBREAK();
	
	g_debugRenderContext->EndCamera( camera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupScreenObjects()
{
	CleanupDebugObjects( m_debugRenderScreenObjects );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupWorldObjects()
{
	CleanupDebugObjects( m_debugRenderWorldObjectsAlways );
	CleanupDebugObjects( m_debugRenderWorldObjectsUseDepth );
	CleanupDebugObjects( m_debugRenderWorldObjectsXRay );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::AddDebugObjectTo( eRenderSpace space , DebugRenderObject* object )
{
	eDebugRenderMode objectRenderMode = object->m_renderMode;
	
	switch ( space )
	{
		case WORLDSPACE:
							{
								switch ( objectRenderMode )
								{
									case DEBUG_RENDER_ALWAYS:
																	g_currentManager->m_debugRenderWorldObjectsAlways.push_back( object );
																	break;
									case DEBUG_RENDER_USE_DEPTH:
																	g_currentManager->m_debugRenderWorldObjectsUseDepth.push_back( object );
																	break;
									case DEBUG_RENDER_XRAY:
																	g_currentManager->m_debugRenderWorldObjectsXRay.push_back( object );
																	break;
									default:
									break;
								}
							}break;
		case SCREENSPACE:
							{
								g_currentManager->m_debugRenderScreenObjects.push_back( object );
				
							}break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::CleanupDebugObjects( std::vector<DebugRenderObject*>& droArray )
{
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] )
		{
			if ( droArray[ index ]->m_isGarbage )
			{
				delete droArray[ index ];
				droArray[ index ] = nullptr;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::UpdateDebugObjects( std::vector<DebugRenderObject*>& droArray , float deltaSeconds )
{
	UNUSED( deltaSeconds );
	
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] )
		{
			droArray[ index ]->Update();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectsAlways( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();
	
	cam->SetClearMode( CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	//cam->CreateMatchingDepthStencilTarget();
	
	RenderObjectArray( droArray , cam );

	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
}
//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectsUseDepth( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();

	cam->SetClearMode( CLEAR_NONE , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	//cam->CreateMatchingDepthStencilTarget();

	RenderObjectArray( droArray , cam );

	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectsXRAY( std::vector<DebugRenderObject*>& droArray , Camera* cam , eBlendMode blendMode )
{
	uint originalClearMode = cam->GetClearMode();
	Rgba8 originalClearColor = cam->GetClearColor();

	cam->SetClearMode( CLEAR_NONE , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	g_debugRenderContext->SetDepthTest( COMPARE_LESS , false );
	
	RenderObjectArray( droArray , cam );

	g_debugRenderContext->EndCamera( *cam );

	//cam->SetClearMode( CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK );
	g_debugRenderContext->BeginCamera( *cam );
	g_debugRenderContext->SetBlendMode( blendMode );
	g_debugRenderContext->SetDepthTest( COMPARE_GREATER , false );

	RenderObjectArrayXRAYPass2( droArray , cam );


	//cam->SetClearMode( originalClearMode , originalClearColor );
	//g_debugRenderContext->BeginCamera( *cam );
	//g_debugRenderContext->EndCamera( *cam );


	
	cam->SetClearMode( originalClearMode , originalClearColor );
	g_debugRenderContext->EndCamera( *cam );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectArray( std::vector<DebugRenderObject*>& droArray , Camera* cam )
{
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] == nullptr )
		{
			continue;
		}
		g_debugRenderContext->BindShader( nullptr );
		//g_debugRenderContext->SetRasterState( FILL_SOLID );
		g_debugRenderContext->BindTexture( nullptr );
		
		eDebugRenderObjectType objType = droArray[ index ]->m_objectType;

		switch ( objType )
		{
			case DRO_INVALID:
			{
				ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
			}	break;

			case DRO_POINT2D:
			{
				DRO_point2D* point2D = ( DRO_point2D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				g_debugRenderContext->SetModelMatrix( point2D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawDisc( point2D->m_position , point2D->m_size , point2D->m_currrentColor );

			}break;

			case DRO_LINE2D:
			{
				DRO_line2D* line2D = ( DRO_line2D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				g_debugRenderContext->SetModelMatrix( line2D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawLine( line2D->m_startPos , line2D->m_endPos , line2D->m_startPosCurrentColor ,
				                                line2D->m_endPosCurrentColor , line2D->m_thickness );

			}break;

			case DRO_ARROW2D:
			{
				DRO_arrow2D* arrow2D = ( DRO_arrow2D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				g_debugRenderContext->SetModelMatrix( arrow2D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawArrow2D( arrow2D->m_startPos , arrow2D->m_endPos ,
				                                   arrow2D->m_shaftStartPosCurrentColor ,
				                                   arrow2D->m_shaftEndPosCurrentColor , arrow2D->m_tipStartPosCurrentColor ,
				                                   arrow2D->m_tipEndPosCurrentColor , arrow2D->m_thickness );

			}break;

			case DRO_QUAD2D:
			{
				DRO_quad2D* quad2D = ( DRO_quad2D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				g_debugRenderContext->SetModelMatrix( quad2D->m_transform.GetAsMatrix() );
				g_debugRenderContext->BindTexture( quad2D->m_texture );
				g_debugRenderContext->DrawAABB2( quad2D->m_bounds , quad2D->m_startPosCurrentColor ,
				                                 quad2D->m_endPosCurrentColor , quad2D->m_texUVs.m_mins ,
				                                 quad2D->m_texUVs.m_maxs );
				g_debugRenderContext->BindTexture( nullptr );

			}break;

			case DRO_TEXT2D:
			{
				DRO_text2D* text2D = ( DRO_text2D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				g_debugRenderContext->SetModelMatrix( text2D->m_transform.GetAsMatrix() );
				g_debugRenderContext->BindTexture( g_bitmapFont->GetTexture() );

				std::vector<Vertex_PCU> textVerts;
				AABB2 screenArea( cam->GetOrthoMin().GetXYComponents() , cam->GetOrthoMax().GetXYComponents() );
				AABB2 textArea( screenArea.m_mins , Vec2( text2D->m_text.size() * text2D->m_size * 1.f , text2D->m_size ) );
					
				float ratioX = RangeMapFloat( 0.f , 1.f , screenArea.m_mins.x , screenArea.m_maxs.x , text2D->m_screenPositionOffsetRatio.z );
				float ratioY = RangeMapFloat( 0.f , 1.f , screenArea.m_mins.y , screenArea.m_maxs.y , text2D->m_screenPositionOffsetRatio.w );
				textArea.Translate( Vec2( ratioX , ratioY ) );
				
				textArea.Translate( Vec2( text2D->m_screenPositionOffsetRatio.x , text2D->m_screenPositionOffsetRatio.y ) );

				float pivotX = ( textArea.m_maxs.x - textArea.m_mins.x ) * text2D->m_pivot.x ;
				float pivotY = ( textArea.m_maxs.y - textArea.m_mins.y ) * text2D->m_pivot.y; 
				textArea.Translate( Vec2( -pivotX , -pivotY ) );
					
				g_bitmapFont->AddVertsForTextInBox2D( textVerts , textArea , text2D->m_size , text2D->m_text ,
				                                      text2D->m_currrentColor , 1.f , text2D->m_pivot );
				g_debugRenderContext->DrawVertexArray( textVerts );
				g_debugRenderContext->BindTexture( nullptr );

			}break;
			
			case DRO_POINT3D:
			{
				DRO_point3D* point3D = ( DRO_point3D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> pointMeshVerts;
				std::vector<uint>		pointIndices;
				CreateUVSphere( 16 , 8 , pointMeshVerts , pointIndices , point3D->m_size , Vec3::ZERO , point3D->m_currrentColor );
				GPUMesh pointMesh( g_debugRenderContext );
				pointMesh.UpdateVertices( pointMeshVerts );
				pointMesh.UpdateIndices( pointIndices );

				g_debugRenderContext->SetModelMatrix( point3D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &pointMesh );

			}break;

			case DRO_LINE3D:
			{
				DRO_line3D* line3D = ( DRO_line3D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> line3DMeshVerts;
				std::vector<uint>		line3DIndices;
				CreateCylinder( line3DMeshVerts , line3DIndices , line3D->m_radius , line3D->m_startPos ,
					line3D->m_endPos , line3D->m_startPosCurrentColor , line3D->m_endPosCurrentColor );

				GPUMesh line3DMesh( g_debugRenderContext );
				line3DMesh.UpdateVertices( line3DMeshVerts );
				line3DMesh.UpdateIndices( line3DIndices );

				g_debugRenderContext->SetModelMatrix( line3D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &line3DMesh );

			}break;

			case DRO_ARROW3D:
			{
				DRO_arrow3D* arrow3D = ( DRO_arrow3D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> arrow3DMeshVerts;
				std::vector<uint>		arrow3DIndices;
				CreateArrow3D( arrow3DMeshVerts , arrow3DIndices , arrow3D->m_shaftRadius , arrow3D->m_tipRadius ,
					arrow3D->m_startPos , arrow3D->m_endPos ,
					arrow3D->m_shaftStartPosCurrentColor , arrow3D->m_shaftEndPosCurrentColor ,
					arrow3D->m_tipStartPosCurrentColor , arrow3D->m_tipEndPosCurrentColor );

				GPUMesh arrow3DMesh( g_debugRenderContext );
				arrow3DMesh.UpdateVertices( arrow3DMeshVerts );
				arrow3DMesh.UpdateIndices( arrow3DIndices );

				g_debugRenderContext->SetModelMatrix( arrow3D->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &arrow3DMesh );
					
			}break;

			case DRO_AABB3:
			{
				DRO_aabb3* box = ( DRO_aabb3* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> boxMeshVerts;
				std::vector<uint>		boxIndices;
				CreateCuboid( boxMeshVerts , boxIndices , box->m_AABB3 , box->m_currrentColor );

				GPUMesh boxMesh( g_debugRenderContext );
				boxMesh.UpdateVertices( boxMeshVerts );
				boxMesh.UpdateIndices( boxIndices );

				g_debugRenderContext->SetRasterState( box->m_rasterState );
				g_debugRenderContext->SetModelMatrix( box->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &boxMesh );
					
			}break;


			case DRO_QUAD3D:
			{
				DRO_quad3D* quad = ( DRO_quad3D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
	
				g_debugRenderContext->SetModelMatrix( quad->m_transform.GetAsMatrix() );
				g_debugRenderContext->BindTexture( quad->m_texture );
				g_debugRenderContext->DrawQuad3D( quad->m_vert0 , quad->m_vert1 , quad->m_vert2 , quad->m_vert3 ,
				                                  quad->m_texUVs , quad->m_startPosCurrentColor ,
				                                  quad->m_startPosCurrentColor );
				g_debugRenderContext->BindTexture( nullptr );

			}break;

			case DRO_UVSPHERE:
			{
				DRO_uvSphere* uvSphere = ( DRO_uvSphere* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				std::vector<Vertex_PCU> uvSphereMeshVerts;
				std::vector<uint>		uvSphereIndices;
				CreateUVSphere( 8,4 , uvSphereMeshVerts , uvSphereIndices , uvSphere->m_radius , uvSphere->m_position , uvSphere->m_currrentColor );
				
				GPUMesh uvSphereMesh( g_debugRenderContext );
				uvSphereMesh.UpdateVertices( uvSphereMeshVerts );
				uvSphereMesh.UpdateIndices( uvSphereIndices );

				g_debugRenderContext->SetRasterState( uvSphere->m_rasterState );
				g_debugRenderContext->SetModelMatrix( uvSphere->m_transform.GetAsMatrix() );
				g_debugRenderContext->DrawMesh( &uvSphereMesh );

			}break;

			case DRO_TEXT3D:
			{
				DRO_text3D* text3D = ( DRO_text3D* ) droArray[ index ];

				g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
				g_debugRenderContext->SetModelMatrix( text3D->m_model );
				g_debugRenderContext->BindTexture( g_bitmapFont->GetTexture() );
				//g_debugRenderContext->BindTexture( g_debugRenderContext->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" ) );

				std::vector<Vertex_PCU> textVerts;
				AABB2 textArea( Vec2::ZERO , Vec2( textVerts.size()* text3D->m_size* text3D->m_textCellAspectRatio , text3D->m_size ) );
				//AABB2 textArea( -0.5f ,-0.5f , 0.5f , 0.5f );
				
				g_bitmapFont->AddVertsForTextInBox2D( textVerts , textArea , text3D->m_size , text3D->m_text , text3D->m_currrentColor , 1.f , text3D->m_pivot );
				g_debugRenderContext->DrawVertexArray( textVerts );
				g_debugRenderContext->BindTexture( nullptr );

			}break;
			
			default:
			{
				ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 2D OBJECT IN A PROJECTIVE CAMERA" );
			}
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DebugRenderObjectsManager::RenderObjectArrayXRAYPass2( std::vector<DebugRenderObject*>& droArray , Camera* cam )
{
	for ( size_t index = 0; index < droArray.size(); index++ )
	{
		if ( droArray[ index ] == nullptr )
		{
			continue;
		}

		eDebugRenderObjectType objType = droArray[ index ]->m_objectType;

		switch ( objType )
		{
		case DRO_INVALID:
		{
			ASSERT_OR_DIE( true , "INVALID DEBUG OBJECT TYPE" );
		}	break;

		case DRO_POINT3D:
		{
			DRO_point3D* point3D = ( DRO_point3D* ) droArray[ index ];
			Rgba8 pointColorCopy = point3D->m_currrentColor;

			pointColorCopy *= 0.5f;
				
			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> pointMeshVerts;
			std::vector<uint>		pointIndices;
			CreateUVSphere( 16 , 8 , pointMeshVerts , pointIndices , point3D->m_size , Vec3::ZERO , pointColorCopy );
			GPUMesh pointMesh( g_debugRenderContext );
			pointMesh.UpdateVertices( pointMeshVerts );
			pointMesh.UpdateIndices( pointIndices );

			g_debugRenderContext->BindShader( nullptr );
			g_debugRenderContext->SetBlendMode( eBlendMode::ALPHA );
			g_debugRenderContext->BindTexture( nullptr );

			g_debugRenderContext->SetModelMatrix( point3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &pointMesh );

		}break;

		case DRO_LINE3D:
		{
			DRO_line3D* line3D = ( DRO_line3D* ) droArray[ index ];
			Rgba8 lineStartColorCopy = line3D->m_startPosCurrentColor;
			lineStartColorCopy *= 0.5f;

			Rgba8 lineEndColorCopy = line3D->m_endPosCurrentColor;
			lineEndColorCopy *= 0.5f;
				
			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> line3DMeshVerts;
			std::vector<uint>		line3DIndices;
			CreateCylinder( line3DMeshVerts , line3DIndices , line3D->m_radius , line3D->m_startPos ,
				line3D->m_endPos , lineStartColorCopy , lineEndColorCopy );

			GPUMesh line3DMesh( g_debugRenderContext );
			line3DMesh.UpdateVertices( line3DMeshVerts );
			line3DMesh.UpdateIndices( line3DIndices );

			g_debugRenderContext->BindShader( nullptr );
			g_debugRenderContext->SetBlendMode( eBlendMode::ALPHA );
			//g_debugRenderContext->SetRasterState( FILL_SOLID );
			g_debugRenderContext->BindTexture( nullptr );

			g_debugRenderContext->SetModelMatrix( line3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &line3DMesh );

		}break;

		case DRO_ARROW3D:
		{
			DRO_arrow3D* arrow3D = ( DRO_arrow3D* ) droArray[ index ];
			Rgba8 shaftStartColorCopy = arrow3D->m_shaftStartPosCurrentColor;
			shaftStartColorCopy *= 0.5f;

			Rgba8 shaftEndColorCopy = arrow3D->m_shaftEndPosCurrentColor;
			shaftEndColorCopy *= 0.5f;

			Rgba8 tipStartColorCopy = arrow3D->m_tipStartPosCurrentColor;
			tipStartColorCopy *= 0.5f;

			Rgba8 tipEndColorCopy = arrow3D->m_tipEndPosCurrentColor;
			tipEndColorCopy *= 0.5f;
				
			g_debugRenderContext->BindDepthStencil( cam->GetDepthStencilTarget() );
			std::vector<Vertex_PCU> arrow3DMeshVerts;
			std::vector<uint>		arrow3DIndices;
			CreateArrow3D( arrow3DMeshVerts , arrow3DIndices , arrow3D->m_shaftRadius , arrow3D->m_tipRadius ,
				arrow3D->m_startPos , arrow3D->m_endPos ,
				shaftStartColorCopy , shaftEndColorCopy ,
				tipStartColorCopy , tipEndColorCopy );

			GPUMesh arrow3DMesh( g_debugRenderContext );
			arrow3DMesh.UpdateVertices( arrow3DMeshVerts );
			arrow3DMesh.UpdateIndices( arrow3DIndices );

			g_debugRenderContext->BindShader( nullptr );
			g_debugRenderContext->SetBlendMode( eBlendMode::ALPHA );
			//g_debugRenderContext->SetRasterState( FILL_SOLID );
			g_debugRenderContext->BindTexture( nullptr );

			g_debugRenderContext->SetModelMatrix( arrow3D->m_transform.GetAsMatrix() );
			g_debugRenderContext->DrawMesh( &arrow3DMesh );
		}
		break;

		default:
		{
			ASSERT_RECOVERABLE( true , "ARE YOU SURE YOU WANT TO RENDER A 2D OBJECT IN A PROJECTIVE CAMERA" );
		}
		break;
		}
	}

	g_debugRenderContext->BindShader( nullptr );
	g_debugRenderContext->SetBlendMode( eBlendMode::SOLID );
	g_debugRenderContext->SetRasterState( FILL_SOLID );
	g_debugRenderContext->BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------