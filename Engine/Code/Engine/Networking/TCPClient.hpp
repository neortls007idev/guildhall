#pragma once
#include "Engine/Networking/NetworkCommon.hpp"
#include <string>
#include <winsock2.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ClientSendMessage
{
	MessageHeader m_header;
	std::string m_gameName;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class TCPClient
{
public:
	TCPClient( SOCKET socket , Mode mode = Mode::Nonblocking );
	~TCPClient();

	//----------------------------------------------------------------------------------------------------------

	SOCKET connect( std::string const& host , std::uint16_t port , Mode mode = Mode::Nonblocking );
	
public:
	SOCKET					m_clientSocket;
private:
	Mode					m_mode;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

