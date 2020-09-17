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
	static bool SendMessageToServer( EventArgs& args );
	static bool ConnectToServer( EventArgs& args );
	static bool DisconnectFromServer( EventArgs& args );
	
private:
	bool					m_isListening	= false;
	int						m_listenPort	= -1;
	TCPServer*				m_TCPServer;
	TCPClient*				m_TCPclient;
	SOCKET					m_linkSocket;
	static bool				m_wasMessageJustSentByServer;
	static bool				m_wasMessageJustSentByClient;
};

//--------------------------------------------------------------------------------------------------------------------------------------------