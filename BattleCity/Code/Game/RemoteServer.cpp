#include "Game/RemoteServer.hpp"
#include "Game/Server.hpp"
#include "Game/GameMultiplayer.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Game/TheApp.hpp"
#include "PlayerClient.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

		RemoteServer*	g_theRemoteServer = nullptr;

extern	NetworkSystem*	g_theNetworkSys;
extern	TheApp*			g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

RemoteServer::RemoteServer()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

RemoteServer::~RemoteServer()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::Startup()
{
	CreateMultiPlayerGame();
	m_multiPlayerGame->Startup();
	m_multiPlayerGame->CreateAndAddPlayerAtpositionAndOrientation();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	//if ( m_gameType == MULTIPLAYER )
	//{
	//	m_multiPlayerGame->Update( deltaSeconds );
	//}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::BeginFrame()
{
	if ( m_gameType == MULTIPLAYER )
	{
		for ( auto index : g_theNetworkSys->m_recievedTCPServerMesageBuffer )
		{
			if( m_isRemoteClientConnectionComplete )
			{
				break;
			}
			if( index != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( index );
				
				if ( data.size() > 0 && _stricmp( data[ 0 ].c_str() , "UDPServerStarted" ) == 0 )
				{
					m_uniqueKey = atoi( data[ 1 ].c_str() );
					LOG_SYSMESSAGE( " UniqueKey = %d" , m_uniqueKey );
					m_hostIPaddr = data[ 2 ];
					m_udpSendToPort = atoi( data[ 3 ].c_str() );
					m_udpListenPort = atoi( data[ 4 ].c_str() );
					
					m_playerID = atoi( data[ 5 ].c_str() );
					
					AddRemoteNewRemotePlayer();

					if ( g_theApp->m_localclient == nullptr )
					{
						g_theApp->m_localclient = new PlayerClient( m_playerID );
						g_theApp->m_localclient->Startup();
					}

					AddPlayerClientToServer( g_theApp->m_localclient );

					EventArgs UDPArgs;

					UDPArgs.SetValue( "bindPort" , ToString( m_udpListenPort ) );
					UDPArgs.SetValue( "sendPort" , ToString( m_udpSendToPort ) );
					UDPArgs.SetValue( "host" , m_hostIPaddr.c_str() );
					
					if ( g_theNetworkSys->OpenUDPPort( UDPArgs ) )
					{
						EventArgs args;
						//g_theNetworkSys->DisconnectFromServer( args );
						m_isRemoteClientConnectionComplete = true;
					}
				}
			}
		}

		// If request to start UDP Server was just recieved ? then start UDP server and let the client know about it.
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::EndFrame()
{
	ParseReceivedMessages( g_theNetworkSys->m_recievedUDPMesageBuffer );
	ParseAndUpdateEntities();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RemoteServer::ParseAndUpdateEntities()
{
	if ( m_gameType == MULTIPLAYER && m_isRemoteClientConnectionComplete )
	{
		Game* currentGame = GetGame();
		
		if ( currentGame == nullptr )
		{
			return;
		}

		for ( auto index : g_theNetworkSys->m_recievedUDPMesageBuffer )
		{
			if ( index != "" )
			{
				Strings data = SplitStringAtGivenDelimiter( index , '|' );
				int identifier = atoi( data[ 0 ].c_str() );
				//LOG_SYSMESSAGE( " UniqueKey = %d" , identifier );
				if ( identifier != m_uniqueKey )
				{
					continue;
				}
								
				Entitylist& entityList = currentGame->m_world->m_currentMap->m_entityListsByType[ atoi( data[ 1 ].c_str() ) ];
				int entityID = atoi( data[ 2 ].c_str() );

				for ( int entityIndex = 0; entityIndex < entityList.size(); entityIndex++ )
				{
					if ( entityList[ entityIndex ]->m_entityID == entityID )
					{
						entityList[ entityIndex ]->m_position = entityList[ entityIndex ]->m_position.SetFromText( data[ 3 ].c_str() );
						entityList[ entityIndex ]->m_orientationDegrees = StringConvertToValue( data[ 4 ].c_str() , entityList[ entityIndex ]->m_orientationDegrees );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//Game* RemoteServer::GetGame()
//{
//	return m_multiPlayerGame;
//}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool RemoteServer::RequestAddPlayerEntityandUDPConnection()
{
	EventArgs args;
	args.SetValue( "msg" , "StartUDPServer" );
	if ( g_theNetworkSys != nullptr )
	{
		return g_theNetworkSys->SendMessageToServer( args );
	}

	return false;
}
