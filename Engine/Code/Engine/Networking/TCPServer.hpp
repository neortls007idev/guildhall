#pragma once
#include "Engine/Networking/NetworkCommon.hpp"
//#include "Engine/Networking/TCPSocket.hpp"
#include <atlalloc.h>
#include <string>

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPServer
{
public:
	TCPServer( int port = 48000 );
	~TCPServer();

	void			Bind();
	void			Listen();
	void			ReceiveClientMessage( SOCKET client , char* bufferAddr , int bufferLength );
	void			SendClientMessage( SOCKET client );
	SOCKET			Accept();

	bool			GetListenPort()														{ return m_listenPort; }
	void			SetListenPort( int port )											{ m_listenPort = port; }
	void			SetServerSendMessage( std::string message )							{ m_sendMessage.m_sendMessage = message; }
	std::string		GetServerSendMessage()												{ return m_sendMessage.m_sendMessage; }
	//std::string GetAddress( TCPClient* client );
	
	SOCKET					m_listenSocket;
	//TCPSocket				m_listenSocket;
private:
	int						m_listenPort = -1;
	FD_SET					m_listenSet;
	timeval					m_timeVal;
	HeaderMessage			m_sendMessage;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

