#include "Game/PlayerClient.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Game/Server.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/AuthServer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole*							g_theDevConsole;	
extern	AudioSystem*						g_theAudioSystem;
extern	InputSystem*						g_theInput;
extern	RenderContext*						g_theRenderer;
extern	Window*								g_theWindow;
extern	BitmapFont*							g_bitmapFont;
extern AuthoritativeServer*					g_theAuthServer;
//--------------------------------------------------------------------------------------------------------------------------------------------
	
PlayerClient::PlayerClient() : Client()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

PlayerClient::~PlayerClient()
{

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
	if ( m_game != nullptr )
	{
		m_game->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::BeginFrame()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PlayerClient::EndFrame()
{
	m_game = g_theAuthServer->GetGame();
}

//--------------------------------------------------------------------------------------------------------------------------------------------