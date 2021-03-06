#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"

//-----------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInput;
	   TheApp*			g_theApp		= nullptr;
	   Game*			g_theGame		= nullptr;

//-----------------------------------------------------------------------------------------------

TheApp::TheApp()
{

}

//-----------------------------------------------------------------------------------------------

TheApp::~TheApp()
{

}

//-----------------------------------------------------------------------------------------------

void TheApp::Startup()
{
	if ( g_theWindow == nullptr )
	{
		g_theWindow = new Window();
	}
	g_theWindow->Startup();

 	if ( g_theInput == nullptr )
 	{
		g_theInput = new InputSystem();
		g_theWindow->SetInputSystem( g_theInput );
 	}
	g_theInput->Startup();

	if ( g_theRenderer == nullptr )
	{
		g_theRenderer = new RenderContext();

		g_theRenderer->Startup( g_theWindow );
		g_theRenderer->ClearScreen(BLACK);
	}

	if ( g_theGame == nullptr )
	{
		g_theGame = new Game();
	}
}

void TheApp::RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double        timeThisFrameStarted = GetCurrentTimeSeconds();
	double		  deltaSeconds		   = timeThisFrameStarted - timeLastFrameStarted;
	timeLastFrameStarted			   = timeThisFrameStarted;
	
	BeginFrame();                        // all engine system and not game systems
	Update( ( float ) deltaSeconds );	
	Render();
	EndFrame();
}

void TheApp::BeginFrame()
{
	// all engine things that must begin at the beginning of each frame and not the game
	g_theWindow->BeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
}

void TheApp::Update( float deltaSeconds )
{
		UpdateFromKeyboard();
		if ( m_isPaused ) { deltaSeconds = 0; }
		else if ( m_isSloMo == true ) { deltaSeconds /= 10.f; }
		if ( m_isSpeedMo ) { deltaSeconds = deltaSeconds * 4.0f; }
		g_theGame->Update( deltaSeconds );
		g_theInput->EndFrame();
}

void TheApp::Render() const
{
		g_theRenderer->ClearScreen( BLACK );
		g_theRenderer->BeginCamera( g_theGame->m_worldCamera );
		g_theGame->Render();
		g_theRenderer->EndCamera( g_theGame->m_worldCamera );

		g_theGame->RenderUI();	
}

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
	//SwapBuffers( m_displayDeviceContext );
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

void TheApp::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
}

bool TheApp::HandleQuitRequested()
{
	g_theWindow->HandleQuitRequested();
	m_isQuitting = true;
	return m_isQuitting;
}

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->IsKeyHeldDown( 'T' ) ) { m_isSloMo = true; }
	else { m_isSloMo = false; }
	if ( g_theInput->IsKeyHeldDown( 'Y' ) ) { m_isSpeedMo = true; }
	else { m_isSpeedMo = false; }

	if ( g_theInput->WasKeyJustPressed( 'P' ) ) { m_isPaused = !m_isPaused; }

	if ( g_theInput->WasKeyJustPressed( KEY_ESC ) ) { HandleQuitRequested(); }
	if ( g_theInput->WasKeyJustPressed( KEY_F4 ) )
	{
		m_debugCamera = !m_debugCamera;
		if (m_debugCamera)
		{
			g_theGame->m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y ) );
		}
	}
	if ( g_theInput->WasKeyJustPressed( KEY_F8 ) )
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
	}
		
}
