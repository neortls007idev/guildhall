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
#include "Game/Gameobject.hpp"

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
	m_worldCamera.SetOutputSize( m_currentCameraOutputSize );
	m_worldCamera.SetPosition( m_cameraDefaultPosition );
		
	m_mousePosition = g_theInput->GetMouseNormalizedClientPosition();
	m_mousePosition = m_worldCamera.GetClientToWorldPosition( m_mousePosition );

	InitialGameObjectsSpawner();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitialGameObjectsSpawner()
{
	int numInitialGameObjects = m_rng.RollRandomIntInRange( MIN_INITIAL_GAME_OBJECTS , MAX_INITIAL_GAME_OBJECTS );

	for ( int index = 0; index < numInitialGameObjects; index++ )
	{
		m_currentColliderRadius = m_rng.RollRandomFloatInRange( m_minColliderRadius , m_maxColliderRadius );
		Vec2 spawnPos = Vec2( m_rng.RollRandomFloatBetweenZerotoAlmostOne() , m_rng.RollRandomFloatBetweenZerotoAlmostOne() );
		spawnPos = m_worldCamera.GetWorldNormalizedToClientPosition( spawnPos );
		GameObject* temp = new GameObject( g_thePhysicsSystem , spawnPos , Vec2::ZERO , m_currentColliderRadius );
		m_gameObjects.push_back( temp );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UpdateFromKeyBoard( deltaSeconds );
	m_mousePosition = m_worldCamera.GetClientToWorldPosition( m_mousePosition );
	m_mousePosition = m_worldCamera.GetWorldNormalizedToClientPosition( m_mousePosition );
	UpdateCamera();
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_worldCamera );
	g_theRenderer->BindTexture( nullptr );

// 	Vec2 nearestPointOnAABB2D		  = GetNearestPointOnAABB2D( m_mousePosition , m_aabb2D );
// 	Vec2 nearestPointOnDisc2D		  = GetNearestPointOnDisc2D( m_mousePosition , m_disc2D.m_center, m_disc2D.m_radius );
	
// 	Rgba8 nearestPointColor				= WHITE;
// 	Rgba8 pointIsInsideObjectColor		= RED;
// 	Rgba8 objectColorWhenPointIsInside  = MAGENTA;
// 	objectColorWhenPointIsInside.a		= 225;
// 	
// 	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnAABB2D			, WHITE , 2.f );
// 	g_theRenderer->DrawLine( m_mousePosition , nearestPointOnDisc2D         , WHITE , 2.f );
// 	
// 	OBB2 mouseCursor = OBB2( m_mousePosition , Vec2( 30 , 50 ) , Vec2( .6f , .8f ) );
// 	
// 	if ( m_hasCursorChangedToOBB )
// 	{
// 		g_theRenderer->DrawOBB2( mouseCursor , Rgba8( 255 , 0 , 0 , 100 ) );
// 	}
// 
// 	if ( IsPointInsideAABB2D( m_mousePosition , m_aabb2D ) )
// 	{
// 		g_theRenderer->DrawAABB2( m_aabb2D , objectColorWhenPointIsInside );
// 		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , pointIsInsideObjectColor );
// 	}
// 	else
// 	{
// 		g_theRenderer->DrawAABB2( m_aabb2D , RED );
// 		g_theRenderer->DrawDisc( Disc2D( nearestPointOnAABB2D , 2.f ) , nearestPointColor );
// 	}
// 	
// 	if ( IsPointOnDisc2D( m_disc2D , m_mousePosition ) )
// 	{
// 		g_theRenderer->DrawDisc( m_disc2D , objectColorWhenPointIsInside );
// 		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , pointIsInsideObjectColor );
// 	}
// 	else
// 	{
// 		g_theRenderer->DrawDisc( m_disc2D , GREEN );
// 		g_theRenderer->DrawDisc( Disc2D( nearestPointOnDisc2D , 2.f ) , nearestPointColor );
// 	}
	
	for ( size_t firstColliderIndex = 0; firstColliderIndex < g_thePhysicsSystem->m_colliders2D.size() ; firstColliderIndex++ )
	{
		DiscCollider2D* firstCollider = ( DiscCollider2D* ) g_thePhysicsSystem->m_colliders2D[ firstColliderIndex ];

		if ( IsPointOnDisc2D( m_mousePosition, firstCollider->m_worldPosition,firstCollider->m_radius ) )
		{
			firstCollider->DebugRender( g_theRenderer , YELLOW , m_fillColor );
		}
		else
		{
			g_thePhysicsSystem->m_colliders2D[ firstColliderIndex ]->DebugRender( g_theRenderer , BLUE , m_fillColor );
		}
		/*for ( size_t secondColliderIndex = 1; secondColliderIndex < g_thePhysicsSystem->m_colliders2D.size() - 1; secondColliderIndex++ )
		{
			DiscCollider2D* secondCollider = ( DiscCollider2D* ) g_thePhysicsSystem->m_colliders2D[ secondColliderIndex ];
			
			if ( secondCollider == firstCollider )
			{
				continue;
			}
			else
			{
				if ( DoDiscsOverlap( firstCollider->m_worldPosition , firstCollider->m_radius , secondCollider->m_worldPosition , secondCollider->m_radius ) )
				{
					firstCollider->DebugRender( g_theRenderer , YELLOW , m_overlapColor );
					secondCollider.
				}
			}
		}
	*/
	}

	DrawMouseCurrentPosition( m_worldCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
	m_worldCamera.SetPosition( m_cameraCurrentPosition );
	m_worldCamera.SetOutputSize( m_currentCameraOutputSize );
	m_worldCamera.CorrectAspectRaio( CLIENT_ASPECT );
	m_worldCamera.SetProjectionOrthographic( m_currentCameraOutputSize.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawMouseCurrentPosition( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	Vec2 mouseCurrentClientPosition = camera.GetClientToWorldPosition( mouseNormalizedPos );
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
		m_currentCameraOutputSize = m_worldCamera.GetOutputSize() + ( Vec2( MAX_CAMERA_ZOOM_VELOCITY_X , MAX_CAMERA_ZOOM_VELOCITY_Y ) * deltaSeconds );
		m_currentCameraOutputSize.x = Clamp( m_currentCameraOutputSize.x , 200.f , 20000.f );
		m_currentCameraOutputSize.y = Clamp( m_currentCameraOutputSize.y , 200.f , 20000.f );
	}

	if ( g_theInput->GetMouseWheelValue() > 0 )
	{
		m_currentCameraOutputSize = m_worldCamera.GetOutputSize() - ( Vec2( MAX_CAMERA_ZOOM_VELOCITY_X , MAX_CAMERA_ZOOM_VELOCITY_Y ) * deltaSeconds );
		m_currentCameraOutputSize.x = Clamp( m_currentCameraOutputSize.x , 200.f , 20000.f );
		m_currentCameraOutputSize.y = Clamp( m_currentCameraOutputSize.y , 200.f , 20000.f );
	}

	if ( g_theInput->WasRightMouseButtonJustPressed() )
	{
		m_currentColliderRadius = m_rng.RollRandomFloatInRange( m_minColliderRadius , m_maxColliderRadius );
		Vec2 spawnPos = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
		GameObject* temp = new GameObject( g_thePhysicsSystem , spawnPos , Vec2::ZERO , m_currentColliderRadius );
		m_gameObjects.push_back( temp );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------