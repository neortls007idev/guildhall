#include "Game/TheApp.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

InputSystem* g_theInput = nullptr;
RenderContext* g_theRenderer = nullptr;
AudioSystem* g_theAudioSystem = nullptr;
TheApp* g_theApp = nullptr;
Game* g_theGame = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Startup()
{
 	if ( g_theInput == nullptr )
 	{
		g_theInput = new InputSystem();
 	}

	if ( g_theRenderer == nullptr )
	{
		g_theRenderer = new RenderContext();

	g_theRenderer->Startup();
	g_theRenderer->ClearScreen(BLACK);
	}
	
	if ( g_theAudioSystem == nullptr )
	{
		g_theAudioSystem = new AudioSystem();
		g_theAudioSystem->Startup();
	}

	if ( g_theGame == nullptr )
	{
		g_theGame = new Game();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::BeginFrame()
{
	// all engine things that must begin at the beginning of each frame and not the game
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theRenderer->BeginCamera( g_theGame->m_worldCamera );
	g_theAudioSystem->BeginFrame();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
	UpdateFromKeyboard();
	if ( m_isPaused ) { deltaSeconds = 0; }
	else if ( m_isSloMo == true ) { deltaSeconds /= 10.f; }
	if ( m_isSpeedMo ) { deltaSeconds = deltaSeconds * 4.0f; }
	g_theGame->Update( deltaSeconds );
	g_theInput->EndFrame();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
		g_theRenderer->ClearScreen( BLACK );
		g_theRenderer->BeginCamera( g_theGame->m_worldCamera );
		g_theGame->Render();
		g_theRenderer->EndCamera( g_theGame->m_worldCamera );

		//g_theGame->RenderUI();
	
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
	//SwapBuffers( m_displayDeviceContext );
	g_theAudioSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;

	g_theAudioSystem->Shutdown();
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;
	
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	
	delete g_theInput;
	g_theInput = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool TheApp::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->GetButtonState( 'T' ).IsPressed() ) { m_isSloMo = true; }
	else { m_isSloMo = false; }
	if ( g_theInput->GetButtonState( 'Y' ).IsPressed() ) { m_isSpeedMo = true; }
	else { m_isSpeedMo = false; }

	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() && g_theGame->m_gameState == GameStates::GAME_STATE_ATTRACT ) 
	{		HandleQuitRequested(); 	}
	
	if ( g_theGame->m_gameState == GameStates::GAME_STATE_PLAYING && m_isPaused && g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() )
	{		g_theGame->m_gameState = GameStates::GAME_STATE_ATTRACT;	}
	
	if ( g_theGame->m_gameState == GameStates::GAME_STATE_PLAYING && ( g_theInput->GetButtonState( 'P' ).WasJustPressed() || g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() ) ) 
	{ 
		m_isPaused = !m_isPaused; 
		
		if ( m_isPaused == true )
		{
			g_theAudioSystem->PlaySound( g_theGame->m_sounds[ SOUND_GAME_PAUSED ] );
		}
		if ( m_isPaused == false  )
		{
			g_theAudioSystem->PlaySound( g_theGame->m_sounds[ SOUND_GAME_UNPAUSED ] );
		}
	}
	
	if ( g_theInput->GetButtonState( KEY_F4 ).WasJustPressed() )
	{
		m_debugCamera = !m_debugCamera;
	}
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() ) 
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
	}
		
}

//--------------------------------------------------------------------------------------------------------------------------------------------
