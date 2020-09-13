#include "Engine/Core/DevConsole.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include <WinSock2.h>
#include "ws2tcpip.h"
#include <array>
#pragma comment(lib, "Ws2_32.lib")

//--------------------------------------------------------------------------------------------------------------------------------------------
		NetworkSystem*		g_theNetworkSys;
extern	DevConsole*			g_theDevConsole;
static	bool				areDevConsoleCommandsAdded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

//--------------------------------------------------------------------------------------------------------------------------------------------

NetworkSystem::NetworkSystem() : m_listenPort( -1 ) ,
								m_isListening( false ) ,
								m_listenSocket( INVALID_SOCKET ) ,
								m_clientSocket( INVALID_SOCKET ) ,
								m_timeVal { 01,01 }
{
	FD_ZERO( &m_listenSet );

	if ( !areDevConsoleCommandsAdded )
	{
		AddNetowrkingCommandsToConsole();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

NetworkSystem::~NetworkSystem()
{
	m_listenPort = -1;
	m_isListening = false;
	m_listenSocket = INVALID_SOCKET;
	m_clientSocket = INVALID_SOCKET;
	m_timeVal = { 01,01 };
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NetworkSystem::Startup()
{
	// Initialize WinSock
	WSAData wsaData;
	WORD wVersion MAKEWORD( 2 , 2 );
	int iResult = WSAStartup( wVersion , &wsaData );

	if( iResult != 0 )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to WSAStartup Failed %i" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NetworkSystem::Shutdown()
{
	int iResult = WSACleanup();

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to WSACleanup Failed %i" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NetworkSystem::BeginFrame()
{
	if( m_isListening )
	{
		if( m_listenSocket == INVALID_SOCKET )
		{
			struct addrinfo addressHintsIn;
			struct addrinfo* pAddressOut;

			ZeroMemory( &addressHintsIn , sizeof( addressHintsIn ) );
			addressHintsIn.ai_family = AF_INET;
			addressHintsIn.ai_socktype = SOCK_STREAM;
			addressHintsIn.ai_protocol = IPPROTO_TCP;
			addressHintsIn.ai_flags = AI_PASSIVE;

			std::string serverPort( "48000" );
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
		else if( m_clientSocket == INVALID_SOCKET )
		{
			FD_ZERO( &m_listenSet );
			FD_SET( m_listenSocket , &m_listenSet );
			int iResult = select( 0 , &m_listenSet , NULL , NULL , &m_timeVal );

			if ( iResult == INVALID_SOCKET )
			{
				g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Select Failed %i" , WSAGetLastError() );
			}

			if ( FD_ISSET( m_listenSocket , &m_listenSet  ) )
			{
				m_clientSocket = accept( m_listenSocket , NULL , NULL );

				if ( m_clientSocket == INVALID_SOCKET )
				{
					g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Accept Failed %i" , WSAGetLastError() );
				}
				g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Clientconnected from %s" , GetAddress().c_str() );
			}
		}
		else
		{
			//--------------------------------------------------------------------------------
			//			NOTES
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//
			//		static bool is for debugging purposes
			//	
			//--------------------------------------------------------------------------------
			static bool message = false;
			if ( !message )
			{
			std::array<char , 256> buffer;
			int iResult = ::recv( m_clientSocket , &buffer[ 0 ] , static_cast< int >( buffer.size() - 1 ) , 0 );

			if( iResult == SOCKET_ERROR )
			{
				g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Receive Failed %i" , WSAGetLastError() );
			}
			else if ( iResult == 0 )
			{
				g_theDevConsole->PrintString( DEVCONSOLE_WARNING , "Socket CLOSED from Client End received 0 Bytes" );
			}
			else
			{
				buffer[ iResult ] = NULL;
				g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Client message: %s" , &buffer[ 0 ] );

				std::string msg( "Hello, Death await you ahead! Ready to get bloodSuckered ? ^_^" );
				iResult = send( m_clientSocket , msg.c_str() , static_cast< int >( msg.length() ) , 0 );
				if( iResult == SOCKET_ERROR )
				{
					g_theDevConsole->PrintString( DEVCONSOLE_WARNING , "Sending Data to Client Failed %i" , WSAGetLastError() );
				}
			}
				message = true;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NetworkSystem::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string NetworkSystem::GetAddress()
{
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void NetworkSystem::AddNetowrkingCommandsToConsole()
{
	EventArgs consoleArgs;
	g_theDevConsole->CreateCommand( "StartTCPServer" , "Start a TCP Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "StartTCPServer" , StartTCPServer );

	g_theDevConsole->CreateCommand( "CloseTCPServer" , "Close TCP Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "CloseTCPServer" , CloseTCPServer );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool NetworkSystem::StartTCPServer( EventArgs& args )
{
	int port = args.GetValue( "port" , 48000 );

	if ( nullptr != g_theNetworkSys )
	{
		g_theNetworkSys->SetIsListening( true );
		g_theNetworkSys->SetListenPort( port );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool NetworkSystem::CloseTCPServer( EventArgs& args )
{
	UNUSED( args );
	
	if( nullptr != g_theNetworkSys )
	{
		g_theNetworkSys->SetIsListening( false );
		g_theNetworkSys->SetListenPort( -1 );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------