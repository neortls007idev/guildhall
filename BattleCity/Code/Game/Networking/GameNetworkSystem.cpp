#include "Engine/Core/DevConsole.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"
#include "Game/Networking/GameTCPServer.hpp"
#include "Game/Networking/GameTCPClient.hpp"
#include "Game/Networking/GameUDPListner.hpp"
#include <WinSock2.h>
#include "ws2tcpip.h"
#include <array>
#pragma comment(lib, "Ws2_32.lib")

//--------------------------------------------------------------------------------------------------------------------------------------------
		GameNetworkSystem*		g_theGameNetworkSys;
extern	DevConsole*				g_theDevConsole;
static	bool					areDevConsoleCommandsAdded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::m_wasMessageJustSentByServer = false;
STATIC bool GameNetworkSystem::m_wasMessageJustSentByClient = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

//--------------------------------------------------------------------------------------------------------------------------------------------

GameNetworkSystem::GameNetworkSystem() :
								m_isListening( false )
								
{
	for( int index = 0 ; index < MAX_CLIENTS ; index++ )
	{
		m_linkSocket[ index ]	= INVALID_SOCKET;
		m_TCPclient[ index ]	= new GameTCPClient( INVALID_SOCKET );
		m_UDPListner[ index ]	= new GameUDPListner();
	}
	if ( !areDevConsoleCommandsAdded )
	{
		AddNetowrkingCommandsToConsole();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GameNetworkSystem::~GameNetworkSystem()
{
	m_isListening = false;

	for ( int index = 0; index < MAX_CLIENTS; index++ )
	{
		m_linkSocket[ index ] = INVALID_SOCKET;
		if ( nullptr != m_TCPclient[ index ] )
		{
			delete m_TCPclient[ index ];
			m_TCPclient[ index ] = nullptr;
		}
	}

	if ( nullptr != m_TCPServer )
	{
		delete m_TCPServer;
		m_TCPServer = nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::Startup()
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

void GameNetworkSystem::Shutdown()
{
	for( int index = 0 ; index < MAX_CLIENTS ; index++ )
	{
		if ( m_UDPListner[ index ] != nullptr )
		{
			SAFE_RELEASE_POINTER( m_UDPListner[ index ] );
		}
	}

	int iResult = WSACleanup();

	if( iResult == SOCKET_ERROR )
	{
		g_theDevConsole->PrintString( DEVCONSOLE_ERROR , "Call to WSACleanup Failed %i" , WSAGetLastError() );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::BeginFrame()
{
	if( m_isListening )
	{
		if( ( m_TCPServer != nullptr ) && ( m_TCPServer->m_listenSocket != INVALID_SOCKET ) )
		{
			for( int index = 0 ; index < MAX_CLIENTS ; index++ )
			{				
				if( m_linkSocket[ index ] == INVALID_SOCKET )
				{
					m_linkSocket[ index ] = m_TCPServer->Accept();

					if( m_linkSocket[ index ] != INVALID_SOCKET )
					{
						g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Client Connected from %s" , GetAddress( m_linkSocket[ index ] ).c_str() );
					}
				}
				if( m_linkSocket[ index ] != INVALID_SOCKET )
				{
					std::array<char , 256> bufferRecieve;
					std::string recievedClientMessage = m_TCPServer->ReceiveClientMessage( m_linkSocket[ index ] , &bufferRecieve[ 0 ] , ( int ) ( bufferRecieve.size() - 1 ) );
					if( recievedClientMessage != "" )
					{
						m_recievedTCPClientMesageBuffer.emplace_back( recievedClientMessage );
					}
					
					if( m_wasMessageJustSentByServer )
					{
						//std::array<char , 256> buffer;
						m_TCPServer->SendClientMessage( m_linkSocket[ index ] );
					}
				}
			}
			m_wasMessageJustSentByServer = false;
		}
	}
	
	for ( int index = 0; index < MAX_CLIENTS; index++ )
	{
		if( m_TCPclient[ index ] != nullptr )
		{
			if( m_TCPclient[ index ]->m_clientSocket != INVALID_SOCKET )
			{
				std::array<char , 256> bufferRecieve;
				
				std::string recievedServerMessage = m_TCPclient[ index ]->ReceiveServerMessage( m_TCPclient[ index ]->m_clientSocket , &bufferRecieve[ 0 ] , ( int ) ( bufferRecieve.size() - 1 ) );
				if ( recievedServerMessage != "" )
				{
					m_recievedTCPServerMesageBuffer.emplace_back( recievedServerMessage );
				}

				if( m_wasMessageJustSentByClient )
				{
					m_TCPclient[ index ]->SendServerMessage( m_TCPclient[ index ]->m_clientSocket );
					m_wasMessageJustSentByClient = false;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::EndFrame()
{
	m_recievedTCPClientMesageBuffer.clear();
	m_recievedTCPServerMesageBuffer.clear();
	m_recievedUDPMesageBuffer.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

std::string GameNetworkSystem::GetAddress( SOCKET socket )
{

	std::array<char , 128> addressString;

	sockaddr clientAddress;
	int addressSize = sizeof( clientAddress );
	int iResult = getpeername( socket , &clientAddress , &addressSize );

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

void GameNetworkSystem::AddNetowrkingCommandsToConsole()
{
	EventArgs consoleArgs;
	g_theDevConsole->CreateCommand( "StartTCPServer" , "Start a TCP Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "StartTCPServer" , StartTCPServer );

	g_theDevConsole->CreateCommand( "CloseTCPServer" , "Close TCP Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "CloseTCPServer" , CloseTCPServer );

	g_theDevConsole->CreateCommand( "ServerSendMessage" , "Send Message from Server to Client" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ServerSendMessage" , SendMessageToClient );

	g_theDevConsole->CreateCommand( "ConnectToServer" , "Connect Client to Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ConnectToServer" , ConnectToServer );

	g_theDevConsole->CreateCommand( "ClientSendMessage" , "Send Message from Client to Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ClientSendMessage" , SendMessageToServer );

	g_theDevConsole->CreateCommand( "DisconnectClient" , "Disconnect Client from the Server" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "DisconnectClient" , DisconnectFromServer );

	g_theDevConsole->CreateCommand( "OpenUDPPort" , "Open UDP Socket at listen and send ports" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "OpenUDPPort" , OpenUDPPort );

	g_theDevConsole->CreateCommand( "SendUDPMessage" , "Send Message over UDP Socket" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "SendUDPMessage" , SendUDPMessage );

	g_theDevConsole->CreateCommand( "CloseUDPPort" , "Close UDP Socket" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "CloseUDPPort" , CloseUDPPort );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::StartTCPServer( EventArgs& args )
{
	int port = args.GetValue( "port" , 48000 );

	if ( nullptr != g_theGameNetworkSys )
	{
		g_theGameNetworkSys->SetIsListening( true );

		if ( nullptr == g_theGameNetworkSys->m_TCPServer )
		{
			g_theGameNetworkSys->m_TCPServer = new GameTCPServer( port );
			g_theGameNetworkSys->m_TCPServer->Bind();
			g_theGameNetworkSys->m_TCPServer->Listen();
		}
		
		g_theGameNetworkSys->m_TCPServer->SetListenPort( port );
		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Server Started" );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::CloseTCPServer( EventArgs& args )
{
	UNUSED( args );
	
	if( nullptr != g_theGameNetworkSys )
	{
		g_theGameNetworkSys->SetIsListening( false );
		g_theGameNetworkSys->m_TCPServer->SetListenPort( -1 );

		for ( int index = 0; index < MAX_CLIENTS; index++ )
		{
			closesocket( g_theGameNetworkSys->m_linkSocket[ index ] );
			g_theGameNetworkSys->m_linkSocket[ index ] = INVALID_SOCKET;
		}

		closesocket( g_theGameNetworkSys->m_TCPServer->m_listenSocket );
		if ( g_theGameNetworkSys->m_TCPServer != nullptr )
		{
			delete g_theGameNetworkSys->m_TCPServer;
			g_theGameNetworkSys->m_TCPServer = nullptr;
		}
		
		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "SERVER CLOSED" );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::SendMessageToClient( EventArgs& args )
{
	if( ( nullptr != g_theGameNetworkSys->m_TCPServer ) )
	{
		for ( int index = 0; index < MAX_CLIENTS; index++ )
		{
			if( ( g_theGameNetworkSys->m_linkSocket[ index ] != INVALID_SOCKET ) )
			{
				std::string message = args.GetValue( "msg" , "InvalidMessage" );
				g_theGameNetworkSys->m_TCPServer->SetServerSendMessage( message );
			}
		}

		m_wasMessageJustSentByServer = true;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::SendMessageToServer( EventArgs& args )
{
	if( ( nullptr != g_theGameNetworkSys->m_TCPclient ) )
	{
		for ( int index = 0; index < MAX_CLIENTS; index++ )
		{
			if( ( g_theGameNetworkSys->m_TCPclient[ index ]->m_clientSocket != INVALID_SOCKET ) )
			{
				std::string message = args.GetValue( "msg" , "InvalidMessage" );
				g_theGameNetworkSys->m_TCPclient[ index ]->SetClientSendMessage( message );
			}
		}
		m_wasMessageJustSentByClient = true;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool GameNetworkSystem::ConnectToServer( EventArgs& args )
{
	if( ( nullptr != g_theGameNetworkSys->m_TCPclient ) )
	{
		std::string host = args.GetValue( "ipaddr" , "" );		
		std::string port = args.GetValue( "port" , "48000" );
	
		for ( int index = 0; index < MAX_CLIENTS; index++ )
		{
			g_theGameNetworkSys->m_TCPclient[ index ]->m_clientSocket = g_theGameNetworkSys->m_TCPclient[ index ]->Connect( host.c_str() ,
			                                                                                      ( uint16_t )atoi( port.c_str() ) ,
			                                                                                      Mode::Nonblocking );
			if( g_theGameNetworkSys->m_TCPclient[ index ]->m_clientSocket != INVALID_SOCKET )
			{
				g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "Server Connected to %s" ,
											  g_theGameNetworkSys->GetAddress( g_theGameNetworkSys->m_TCPclient[ index ]->m_clientSocket ).c_str() );
				break;
			}
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::DisconnectFromServer( EventArgs& args )
{
	UNUSED( args );

	if( nullptr != g_theGameNetworkSys )
	{
		for ( int index = 0; index < MAX_CLIENTS; index++ )
		{
			closesocket( g_theGameNetworkSys->m_linkSocket[ index ] );
			closesocket( g_theGameNetworkSys->m_TCPclient[ index ]->m_clientSocket );

			g_theGameNetworkSys->m_TCPclient[ index ]->m_clientSocket = INVALID_SOCKET;
			g_theGameNetworkSys->m_linkSocket[ index ] = INVALID_SOCKET;	
			break;
		}

		g_theDevConsole->PrintString( DEVCONSOLE_SYTEMLOG , "CLIENT DISCONNECTED" );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::OpenUDPPort( EventArgs& args )
{
	if ( nullptr != g_theGameNetworkSys )
	{
		int bindPort	= args.GetValue( "bindPort" , 48000 );
		int sendToPort	= args.GetValue( "sendPort" , 48001 );
		std::string host = args.GetValue( "host" , "127.0.0.1" );

		g_theGameNetworkSys->StartUDPListner( bindPort , sendToPort , host );
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::SendUDPMessage( EventArgs& args )
{
	if ( nullptr != g_theGameNetworkSys )
	{
		std::string msg = args.GetValue( "msg" , "" );
		if( msg != "" )
		{
			g_theGameNetworkSys->SendUDPMessage( msg );
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool GameNetworkSystem::CloseUDPPort( EventArgs& args )
{
	if ( nullptr != g_theGameNetworkSys )
	{
		for( int index = 0 ; index < MAX_CLIENTS ; index++ )
		{
			std::string bindPort = args.GetValue( "bindPort" , "48000" );
			g_theGameNetworkSys->m_UDPListner[ index ]->m_listenSocket->Close();
			// TODO close only the correct client
			break;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
	
void GameNetworkSystem::StartUDPListner( int bindPort , int sendPort , std::string host /*= "127.0.0.1"*/ )
{
	for( int index = 0 ; index < MAX_CLIENTS ; index++ )
	{
		m_UDPListner[ index ]->StartSocket( bindPort , sendPort , host );
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GameNetworkSystem::SendUDPMessage( std::string message )
{
	for ( int index = 0; index < MAX_CLIENTS; index++ )
	{
		m_UDPListner[ index ]->AddMessage( message );
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------