#pragma once
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class PlayerClient;
class Client;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class Server
{
public:
	Server( Game* game );
	~Server();

	virtual void Startup();
	virtual void Shutdown();
	
	virtual void BeginFrame();
	virtual void Update( float deltaSeconds );
	virtual void EndFrame();

	void AddPlayerClientToServer( PlayerClient* playerClient );
	void RemovePlayerClientFromServer( PlayerClient* playerClient );
	
public:
	Game*						m_theGame		= nullptr;
	
	std::vector< Client* >		m_playerClients;
};

//--------------------------------------------------------------------------------------------------------------------------------------------