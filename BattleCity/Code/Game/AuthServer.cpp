#include "Game/AuthServer.hpp"
#include "Game/Server.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiplayer.hpp"
#include "Game/TheApp.hpp"
#include "PlayerClient.hpp"
#include "RemoteClient.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Time/Timer.hpp"
#include "Game/Networking/GameNetworkSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
AuthoritativeServer* g_theAuthServer = nullptr;
extern	GameNetworkSystem* g_theGameNetworkSys;
extern TheApp* g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

AuthoritativeServer::AuthoritativeServer()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

AuthoritativeServer::~AuthoritativeServer()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::Startup()
{
	m_sendFreq = new Timer();
	m_sendFreq->SetSeconds( 0.032 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::Shutdown()
{
	EventArgs args;
	args.SetValue( "bindPort" , ToString( m_udpListenPort ) );

	if ( m_gameType == MULTIPLAYER && g_theGameNetworkSys != nullptr )
	{
		g_theGameNetworkSys->CloseUDPPort( args );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::Update( float deltaSeconds )
{
	if( m_gameType == SINGLE_PLAYER )
	{
		m_singlePlayerGame->Update( deltaSeconds );
	}
	else if( m_gameType == MULTIPLAYER )
	{
		m_multiPlayerGame->Update( deltaSeconds );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::BeginFrame()
{
	if( m_gameType == MULTIPLAYER )
	{
		for( auto index : g_theGameNetworkSys->m_recievedTCPClientMesageBuffer )
		{
			if ( m_isRemoteClientConnectionComplete )
			{
				break;
			}

			Strings updSocketEventData = SplitStringAtGivenDelimiter( index );
			if( updSocketEventData.size() == 2 )
			{
				if ( _stricmp( updSocketEventData[ 0 ].c_str() , "StartUDPServer" ) == 0 )
				{
					RandomNumberGenerator rng;
					m_uniqueKey = rng.RollRandomIntInRange( INT16_MIN , INT16_MAX - 1 );
					std::string ipaddr = updSocketEventData[ 1 ].c_str();
					m_udpListenPort = rng.RollRandomIntInRange( 48000 , 49000 );
					m_udpSendToPort = rng.RollRandomIntInRange( 48000 , 49000 );

					while ( m_udpSendToPort == m_udpListenPort )
					{
						m_udpSendToPort = rng.RollRandomIntInRange( 48000 , 49000 );
						rng.manuallyIncrementPosition();
					}
					m_numRemotePlayers++;

					EventArgs args;
					std::string message = "UDPServerStarted," + 
										  ToString( m_uniqueKey ) + "," +
										  ipaddr.c_str() + "," +
										  ToString( m_udpListenPort ) + "," +
										  ToString( m_udpSendToPort ) + "," +
										  ToString( m_numRemotePlayers );

					LOG_SYSMESSAGE( " UniqueKey = %d" , m_uniqueKey );

					EventArgs UDPArgs;

					UDPArgs.SetValue( "bindPort" , ToString( m_udpListenPort ) );
					UDPArgs.SetValue( "sendPort" , ToString( m_udpSendToPort ) );
					UDPArgs.SetValue( "host" , ipaddr.c_str() );

					args.SetValue( "msg" , message );

					if ( g_theGameNetworkSys->SendMessageToClient( args ) )
					{
						EventArgs unusedArgs;

						if ( g_theGameNetworkSys->OpenUDPPort( UDPArgs ) )
						{
							AddRemoteNewRemotePlayer();

							if ( g_theApp->m_distantClient == nullptr )
							{
								g_theApp->m_distantClient = new RemoteClient( m_numRemotePlayers );
								g_theApp->m_distantClient->Startup();
							}

							g_theAuthServer->AddPlayerClientToServer( g_theApp->m_distantClient );

							m_isRemoteClientConnectionComplete = true;
							
							//g_theNetworkSys->CloseTCPServer( unusedArgs );
						}
					}
				}
			}
		}
		
		// If request to start UDP Server was just received ? then start UDP server and let the client know about it.
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::EndFrame()
{
	ParseReceivedMessages( g_theGameNetworkSys->m_recievedUDPMesageBuffer );

	if( m_sendFreq->HasElapsed() )
	{
		ParseAndSendEntityData();
		m_sendFreq->Reset();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//Game* AuthoritativeServer::GetGame()
//{
//	if( m_gameType == SINGLE_PLAYER )
//	{
//		return m_singlePlayerGame;
//	}
//	else if( m_gameType == MULTIPLAYER )
//	{
//		return m_multiPlayerGame;
//	}
//	else
//	{
//		__debugbreak();
//	}
//	return nullptr;
//}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::AddPlayers()
{
	if( m_gameType == SINGLE_PLAYER )
	{
		m_singlePlayerGame->Startup();
		m_singlePlayerGame->CreatePlayer();
	}
	else if( m_gameType == MULTIPLAYER )
	{
		m_multiPlayerGame->Startup();
		m_multiPlayerGame->CreateAndAddPlayerAtpositionAndOrientation();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void AuthoritativeServer::ParseAndSendEntityData()
{

	if ( m_gameType == MULTIPLAYER && m_isRemoteClientConnectionComplete )
	{
		Game* currentGame = GetGame();

		if ( currentGame == nullptr )
		{
			return;
		}
		
		for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
		{
			if ( Entitytype == PLAYERTANK_ENTITY )
			{
				continue;
			}

			Entitylist& currentList = currentGame->m_world->m_currentMap->m_entityListsByType[ Entitytype ];
			for ( int entityIndex = 0; entityIndex < ( int ) currentList.size(); entityIndex++ )
			{
				if ( currentList[ entityIndex ] )
				{
					Entity entityData = *currentList[ entityIndex ];
					GameUDPData packet;
					packet.m_frameID = 0;
					packet.m_packetType = ( UDPDataPacketType ) Entitytype;
					memcpy( &packet.m_data[ 0 ] , &entityData , sizeof( Entity ) );
					g_theGameNetworkSys->SendUDPMessage( packet );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------