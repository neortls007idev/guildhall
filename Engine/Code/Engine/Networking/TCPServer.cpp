#include "Engine/Core/DevConsole.hpp"
#include "Engine/Networking/TCPServer.hpp"
#include <ws2tcpip.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole* g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

TCPServer::TCPServer( int port /*= 48000 */ ) :
												m_listenPort( port ) ,
												m_listenSocket( INVALID_SOCKET ) ,
												m_timeVal { 01,01 }
{
	FD_ZERO( &m_listenSet );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TCPServer::~TCPServer()
{
	FD_ZERO( &m_listenSet );
	m_listenSocket = INVALID_SOCKET;
	m_listenPort = -1;
	m_timeVal = timeval{ 01,01 };
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TCPServer::Bind()
{
	struct addrinfo addressHintsIn;
	struct addrinfo* pAddressOut;

	ZeroMemory( &addressHintsIn , sizeof( addressHintsIn ) );
	addressHintsIn.ai_family = AF_INET;
	addressHintsIn.ai_socktype = SOCK_STREAM;
	addressHintsIn.ai_protocol = IPPROTO_TCP;
	addressHintsIn.ai_flags = AI_PASSIVE;

	std::string serverPort( Stringf( "%d" , m_listenPort ) );
	int iResult = getaddrinfo( NULL , serverPort.c_str() , &addressHintsIn , &pAddressOut );

	if( iResult != 0 )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to getaddrinfo Failed %i" , WSAGetLastError() );
	}

	//SOCKET listenSocket
	m_listenSocket = socket( pAddressOut->ai_family , pAddressOut->ai_socktype , pAddressOut->ai_protocol );

	if( m_listenSocket == INVALID_SOCKET )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Invalid Listen Socket. ListenFailed  Failed %i" , WSAGetLastError() );
	}

	unsigned long blockingMode = 1;
	iResult = ioctlsocket( m_listenSocket , FIONBIO , &blockingMode );

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to ioctlsocket Failed %i" , WSAGetLastError() );
	}

	iResult = bind( m_listenSocket , pAddressOut->ai_addr , ( int ) pAddressOut->ai_addrlen );

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to bind Failed %i" , WSAGetLastError() );
	}

	iResult = listen( m_listenSocket , SOMAXCONN );
	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to listen Failed %i" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void TCPServer::Listen()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

SOCKET TCPServer::Accept()
{
	SOCKET clientSocket = INVALID_SOCKET;
	
	FD_ZERO( &m_listenSet );
	FD_SET( m_listenSocket , &m_listenSet );
	int iResult = select( 0 , &m_listenSet , NULL , NULL , &m_timeVal );

	if( iResult == INVALID_SOCKET )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Select Failed %i" , WSAGetLastError() );
	}

	if( FD_ISSET( m_listenSocket , &m_listenSet ) )
	{
		clientSocket = accept( m_listenSocket , NULL , NULL );

		if( clientSocket == INVALID_SOCKET )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Accept Failed %i" , WSAGetLastError() );
		}
		//g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Clientconnected from %s" , GetAddress().c_str() );
	}

	return clientSocket;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string TCPServer::GetAddress()
{
	/*
	std::array<char , 128> addressString;

	sockaddr clientAddress;
	int addressSize = sizeof( clientAddress );
	int iResult = getpeername( m_clientSocket , &clientAddress , &addressSize );

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Get Peer Name Failed %i" , WSAGetLastError() );
	}

	DWORD outLength = static_cast< DWORD >( addressString.size() );
#pragma warning( push )
#pragma warning( disable : 4996  )
	iResult = WSAAddressToStringA( &clientAddress , addressSize , NULL , &addressString[0] , &outLength );
#pragma warning( pop )

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Get Peer Name Failed %i" , WSAGetLastError() );
	}

	addressString[ outLength ] = NULL;
	return std::string( &addressString[ 0 ] );
	*/
	return "";
}

//--------------------------------------------------------------------------------------------------------------------------------------------