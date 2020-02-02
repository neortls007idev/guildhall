#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern AudioSystem* g_theAudioSystem;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC RandomNumberGenerator Game::m_rng;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	//m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 1600.f , 800.f ) );
	m_worldCamera.SetOutputSize( Vec2( 1600.f , 800.f ) );
	//m_worldCamera.SetProjectionOrthographic( 800.f );
	m_worldCamera.SetPosition( m_cameraDefaultPosition );
	//m_worldCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );
	Vec2 cameraBottomLeft = m_worldCamera.GetPosition() - ( m_worldCamera.GetOutputSize() / 2.f );
	Vec2 cameraTopRight = m_worldCamera.GetPosition() + ( m_worldCamera.GetOutputSize() / 2.f );
	
	m_color.RollRandomColor( m_rng );
	m_color.a = 100;

	m_mousePosition = g_theInput->GetMouseNormalizedClientPosition() * ( cameraTopRight - cameraBottomLeft );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	Vec2 cameraBottomLeft = m_worldCamera.GetPosition() - ( m_worldCamera.GetOutputSize() / 2.f );
	Vec2 cameraTopRight = m_worldCamera.GetPosition() + ( m_worldCamera.GetOutputSize() / 2.f );
	UpdateFromKeyBoard( deltaSeconds );
	m_mousePosition = g_theInput->GetMouseNormalizedClientPosition() * ( cameraTopRight - cameraBottomLeft );
	UpdateCamera();
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_worldCamera );
	g_theRenderer->BindTexture( nullptr );

	Vec2 nearestPointOnAABB2D		  = GetNearestPointOnAABB2D( m_mousePosition , m_aabb2D );
	Vec2 nearestPointOnOBB2D		  = GetNearestPointOnOBB2D( m_mousePosition , m_Obb2D );
	Vec2 nearestPointOnCapsule2D	  = GetNearestPointOnCapsule2D( m_mousePosition , m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius );
	Vec2 nearestPointOnDisc2D		  = GetNearestPointOnDisc2D( m_mousePosition , m_disc2D.m_center, m_disc2D.m_radius );
	Vec2 nearestPointOnLineSegment2D  = GetNearestPointOnLineSegment2D( m_mousePosition  , m_lineSegment2D.m_start, m_lineSegment2D.m_end );

	Rgba8 nearestPointColor				= WHITE;
	Rgba8 pointIsInsideObjectColor		= RED;
	Rgba8 objectColorWhenPointIsInside  = m_color;
	objectColorWhenPointIsInside.a		= 225;
	
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnAABB2D			, WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnOBB2D			, WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnCapsule2D	    , WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnLineSegment2D  , WHITE , 2.f );
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnDisc2D         , WHITE , 2.f );
	
	OBB2 mouseCursor = OBB2( m_mousePosition , Vec2( 30 , 50 ) , Vec2( .6f , .8f ) );
	
	if ( m_hasCursorChangedToOBB )
	{
		g_theRenderer->DrawOBB2( mouseCursor , Rgba8( 255 , 0 , 0 , 100 ) );
	}

	if ( IsPointInsideAABB2D( m_mousePosition , m_aabb2D ) )
	{
		g_theRenderer->DrawAABB2( m_aabb2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawAABB2( m_aabb2D , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , nearestPointColor );
	}

	if ( IsPointInsideOBB2D( m_mousePosition , m_Obb2D ) )
	{
		
		g_theRenderer->DrawOBB2( m_Obb2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnOBB2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawOBB2( m_Obb2D , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnOBB2D , 2.f ) , nearestPointColor );
	}


	if ( IsPointInsideCapsule2D( m_mousePosition , m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius ) )
	{
		g_theRenderer->DrawCapsule2D( m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnCapsule2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawCapsule2D( m_capsule2D.m_capsuleMidStart , m_capsule2D.m_capsuleMidEnd , m_capsule2D.m_radius , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnCapsule2D , 2.f ) , nearestPointColor );
	}
	
	if ( IsPointOnDisc2D( m_disc2D , m_mousePosition ) )
	{
		g_theRenderer->DrawDisc( m_disc2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawDisc( m_disc2D , m_color );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , nearestPointColor );
	}

	if ( m_hasCursorChangedToOBB && DoOBBsOverlap( mouseCursor , m_Obb2D ) )
	{
		g_theRenderer->DrawOBB2( m_Obb2D , Rgba8( 255 , 255 , 0 , 255 ) );
		g_theRenderer->DrawOBB2( mouseCursor , Rgba8( 255 , 0 , 0 , 255 ) );
	}

	g_theRenderer->DrawLine( m_lineSegment2D.m_start, m_lineSegment2D.m_end , m_color , 5.f );

	DrawMouseCurrentPosition( m_worldCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
	m_worldCamera.SetPosition( m_cameraCurrentPosition );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawMouseCurrentPosition( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedClientPosition();
//	AABB2 orthoBounds = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
//Vec2 mouseDrawPos = orthoBounds.GetPointForUV( mouseNormalizedPos );
	Vec2 mouseCurrentClientPosition = camera.GetClientToWorldPosition( mouseNormalizedPos );
	//g_theRenderer->DrawRing( mouseDrawPos , 2.f , RED , 2.f );
	g_theRenderer->DrawDisc( Disc2D( mouseCurrentClientPosition , 2.5f ) , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard( float deltaSeconds )
{
	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_cameraCurrentPosition = m_cameraDefaultPosition;
	}
	
	if ( g_theInput->IsKeyHeldDown( 'D' ) )
	{
		m_cameraCurrentPosition += ( Vec3( m_cameraMoveVelocity.x , 0.f , 0.f ) * deltaSeconds );
	}
	
	if ( g_theInput->IsKeyHeldDown( 'A' ) )
	{
		m_cameraCurrentPosition -= ( Vec3( m_cameraMoveVelocity.x , 0.f , 0.f ) * deltaSeconds );
	}

	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		m_cameraCurrentPosition -= ( Vec3( 0.f , m_cameraMoveVelocity.y , 0.f ) * deltaSeconds );
	}

	if ( g_theInput->IsKeyHeldDown( 'W' ) )
	{
		m_cameraCurrentPosition += ( Vec3( 0.f , m_cameraMoveVelocity.y , 0.f ) * deltaSeconds );
	}
	if ( g_theInput->GetMouseWheelValue() < 0 )
	{
		Vec2 currentCameraOutputSize = m_worldCamera.GetOutputSize() + ( Vec2( MAX_CAMERA_ZOOM_VELOCITY_X , MAX_CAMERA_ZOOM_VELOCITY_Y ) * deltaSeconds );
		currentCameraOutputSize.x = Clamp( currentCameraOutputSize.x , 200.f , 20000.f );
		currentCameraOutputSize.y = Clamp( currentCameraOutputSize.y , 200.f , 20000.f );
		m_worldCamera.SetOutputSize( currentCameraOutputSize );
		m_worldCamera.SetProjectionOrthographic( currentCameraOutputSize.y );
	}

	if ( g_theInput->GetMouseWheelValue() > 0 )
	{
		Vec2 currentCameraOutputSize = m_worldCamera.GetOutputSize() - ( Vec2( MAX_CAMERA_ZOOM_VELOCITY_X , MAX_CAMERA_ZOOM_VELOCITY_Y ) * deltaSeconds );
		currentCameraOutputSize.x = Clamp( currentCameraOutputSize.x , 200.f , 20000.f );
		currentCameraOutputSize.y = Clamp( currentCameraOutputSize.y , 200.f , 20000.f );
		m_worldCamera.SetOutputSize( currentCameraOutputSize );
		m_worldCamera.SetProjectionOrthographic( currentCameraOutputSize.y );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------