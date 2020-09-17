#pragma once
#include <atlalloc.h>
#include <list>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPServer;
class TCPClient;

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
	static bool SendMessageToClient( EventArgs& args );
	static bool ConnectToServer( EventArgs& args );
	
private:
	bool					m_isListening	= false;
	int						m_listenPort	= -1;
	//std::list< TCPClient* > m_clients;
	//std::list< TCPServer* > m_TCPServers;
	TCPServer*				m_TCPServer;
	TCPClient*				m_TCPclient;
	static bool				m_wasMessageJustSentByServer;
};

//--------------------------------------------------------------------------------------------------------------------------------------------