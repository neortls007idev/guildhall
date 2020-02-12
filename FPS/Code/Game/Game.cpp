#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/Game.hpp"
#include "Engine/Time/Time.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Game::Game()
{
	m_worldCamera.SetOrthoView( Vec2( -WORLD_CAMERA_SIZE_X , -WORLD_CAMERA_SIZE_Y ) , Vec2( WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_color = BLACK;
}

void Game::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	//m_color.g++;
	m_color.g = ( uchar ) RangeMapFloat( -1.f , 1.f , 0 , 255 , SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );
	m_worldCamera.SetClearMode( CLEAR_COLOR_BIT , m_color );

	Vec3 movement = Vec3( Vec2::ZERO , 0.f );
	if ( g_theInput->IsKeyHeldDown( 'A' ) )
	{
		movement.x -= 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'D' ) )
	{
		movement.x += 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'W' ) )
	{
		movement.y += 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		movement.y -= 1.f;
	}

	float speed = 4.0f;
	m_cameraPosition += movement * speed * deltaSeconds;
	m_worldCamera.SetPostion( m_cameraPosition );
}

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_worldCamera );
	//g_theRenderer->Draw( 3 , 0 );
	g_theRenderer->DrawAABB2( AABB2( -10.f , -10.f , 10.f , 10.f ) , WHITE );
	g_theRenderer->EndCamera( m_worldCamera );
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
	if ( g_theInput->GetButtonState( KEY_F1 ).WasJustPressed() )
	{ m_debugDraw = !m_debugDraw; }
	
	if ( g_theInput->GetButtonState( KEY_F3 ).WasJustPressed() ) 
	{ m_isClipping = !m_isClipping; }
	
}