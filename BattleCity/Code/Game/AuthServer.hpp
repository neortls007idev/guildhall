#pragma once
#include "Game/Server.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GameSinglePlayer;
class GameMultiplayer;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class AuthoritativeServer : public Server
{
public:
	AuthoritativeServer();
	~AuthoritativeServer();
	
	virtual void	Startup() override;
	virtual void	Shutdown() override;
	virtual void	Update( float deltaSeconds ) override;
	virtual void	BeginFrame() override;
	virtual void	EndFrame() override;
//			Game*	GetGame();
			void	AddPlayers();
//			void	AssignInputSystem( InputSystem* inputSystem );
public:
	
private:
	int m_numRemotePlayers = 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------