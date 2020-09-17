#include "Engine/Core/DevConsole.hpp"
#include "Engine/Networking/TCPServer.hpp"
#include "Engine/Networking/TCPClient.hpp"
#include <array>
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
	m_timeVal = timeval { NULL,NULL };
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

void TCPServer::ReceiveClientMessage( SOCKET client , char* bufferAddr , int bufferLength )
{
	int iResult = ::recv( client , bufferAddr , static_cast< int >( bufferLength ) , 0 );
	if( iResult == SOCKET_ERROR )
	{
		int error = WSAGetLastError();

		if( error == WSAEWOULDBLOCK /*&& m_mode == Mode::Nonblocking*/ )
		{
			//g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to receive failed = %d", WSAGetLastError() );
			return;/* TCPData { TCPData::DATA_PENDING, NULL };*/
		}
		else
		{
			
			//g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to receive failed = %d", WSAGetLastError() );
			return;
			//closesocket( m_socket );
			//throw std::runtime_error( msg.str() );
		}
	}

	if ( iResult > 0 )
	{
		std::string message = bufferAddr;
		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Client Message = %s", message.c_str() );
	}
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
	}

	return clientSocket;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
