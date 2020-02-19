#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Time/Time.hpp"

#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Game::Game()
{
	m_gameCamera.SetOrthoView( Vec2( -WORLD_CAMERA_SIZE_X , -WORLD_CAMERA_SIZE_Y ) , Vec2( WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_color = BLACK;
	m_invertColorShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/InvertColor.hlsl" );
	m_imageTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );

	m_normalImage = AABB2( -WORLD_CAMERA_SIZE_X , -WORLD_CAMERA_SIZE_Y , WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y );
	
	AABB2 boxCopy = m_normalImage;
	m_invertedColorImage = boxCopy.CarveBoxOffRight( 0.5f , 0.f );
	m_normalImage = m_normalImage.GetBoxAtLeft( 0.5f , 0.f );

	m_normalImage.SetDimensions( m_normalImage.GetDimensions() * 0.75f );
	m_normalImage.AlignWithinAABB2( boxCopy , ALIGN_CENTERED );

	boxCopy = m_invertedColorImage;
	m_invertedColorImage.SetDimensions( m_invertedColorImage.GetDimensions() * 0.75f );
	m_invertedColorImage.AlignWithinAABB2( boxCopy , ALIGN_CENTERED );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_invertColorShader;
	m_invertColorShader = nullptr;

	delete m_imageTex;
	m_imageTex = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
// 	//m_color.g++;
// 	m_color.g = ( uchar ) RangeMapFloat( -1.f , 1.f , 0 , 255 , SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );
// 	m_worldCamera.SetClearMode( CLEAR_COLOR_BIT , m_color );
// 
// 	Vec3 movement = Vec3( Vec2::ZERO , 0.f );
// 	if ( g_theInput->IsKeyHeldDown( 'A' ) )
// 	{
// 		movement.x -= 1.f;
// 	}
// 	if ( g_theInput->IsKeyHeldDown( 'D' ) )
// 	{
// 		movement.x += 1.f;
// 	}
// 	if ( g_theInput->IsKeyHeldDown( 'W' ) )
// 	{
// 		movement.y += 1.f;
// 	}
// 	if ( g_theInput->IsKeyHeldDown( 'S' ) )
// 	{
// 		movement.y -= 1.f;
// 	}

// 	float speed = 4.0f;
// 	m_cameraPosition += movement * speed * deltaSeconds;
// 	m_worldCamera.SetPostion( m_cameraPosition );
	m_invertColorShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/InvertColor.hlsl" );
}

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );
	
	g_theRenderer->BindShader( nullptr );
	
	g_theRenderer->BindTexture( m_imageTex );
	g_theRenderer->SetBlendMode( BlendMode::SOLID );
	
	g_theRenderer->DrawAABB2( m_normalImage , WHITE );
	g_theRenderer->BindShader( m_invertColorShader );
	g_theRenderer->DrawAABB2( m_invertedColorImage , WHITE );
	
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( nullptr );

	g_theRenderer->EndCamera( m_gameCamera );
}

void Game::RenderUI() const
{

}

void Game::UpdateCamera()
{
	
}

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

void Game::GarbageCollection()
{

	GarbageDeletion();

}

void Game::GarbageDeletion()
{

}

void Game::Die()
{

}

void Game::UpdateFromKeyBoard()
{

}