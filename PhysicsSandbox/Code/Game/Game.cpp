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
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"

#include "Game/TheApp.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern Physics2D*		g_thePhysicsSystem;
extern TheApp*			g_theApp;

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
	
	
	m_fillColor.a = 127;

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
	Vec2 nearestPointOnDisc2D		  = GetNearestPointOnDisc2D( m_mousePosition , m_disc2D.m_center, m_disc2D.m_radius );
	
	Rgba8 nearestPointColor				= WHITE;
	Rgba8 pointIsInsideObjectColor		= RED;
	Rgba8 objectColorWhenPointIsInside  = MAGENTA;
	objectColorWhenPointIsInside.a		= 225;
	
	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnAABB2D			, WHITE , 2.f );
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
		g_theRenderer->DrawAABB2( m_aabb2D , RED );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , nearestPointColor );
	}
	
	if ( IsPointOnDisc2D( m_disc2D , m_mousePosition ) )
	{
		g_theRenderer->DrawDisc( m_disc2D , objectColorWhenPointIsInside );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , pointIsInsideObjectColor );
	}
	else
	{
		g_theRenderer->DrawDisc( m_disc2D , GREEN );
		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , nearestPointColor );
	}
	
	for ( size_t coliderIndex = 0; coliderIndex < g_thePhysicsSystem->m_colliders2D.size(); coliderIndex++ )
	{
		DiscCollider2D* temp = ( DiscCollider2D* ) g_thePhysicsSystem->m_colliders2D[ coliderIndex ];
		if ( IsPointOnDisc2D( m_mousePosition, temp->m_worldPosition,temp->m_radius ) )
		{
			g_thePhysicsSystem->m_colliders2D[ coliderIndex ]->DebugRender( g_theRenderer , YELLOW , m_fillColor );
		}
		else
		{
			g_thePhysicsSystem->m_colliders2D[ coliderIndex ]->DebugRender( g_theRenderer , BLUE , m_fillColor );
		}
	}

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
		m_worldCamera.CorrectAspectRaio( CLIENT_ASPECT );
		m_worldCamera.SetProjectionOrthographic( currentCameraOutputSize.y );
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		m_currentColliderRadius = m_rng.RollRandomFloatInRange( m_minColliderRadius , m_maxColliderRadius );
		g_thePhysicsSystem->CreateRigidbody( m_mousePosition , m_mousePosition , m_currentColliderRadius );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------