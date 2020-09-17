#include "Engine/Core/DevConsole.hpp"
#include "Engine/Networking/TCPClient.hpp"
#include <strstream>
#include <ws2tcpip.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole* g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

TCPClient::TCPClient( SOCKET socket , Mode mode /*= Mode::Nonblocking */ ) : m_mode( mode )
{
	m_clientSocket = socket;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TCPClient::~TCPClient()
{
	m_clientSocket = INVALID_SOCKET;
}

SOCKET TCPClient::connect( std::string const& host , std::uint16_t port , Mode mode /*= Mode::Nonblocking */ )
{
	m_mode = mode;
	// Resolve the port locally
	struct addrinfo  addrHintsIn;
	struct addrinfo* pAddrOut = NULL;
	ZeroMemory( &addrHintsIn , sizeof( addrHintsIn ) );
	addrHintsIn.ai_family = AF_INET;
	addrHintsIn.ai_socktype = SOCK_STREAM;
	addrHintsIn.ai_protocol = IPPROTO_TCP;
	
	// Get the Server IP address and port
	std::string serverPort;
	serverPort = std::to_string( port );
	int iResult = ::getaddrinfo( host.c_str() , serverPort.c_str() , &addrHintsIn , &pAddrOut );
	if( iResult != 0 )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to getaddrinfo() failed, error = %d" , iResult );
	}
	// Create the connection socket.
	SOCKET socket = ::socket( pAddrOut->ai_family , pAddrOut->ai_socktype , pAddrOut->ai_protocol );
	if( socket == INVALID_SOCKET )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Connection socket creation error, error =  = %d" , WSAGetLastError() );
		freeaddrinfo( pAddrOut );
	}
	// Connect to the server
	iResult = ::connect( socket , pAddrOut->ai_addr , ( int ) pAddrOut->ai_addrlen );
	if( iResult == SOCKET_ERROR )
	{
		closesocket( socket );
		socket = INVALID_SOCKET;
	}
	freeaddrinfo( pAddrOut );
	if( socket == INVALID_SOCKET )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Connection Failed, Invalid Socket" );
	}
	// Set blocking mode as needed.
	if( m_mode == Mode::Nonblocking )
	{
		u_long winsockmode = 1;
		iResult = ioctlsocket( socket , FIONBIO , &winsockmode );
		if( iResult == SOCKET_ERROR )
		{
			std::ostrstream msg;
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "IOCTL on server listen socket failed, error = %d" , WSAGetLastError() );
			closesocket( socket );
		}
	}
	return socket;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
