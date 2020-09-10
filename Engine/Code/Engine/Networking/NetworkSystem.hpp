#pragma once
#include <atlalloc.h>
#include <string>

#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
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
	bool GetListenPort()									{ return m_listenPort;  }
//	void SetListenPort();{ m_listenPort = }
	
	std::string GetAddress();
	
	//----------------------------------------------------------------------------------------------------------

	void		AddNetowrkingCommandsToConsole();
	static bool StartTCPServer( EventArgs& args );
	
private:
	int						m_listenPort;
	bool					m_isListening;
	FD_SET					m_listenSet;
	SOCKET					m_listenSocket;
	SOCKET					m_clientSocket;
	timeval					m_timeVal;
};

//--------------------------------------------------------------------------------------------------------------------------------------------