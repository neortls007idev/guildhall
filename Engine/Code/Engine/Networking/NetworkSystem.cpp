#include "Engine/Core/DevConsole.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/Networking/TCPServer.hpp"
#include "Engine/Networking/TCPClient.hpp"
#include <WinSock2.h>
#include "ws2tcpip.h"
#include <array>
#pragma comment(lib, "Ws2_32.lib")

//--------------------------------------------------------------------------------------------------------------------------------------------
		NetworkSystem*		g_theNetworkSys;
extern	DevConsole*			g_theDevConsole;
static	bool				areDevConsoleCommandsAdded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool NetworkSystem::m_wasMessageJustSentByServer = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

//--------------------------------------------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------------------------------------------

NetworkSystem::NetworkSystem() :
								m_isListening( false )
								
{
	m_TCPclient = new TCPClient( INVALID_SOCKET );
	if ( !areDevConsoleCommandsAdded )
	{
		AddNetowrkingCommandsToConsole();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

NetworkSystem::~NetworkSystem()
{
	m_isListening = false;
	delete m_TCPclient;
	m_TCPclient = nullptr;

	delete m_TCPServer;
	m_TCPServer = nullptr;
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
	if( m_isListening && ( nullptr == m_TCPServer ) )
	{
		//m_TCPServer.push_back( new TCPServer( m_listenPort ) );
		//m_TCPServer.front()->Bind();
		//m_TCPServer.front()->Listen();
		m_TCPServer = new TCPServer( m_listenPort );
		m_TCPServer->Bind();
		m_TCPServer->Listen();
	}
	else if ( m_isListening )
	{
		if( m_TCPclient->m_clientSocket == INVALID_SOCKET )
		{
			//m_clientSocket = m_TCPServers.front()->Accept();
			m_TCPclient->m_clientSocket = m_TCPServer->Accept();
			if ( m_TCPclient->m_clientSocket != INVALID_SOCKET )
			{
				g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Client Connected from %s" , GetAddress().c_str() );
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
			if ( m_wasMessageJustSentByServer )
			{
				std::array<char , 256> buffer;
				//int iResult = ::recv( m_clientSocket , &buffer[ 0 ] , static_cast< int >( buffer.size() - 1 ) , 0 );
				int iResult = ::recv( m_TCPclient->m_clientSocket , &buffer[ 0 ] , static_cast< int >( buffer.size() - 1 ) , 0 );

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
					//buffer[ iResult ] = NULL;
					MessageHeader* header = reinterpret_cast< MessageHeader* >( &buffer[ 0 ] );

					ServerListenMessage serverListenMessage;
					serverListenMessage.m_header = *header;

					buffer[ iResult ] = NULL;
					serverListenMessage.m_recieveMessage = std::string( &buffer[ 4 ] );
					
					g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Client message: %s" , &buffer[ 0 ] );

					std::string msg = m_TCPServer->GetServerSendMessage();
					iResult = send( m_TCPclient->m_clientSocket , msg.c_str() , static_cast< int >( msg.length() ) , 0 );
					if( iResult == SOCKET_ERROR )
					{
						g_theDevConsole->PrintString( DEVCONSOLE_WARNING , "Sending Data to Client Failed %i" , WSAGetLastError() );
					}
				}
				m_wasMessageJustSentByServer = false;
			}
			std::array<char , 256> clientMessage;
			m_TCPServer->ReceiveClientMessage( m_TCPclient->m_clientSocket , &clientMessage[0] , 256 );
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
	int iResult = getpeername( m_TCPclient->m_clientSocket , &clientAddress , &addressSize );

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to Get Peer Name Failed %i" , WSAGetLastError() );
	}

	DWORD outLength = static_cast< DWORD >( addressString.size() );
#pragma warning( push )
#pragma warning( disable : 4996  )
	iResult = WSAAddressToStringA( &clientAddress , addressSize , NULL , &addressString[ 0 ] , &outLength );
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

	g_theDevConsole->CreateCommand( "ServerSendsMessage" , "Send Message from Server to Client" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ServerSendsMessage" , SendMessageToClient );

	g_theDevConsole->CreateCommand( "ConnectToServer" , "Connect Client to Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ConnectToServer" , ConnectToServer );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool NetworkSystem::StartTCPServer( EventArgs& args )
{
	int port = args.GetValue( "port" , 48000 );

	if ( nullptr != g_theNetworkSys )
	{
		g_theNetworkSys->SetIsListening( true );

		if ( nullptr == g_theNetworkSys->m_TCPServer )
		{
			g_theNetworkSys->m_TCPServer = new TCPServer( port );
			g_theNetworkSys->m_TCPServer->Bind();
			g_theNetworkSys->m_TCPServer->Listen();
		}
		
		g_theNetworkSys->m_TCPServer->SetListenPort( port );
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
		g_theNetworkSys->m_TCPServer->SetListenPort( -1 );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool NetworkSystem::SendMessageToClient( EventArgs& args )
{
	if( ( nullptr != g_theNetworkSys->m_TCPServer ) && ( nullptr != g_theNetworkSys->m_TCPclient ) )
	{
		std::string message = args.GetValue( "msg" , "InvalidMessage" );
		g_theNetworkSys->m_TCPServer->SetServerSendMessage( message );
		m_wasMessageJustSentByServer = true;

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool NetworkSystem::ConnectToServer( EventArgs& args )
{
	if( /*( nullptr != g_theNetworkSys->m_TCPServer ) && */( nullptr != g_theNetworkSys->m_TCPclient ) )
	{
		std::string host = args.GetValue( "ipaddr" , "" );
		std::string port = args.GetValue( "port" , "48000" );
		g_theNetworkSys->m_TCPServer = new TCPServer( ( uint16_t ) atoi( port.c_str() ) );
		g_theNetworkSys->m_TCPServer->m_listenSocket = g_theNetworkSys->m_TCPclient->connect( host.c_str() ,
		                                                                                      ( uint16_t )atoi(
			                                                                                      port.c_str() ) ,
		                                                                                      Mode::Blocking );
		std::string msg( "Send me some data" );
		//g_theNetworkSys->m_TCPServer->m_listenSocket.send( msg.c_str() , msg.length() );
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------