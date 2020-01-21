#include "Game/TheApp.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//			GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

InputSystem*				g_theInput			= nullptr;
RenderContext*				g_theRenderer		= nullptr;
/*BitmapFont* g_bitmapFont = nullptr;*/
AudioSystem*				  g_theAudioSystem	= nullptr;
TheApp*						  g_theApp			= nullptr;
Game*						  g_theGame			= nullptr;
DevConsole*					  g_theDevConsole   = nullptr;
extern BitmapFont*			  g_bitmapFont;
extern NamedStrings		      g_gameConfigBlackboard;

//--------------------------------------------------------------------------------------------------------------------------------------------
bool SunRise( EventArgs& abc )
{
	UNUSED( abc );
	DebuggerPrintf( "Sunrise has been called.\n" );
	g_theDevConsole->PrintString( WHITE , "Sunrise has been called." );
	return true;
}

void SunSet()
{
	DebuggerPrintf( "Sunset has been called.\n" );
}
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
	if ( g_theEventSystem == nullptr )
	{
		g_theEventSystem = new EventSystem();
		g_theEventSystem->Startup();
	}
 	if ( g_theInput == nullptr )
 	{
		g_theInput = new InputSystem();
		g_theInput->Startup();
 	}

	if ( g_theRenderer == nullptr )
	{
		g_theRenderer = new RenderContext();
		g_theRenderer->Startup();
		g_theRenderer->ClearScreen(BLACK);
	}

	if ( g_bitmapFont == nullptr )
	{
		g_bitmapFont = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" ); // TO DO PASS IN THE FONT ADDRESS AND THE TEXTURE POINTER TO IT.
	}

	if ( g_theAudioSystem == nullptr )
	{
		//g_theAudioSystem = new AudioSystem();
		//g_theAudioSystem->Startup();
	}

	if ( g_theGame == nullptr )
	{
		g_theGame = new Game();
	}

	if ( g_theDevConsole == nullptr)
	{
		g_theDevConsole = new DevConsole();
	}

	g_theDevConsole->Startup();

	g_theDevConsole->PrintString( RED , "HELLO" );
	g_theDevConsole->PrintString( GREEN , "BYE" );
	g_theDevConsole->PrintString( MAGENTA , "THIS IS A GREAT TEXT" );

	g_theEventSystem->SubscribeToEvent( "sunrise" ,  SunRise );
	g_theEventSystem->FireEvent( "sunrise" );
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
	g_theDevConsole->BeginFrame();
	//g_theAudioSystem->BeginFrame();
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

		g_theGame->m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 1600.f , 800.f ) );

		if ( g_theDevConsole->IsOpen() )
		{
			g_theDevConsole->Render( *g_theRenderer , g_theGame->m_worldCamera , 20.f );
		}

		g_theRenderer->EndCamera( g_theGame->m_worldCamera );

		g_theGame->Render();	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
	//SwapBuffers( m_displayDeviceContext );
	//g_theAudioSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

// STATIC bool TheApp::CommandQuit( EventArgs& args )
// {
// 	UNUSED( args );
// 	g_theApp->HandleQuitRequested();
// 	return false;
// }

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;

	//g_theAudioSystem->Shutdown();
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;

	g_theDevConsole->Shutdown();
	
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

	if ( g_theInput->GetButtonState( 'P' ).WasJustPressed() ) 
	{ m_isPaused = !m_isPaused; }

	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() ) { HandleQuitRequested(); }
	
	if ( g_theInput->GetButtonState( KEY_F4 ).WasJustPressed() )
	{
		m_debugCamera = !m_debugCamera;
		if (m_debugCamera)
		{
			
		}
	}
	
	if ( g_theInput->GetButtonState( KEY_TILDE ).WasJustPressed() )
	{
		//m_isDevConsoleVisbile = !m_isDevConsoleVisbile;
		//g_theDevConsole->SetIsOpen( m_isDevConsoleVisbile );
		g_theDevConsole->ToggleVisibility();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------


