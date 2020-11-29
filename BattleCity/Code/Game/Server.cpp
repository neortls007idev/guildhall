#include "Game/Server.hpp"

#include "Engine/Core/StdExtensions.hpp"
#include "Game/GameSinglePlayer.hpp"
#include "Game/GameMultiPlayer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Server* g_theServer = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------------------

Server::Server()				
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Server::~Server()
{
	SAFE_RELEASE_POINTER( m_singlePlayerGame );
	SAFE_RELEASE_POINTER( m_multiPlayerGame );
	
	for ( auto index : m_clients )
	{
		index = nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Startup()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::Update( float deltaSeconds )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::CreateSinglePlayerGame()
{
	m_singlePlayerGame = new GameSinglePlayer();
	m_gameType = SINGLE_PLAYER;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::CreateMultiPlayerGame()
{
	m_multiPlayerGame = new GameMultiplayer();
	m_gameType = MULTIPLAYER;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::AddPlayerClientToServer( Client* client )
{
	EmplaceBackAtEmptySpace( m_clients , client );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Server::RemovePlayerClientFromServer( Client* client )
{
	for( int index = 0 ; index < m_clients.size(); index++ )
	{
		if( m_clients[ index ] == nullptr )
		{
			continue;
		}

		if( m_clients[ index ] == client )
		{
			m_clients[ index ] = nullptr;
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::AddRemoteNewRemotePlayer()
{
	Player* secondplayer = m_multiPlayerGame->CreateAndAddPlayerAtpositionAndOrientation( Vec2( 46.5 , 1.5f ) , 135.f );
	secondplayer->UpdatePlayerColor( MAGENTA );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game* Server::GetGame()
{
	if ( m_gameType == SINGLE_PLAYER )
	{
		return m_singlePlayerGame;
	}
	else if ( m_gameType == MULTIPLAYER )
	{
		return m_multiPlayerGame;
	}
	else
	{
		__debugbreak();
	}
	return nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Server::ParseReceivedMessages( std::vector< GameUDPData > messageBuffer )
{
	if ( m_gameType == MULTIPLAYER && m_isRemoteClientConnectionComplete )
	{
		for ( auto index : messageBuffer )
		{
			if ( index.m_packetType != INVAlID_PACKET )
			{
				Entity* entityData = new Entity( GetGame() , Vec2::ZERO , 0.f );
				memcpy( &entityData , &index.m_data[ 0 ] , sizeof( Entity ) - sizeof( Game* ) );
				//Strings enityData = SplitStringAtGivenDelimiter( index , '|' );
				//int identifier = atoi( enityData[ 0 ].c_str() );
				////LOG_SYSMESSAGE( " UniqueKey = %d" , identifier );
				//if ( identifier != m_uniqueKey )
				//{
				//	continue;
				//}

				Game* currentGame = GetGame();
				Entitylist& entityList = currentGame->m_world->m_currentMap->m_entityListsByType[ entityData->m_entityType ];
				int entityID = entityData->m_entityID;

				for ( int entityIndex = 0; entityIndex < entityList.size(); entityIndex++ )
				{
					if ( entityList[ entityIndex ]->m_entityID == entityID )
					{
						entityList[ entityIndex ] = entityData;
						
						//entityList[ entityIndex ]->m_position = entityList[ entityIndex ]->m_position.SetFromText( entityData[ 3 ].c_str() );
						//entityList[ entityIndex ]->m_orientationDegrees = StringConvertToValue( entityData[ 4 ].c_str() , entityList[ entityIndex ]->m_orientationDegrees );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------