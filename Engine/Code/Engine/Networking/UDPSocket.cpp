#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole* g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

UDPSocket::UDPSocket( const std::string& host , int port )
{
	// AF_INET stands for IPv4 SF_INET6 stnads for IPv6

	m_toAddress.sin_family = AF_INET;
	m_toAddress.sin_port = htons( ( u_short ) port );
	m_toAddress.sin_addr.s_addr = inet_addr( host.c_str() );

	m_socket = ::socket( AF_INET , SOCK_DGRAM , IPPROTO_UDP );

	if ( m_socket == INVALID_SOCKET )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "UDP Socket instantiate failed, Error = %d\n" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

UDPSocket::~UDPSocket()
{
	Close();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UDPSocket::Bind( int port )
{
	sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons( ( u_short ) port );
	bindAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	int iResult = ::bind( m_socket , ( SOCKADDR* ) &bindAddr , sizeof( bindAddr ) );

	if( iResult != 0 )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "UDP Socket Bind failed, Error = %d\n" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int UDPSocket::Send()
{
	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int UDPSocket::Receive()
{
	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void UDPSocket::Close()
{
	if ( m_socket != INVALID_SOCKET )
	{
		int iResult = closesocket( m_socket );

		if ( iResult == SOCKET_ERROR )
		{
			g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "UDP Socket Close failed, Error = %d\n" , WSAGetLastError() );
		}
		m_socket = INVALID_SOCKET;
	}
}
