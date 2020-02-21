#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

RenderContext* g_theRenderer = nullptr;
TheApp* g_theApp = nullptr;
InputSystem* g_theInput = nullptr;
Game* g_theGame = nullptr;
DevConsole* g_theDevConsole = nullptr;
extern BitmapFont* g_bitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::TheApp()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TheApp::~TheApp()
{
	delete g_theGame;
	g_theGame = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Startup()
{
	if ( g_theEventSystem == nullptr )
	{
		g_theEventSystem = new EventSystem();
	}
	g_theEventSystem->Startup();

 	if ( g_theInput == nullptr )
 	{
		g_theInput = new InputSystem();
		g_theWindow->SetInputSystem( g_theInput );
 	}
	g_theInput->Startup();
	
	if ( g_theRenderer == nullptr )
	{
		g_theRenderer = new RenderContext();
	}
	g_theRenderer->Startup( g_theWindow );

	if ( g_theDevConsole == nullptr )
	{
		g_theDevConsole = new DevConsole();
	}
	g_theDevConsole->Startup();

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
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();

	if ( m_taskbarProgress < 100.f  && m_taskbarProgressMode == WND_PROGRESS_VALUE )
	{
		m_taskbarProgress += 0.166f;
		g_theWindow->SetProgress( WND_PROGRESS_VALUE , m_taskbarProgress );
	}
	else if ( m_taskbarProgress >= 100.f && m_taskbarProgressMode == WND_PROGRESS_VALUE )
	{
		m_taskbarProgress = 0.f;
		m_taskbarProgressMode = WND_PROGRESS_NONE;
	}
	else
	{
		g_theWindow->SetProgress( m_taskbarProgressMode , m_taskbarProgress );
		//m_taskbarProgress = 0.f;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Update( float deltaSeconds )
{
		g_theRenderer->UpdateFrameTime( deltaSeconds );

		UpdateFromKeyboard();
		
		if ( m_isPaused ) { deltaSeconds = 0; }
		else if ( m_isSloMo == true ) { deltaSeconds /= 10.f; }
		if ( m_isSpeedMo ) { deltaSeconds = deltaSeconds * 4.0f; }
		
		g_theGame->Update( deltaSeconds );

		if ( g_theDevConsole->IsOpen() )
		{
			g_theDevConsole->Update();
		}

		//g_theDevConsole->Update();
		g_theInput->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
 		g_theGame->Render();

		if ( g_theDevConsole->IsOpen() )
		{
			g_theDevConsole->Render( *g_theRenderer , g_theGame->m_gameCamera , 0.5f );
		}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::EndFrame()
{
	// all engine things that must end at the end of the frame and not the game
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();

// 	if ( g_theRenderer->HasAnyShaderChangedAtPath( L"\\Data\\Shaders\\" ) )
// 	{
// 		g_theRenderer->ReCompileAllShaders();
// 	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Shutdown()
{
	delete g_theGame;
	g_theGame = nullptr;
	g_theDevConsole->Shutdown();
	delete g_theDevConsole;
	g_theDevConsole = nullptr;
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	g_theInput->Shutdown();
	delete g_theInput;
	g_theInput = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::UpdateFromKeyboard()
{
	if ( g_theInput->GetButtonState( KEY_ESC ).WasJustPressed() ) { g_theWindow->HandleQuitRequested(); }

	if ( g_theInput->WasKeyJustPressed( KEY_TILDE ) )
	{
		g_theDevConsole->ToggleVisibility();
	}

	if ( g_theDevConsole->IsOpen() )
	{
		return;
	}

	if ( g_theInput != nullptr && g_theInput->WasKeyJustPressed( 'C' ) /*&& g_theInput->WasKeyJustPressed( 'T' )*/ )
	{
		g_theWindow->SetTitle( "Function Works" );
	}

	if ( g_theInput != nullptr && g_theInput->WasKeyJustPressed( 'I' ) )
	{
		g_theWindow->SetNewIcon( eIcon::INFORMATION );
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'B' ) )
	{
		g_theWindow->DisplaySettings( BORDERLESS );
	}
	
	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'R' ) )
	{
		g_theWindow->DisplaySettings( REGULAR );
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'F' ) )
	{
		g_theWindow->DisplaySettings( FULLSCREEN );
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'T' ) )
	{
		m_taskbarProgress = 0.f;
	}
	
	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'P' ) )
	{
		m_taskbarProgressMode = WND_PROGRESS_PAUSED;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'I' ) )
	{
		m_taskbarProgressMode = WND_PROGRESS_INDETERMINATE;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'R' ) )
	{
		m_taskbarProgressMode = WND_PROGRESS_VALUE;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'E' ) )
	{
		m_taskbarProgressMode = WND_PROGRESS_ERROR;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_taskbarProgress = 0.f;
		m_taskbarProgressMode = WND_PROGRESS_NONE;
	}

	if ( g_theInput->GetButtonState( 'P' ).WasJustPressed() ) { m_isPaused = !m_isPaused; }

	
	if ( g_theInput->GetButtonState( KEY_F4 ).WasJustPressed() )
	{
		m_debugCamera = !m_debugCamera;
		if (m_debugCamera)
		{
			g_theGame->m_gameCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y ) );
		}
	}
	
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() ) 
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
	}

	if ( g_theInput->WasKeyJustPressed( 'R' ) )
	{
		g_theRenderer->ReCompileAllShaders();
	}
}

