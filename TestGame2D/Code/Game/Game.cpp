#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
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

Game::Game()
{
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 1600.f , 800.f ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	GarbageCollection();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	DrawMouseTest( m_worldCamera );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera( m_uiCamera );
	RenderUIVerts();
	g_theRenderer->EndCamera( m_uiCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageCollection()
{

	GarbageDeletion();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageDeletion()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DrawMouseTest( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = g_theInput->GetMouseNormalizedClientPosition();
	AABB2 orthoBounds = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
	Vec2 mouseDrawPos = orthoBounds.GetPointForUV( mouseNormalizedPos );
	//g_theRenderer->DrawRing( mouseDrawPos , 2.f , RED , 2.f );
	g_theRenderer->DrawDisc( Disc2D( mouseDrawPos , 2.5f ) , RED );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard()
{
	if ( g_theInput->GetButtonState( KEY_F1 ).WasJustPressed() )
	{ m_debugDraw = !m_debugDraw; }
	
	if ( g_theInput->GetButtonState( KEY_F3 ).WasJustPressed() ) 
	{ m_isClipping = !m_isClipping; }
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUIVerts() const
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------