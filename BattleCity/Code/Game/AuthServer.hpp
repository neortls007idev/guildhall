#pragma once
#include "Game/Server.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameSinglePlayer;
class GameMultiplayer;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class AuthoritativeServer : protected Server
{
public:
	AuthoritativeServer();
	~AuthoritativeServer();
	
	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;
			Game*	GetGame();

public:
	GameSinglePlayer*	m_singlePlayerGame		= nullptr;
	GameMultiplayer*	m_multiplayerGame		= nullptr;	
};

//--------------------------------------------------------------------------------------------------------------------------------------------