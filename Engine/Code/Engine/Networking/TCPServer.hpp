#pragma once
#include "Engine/Networking/NetworkCommon.hpp"
#include <atlalloc.h>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPSocket;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ServerListenMessage
{
	MessageHeader m_header;
	std::string m_gameName;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPServer
{
public:
	TCPServer( int port = 48000 );
	~TCPServer();

	void		Bind();
	void		Listen();
	SOCKET		Accept();

	bool		GetListenPort()														{ return m_listenPort; }
	void		SetListenPort( int port )											{ m_listenPort = port; }
	std::string GetAddress();
	
private:
	int						m_listenPort = -1;
	FD_SET					m_listenSet;
	SOCKET					m_listenSocket;
	timeval					m_timeVal;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

