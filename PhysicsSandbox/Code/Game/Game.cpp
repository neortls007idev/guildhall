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
#include "Engine/Physics/Rigidbody2D.hpp"

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
		m_isMouseOnGameObject.push_back( false );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UpdateFromKeyBoard( deltaSeconds );
	m_mousePosition = m_worldCamera.GetClientToWorldPosition( m_mousePosition );
	m_mousePosition = m_worldCamera.GetWorldNormalizedToClientPosition( m_mousePosition );
	UpdateCamera();
	UpdateGameObjectPosition();
	UpdateGameObjects();
	g_thePhysicsSystem->Update( deltaSeconds );
	//UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_worldCamera );
	g_theRenderer->BindTexture( nullptr );

	for ( size_t index = 0; index < g_thePhysicsSystem->m_colliders2D.size(); index++ )
	{
		DiscCollider2D* Collider = ( DiscCollider2D* ) g_thePhysicsSystem->m_colliders2D[ index ];
		Collider->DebugRender( g_theRenderer , m_gameObjects[index]->m_borderColor , m_gameObjects[index]->m_fillColor );
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

void Game::UpdateGameObjectPosition()
{
	if ( !m_selectedGameObject )
	{
		return;
	}
	else
	{
		Vec2 newWorldPosition = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
		m_selectedGameObject->m_rigidbody->SetPosition( newWorldPosition );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateGameObjects()
{
	for ( size_t colliderIndex = 0; colliderIndex < m_gameObjects.size(); colliderIndex++ )
	{
		Collider2D* collider = m_gameObjects[ colliderIndex ]->m_rigidbody->GetCollider();

		bool isMouseInside = collider->Contains( m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() ) );

		bool isSelectedGameObject = m_gameObjects[ colliderIndex ]->m_isSelected;

		if ( isSelectedGameObject )
		{
			m_gameObjects[ colliderIndex ]->m_borderColor = m_selectedObjectColor;
		}

		if ( isMouseInside && !isSelectedGameObject )
		{
			m_gameObjects[ colliderIndex ]->m_borderColor = YELLOW;
		}
		else if ( !isMouseInside && !isSelectedGameObject )
		{
			m_gameObjects[ colliderIndex ]->m_borderColor = BLUE;
		}
	}
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//				COLLISION UPDATE
//--------------------------------------------------------------------------------------------------------------------------------------------

	for ( int firstColliderIndex = 0; firstColliderIndex < m_gameObjects.size(); firstColliderIndex++ )
	{
		if ( m_gameObjects[ firstColliderIndex ] == nullptr )
		{
			continue;
		}
		for ( int secondColliderIndex = 0; secondColliderIndex < m_gameObjects.size(); secondColliderIndex++ )
		{
			if ( m_gameObjects[ secondColliderIndex ] == nullptr )
			{
				continue;
			}
			if ( m_gameObjects[ firstColliderIndex ] != m_gameObjects[ secondColliderIndex ] )
			{
				if ( m_gameObjects[ firstColliderIndex ]->m_rigidbody->m_collider->Intersects( m_gameObjects[ secondColliderIndex ]->m_rigidbody->m_collider ) )
				{
					m_gameObjects[ firstColliderIndex ]->m_fillColor = m_overlapColor;
					break;
				}
				else
				{
					m_gameObjects[ firstColliderIndex ]->m_fillColor = m_fillColor;
				}
			}
		}
	}
}
	

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawMouseCurrentPosition( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	Vec2 mouseCurrentClientPosition = camera.GetClientToWorldPosition( mouseNormalizedPos );
	g_theRenderer->DrawDisc( Disc2D( mouseCurrentClientPosition , 2.5f ) , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameObject* Game::PickGameobject( Vec2 mousePos )
{
	bool isMouseOverAnygameobject = false;
	for ( size_t gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
	{
		DiscCollider2D* collider = ( DiscCollider2D* ) m_gameObjects[ gameObjectIndex ]->m_rigidbody->m_collider;
		if ( collider && collider->Contains( mousePos ) )
		{
			m_isMouseOnGameObject[ gameObjectIndex ] = true;
			isMouseOverAnygameobject = true;
		}
	}

	if ( !isMouseOverAnygameobject )
	{
		return nullptr;
	}
	else
	{
		size_t gameObjectIndex = m_isMouseOnGameObject.size();

		for ( --gameObjectIndex; gameObjectIndex >= 0; --gameObjectIndex )
		{
			if ( m_isMouseOnGameObject[ gameObjectIndex ] )
			{
				return m_gameObjects[ gameObjectIndex ];
			}
		}
		return nullptr;
	}
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
		m_isMouseOnGameObject.push_back( false );
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		static bool clickCounter = true;
		if ( clickCounter)
		{
			Vec2 PickObjectPosition = m_worldCamera.GetWorldNormalizedToClientPosition( g_theInput->GetMouseNormalizedClientPosition() );
			m_selectedGameObject = PickGameobject( PickObjectPosition );
			clickCounter = false;
			if ( m_selectedGameObject )
			{
				m_selectedGameObject->m_borderColor = Rgba8( 0 , 127 , 0 , 255 );
				m_selectedGameObject->m_isSelected	= true;
			}
		}
		else
		{
			if ( m_selectedGameObject != nullptr )
			{
				m_selectedGameObject->m_isSelected = false;
			}
			m_selectedGameObject = nullptr;
			clickCounter = true;
			for ( size_t gameObjectIndex = 0; gameObjectIndex < m_gameObjects.size(); gameObjectIndex++ )
			{
				m_isMouseOnGameObject[ gameObjectIndex ] = false;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------