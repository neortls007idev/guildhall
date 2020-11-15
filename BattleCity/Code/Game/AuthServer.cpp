#include "Game/AuthServer.hpp"
#include "Game/Server.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiplayer.hpp"
#include "Engine/Networking/NetworkSystem.hpp"
#include "Game/TheApp.hpp"
#include "PlayerClient.hpp"
#include "RemoteClient.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Time/Timer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
AuthoritativeServer* g_theAuthServer = nullptr;
extern	NetworkSystem* g_theNetworkSys;
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
		for( auto index : g_theNetworkSys->m_recievedTCPClientMesageBuffer )
		{
			if ( m_isRemoteClientConnectionComplete )
			{
				break;
			}

			if ( _stricmp( index.c_str() , "StartUDPServer" ) == 0 )
			{
				RandomNumberGenerator rng;
				m_uniqueKey = rng.RollRandomIntInRange( INT16_MIN , INT16_MAX - 1 );
				std::string ipaddr = "127.0.0.1";
				int udpListenPort = rng.RollRandomIntInRange( 48000 , 49000 );
				int udpSendToPort = rng.RollRandomIntInRange( 48000 , 49000 );

				while ( udpSendToPort == udpListenPort )
				{
					udpSendToPort = rng.RollRandomIntInRange( 48000 , 49000 );
					rng.manuallyIncrementPosition();
				}
				m_numRemotePlayers++;

				EventArgs args;
				std::string message = "UDPServerStarted," + ToString( m_uniqueKey ) + "," + ipaddr.c_str() + "," + ToString( udpListenPort ) + "," + ToString( udpSendToPort ) + "," + ToString( m_numRemotePlayers );
				LOG_SYSMESSAGE( " UniqueKey = %d" , m_uniqueKey );

				EventArgs UDPArgs;

				UDPArgs.SetValue( "bindPort" , ToString( udpListenPort ) );
				UDPArgs.SetValue( "sendPort" , ToString( udpSendToPort ) );
				UDPArgs.SetValue( "host" , ipaddr.c_str() );
				if ( g_theNetworkSys->OpenUDPPort( UDPArgs ) )
				{
					args.SetValue( "msg" , message );
					AddRemoteNewRemotePlayer();

					if ( g_theApp->m_distantClient == nullptr )
					{
						g_theApp->m_distantClient = new RemoteClient( m_numRemotePlayers );
						g_theApp->m_distantClient->Startup();
					}

					g_theAuthServer->AddPlayerClientToServer( g_theApp->m_distantClient );


					if ( g_theNetworkSys->SendMessageToClient( args ) )
					{
						EventArgs unusedArgs;
						//g_theNetworkSys->CloseTCPServer( unusedArgs );
						m_isRemoteClientConnectionComplete = true;
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
	ParseReceivedMessages( g_theNetworkSys->m_recievedUDPMesageBuffer );

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
					EventArgs EntityUpdateArgs;
					std::string EntityDataAsString = ToString( m_uniqueKey ) + "|" +
													 ToString( Entitytype ) + "|" +
													 ToString( currentList[ entityIndex ]->m_entityID ) + "|" +
													 ToString( currentList[ entityIndex ]->m_position ) + "|" +
													 ToString( currentList[ entityIndex ]->m_orientationDegrees );

					EntityUpdateArgs.SetValue( "msg" , EntityDataAsString.c_str() );
					g_theNetworkSys->SendUDPMessage( EntityUpdateArgs );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------