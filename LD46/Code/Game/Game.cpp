#include "Game/Game.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/TheApp.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern AudioSystem*			g_theAudioSystem;
extern Physics2D*			g_thePhysicsSystem;
extern DevConsole*			g_theDevConsole;
extern TheApp*				g_theApp;
extern BitmapFont*			g_bitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	LoadShaders();

	m_gameCamera.SetOrthoView( Vec2( -800.f , -400.f ) , Vec2( 800.f , 400.f ) );
	m_gameCamera.SetPosition( Vec3::ZERO );
	m_gameCamera.SetClearMode( CLEAR_COLOR_BIT , Rgba8( 0 , 29 , 69 , 127 ) );

	m_player = new Player( this , Vec2::ZERO , Vec2::ZERO , 0.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadShaders()
{
	m_shaders[ ShaderTypes::UNLIT ] = g_theRenderer->GetOrCreateShader( "Data/Shaders/default.hlsl" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	m_timeElapsed += deltaSeconds;
	DebugDrawUI( deltaSeconds );

	m_player->Update( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugDrawUI( float deltaSeconds )
{
	float leftVerticalAlignment = ( 1080.f * 0.25f ) / 16.f;
	float normalizedOffset = RangeMapFloat( 0.f , 1080.f , 0.f , 1.f , leftVerticalAlignment );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1.00f - ( 1 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"Time Elapsed = %.3f" , m_timeElapsed );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindDepthStencil( nullptr );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode( ALPHA );
	g_theRenderer->SetRasterState( FILL_SOLID );
	
	//g_theRenderer->DrawAABB2( AABB2::ZERO_TO_ONE , MAGENTA , PINK );
	m_player->Render();
	
	g_theRenderer->EndCamera( m_gameCamera );

	//DebugRenderScreenTo( nullptr );
}


//--------------------------------------------------------------------------------------------------------------------------------------------