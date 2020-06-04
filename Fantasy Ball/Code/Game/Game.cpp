#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Game/Level.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern BitmapFont*		g_bitmapFont;
extern AudioSystem*		g_theAudioSystem;
extern TheApp*			g_theApp;
extern InputSystem*		g_theInput;

//--------------------------------------------------------------------------------------------------------------------------------------------

SpriteSheet* g_tileSpriteSheet		= nullptr;
SpriteSheet* g_characterSpriteSheet = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );
	
	m_worldCamera.SetOrthoView( Vec2( -960.f * CLIENT_ASPECT , -960.f ) , Vec2( 960.f * CLIENT_ASPECT , 960.f ) );
	LoadAssets();
	m_currentLevel				= new Level( this );
	Vec2 cameraMins				= m_worldCamera.GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs				= m_worldCamera.GetOrthoMax().GetXYComponents();
	m_currentLevel->m_leftWall	= AABB2( cameraMins.x , -960.f , cameraMins.x + 5.f , 960.f );
	m_currentLevel->m_rightWall = AABB2( cameraMaxs.x - 5.f , -960.f , cameraMaxs.x , 960.f );
	m_currentLevel->m_topWall	= AABB2( -540.f , 960.f , 540.f , 961.f );
	m_currentLevel->m_pit		= AABB2( -540.f , cameraMins.y , 540.f , cameraMins.y - 1 );
	m_currentLevel->SpawnNewEntity( PADDLE , Vec2::ZERO );
	m_currentLevel->SpawnNewEntity( BALL , Vec2::ZERO );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	LoadAllTextures();
	LoadAllSounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllTextures()
{
 	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
//	UpdateCamera();
	m_currentLevel->Update( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	m_currentLevel->Render();
	Vec2 cameraMins = m_worldCamera.GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs = m_worldCamera.GetOrthoMax().GetXYComponents();
	g_theRenderer->DrawLine( Vec2( cameraMins.x , 0.f ) , Vec2( cameraMaxs.x , 0.f ) , MAGENTA , 5.f );
	g_theRenderer->DrawLine( Vec2( 0.f , cameraMins.y ) , Vec2( 0.f , cameraMaxs.y ) , MAGENTA , 5.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//					CLAMPING THE CAMERA 
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec2 cameraCoords;

	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = ClampZeroToOne( m_screenShakeIntensity );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	float cameraShakeY = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	
	m_worldCamera.Translate2D( Vec2( cameraShakeX , cameraShakeY ) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
