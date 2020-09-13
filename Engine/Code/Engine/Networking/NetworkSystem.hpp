#pragma once
#include <atlalloc.h>
#include <string>

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

	bool GetListenPort()									{ return m_listenPort; }
	void SetListenPort( int port )							{ m_listenPort = port; }
	bool GetIsListening()									{ return m_isListening; }
	void SetIsListening( bool listen )						{ m_isListening = listen; }
	
	std::string GetAddress();
	
	//----------------------------------------------------------------------------------------------------------

	void		AddNetowrkingCommandsToConsole();
	static bool StartTCPServer( EventArgs& args );
	static bool CloseTCPServer( EventArgs& args );
	
private:
	int						m_listenPort	= -1;
	bool					m_isListening	= false;
	FD_SET					m_listenSet;
	SOCKET					m_listenSocket;
	SOCKET					m_clientSocket;
	timeval					m_timeVal;
};

//--------------------------------------------------------------------------------------------------------------------------------------------