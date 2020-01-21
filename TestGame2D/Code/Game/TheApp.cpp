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

	Image* testImage = new Image( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	testImage->GetOrCreateImageFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	Rgba8 testRgba = testImage->GetTexelColor( IntVec2( 2 , 0 ) );
	g_theDevConsole->PrintString( RED , "HELLO" );
	g_theDevConsole->PrintString( GREEN , "BYE" );
	g_theDevConsole->PrintString( MAGENTA , "THIS IS A GREAT TEXT" );
	//g_gameConfigBlackboard.PopulateFromXmlElementAttributes( "Data/Gameconfig.xml" );
	//bool testValue = g_gameConfigBlackboard.GetValue( "isFullscreen" , true );
	FloatRange myintr( 1.38f , 26.9f );
	std::string teststr = myintr.GetAsString();

	//EventCallbackFunctionPtrType mysun = SunRise;
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
		
		float deltaChange =  50 * SinDegrees( 10 * deltaSeconds );
		m_testBox.Translate( Vec2( 0.f , deltaChange ) );
		Vec2 textToVec2; 
		textToVec2.SetFromText( " 2.3f , 5.86f " );
		IntVec2 textToIntVec2;
		textToIntVec2.SetFromText( " 2 , 5 " );
		//bool test;
		Vec2 refPoint = Vec2( g_theInput->GetMouseNormalizedClientPosition() * g_theGame->m_worldCamera.GetOrthoTopRight() );
		m_testObb.StretchAndMoveCenterToIncludePoint( refPoint );
		Vec2 testUv = m_testObb.GetUVForPoint( refPoint );
		Vec2 testPointforUVs = m_testObb.GetPointForUV( testUv );

		g_theInput->EndFrame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TheApp::Render() const
{
		g_theRenderer->ClearScreen( BLACK );
		g_theRenderer->BeginCamera( g_theGame->m_worldCamera );
		g_theGame->Render();

		g_theGame->m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( 1600.f , 800.f ) );
		//std::vector<Vertex_PCU> textVerts;
		//g_bitmapFont->AddVertsForText2D( textVerts , Vec2( 10.f , 20.f ) , 2.f , "Hello, world" , Rgba8( 100 , 0 , 0 , 255 ) );
		//g_testFont->AddVertsForText2D( textVerts, Vec2( 250.f, 400.f ), 15.f, "Options!", Rgba::RED, 0.6f );
		//g_theRenderer->DrawAABB2( m_testBox , WHITE );

// 		g_theRenderer->BindTexture( g_bitmapFont->GetTexture() );
// 		//g_theRenderer->DrawVertexArray( textVerts );
// 		std::vector<Vertex_PCU> textVerts;
// 		g_bitmapFont->AddVertsForText2D( textVerts , Vec2( 100.f , 200.f ) , 30.f , "Hello, world" );
// 		g_bitmapFont->AddVertsForText2D( textVerts , Vec2( 250.f , 400.f ) , 15.f , "Options!" , RED , 0.6f );
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_BOTTOM_LEFT );
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_BOTTOM_CENTERED);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_BOTTOM_RIGHT);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_CENTERED_LEFT);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_CENTERED);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_CENTERED_RIGHT);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_TOP_LEFT);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_TOP_CENTERED);
// 		g_bitmapFont->AddVertsForTextInBox2D( textVerts , m_testBox , 20.f , "ALIGNED TEXT" , BLUE , 0.5f , ALIGN_TOP_RIGHT );
// 		g_theRenderer->BindTexture( g_bitmapFont->GetTexture() );
// 		g_theRenderer->DrawVertexArray( textVerts );
 		g_theRenderer->BindTexture( nullptr );
		Rgba8 green = Rgba8( 0 , 255 , 0 , 100 );
 		g_theRenderer->DrawCapsule2D( Vec2( 600 , 600 ) , Vec2( 400 , 400 ) , 20.f , green );
// 		OBB2 testOBB2( Vec2( 250 , 250 ) , Vec2( 100 , 100 ) , 30.f );
// 		g_theRenderer->DrawOBB2( testOBB2 , CYAN );
// 		Vec2 refPoint = Vec2( g_theInput->GetMouseNormalizedClientPosition() * g_theGame->m_worldCamera.GetOrthoTopRight() );
// 		Vec2 np = testOBB2.GetNearestPoint( refPoint );
// 		Vec2 np2 = GetNearestPointOnOBB2D( refPoint , testOBB2 );
// 		Vec2 np3 = GetNearestPointOnCapsule2D( refPoint , Vec2( 600 , 600 ) , Vec2( 400 , 400 ) , 20.f );
// 		g_theRenderer->DrawDisc( Disc2D( np , 2.f ) , WHITE );
// 		g_theRenderer->DrawDisc( Disc2D( np2 , 2.f ) , MAGENTA );
// 		g_theRenderer->DrawDisc( Disc2D( np3 , 3.f ) , BLUE );
// 
// 		g_theRenderer->DrawLine( Vec2( 900 , 300 ) , Vec2( 800 , 600 ) , WHITE , 5.f );
// 		Vec2 np4 = GetNearestPointOnLineSegment2D( refPoint , Vec2( 900 , 300 ) , Vec2( 800 , 600 ) );
// 		Vec2 np5 = GetNearestPointOnInfiniteLine2D( refPoint , Vec2( 850 , 450 ) , Vec2( 900 , 300 ) );
// 		g_theRenderer->DrawDisc( Disc2D( np5 , 2.f ) , BLUE );
// 		AABB2 cameraDim = AABB2( g_theGame->m_worldCamera.GetOrthoBottomLeft() , g_theGame->m_worldCamera.GetOrthoTopRight() );
// 		//g_theRenderer->DrawRing( cameraDim.GetCenter() , 400.f , RED , 10.f );
// 
// 		g_theRenderer->DrawLine( refPoint , np , RED , 2.f );
// 		g_theRenderer->DrawLine( refPoint , np2 , RED , 2.f );
// 		g_theRenderer->DrawLine( refPoint , np3 , RED , 2.f );
// 		g_theRenderer->DrawLine( refPoint , np4 , RED , 2.f );
// 		g_theRenderer->DrawLine( refPoint , np5 , RED , 2.f );
		
		g_theRenderer->DrawOBB2( m_testObb , CYAN );
		g_theRenderer->DrawRing( m_testObb.GetCenter() , m_testObb.m_halfDimensions.x , RED , 2.f );
		if ( g_theDevConsole->IsOpen() )
		{
			g_theDevConsole->Render( *g_theRenderer , g_theGame->m_worldCamera , 20.f );
		}

		g_theRenderer->EndCamera( g_theGame->m_worldCamera );

		g_theGame->Render();

		//g_theGame->RenderUI();
	
	
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
	
	if ( g_theInput->GetButtonState( KEY_F8 ).WasJustPressed() ) 
	{
		delete g_theGame;
		g_theGame = nullptr;
		g_theGame = new Game();
	}
	
	if ( g_theInput->GetButtonState( KEY_TILDE ).WasJustPressed() )
	{
		//m_isDevConsoleVisbile = !m_isDevConsoleVisbile;
		//g_theDevConsole->SetIsOpen( m_isDevConsoleVisbile );
		g_theDevConsole->ToggleVisibility();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------


