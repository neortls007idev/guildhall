#include "Engine/Core/DevConsole.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Engine/Networking/TCPServer.hpp"
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

NetworkSystem::NetworkSystem() :
								m_isListening( false ) ,
								m_clientSocket( INVALID_SOCKET )
{
	if ( !areDevConsoleCommandsAdded )
	{
		AddNetowrkingCommandsToConsole();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

NetworkSystem::~NetworkSystem()
{
	m_isListening = false;
	m_clientSocket = INVALID_SOCKET;
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
	if( m_isListening && m_TCPServers.empty() )
	{
		m_TCPServers.push_back( new TCPServer( m_listenPort ) );
		m_TCPServers.front()->Bind();
		m_TCPServers.front()->Listen();
	}
	else if ( m_isListening )
	{
		//if( m_isListening )
		//{
		if( m_clientSocket == INVALID_SOCKET )
		{
			m_clientSocket = m_TCPServers.front()->Accept();
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
				//buffer[ iResult ] = NULL;
				MessageHeader* header = reinterpret_cast< MessageHeader* >( &buffer[ 0 ] );
				
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
		//g_theNetworkSys->SetListenPort( port );
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
		//g_theNetworkSys->SetListenPort( -1 );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------