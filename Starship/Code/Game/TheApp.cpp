#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Time/Time.hpp"

#include <windows.h>

RenderContext* g_theRenderer = nullptr;
TheApp* g_theApp = nullptr;
InputSystem* g_theInput = nullptr;
//extern Camera* g_Camera;
//Rgba8 Clrscr(0,0,0,255);

TheApp::TheApp()
{

}

TheApp::~TheApp()
{

}

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
	g_theCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	g_theRenderer->ClearScreen( BLACK );
	}
	if ( m_theAttractMode == nullptr )
	{
		m_theAttractMode = new AttractMode();
		m_attractModeStatus = true;
	}
	if ( m_theGame == nullptr )
	{
		m_theGame = new Game();
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
	Startup();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theRenderer->BeginCamera(*g_theCamera);

}

void TheApp::Update( float deltaSeconds )
{
	
	HandleKeyPressed();
	if (m_attractModeStatus)
	{
		m_theAttractMode->Update(deltaSeconds);
	}
	if (m_attractModeStatus == false)
	{
		if (m_isPaused) { deltaSeconds = 0; }
		else if (m_isSloMo == true) { deltaSeconds /= 10.f; }
		m_theGame->Update(deltaSeconds);
	}
	if (m_theGame->m_waveNumber == 5 && m_theGame->m_Ship->m_age <= 0)
	{
		m_keyPressed[KEY_SPACE] = false;
		m_keyPressed['N'] = false;
		m_attractModeStatus = true;
		delete m_theGame;
		m_theGame = new Game;
	}
		g_theInput->EndFrame();
	HandleKeyReleased();
}

void TheApp::Render() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->ClearScreen( BLACK );
	g_theRenderer->BeginCamera(*g_theCamera);
	
	if (m_attractModeStatus)
	{
		m_theAttractMode->Render();
	}
	if (m_attractModeStatus == false && m_theGame->m_waveNumber < 5)
	{
		m_theGame->Render();
		m_theGame->RenderUI();
	}
		g_theRenderer->EndCamera(*g_theCamera);	
	
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
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete m_theGame;
	m_theGame = nullptr;
	delete m_theAttractMode;
	m_theAttractMode = nullptr;
}

bool TheApp::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

bool TheApp::IsKeyPressed( unsigned char keyCode )
{
	return m_keyPressed[keyCode];
}

bool TheApp::HandleKeyReleased( unsigned char keyCode )
{
	
	if (keyCode == 'T')
	{
		m_isSloMo = false;
		
	}
	
	m_keyPressed[keyCode] = false;
	return(m_keyPressed[keyCode]);
}

bool TheApp::HandleKeyReleased()
{

	if (!g_theInput->IsKeyHeldDown('T'))
	{
		m_isSloMo = false;

	}
	
	return true;
}
bool TheApp::HandleKeyPressed()
{
	if (g_theInput->IsKeyHeldDown('T'))
	{
		m_isSloMo = true;
	}

	if (g_theInput->WasKeyJustPressed('P'))
	{
			m_isPaused = !m_isPaused;
	}
	
	if (g_theInput->WasKeyJustPressed(KEY_ESC)) // 0x1B is the VK_ESCAPE macro defined in windows.h and used here directly in hexadecimal.
	{
		HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	if (g_theInput->WasKeyJustPressed(KEY_F8))
	{
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game();
	}
	//g_theInput->HandleKeyPressed()
	return false;
}

bool TheApp::HandleKeyPressed( unsigned char keyCode )
{
	if (keyCode == 'T' )
	{
		m_isSloMo = true;
	
	}

	if ( keyCode == 'P' )
	{
		if ( m_keyPressed['P'] == false )
		{
			m_isPaused = !m_isPaused;
		}
		
	}
  	
	if( keyCode == KEY_ESC ) // 0x1B is the VK_ESCAPE macro defined in windows.h and used here directly in hexadecimal.
	{
		HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	if ( keyCode == KEY_F8 )
	{
		delete m_theGame;
		m_theGame = nullptr;
		m_theGame = new Game();
	}

	m_keyPressed[keyCode] = true;
	

	return( m_keyPressed[keyCode] );
}