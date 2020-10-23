#include "Game/PlayerClient.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole*							g_theDevConsole;	
extern	AudioSystem*						g_theAudioSystem;
extern	InputSystem*						g_theInput;
extern	RenderContext*						g_theRenderer;
extern	Window*								g_theWindow;
extern	BitmapFont*							g_bitmapFont;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
PlayerClient::PlayerClient( Server* ownerServer ) : Client( ownerServer )
{
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

	if ( g_bitmapFont == nullptr )
	{
		g_bitmapFont = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	}

	if ( g_theDevConsole == nullptr )
	{
		g_theDevConsole = new DevConsole();
	}
	g_theDevConsole->Startup();

	if ( g_theAudioSystem == nullptr )
	{
		g_theAudioSystem = new AudioSystem();
	}
	g_theAudioSystem->Startup();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

PlayerClient::~PlayerClient()
{
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;

	delete g_theDevConsole;
	g_theDevConsole = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

	delete g_theInput;
	g_theInput = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Update( float deltaSeconds )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::Render()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------