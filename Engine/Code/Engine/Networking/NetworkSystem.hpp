#pragma once
#include <atlalloc.h>
#include <list>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPServer;

//--------------------------------------------------------------------------------------------------------------------------------------------

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	//----------------------------------------------------------------------------------------------------------
	bool GetIsListening()																		{ return m_isListening; }
	void SetIsListening( bool listen )															{ m_isListening = listen; }

	std::string GetAddress();
	
	//----------------------------------------------------------------------------------------------------------

	void		AddNetowrkingCommandsToConsole();
	static bool StartTCPServer( EventArgs& args );
	static bool CloseTCPServer( EventArgs& args );
	
private:
	bool					m_isListening	= false;
	int						m_listenPort	= -1;
	SOCKET					m_clientSocket;
	std::list< TCPServer* > m_TCPServers;
};

//--------------------------------------------------------------------------------------------------------------------------------------------