#include "Game/AuthServer.hpp"
#include "Game/Server.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiplayer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
AuthoritativeServer* g_theAuthServer = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

AuthoritativeServer::AuthoritativeServer()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

AuthoritativeServer::~AuthoritativeServer()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::Update( float deltaSeconds )
{
	if( m_gameType == SINGLE_PLAYER )
	{
		m_singlePlayerGame->Update( deltaSeconds );
	}
	else if( m_gameType == MULTIPLAYER )
	{
		m_multiPlayerGame->Update( deltaSeconds );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game* AuthoritativeServer::GetGame()
{
	if( m_gameType == SINGLE_PLAYER )
	{
		return m_singlePlayerGame;
	}
	else if( m_gameType == MULTIPLAYER )
	{
		return m_multiPlayerGame;
	}
	else
	{
		__debugbreak();
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::AddPlayers()
{
	if( m_gameType == SINGLE_PLAYER )
	{
		m_singlePlayerGame->Startup();
		m_singlePlayerGame->CreatePlayer();
	}
	else if( m_gameType == MULTIPLAYER )
	{
		// TODO :- COMPLETE IMPLEMENTATION
		m_multiPlayerGame->Startup();
		__debugbreak();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------